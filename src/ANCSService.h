#ifndef ANCSSERVICE_H_
#define ANCSSERVICE_H_
#include <Task.h>
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <esp_bt_main.h>
#include "Security.h"
#include "ANCS.h"

#define tskNO_AFFINITY INT_MAX
#define LOG_TAG "BLEService"

typedef void (*NotificationCallback)(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

class ANCSServiceServerCallback {
    public:
        virtual void onConnect();
        virtual void onDisconnect();
};

class ANCSServiceClientCallback {
    public:
        virtual void onConnect();
        virtual void onDisconnect();
};

class ANCSService {
    public:
        ANCSService(NotificationCallback notificationSourceCallback, NotificationCallback dataSourceCallback) : _notificationSourceCallback(notificationSourceCallback), _dataSourceCallback(dataSourceCallback) {};
        //~BLEService();
        void startServer(std::string appName, uint16_t visibleTime);
        void retriveNotificationData(uint32_t notifyUUID);
        void setServerCallback(ANCSServiceServerCallback *serverCallback);
        void setClientCallback(ANCSServiceClientCallback *clientCallback);
    private:
        const BLEUUID ancsServiceUUID = BLEUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0");
        const BLEUUID notificationSourceCharacteristicUUID = BLEUUID("9FBF120D-6301-42D9-8C58-25E699A21DBD");
        const BLEUUID controlPointCharacteristicUUID =  BLEUUID("69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9");
        const BLEUUID dataSourceCharacteristicUUID =  BLEUUID("22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB");
        BLERemoteCharacteristic *_controlPointCharacteristic, *_notificationSourceCharacteristic, *_dataSourceCharacteristic;
        NotificationCallback _notificationSourceCallback, _dataSourceCallback;
        ANCSServiceServerCallback *_serverCallback = nullptr;
        ANCSServiceClientCallback *_clientCallback = nullptr;
        xTaskHandle clientTaskHandle;

        static void startClient(void *data);
        class ServerCallback : public BLEServerCallbacks {
            friend class ANCSService;
            public:
                ServerCallback(ANCSService *ancsService) : ancsService(ancsService) {};
                void onConnect(BLEServer *pServer);
                void onDisconnect(BLEServer *pServer);
            private:
                ANCSService *ancsService;
        };
        class ClientCallback : public BLEClientCallbacks {
            friend class ANCSService;
            public:
                ClientCallback(ANCSService *ancsService) : ancsService(ancsService) {};
                void onConnect(BLEClient *pClient);
                void onDisconnect(BLEClient *pClient);
            private:
                ANCSService *ancsService;
        };
        struct clientParameter {
            BLEAddress *bleAddress;
            ANCSService *ancsService;
            clientParameter(BLEAddress *_bleAddress, ANCSService *_ancsserive) : bleAddress(_bleAddress), ancsService(_ancsserive) {};
        };
};

#endif