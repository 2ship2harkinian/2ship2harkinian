#ifndef S2H_MOD_API_H
#define S2H_MOD_API_H

#include <stdarg.h>
#include <stdint.h>

#include "2s2h/GameInteractor/GameInteractor.h"

#ifdef _WIN32
#define S2H_MOD_EXPORT __declspec(dllexport)
#else
#define S2H_MOD_EXPORT __attribute__((visibility("default")))
#endif

#define DEFINE_HOOK(name, args) typedef void (*S2HCb_##name) args;
#include "2s2h/GameInteractor/GameInteractor_HookTable.h"
#undef DEFINE_HOOK

typedef void (*S2HVbCallback)(bool* should, va_list args);

typedef struct {
    uint32_t tableSize;

    bool (*RegisterHookByName)(const char* name, void* callback);
    bool (*RegisterVB)(GIVanillaBehavior flag, S2HVbCallback callback);
    void* (*GetSymbol)(const char* name);
} S2HModApi;

#define S2H_REGISTER_HOOK(api, name, callback) \
    ((api)->RegisterHookByName(#name, (void*)(S2HCb_##name)(callback)))

#define S2H_BIND(api, symbol) (*(void**)&(symbol) = (api)->GetSymbol(#symbol))

#define S2H_MOD_API_MATCHES(api) ((api) != NULL && (api)->tableSize == sizeof(S2HModApi))

typedef void (*S2HModSetApiFunc)(const S2HModApi* api);
typedef void (*S2HModInitFunc)(void);

#ifdef __cplusplus
const S2HModApi* ModApi_Get();
#endif

#endif // S2H_MOD_API_H
