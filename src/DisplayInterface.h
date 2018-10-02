#ifndef DISPLAY_INTERFACE_H_
#define DISPLAY_INTERFACE_H_
#include "NotificationService.h"

typedef enum
{
    BLE_DISCONNECTED = 0,
    BLE_PAIRING = 1,
    BLE_CONNECTED = 2
} conn_state_def;

class DisplayInterface
{
  public:
    virtual void init();
    virtual void mainScreen();
    virtual void showNotification(notification_def *notification);
    //virtual void setNotificationList(std::vector<notification_def> notificationList);

  protected:
    std::vector<notification_def> notificationList;
};

#endif