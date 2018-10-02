#ifndef NOTIFICATION_SERVICE_H_
#define NOTIFICATION_SERVICE_H_
#include <Arduino.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <time.h>
#include "iPhoneApplicationList.h"


struct notification_def {
    std::string title;
    std::string message;
    time_t time;
    iphone_application_def type;
    uint32_t uuid = 0;
    boolean showed = false;
    boolean isComplete = false;
};

class NotificationService{
    public:
        void addPendingNotification(uint32_t uuid);
        uint32_t getNextPendingNotification();
        void addNotification(uint32_t uuid, notification_def nofitication, bool isCalling);
        void removeNotification(uint32_t uuid);
        void removeCallNotification();
        bool isNotificationInit(uint32_t uuid);
        bool isCallingNotification();
        notification_def *getCallingNotification();
        notification_def *getNotification(uint32_t uuid);
        std::map<uint32_t, notification_def> *getNotificationList();
        
    private:
        static const int notificationListSize = 4;
        std::map<uint32_t, notification_def> notificationList;
        std::stack<uint32_t> pendingNotification;
        notification_def callingNotification;
};
#endif