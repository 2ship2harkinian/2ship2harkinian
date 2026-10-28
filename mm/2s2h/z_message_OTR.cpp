#include "BenPort.h"
#include <ship/resource/ResourceManager.h>
#include "2s2h/resource/type/Scene.h"
#include <ship/utils/StringHelper.h>
#include "2s2h/resource/type/TextMM.h"
#include <message_data_static.h>
#include <spdlog/spdlog.h>
#include <algorithm>

extern "C" MessageTableEntry* sMessageTableNES;
extern "C" MessageTableEntry* sMessageTableCredits;

static MessageTableEntry* BuildTable(const std::vector<MessageTableEntry>& entries) {
    // Allocate room for an additional message
    // OTRTODO: Should not be malloc'ing here. It's fine for now since we check elsewhere that the message table is
    // already null.
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * (entries.size() + 1));

    std::copy(entries.begin(), entries.end(), table);

    return table;
}

static MessageTableEntry BuildEntry(const SOH::MessageEntryMM& msg) {
    MessageTableEntry entry;
    entry.textId = msg.id;
    entry.typePos = (msg.textboxType << 4) | msg.textboxYPos;
    entry.msgSize = msg.msg.size() + SOH::TextMM::HeaderSize;

    auto segment = (char*)malloc(entry.msgSize);

    segment[0] = msg.textboxType;
    segment[1] = msg.textboxYPos;
    segment[2] = msg.icon;
    segment[3] = (msg.nextMessageID & 0xFF00) >> 8;
    segment[4] = (msg.nextMessageID & 0x00FF);
    segment[5] = (msg.firstItemCost & 0xFF00) >> 8;
    segment[6] = (msg.firstItemCost & 0x00FF);
    segment[7] = (msg.secondItemCost & 0xFF00) >> 8;
    segment[8] = (msg.secondItemCost & 0x00FF);
    segment[9] = 0xFF;
    segment[10] = 0xFF;

    memcpy(&segment[SOH::TextMM::HeaderSize], msg.msg.c_str(), msg.msg.size());

    entry.segment = segment;
    return entry;
}

static std::vector<MessageTableEntry> BuildEntries(const std::vector<SOH::MessageEntryMM>& messages) {
    std::vector<MessageTableEntry> entries;
    entries.reserve(messages.size());

    for (auto& msg : messages) {
        entries.push_back(BuildEntry(msg));
    }

    return entries;
}

static void UpsertEntry(std::vector<MessageTableEntry>& entries, const SOH::MessageEntryMM& msg) {
    if (msg.id == 0xFFFF) {
        SPDLOG_WARN("Message override 0xFFFF ends the table and can't be overridden");
        return;
    }

    if (msg.msg.size() > SOH::TextMM::MaxMessageSize) {
        SPDLOG_WARN("Message override 0x{:04X} is {} bytes, over the {} byte limit", msg.id, msg.msg.size(),
                    SOH::TextMM::MaxMessageSize);
        return;
    }

    // Lookups stop at the 0xFFFF entry, if the table has one
    auto end = std::find_if(entries.begin(), entries.end(),
                            [](const MessageTableEntry& entry) { return entry.textId == 0xFFFF; });
    auto existing =
        std::find_if(entries.begin(), end, [&](const MessageTableEntry& entry) { return entry.textId == msg.id; });

    if (existing == end) {
        entries.insert(end, BuildEntry(msg));
    } else {
        free((void*)existing->segment);
        *existing = BuildEntry(msg);
    }
}

static std::vector<std::string> ListOverrideFiles(const std::string& folder) {
    auto archiveManager = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
    std::vector<std::string> files;

    // A .meta alias is loaded by its path without the extension
    auto paths = archiveManager->ListFiles(folder + "/*");
    for (auto& path : *paths) {
        files.push_back(path.ends_with(".meta") ? path.substr(0, path.size() - 5) : path);
    }
    std::sort(files.begin(), files.end());
    files.erase(std::unique(files.begin(), files.end()), files.end());

    return files;
}

static std::vector<std::shared_ptr<SOH::TextMM>> LoadOverrides(const std::string& folder) {
    auto resourceManager = Ship::Context::GetRawInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    std::vector<std::shared_ptr<SOH::TextMM>> files;

    for (auto& path : ListOverrideFiles(folder)) {
        auto file = std::dynamic_pointer_cast<SOH::TextMM>(resourceManager->LoadResource(path));

        if (file == nullptr) {
            SPDLOG_ERROR("Failed to load message override {}", path);
            continue;
        }

        files.push_back(file);
    }

    // Apply lowest-to-highest resolved archive priority so higher priority wins; keep path order for ties.
    std::stable_sort(files.begin(), files.end(), [&](const auto& a, const auto& b) {
        return archiveManager->GetFilePriority(a->GetInitData()->Path) <
               archiveManager->GetFilePriority(b->GetInitData()->Path);
    });

    return files;
}

static void ApplyOverrides(std::vector<MessageTableEntry>& entries, const std::string& folder) {
    for (auto& file : LoadOverrides(folder)) {
        for (auto& msg : file->messages) {
            UpsertEntry(entries, msg);
        }
    }
}

MessageTableEntry* OTRMessage_LoadTable(const char* filePath, bool isNES) {
    auto file = std::static_pointer_cast<SOH::TextMM>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;

    std::string path = filePath;
    std::vector<MessageTableEntry> entries = BuildEntries(file->messages);
    ApplyOverrides(entries, "override/" + path.substr(0, path.find_last_of('/')));

    return BuildTable(entries);
}

extern "C" void OTRMessage_Init() {
    sMessageTableNES = OTRMessage_LoadTable("text/message_data_static/message_data_static", true);

    auto file2 =
        std::static_pointer_cast<SOH::TextMM>(Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(
            "text/staff_message_data_static/staff_message_data_static"));
    sMessageTableCredits = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * file2->messages.size());

    for (size_t i = 0; i < file2->messages.size(); i++) {
        sMessageTableCredits[i].textId = file2->messages[i].id;
        sMessageTableCredits[i].typePos = (file2->messages[i].textboxType << 4) | file2->messages[i].textboxYPos;
        sMessageTableCredits[i].segment = file2->messages[i].msg.c_str();
        sMessageTableCredits[i].msgSize = file2->messages[i].msg.size();
    }
}
