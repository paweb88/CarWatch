#include "ANCSService.h"

void ANCSService::ServerCallback::onConnect(BLEServer *pServer)
{
    ESP_LOGI(ancsService.LOG_TAG, "\n**Device  connected %s**\n", BLEAddress(BLEDevice::m_remoteBda).toString().c_str());
    if (ancsService->clientTaskHandle != nullptr) {
        ::xTaskCreatePinnedToCore(&ANCSService::startClient, "ClientTask", 10000, new clientParameter(new BLEAddress(BLEDevice::m_remoteBda), ancsService), 5, &ancsService->clientTaskHandle, 0);
    }
    if (ancsService->_serverCallback != nullptr) {
        ancsService->_serverCallback->onConnect();
    }
};

void ANCSService::ServerCallback::onDisconnect(BLEServer *pServer)
{
    ::vTaskDelete(ancsService->clientTaskHandle);
    ancsService->clientTaskHandle = nullptr;
    ESP_LOGI(ancsService.LOG_TAG, "\n**Device  disconnected**\n");
        if (ancsService->_serverCallback != nullptr) {
        ancsService->_serverCallback->onDisconnect();
    }
};

void ANCSService::ClientCallback::onConnect(BLEClient *pClient)
{
    ESP_LOGI(ancsService.LOG_TAG, "\n**Device client connected **\n");
    if (ancsService->_clientCallback != nullptr) {
        ancsService->_clientCallback->onConnect();
    }
};

void ANCSService::ClientCallback::onDisconnect(BLEClient *pClient)
{
    ESP_LOGI(ancsService.LOG_TAG, "\n**Device Client disconnected**\n");
    if (ancsService->_clientCallback != nullptr) {
        ancsService->_clientCallback->onDisconnect();
    }
};

void ANCSService::startServer(std::string appName, uint16_t visibleTime)
{
    ESP_LOGD(LOG_TAG, "Starting BLE work!");
    esp_log_buffer_char(LOG_TAG, LOG_TAG, sizeof(LOG_TAG));
    esp_log_buffer_hex(LOG_TAG, LOG_TAG, sizeof(LOG_TAG));

    // Initialize device
    BLEDevice::init(appName);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallback(this));
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new Security());

    // Advertising parameters:
    // Soliciting ANCS
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    oAdvertisementData.setFlags(0x01);
    oAdvertisementData.setServiceSolicitation(BLEUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0"));
    pAdvertising->setAdvertisementData(oAdvertisementData);

    // Set security
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    //Start advertising
    pAdvertising->start();
};

void ANCSService::startClient(void *data)
{
    ESP_LOGI(ancsService.LOG_TAG, "\n**Client start**\n");
    BLEClient *pClient = BLEDevice::createClient();
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new Security());

    clientParameter *clientParam = (clientParameter *)data;
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_IO);
    pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pClient->setClientCallbacks(new ClientCallback(clientParam->ancsService));
    pClient->connect(*clientParam->bleAddress);

    BLERemoteService *pAncsService = pClient->getService(BLEUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0"));
    if (pAncsService == nullptr)
    {
        ESP_LOGD(LOG_TAG, "Failed to find our service UUID: %s", ancsServiceUUID.toString().c_str());
        return;
    }
    clientParam->ancsService->_notificationSourceCharacteristic = pAncsService->getCharacteristic(clientParam->ancsService->notificationSourceCharacteristicUUID);
    if (clientParam->ancsService->_notificationSourceCharacteristic == nullptr)
    {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", notificationSourceCharacteristicUUID.toString().c_str());
        return;
    }
    clientParam->ancsService->_controlPointCharacteristic = pAncsService->getCharacteristic(clientParam->ancsService->controlPointCharacteristicUUID);
    if (clientParam->ancsService->_controlPointCharacteristic == nullptr)
    {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", controlPointCharacteristicUUID.toString().c_str());
        return;
    }
    clientParam->ancsService->_dataSourceCharacteristic = pAncsService->getCharacteristic(clientParam->ancsService->dataSourceCharacteristicUUID);
    if (clientParam->ancsService->_dataSourceCharacteristic == nullptr)
    {
        ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", dataSourceCharacteristicUUID.toString().c_str());
        return;
    }
    const uint8_t v[] = {0x1, 0x0};
    clientParam->ancsService->_notificationSourceCharacteristic->registerForNotify(clientParam->ancsService->_notificationSourceCallback);
    clientParam->ancsService->_notificationSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)v, 2, true);
    clientParam->ancsService->_dataSourceCharacteristic->registerForNotify(clientParam->ancsService->_dataSourceCallback);
    clientParam->ancsService->_dataSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)v, 2, true);
    while (1)
    {
        delay(1000);
    }
}

void ANCSService::retriveNotificationData(uint32_t notifyUUID) {
        uint8_t uuid[4];
        uuid[0] = notifyUUID;
        uuid[1] = notifyUUID >> 8;
        uuid[2] = notifyUUID >> 16;
        uuid[3] = notifyUUID >> 24;
        const uint8_t vIdentifier[] = {0x0,uuid[0],uuid[1],uuid[2],uuid[3], ANCS::NotificationAttributeIDAppIdentifier};
        _controlPointCharacteristic->writeValue((uint8_t *)vIdentifier, 6, true);
        const uint8_t vTitle[] = {0x0,uuid[0],uuid[1],uuid[2],uuid[3], ANCS::NotificationAttributeIDTitle, 0x0, 0x10};
        _controlPointCharacteristic->writeValue((uint8_t *)vTitle, 8, true);
        const uint8_t vMessage[] = {0x0,uuid[0],uuid[1],uuid[2],uuid[3], ANCS::NotificationAttributeIDMessage, 0x0,0x10};
        _controlPointCharacteristic->writeValue((uint8_t *)vMessage, 8, true);
        const uint8_t vDate[] = {0x0,uuid[0],uuid[1],uuid[2],uuid[3], ANCS::NotificationAttributeIDDate};
        _controlPointCharacteristic->writeValue((uint8_t *)vDate, 6, true);
}

void ANCSService::setServerCallback(ANCSServiceServerCallback *serverCallback) {
    _serverCallback = serverCallback;
};

void ANCSService::setClientCallback(ANCSServiceClientCallback *clientCallback) {
    _clientCallback = clientCallback;
};