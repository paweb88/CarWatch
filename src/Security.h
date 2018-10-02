#ifndef SECURITY_CALLBACK_H_
#define SECURITY_CALLBACK_H_
#include <Arduino.h>
#include "BLESecurity.h"

class Security : public BLESecurityCallbacks {
    public:
        uint32_t onPassKeyRequest();
        void onPassKeyNotify(uint32_t pass_key);
        bool onSecurityRequest();
        bool onConfirmPIN(uint32_t pin);
        void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl);
};
#endif