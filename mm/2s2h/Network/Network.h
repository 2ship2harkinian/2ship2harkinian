#ifndef NETWORK_H
#define NETWORK_H
#ifdef __cplusplus

#include <atomic>
#include <thread>
#include <SDL2/SDL_net.h>
#include <nlohmann/json.hpp>

class Network {
  private:
    IPaddress networkAddress;
    TCPsocket networkSocket;
    std::thread receiveThread;
    std::string receivedData;

    void ReceiveFromServer();
    void HandleCompletePacket(std::string payload);
    void ProcessOutgoingPackets();

    std::mutex incomingPacketQueueMutex;
    std::queue<nlohmann::json> incomingPacketQueue;
    std::mutex outgoingPacketQueueMutex;
    std::queue<nlohmann::json> outgoingPacketQueue;

  public:
    std::atomic<bool> isEnabled{ false };
    std::atomic<bool> isConnected{ false };

    bool Enable(const char* host, uint16_t port);
    void Disable();

    virtual void OnConnected();
    virtual void OnDisconnected();

    // To be called from the Game thread
    void SwapIncomingPacketQueue(std::queue<nlohmann::json>& emptyQueue);
    void QueueOutgoingPacket(nlohmann::json packet);
};

#endif // __cplusplus
#endif // NETWORK_H
