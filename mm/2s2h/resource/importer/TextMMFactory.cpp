#include "2s2h/resource/importer/TextMMFactory.h"
#include "2s2h/resource/type/TextMM.h"
#include <spdlog/spdlog.h>
#include <tinyxml2.h>
#include <algorithm>
#include <optional>
#include <unordered_map>
#include "message_data_fmt_nes.h"

extern "C" {
#include "z64message.h"
}

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextMMV0::ReadResource(std::shared_ptr<Ship::File> file,
                                            std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto text = std::make_shared<TextMM>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    const uint32_t msgCount = reader->ReadUInt32();
    text->messages.reserve(msgCount);

    for (uint32_t i = 0; i < msgCount; i++) {
        MessageEntryMM entry;
        entry.id = reader->ReadUInt16();
        entry.textboxType = reader->ReadUByte();
        entry.textboxYPos = reader->ReadUByte();
        // BENTODO: the new ZAPD reads and exports this as an int16 for JP but nothing currently uses that and the game
        // expects an int8. Use this for now.
        entry.icon = (int8_t)reader->ReadUInt16();
        entry.nextMessageID = reader->ReadUInt16();
        entry.firstItemCost = reader->ReadUInt16();
        entry.secondItemCost = reader->ReadUInt16();
        entry.msg = reader->ReadString();

        text->messages.push_back(entry);
    }

    return text;
}

struct TextControl {
    uint8_t code;
    uint8_t valueSize;
};

static const std::unordered_map<std::string, TextControl> sTextControls = {
    { "Space", { ' ', 0 } },
    { "TextSpeed", { MESSAGE_TEXT_SPEED, 0 } },
    { "HighScoreBoatArchery", { MESSAGE_HS_BOAT_ARCHERY, 0 } },
    { "StrayFairies", { MESSAGE_STRAY_FAIRIES, 0 } },
    { "Tokens", { MESSAGE_TOKENS, 0 } },
    { "PointsTens", { MESSAGE_POINTS_TENS, 0 } },
    { "Points", { MESSAGE_POINTS_THOUSANDS, 0 } },
    { "BoxBreak", { MESSAGE_BOX_BREAK, 0 } },
    { "NewLine", { MESSAGE_NEWLINE, 0 } },
    { "BoxBreak2", { MESSAGE_BOX_BREAK2, 0 } },
    { "CarriageReturn", { MESSAGE_CARRIAGE_RETURN, 0 } },
    { "Shift", { MESSAGE_SHIFT, 1 } },
    { "Continue", { MESSAGE_CONTINUE, 0 } },
    { "PlayerName", { MESSAGE_NAME, 0 } },
    { "QuickTextEnable", { MESSAGE_QUICKTEXT_ENABLE, 0 } },
    { "QuickTextDisable", { MESSAGE_QUICKTEXT_DISABLE, 0 } },
    { "Event", { MESSAGE_EVENT, 0 } },
    { "Persistent", { MESSAGE_PERSISTENT, 0 } },
    { "BoxBreakDelayed", { MESSAGE_BOX_BREAK_DELAYED, 2 } },
    { "Fade", { MESSAGE_FADE, 2 } },
    { "FadeSkippable", { MESSAGE_FADE_SKIPPABLE, 2 } },
    { "Sfx", { MESSAGE_SFX, 2 } },
    { "Delay", { MESSAGE_DELAY, 2 } },
    { "End", { MESSAGE_END, 0 } },
    { "Background", { MESSAGE_BACKGROUND, 0 } },
    { "TwoChoice", { MESSAGE_TWO_CHOICE, 0 } },
    { "ThreeChoice", { MESSAGE_THREE_CHOICE, 0 } },
    { "TimerPostman", { MESSAGE_TIMER_POSTMAN, 0 } },
    { "TimerMinigame1", { MESSAGE_TIMER_MINIGAME_1, 0 } },
    { "Timer2", { MESSAGE_TIMER_2, 0 } },
    { "TimerMoonCrash", { MESSAGE_TIMER_MOON_CRASH, 0 } },
    { "TimerMinigame2", { MESSAGE_TIMER_MINIGAME_2, 0 } },
    { "TimerEnvHazard", { MESSAGE_TIMER_ENV_HAZARD, 0 } },
    { "Time", { MESSAGE_TIME, 0 } },
    { "ChestFlags", { MESSAGE_CHEST_FLAGS, 0 } },
    { "InputBank", { MESSAGE_INPUT_BANK, 0 } },
    { "RupeesSelected", { MESSAGE_RUPEES_SELECTED, 0 } },
    { "RupeesTotal", { MESSAGE_RUPEES_TOTAL, 0 } },
    { "TimeUntilMoonCrash", { MESSAGE_TIME_UNTIL_MOON_CRASH, 0 } },
    { "InputDoggyRacetrackBet", { MESSAGE_INPUT_DOGGY_RACETRACK_BET, 0 } },
    { "InputBomberCode", { MESSAGE_INPUT_BOMBER_CODE, 0 } },
    { "PauseMenu", { MESSAGE_PAUSE_MENU, 0 } },
    { "TimeSpeed", { MESSAGE_TIME_SPEED, 0 } },
    { "OwlWarp", { MESSAGE_OWL_WARP, 0 } },
    { "InputLotteryCode", { MESSAGE_INPUT_LOTTERY_CODE, 0 } },
    { "SpiderHouseMaskCode", { MESSAGE_SPIDER_HOUSE_MASK_CODE, 0 } },
    { "StrayFairiesLeftWoodfall", { MESSAGE_STRAY_FAIRIES_LEFT_WOODFALL, 0 } },
    { "StrayFairiesLeftSnowhead", { MESSAGE_STRAY_FAIRIES_LEFT_SNOWHEAD, 0 } },
    { "StrayFairiesLeftGreatBay", { MESSAGE_STRAY_FAIRIES_LEFT_GREAT_BAY, 0 } },
    { "StrayFairiesLeftStoneTower", { MESSAGE_STRAY_FAIRIES_LEFT_STONE_TOWER, 0 } },
    { "PointsBoatArchery", { MESSAGE_POINTS_BOAT_ARCHERY, 0 } },
    { "LotteryCode", { MESSAGE_LOTTERY_CODE, 0 } },
    { "LotteryCodeGuess", { MESSAGE_LOTTERY_CODE_GUESS, 0 } },
    { "HeldItemPrice", { MESSAGE_HELD_ITEM_PRICE, 0 } },
    { "BomberCode", { MESSAGE_BOMBER_CODE, 0 } },
    { "Event2", { MESSAGE_EVENT2, 0 } },
    { "SpiderHouseMaskCode1", { MESSAGE_SPIDER_HOUSE_MASK_CODE_1, 0 } },
    { "SpiderHouseMaskCode2", { MESSAGE_SPIDER_HOUSE_MASK_CODE_2, 0 } },
    { "SpiderHouseMaskCode3", { MESSAGE_SPIDER_HOUSE_MASK_CODE_3, 0 } },
    { "SpiderHouseMaskCode4", { MESSAGE_SPIDER_HOUSE_MASK_CODE_4, 0 } },
    { "SpiderHouseMaskCode5", { MESSAGE_SPIDER_HOUSE_MASK_CODE_5, 0 } },
    { "SpiderHouseMaskCode6", { MESSAGE_SPIDER_HOUSE_MASK_CODE_6, 0 } },
    { "HoursUntilMoonCrash", { MESSAGE_HOURS_UNTIL_MOON_CRASH, 0 } },
    { "TimeUntilNewDay", { MESSAGE_TIME_UNTIL_NEW_DAY, 0 } },
    { "HighScorePointsBankRupees", { MESSAGE_HS_POINTS_BANK_RUPEES, 0 } },
    { "HighScorePointsUnknown1", { MESSAGE_HS_POINTS_UNK_1, 0 } },
    { "HighScorePointsFishing", { MESSAGE_HS_POINTS_FISHING, 0 } },
    { "HighScoreTimeBoatArchery", { MESSAGE_HS_TIME_BOAT_ARCHERY, 0 } },
    { "HighScoreTimeHorseBackBalloon", { MESSAGE_HS_TIME_HORSE_BACK_BALLOON, 0 } },
    { "HighScoreTimeLotteryGuess", { MESSAGE_HS_TIME_LOTTERY_GUESS, 0 } },
    { "HighScoreTownShootingGallery", { MESSAGE_HS_TOWN_SHOOTING_GALLERY, 0 } },
    { "HighScoreUnknown1", { MESSAGE_HS_UNK_1, 0 } },
    { "HighScoreUnknown3Lower", { MESSAGE_HS_UNK_3_LOWER, 0 } },
    { "HighScoreHorseBackBalloon", { MESSAGE_HS_HORSE_BACK_BALLOON, 0 } },
    { "HighScoreDekuPlaygroundDay1", { MESSAGE_HS_DEKU_PLAYGROUND_DAY_1, 0 } },
    { "HighScoreDekuPlaygroundDay2", { MESSAGE_HS_DEKU_PLAYGROUND_DAY_2, 0 } },
    { "HighScoreDekuPlaygroundDay3", { MESSAGE_HS_DEKU_PLAYGROUND_DAY_3, 0 } },
    { "DekuPlaygroundNameDay1", { MESSAGE_DEKU_PLAYGROUND_NAME_DAY_1, 0 } },
    { "DekuPlaygroundNameDay2", { MESSAGE_DEKU_PLAYGROUND_NAME_DAY_2, 0 } },
    { "DekuPlaygroundNameDay3", { MESSAGE_DEKU_PLAYGROUND_NAME_DAY_3, 0 } },
};

static const std::unordered_map<std::string, uint8_t> sTextColors = {
    { "Default", MESSAGE_COLOR_DEFAULT }, { "Red", MESSAGE_COLOR_RED },       { "Green", MESSAGE_COLOR_GREEN },
    { "Blue", MESSAGE_COLOR_BLUE },       { "Yellow", MESSAGE_COLOR_YELLOW }, { "LightBlue", MESSAGE_COLOR_LIGHTBLUE },
    { "Pink", MESSAGE_COLOR_PINK },       { "Silver", MESSAGE_COLOR_SILVER }, { "Orange", MESSAGE_COLOR_ORANGE },
};

static const std::unordered_map<std::string, uint8_t> sTextButtons = {
    { "A", MESSAGE_BTN_A },           { "B", MESSAGE_BTN_B },          { "C", MESSAGE_BTN_C },
    { "L", MESSAGE_BTN_L },           { "R", MESSAGE_BTN_R },          { "Z", MESSAGE_BTN_Z },
    { "CUp", MESSAGE_BTN_CUP },       { "CDown", MESSAGE_BTN_CDOWN },  { "CLeft", MESSAGE_BTN_CLEFT },
    { "CRight", MESSAGE_BTN_CRIGHT }, { "ZTarget", MESSAGE_Z_TARGET }, { "ControlStick", MESSAGE_CONTROL_PAD },
};

static const std::unordered_map<std::string, uint32_t> sBoxTypes = {
    { "Black", TEXTBOX_TYPE_BLACK },
    { "Sign", TEXTBOX_TYPE_WOODEN },
    { "Blue", TEXTBOX_TYPE_BLUE_FADED },
    { "Ocarina", TEXTBOX_TYPE_OCARINA },
    { "NoneBottom", TEXTBOX_TYPE_4 },
    { "NoneDark", TEXTBOX_TYPE_CLEAR },
    { "BlackInstant", TEXTBOX_TYPE_DISPLAY_ALL },
    { "None", TEXTBOX_TYPE_CLEAR_DISPLAY_ALL },
    { "BlueInstant", TEXTBOX_TYPE_BLUE },
    { "RedInstant", TEXTBOX_TYPE_PAUSE_INFO },
    { "Banner", TEXTBOX_TYPE_A },
    { "NoneInstant", TEXTBOX_TYPE_B },
    { "Notebook", TEXTBOX_TYPE_NOTEBOOK_NOTIFICATION },
};

static const std::unordered_map<std::string, uint32_t> sBoxPositions = {
    { "Auto", 0 }, { "Top", 1 }, { "Middle", 2 }, { "Bottom", 3 }, { "LowFixed", 7 },
};

static const std::unordered_map<std::string, uint32_t> sBoxSkips = {
    { "Normal", 0 },
    { "Unskippable", 1 },
    { "FadeInOut", 3 },
};

// Font characters from 0x7F on
static const std::vector<std::string> sExtendedChars = {
    "º", "À", "Á", "Â", "Ä", "Ç", "È", "É", "Ê", "Ë", "Ì", "Í", "Î", "Ï", "Ñ", "Ò", "Ó",
    "Ô", "Ö", "Ù", "Ú", "Û", "Ü", "ß", "à", "á", "â", "ä", "ç", "è", "é", "ê", "ë", "ì",
    "í", "î", "ï", "ñ", "ò", "ó", "ô", "ö", "ù", "ú", "û", "ü", "¡", "¿", "ª",
};

static const char* ValueOrEmpty(const char* value) {
    return value != nullptr ? value : "";
}

static std::optional<uint32_t> ParseNumber(const char* text, uint32_t max) {
    unsigned value;
    if (text == nullptr || !tinyxml2::XMLUtil::ToUnsigned(text, &value) || value > max) {
        return std::nullopt;
    }

    return value;
}

static uint16_t ReadHeaderField(const tinyxml2::XMLElement* element, const char* attribute, uint16_t fallback,
                                uint32_t max, const std::string& logContext) {
    const char* text = element->Attribute(attribute);
    if (text == nullptr) {
        return fallback;
    }

    auto value = ParseNumber(text, max);
    if (value.has_value()) {
        return *value;
    }

    SPDLOG_WARN("{}: {} \"{}\" is not a number up to {}", logContext, attribute, text, max);
    return fallback;
}

static uint8_t ReadBoxField(const tinyxml2::XMLElement* element, const char* attribute,
                            const std::unordered_map<std::string, uint32_t>& names, const std::string& logContext) {
    const char* text = element->Attribute(attribute);
    if (text == nullptr) {
        return 0;
    }

    auto name = names.find(text);
    if (name != names.end()) {
        return name->second;
    }

    auto value = ParseNumber(text, 0xF);
    if (value.has_value()) {
        return *value;
    }

    SPDLOG_WARN("{}: {} \"{}\" is not a name or a number up to 15", logContext, attribute, text);
    return 0;
}

static void ReadBox(const tinyxml2::XMLElement* element, MessageEntryMM& entry, const std::string& logContext) {
    uint8_t type = ReadBoxField(element, "Type", sBoxTypes, logContext);
    uint8_t position = ReadBoxField(element, "Position", sBoxPositions, logContext);
    uint8_t skip = ReadBoxField(element, "Skip", sBoxSkips, logContext);

    entry.textboxType = type | (element->BoolAttribute("Centered") ? 0x10 : 0);
    entry.textboxYPos = (position << 4) | skip;
}

static std::vector<std::string> SplitLines(const std::string& text) {
    std::vector<std::string> lines;
    size_t start = 0;
    size_t end;

    while ((end = text.find('\n', start)) != std::string::npos) {
        lines.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    lines.push_back(text.substr(start));

    return lines;
}

// Like JSX, spaces next to a line break are removed and the remaining lines are joined with one space
static std::string CollapseWhitespace(const std::string& text) {
    std::vector<std::string> lines = SplitLines(text);
    std::string out;

    for (size_t i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        std::erase(line, '\t');
        if (i > 0) {
            line.erase(0, line.find_first_not_of(' '));
        }
        if (i < lines.size() - 1) {
            line.erase(line.find_last_not_of(' ') + 1);
        }

        if (line.empty()) {
            continue;
        }
        if (!out.empty()) {
            out += ' ';
        }
        out += line;
    }

    return out;
}

static size_t Utf8CharLength(uint8_t firstByte) {
    if (firstByte >= 0xF0) {
        return 4;
    }
    if (firstByte >= 0xE0) {
        return 3;
    }
    if (firstByte >= 0xC0) {
        return 2;
    }
    return 1;
}

static std::optional<uint8_t> FontByte(const std::string& c) {
    // The font draws 0x5C as a yen sign
    if (c == "¥") {
        return 0x5C;
    }
    if (c.size() == 1 && c[0] >= ' ' && c[0] <= '~' && c[0] != '\\') {
        return c[0];
    }

    auto extended = std::find(sExtendedChars.begin(), sExtendedChars.end(), c);
    if (extended != sExtendedChars.end()) {
        return 0x7F + (extended - sExtendedChars.begin());
    }

    return std::nullopt;
}

static bool HasContent(const tinyxml2::XMLNode* node) {
    for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
        if (child->ToText() != nullptr || child->ToElement() != nullptr) {
            return true;
        }
    }
    return false;
}

class MessageWriter {
  public:
    MessageWriter(const std::string& logContext) : mLogContext(logContext) {
    }

    std::string Write(const tinyxml2::XMLElement* element) {
        WriteContent(element);
        if (!mEnded) {
            Push(MESSAGE_END);
        }
        return mMsg;
    }

  private:
    std::string mLogContext;
    std::string mMsg;
    uint8_t mColor = MESSAGE_COLOR_DEFAULT;
    // Track the End command separately; control arguments can also contain 0xBF.
    bool mEnded = false;

    void Push(uint8_t byte) {
        mMsg += (char)byte;
        mEnded = false;
    }

    void WriteContent(const tinyxml2::XMLNode* node) {
        for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
            if (child->ToText() != nullptr) {
                WriteText(child->Value());
            } else if (child->ToElement() != nullptr) {
                WriteTag(child->ToElement());
            }
        }
    }

    void WriteText(const std::string& text) {
        std::string collapsed = CollapseWhitespace(text);
        size_t pos = 0;

        while (pos < collapsed.size()) {
            std::string c = collapsed.substr(pos, Utf8CharLength(collapsed[pos]));
            pos += c.size();

            auto byte = FontByte(c);
            if (byte.has_value()) {
                Push(*byte);
            } else {
                SPDLOG_WARN("{}: \"{}\" is not in the font", mLogContext, c);
            }
        }
    }

    void WriteTag(const tinyxml2::XMLElement* element) {
        std::string name = element->Name();
        auto control = sTextControls.find(name);

        if (name == "Color") {
            WriteColor(element);
            return;
        }

        if (name == "QuickText") {
            Push(MESSAGE_QUICKTEXT_ENABLE);
            WriteContent(element);
            Push(MESSAGE_QUICKTEXT_DISABLE);
            return;
        }

        if (name == "Button") {
            WriteButton(element);
        } else if (control != sTextControls.end()) {
            WriteControl(element, control->second);
        } else {
            SPDLOG_WARN("{}: unknown tag <{}>", mLogContext, name);
            WriteContent(element);
            return;
        }

        if (HasContent(element)) {
            SPDLOG_WARN("{}: <{}> can't wrap text, so its text is written after it", mLogContext, name);
            WriteContent(element);
        }
    }

    void WriteColor(const tinyxml2::XMLElement* element) {
        const char* value = ValueOrEmpty(element->Attribute("Value"));
        auto color = sTextColors.find(value);

        if (color == sTextColors.end()) {
            SPDLOG_WARN("{}: <Color> Value \"{}\" is not a color", mLogContext, value);
            WriteContent(element);
            return;
        }

        uint8_t previousColor = mColor;
        mColor = color->second;
        Push(mColor);
        if (!HasContent(element)) {
            return;
        }

        // Closing a wrap goes back to the color around it
        WriteContent(element);
        mColor = previousColor;
        Push(mColor);
    }

    void WriteButton(const tinyxml2::XMLElement* element) {
        const char* value = ValueOrEmpty(element->Attribute("Value"));
        auto button = sTextButtons.find(value);

        if (button == sTextButtons.end()) {
            SPDLOG_WARN("{}: <Button> Value \"{}\" is not a button", mLogContext, value);
            return;
        }
        Push(button->second);
    }

    void WriteControl(const tinyxml2::XMLElement* element, TextControl control) {
        if (control.valueSize == 0) {
            Push(control.code);
            mEnded = control.code == MESSAGE_END;
            return;
        }

        const char* value = element->Attribute("Value");
        uint32_t max = control.valueSize == 2 ? 0xFFFF : 0xFF;
        auto number = ParseNumber(value, max);
        if (!number.has_value()) {
            SPDLOG_WARN("{}: <{}> Value \"{}\" is not a number up to {}", mLogContext, element->Name(),
                        ValueOrEmpty(value), max);
            return;
        }

        Push(control.code);
        if (control.valueSize == 2) {
            Push(*number >> 8);
        }
        Push(*number & 0xFF);
    }
};

static MessageEntryMM ReadEntry(const tinyxml2::XMLElement* element, uint16_t id, const std::string& logContext) {
    MessageEntryMM entry;
    entry.id = id;

    ReadBox(element, entry, logContext);
    entry.icon = ReadHeaderField(element, "Icon", 0xFE, 0xFF, logContext);
    entry.nextMessageID = ReadHeaderField(element, "Next", 0xFFFF, 0xFFFF, logContext);
    entry.firstItemCost = ReadHeaderField(element, "FirstPrice", 0xFFFF, 0xFFFF, logContext);
    entry.secondItemCost = ReadHeaderField(element, "SecondPrice", 0xFFFF, 0xFFFF, logContext);
    entry.msg = MessageWriter(logContext).Write(element);

    return entry;
}

static std::optional<MessageEntryMM> EncodeTextEntry(const tinyxml2::XMLElement* element, const std::string& path) {
    auto id = ParseNumber(element->Attribute("ID"), 0xFFFF);
    if (!id.has_value()) {
        SPDLOG_WARN("TextMM XML {}: a <TextEntry> has no valid ID", path);
        return std::nullopt;
    }

    std::string logContext = fmt::format("TextMM XML {} 0x{:04X}", path, *id);
    MessageEntryMM entry = ReadEntry(element, *id, logContext);
    if (entry.msg.size() > TextMM::MaxMessageSize) {
        SPDLOG_WARN("{}: message is {} bytes, over the {} byte limit", logContext, entry.msg.size(),
                    TextMM::MaxMessageSize);
        return std::nullopt;
    }

    return entry;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLTextMMV1::ReadResource(std::shared_ptr<Ship::File> file,
                                         std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto text = std::make_shared<TextMM>(initData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();
    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            auto entry = EncodeTextEntry(child, initData->Path);
            if (entry.has_value()) {
                text->messages.push_back(*entry);
            }
        }

        child = child->NextSiblingElement();
    }

    return text;
}
} // namespace SOH
