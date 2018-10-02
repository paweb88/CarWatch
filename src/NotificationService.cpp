#include "NotificationService.h"
#include <type_traits>

void NotificationService::addPendingNotification(uint32_t uuid)
{
    pendingNotification.push(uuid);
}

uint32_t NotificationService::getNextPendingNotification()
{
    if (pendingNotification.size() > 0)
    {
        uint32_t uuid = pendingNotification.top();
        pendingNotification.pop();
        return uuid;
    }
    else
    {
        return 0;
    }
};

void NotificationService::addNotification(uint32_t uuid, notification_def notification, bool isCalling)
{
    notification.uuid = uuid;
    if (isCalling)
    {
        callingNotification = notification;
    }
    else
    {
        if (notificationList.size() >= notificationListSize)
        {
            std::map<uint32_t, notification_def>::iterator it = notificationList.begin();
            notificationList.erase(it);
        }
        notificationList.insert(std::pair<uint32_t, notification_def>(uuid, notification));
    }
};

std::map<uint32_t, notification_def> *NotificationService::getNotificationList()
{
    return &notificationList;
    /*
    std::vector<notification_def*> result;
    std::map<uint32_t, notification_def>::iterator it = notificationList.begin();
    for (;it != notificationList.end(); ++it)
    {
        result.push_back(&it->second);
    }
    return result;
    */
}

bool NotificationService::isNotificationInit(uint32_t uuid)
{
    std::map<uint32_t, notification_def>::iterator it = notificationList.find(uuid);
    return (it != notificationList.end()) | (callingNotification.uuid != 0);
}
notification_def *NotificationService::getNotification(uint32_t uuid)
{
    if (callingNotification.uuid == uuid)
    {
        return &callingNotification;
    }
    std::map<uint32_t, notification_def>::iterator it = notificationList.find(uuid);
    return (it != notificationList.end()) ? &it->second : new notification_def();
}

void NotificationService::removeCallNotification()
{
    callingNotification.uuid = 0;
}

bool NotificationService::isCallingNotification(){
    return (callingNotification.uuid != 0);
}

notification_def *NotificationService::getCallingNotification() {
    return &callingNotification;
}

void NotificationService::removeNotification(uint32_t uuid) {
    std::map<uint32_t, notification_def>::iterator it = notificationList.find(uuid);
    if (it != notificationList.end()) notificationList.erase(it);
}