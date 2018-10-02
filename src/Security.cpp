#include "Security.h"

uint32_t Security::onPassKeyRequest()
{
    ESP_LOGI(LOG_TAG, "PassKeyRequest");
    return 123456;
}

void Security::onPassKeyNotify(uint32_t pass_key)
{
    ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
}

bool Security::onSecurityRequest()
{
    ESP_LOGI(LOG_TAG, "On Security Request");
    return true;
}

bool Security::onConfirmPIN(uint32_t pin)
{
    ESP_LOGI(LOG_TAG, "On Confrimed Pin Request");
    return true;
}

void Security::onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
{
    ESP_LOGI(LOG_TAG, "Starting BLE work!");
    if (cmpl.success)
    {
        uint16_t length;
        esp_ble_gap_get_whitelist_size(&length);
        ESP_LOGD(LOG_TAG, "size: %d", length);
    }
}