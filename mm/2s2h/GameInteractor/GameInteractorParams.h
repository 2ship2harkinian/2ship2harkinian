#ifndef GAME_INTERACTOR_PARAMS_H
#define GAME_INTERACTOR_PARAMS_H

#ifdef __cplusplus

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// Passing a double (`1.5`) is a compile error because it narrows -- write `1.5f`.
using GIParamValue = std::variant<bool, int32_t, float, std::string>;

// Must stay in the same order as GIParamValue's alternatives; GIParamTypeOf() relies on it.
typedef enum {
    GI_PARAM_BOOL,
    GI_PARAM_INT,
    GI_PARAM_FLOAT,
    GI_PARAM_STRING,
} GIParamType;

const char* GIParamTypeName(GIParamType type);

inline GIParamType GIParamTypeOf(const GIParamValue& value) {
    return static_cast<GIParamType>(value.index());
}

class GIParams {
  public:
    GIParams() = default;
    GIParams(std::initializer_list<std::pair<const std::string, GIParamValue>> init) : values(init) {
    }

    void Set(std::string key, GIParamValue value) {
        values.insert_or_assign(std::move(key), std::move(value));
    }
    const GIParamValue* Find(const std::string& key) const {
        auto it = values.find(key);
        return it == values.end() ? nullptr : &it->second;
    }

    // Guaranteed to succeed for every key the schema declares, once the params have been validated.
    bool Bool(const std::string& key) const;
    int32_t Int(const std::string& key) const;
    float Float(const std::string& key) const;
    const std::string& String(const std::string& key) const;

  private:
    std::unordered_map<std::string, GIParamValue> values;
};

struct GIParamSpec {
    const char* name;
    GIParamType type;
    bool required = false;
    // Used when the param is absent and not required. Defaults to the zero value for `type`.
    std::optional<GIParamValue> defaultValue = std::nullopt;
    // Inclusive bounds for INT/FLOAT params.
    std::optional<float> min = std::nullopt;
    std::optional<float> max = std::nullopt;
};

// Fills in defaults, widens ints where the schema wants a float, and range-checks. Returns a
// description of the first problem, or nullopt if `params` is now safe to hand to callbacks.
std::optional<std::string> GIParamsValidate(const std::vector<GIParamSpec>& schema, GIParams& params);

#endif // __cplusplus

#endif // GAME_INTERACTOR_PARAMS_H
