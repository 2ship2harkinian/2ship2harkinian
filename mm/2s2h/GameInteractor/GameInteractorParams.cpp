#include "GameInteractorParams.h"

const char* GIParamTypeName(GIParamType type) {
    switch (type) {
        case GI_PARAM_BOOL:
            return "bool";
        case GI_PARAM_INT:
            return "int";
        case GI_PARAM_FLOAT:
            return "float";
        case GI_PARAM_STRING:
            return "string";
    }
    return "unknown";
}

bool GIParams::Bool(const std::string& key) const {
    auto it = values.find(key);
    return it == values.end() ? false : std::get<bool>(it->second);
}

int32_t GIParams::Int(const std::string& key) const {
    auto it = values.find(key);
    return it == values.end() ? 0 : std::get<int32_t>(it->second);
}

float GIParams::Float(const std::string& key) const {
    auto it = values.find(key);
    return it == values.end() ? 0.0f : std::get<float>(it->second);
}

const std::string& GIParams::String(const std::string& key) const {
    static const std::string empty;
    auto it = values.find(key);
    return it == values.end() ? empty : std::get<std::string>(it->second);
}

static GIParamValue ZeroValueFor(GIParamType type) {
    switch (type) {
        case GI_PARAM_BOOL:
            return false;
        case GI_PARAM_INT:
            return static_cast<int32_t>(0);
        case GI_PARAM_FLOAT:
            return 0.0f;
        case GI_PARAM_STRING:
            break;
    }
    return std::string();
}

std::optional<std::string> GIParamsValidate(const std::vector<GIParamSpec>& schema, GIParams& params) {
    for (const auto& spec : schema) {
        const GIParamValue* value = params.Find(spec.name);

        if (value == nullptr) {
            if (spec.required) {
                return std::string("missing required param '") + spec.name + "'";
            }
            params.Set(spec.name, spec.defaultValue.value_or(ZeroValueFor(spec.type)));
            value = params.Find(spec.name);
        }

        if (GIParamTypeOf(*value) != spec.type) {
            // A remote sending {"scale": 3} means 3.0f, not a type error.
            if (spec.type == GI_PARAM_FLOAT && GIParamTypeOf(*value) == GI_PARAM_INT) {
                params.Set(spec.name, static_cast<float>(std::get<int32_t>(*value)));
            } else {
                return std::string("param '") + spec.name + "' expects " + GIParamTypeName(spec.type) + ", got " +
                       GIParamTypeName(GIParamTypeOf(*value));
            }
        }

        if (spec.min.has_value() || spec.max.has_value()) {
            float number =
                spec.type == GI_PARAM_FLOAT ? params.Float(spec.name) : static_cast<float>(params.Int(spec.name));
            if ((spec.min.has_value() && number < *spec.min) || (spec.max.has_value() && number > *spec.max)) {
                return std::string("param '") + spec.name + "' is out of range";
            }
        }
    }

    return std::nullopt;
}
