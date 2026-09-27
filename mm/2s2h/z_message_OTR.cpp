#include "BenPort.h"
#include <ship/resource/ResourceManager.h>
#include "2s2h/resource/type/Scene.h"
#include <ship/utils/StringHelper.h>
#include "2s2h/resource/type/TextMM.h"
#include <message_data_static.h>
#include "z64save.h"

extern "C" MessageTableEntry* sMessageTableNES;
extern "C" MessageTableEntry* sMessageTableCredits;
static MessageTableEntry* sMessageTables[LANGUAGE_MAX] = {};

MessageTableEntry* OTRMessage_LoadTable(const char* filePath, bool isNES) {
    auto file = std::static_pointer_cast<SOH::TextMM>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;

    // Allocate room for an additional message
    // OTRTODO: Should not be malloc'ing here. It's fine for now since we check elsewhere that the message table is
    // already null.
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * (file->messages.size() + 1));

    for (size_t i = 0; i < file->messages.size(); i++) {
        table[i].textId = file->messages[i].id;
        table[i].typePos = (file->messages[i].textboxType << 4) | file->messages[i].textboxYPos;
        table[i].segment = (const char*)malloc(file->messages[i].msg.size() + 11);

        auto segment = (char*)table[i].segment;

        segment[0] = file->messages[i].textboxType;
        segment[1] = file->messages[i].textboxYPos;
        segment[2] = file->messages[i].icon;
        segment[3] = (file->messages[i].nextMessageID & 0xFF00) >> 8;
        segment[4] = (file->messages[i].nextMessageID & 0x00FF);
        segment[5] = (file->messages[i].firstItemCost & 0xFF00) >> 8;
        segment[6] = (file->messages[i].firstItemCost & 0x00FF);
        segment[7] = (file->messages[i].secondItemCost & 0xFF00) >> 8;
        segment[8] = (file->messages[i].secondItemCost & 0x00FF);
        segment[9] = 0xFF;
        segment[10] = 0xFF;

        memcpy((void*)(&table[i].segment[11]), file->messages[i].msg.c_str(), file->messages[i].msg.size());

        table[i].msgSize = file->messages[i].msg.size() + 11;

        // if (isNES && file->messages[i].id == 0xFFFC)
        //_message_0xFFFC_nes = (char*)file->messages[i].msg.c_str();
    }

    return table;
}

extern "C" MessageTableEntry* OTRMessage_GetTable(u8 language) {
    if (language >= LANGUAGE_MAX || sMessageTables[language] == nullptr) {
        return sMessageTables[LANGUAGE_ENG];
    }

    return sMessageTables[language];
}

extern "C" void OTRMessage_Init() {
    sMessageTables[LANGUAGE_ENG] = OTRMessage_LoadTable("text/message_data_static/message_data_static", true);

    if (ResourceMgr_HasGameVersion(MM_PAL_11)) {
        sMessageTables[LANGUAGE_GER] =
            OTRMessage_LoadTable("text/message_data_static_ger/message_data_static_ger", true);
        sMessageTables[LANGUAGE_FRE] =
            OTRMessage_LoadTable("text/message_data_static_fra/message_data_static_fra", true);
        sMessageTables[LANGUAGE_SPA] =
            OTRMessage_LoadTable("text/message_data_static_esp/message_data_static_esp", true);
    } else {
        // Non-PAL releases only provide the English message bank.
        sMessageTables[LANGUAGE_GER] = sMessageTables[LANGUAGE_ENG];
        sMessageTables[LANGUAGE_FRE] = sMessageTables[LANGUAGE_ENG];
        sMessageTables[LANGUAGE_SPA] = sMessageTables[LANGUAGE_ENG];
    }

    // No supported release provides a separate Japanese message bank.
    sMessageTables[LANGUAGE_JPN] = sMessageTables[LANGUAGE_ENG];

    for (size_t language = 0; language < LANGUAGE_MAX; language++) {
        if (sMessageTables[language] == nullptr) {
            sMessageTables[language] = sMessageTables[LANGUAGE_ENG];
        }
    }

    // Preserve existing code paths that still access the global directly.
    sMessageTableNES = sMessageTables[LANGUAGE_ENG];

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
