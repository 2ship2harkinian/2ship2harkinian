#include "Network.h"
#include <spdlog/spdlog.h>
#include <libultraship/libultraship.h>
#include <chrono>

// MARK: - Public

bool Network::Enable(const char* host, uint16_t port) {
    if (isEnabled) {
        return true;
    }

    if (SDLNet_ResolveHost(&networkAddress, host, port) == -1) {
        SPDLOG_ERROR("[Network] SDLNet_ResolveHost: {}", SDLNet_GetError());
        return false;
    }

    isEnabled = true;

    // First check if there is a thread running, if so, join it
    if (receiveThread.joinable()) {
        receiveThread.join();
    }

    receiveThread = std::thread(&Network::ReceiveFromServer, this);
    return true;
}

void Network::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
    isConnected = false;
}

void Network::OnConnected() {
}

void Network::OnDisconnected() {
}

void Network::SwapIncomingPacketQueue(std::queue<nlohmann::json>& emptyQueue) {
    std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);
    std::swap(incomingPacketQueue, emptyQueue);
}

void Network::QueueOutgoingPacket(nlohmann::json packet) {
    if (!isConnected) {
        return;
    }

    std::lock_guard<std::mutex> lock(outgoingPacketQueueMutex);
    outgoingPacketQueue.push(std::move(packet));
}

// MARK: - Private

void Network::ReceiveFromServer() {
    while (isEnabled) {
        while (!isConnected && isEnabled) {
            SPDLOG_TRACE("[Network] Attempting to make connection to server...");
            networkSocket = SDLNet_TCP_Open(&networkAddress);

            if (networkSocket) {
                isConnected = true;
                receivedData.clear();
                SPDLOG_INFO("[Network] Connection to server established!");

                OnConnected();
                break;
            }

            for (int i = 0; i < 1000 / 100 && isEnabled; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
        if (networkSocket) {
            SDLNet_TCP_AddSocket(socketSet, networkSocket);
        }

        // Listen to socket messages
        while (isConnected && networkSocket && isEnabled) {
            // we check first if socket has data, to not block in the TCP_Recv
            int socketsReady = SDLNet_CheckSockets(socketSet, 10); // 10ms timeout

            if (socketsReady == -1) {
                SPDLOG_ERROR("[Network] SDLNet_CheckSockets: {}", SDLNet_GetError());
                break;
            }

            if (socketsReady > 0) {
                char remoteDataReceived[512];
                memset(remoteDataReceived, 0, sizeof(remoteDataReceived));
                int len = SDLNet_TCP_Recv(networkSocket, &remoteDataReceived, sizeof(remoteDataReceived));
                if (!len || !networkSocket || len == -1) {
                    SPDLOG_ERROR("[Network] SDLNet_TCP_Recv: {}", SDLNet_GetError());
                    break;
                }

                receivedData.append(remoteDataReceived, len);

                // Process all complete packets
                size_t delimiterPos = receivedData.find('\0');
                while (delimiterPos != std::string::npos) {
                    // Extract the complete packet until the delimiter
                    std::string packet = receivedData.substr(0, delimiterPos);
                    // Remove the packet (including the delimiter) from the received data
                    receivedData.erase(0, delimiterPos + 1);
                    HandleCompletePacket(packet);
                    // Find the next delimiter
                    delimiterPos = receivedData.find('\0');
                }
            }

            // Always send outgoing packets, not just when receiving data
            ProcessOutgoingPackets();
        }

        if (socketSet) {
            SDLNet_FreeSocketSet(socketSet);
        }

        if (isConnected && networkSocket) {
            SDLNet_TCP_Close(networkSocket);
            networkSocket = nullptr;
            isConnected = false;
            receivedData.clear();
            {
                std::lock_guard<std::mutex> lock(outgoingPacketQueueMutex);
                outgoingPacketQueue = {};
            }
            OnDisconnected();
            SPDLOG_INFO("[Network] Ending receiving thread...");
        }
    }
}

void Network::HandleCompletePacket(std::string payload) {
    try {
        nlohmann::json jsonPayload = nlohmann::json::parse(payload);
        if (!jsonPayload.contains("quiet")) {
            SPDLOG_TRACE("[Network] Received json:\n{}", jsonPayload.dump());
        }

        std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);
        incomingPacketQueue.push(jsonPayload);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[Network] Failed to parse json: \n{}\n{}\n", payload, e.what());
        return;
    }
}

void Network::ProcessOutgoingPackets() {
    if (!isConnected || !networkSocket) {
        return;
    }

    // Copy all queued packets while holding the lock, then send them after releasing
    std::queue<nlohmann::json> packetsToSend;
    {
        std::lock_guard<std::mutex> lock(outgoingPacketQueueMutex);
        packetsToSend.swap(outgoingPacketQueue);
    }

    while (!packetsToSend.empty()) {
        nlohmann::json payload = std::move(packetsToSend.front());
        packetsToSend.pop();

        std::string rawPayload = payload.dump();
        if (!payload.contains("quiet")) {
            SPDLOG_TRACE("[Network] Sending json:\n{}", rawPayload);
        }

        int sent = SDLNet_TCP_Send(networkSocket, rawPayload.c_str(), rawPayload.length() + 1);
        if (sent < (int)(rawPayload.length() + 1)) {
            SPDLOG_ERROR("[Network] SDLNet_TCP_Send: {}", SDLNet_GetError());
            return;
        }
    }
}
