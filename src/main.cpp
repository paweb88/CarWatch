#include <Arduino.h>
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <esp_bt_main.h>
#include <string>
#include <Task.h>
#include <sys/time.h>
#include <time.h>
#include "sdkconfig.h"
#include "Security.h"
#include "NotificationService.h"
#include "ANCS.h"
#include "TFTDisplay.h"
#include "ANCSService.h"
#include "Task.h"

#define NOTIFICATION_LIST_SIZE 8

NotificationService *notificationService;
iPhoneApplicationList *iphoneApplication;
ANCSService *ancsService;
TFTDisplay *display;

static void dataSourceNotifyCallback(
    BLERemoteCharacteristic *pDataSourceCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    std::string message;

    uint32_t messageId = pData[4];
    messageId = messageId << 8 | pData[3];
    messageId = messageId << 16 | pData[2];
    messageId = messageId << 24 | pData[1];
    bool issetNotification = notificationService->isNotificationInit(messageId);
    for (int i = 8; i < length; i++)
    {
        message += (char)pData[i];
    }
    if (issetNotification == false && pData[5] == ANCS::NotificationAttributeIDAppIdentifier)
    {
        if (iphoneApplication->isAllowedApplication(message))
        {
            iphone_application_def applicationType = iphoneApplication->getApplicationId(message);
            notification_def notification;
            notification.type = applicationType;
            notificationService->addNotification(messageId, notification, (applicationType == APP_PHONE || applicationType == APP_FACETIME));
        }
    }
    if (issetNotification)
    {
        notification_def *notification = notificationService->getNotification(messageId);
        switch (pData[5])
        {
            case 0x1:
                notification->title = message;
                Serial.println(message.c_str());
                break;
            case 0x3:
                Serial.println(message.c_str());
                notification->message = message;
                break;
        }
        if (!notification->title.empty() && !notification->message.empty()) {
            notification->isComplete = true;
        }
    }
};

static void NotificationSourceNotifyCallback(
    BLERemoteCharacteristic *pNotificationSourceCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    uint32_t messageId;
    messageId = pData[7];
    messageId = messageId << 8 | pData[6];
    messageId = messageId << 16 | pData[5];
    messageId = messageId << 24 | pData[4];
    if (pData[0] == ANCS::EventIDNotificationRemoved)
    {
        notification_def *notification = notificationService->getNotification(messageId);
        if (notification->type == APP_PHONE || notification->type == APP_FACETIME)
        {
            //notificationService->addNotification(messageId, *notification, false);
            notificationService->removeCallNotification();
        }
    }
    if (pData[0] == ANCS::EventIDNotificationAdded)
    {
        switch (pData[2])
        {
            //Incoming Call
        case ANCS::CategoryIDIncomingCall:
        {
            notificationService->addPendingNotification(messageId);
            break;
        }
        case ANCS::CategoryIDSocial:
        case ANCS::CategoryIDOther:
        {
            notificationService->addPendingNotification(messageId);
            break;
        }
        }
    }
}

class NotificationDescription : public Task {
    void run(void *data) {
        while(1) {
            uint32_t pendingNotificationId = notificationService->getNextPendingNotification();
            if (pendingNotificationId != 0)
            {
                ancsService->retriveNotificationData(pendingNotificationId);
            }
            delay(500);
        }
    }
};

class MyServerCallback : public ANCSServiceServerCallback {
    public:
        MyServerCallback(TFTDisplay *_tft) : tft(_tft) {};
        void onConnect() {
            tft->setBLEConnectionState(BLE_CONNECTED);
        }
        void onDisconnect() {
            tft->setBLEConnectionState(BLE_DISCONNECTED);
        }
    private:
        TFTDisplay *tft;
};

class MyClientCallback : public ANCSServiceClientCallback {
    public:
        MyClientCallback(TFTDisplay *_tft) : tft(_tft) {};
        void onConnect() {
            tft->setBLEConnectionState(BLE_CONNECTED);
        }
        void onDisconnect() {
            tft->setBLEConnectionState(BLE_DISCONNECTED);
        }
    private:
        TFTDisplay *tft;
};

void setup()
{
    Serial.begin(115200);
    iphoneApplication = new iPhoneApplicationList();
    notificationService = new NotificationService();
    ancsService = new ANCSService(&NotificationSourceNotifyCallback, &dataSourceNotifyCallback);
    display = new TFTDisplay();
    display->init();
    display->mainScreen();
    ancsService->startServer("CarWatch", 30000);
    NotificationDescription *notificationReceiver = new NotificationDescription();
    notificationReceiver->setStackSize(50000);
    notificationReceiver->setName("notificationReceiver");
    notificationReceiver->start();
    ancsService->setServerCallback(new MyServerCallback(display));
    //ancsService->setClientCallback(new MyClientCallback(display));
}

void loop()
{

    //display->getTouch();
    std::map<uint32_t, notification_def>::iterator it = notificationService->getNotificationList()->begin();
    for (; it != notificationService->getNotificationList()->end(); ++it)
    {
        if (notificationService->isCallingNotification())
        {
            break;
        }
        if (it->second.showed == false)
        {
            display->showNotification(&it->second);
            it->second.showed = true;
            delay(5000);
        }
    }
    if (notificationService->isCallingNotification())
    {
        notification_def *callingNotification = notificationService->getCallingNotification();
        if (callingNotification->isComplete == true) {
            display->showNotification(callingNotification);
            delay(1000);
        }
    } else {
        display->mainScreen();
    }
    delay(100);
}