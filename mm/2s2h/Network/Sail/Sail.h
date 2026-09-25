#ifndef NETWORK_SAIL_H
#define NETWORK_SAIL_H
#ifdef __cplusplus

#include "2s2h/Network/Network.h"

class Sail : public Network {
  private:
    void OnIncomingJson(nlohmann::json payload);

  public:
    static Sail* Instance;

    bool Enable();
    void Disable();
};

#endif // __cplusplus
#endif // NETWORK_SAIL_H
