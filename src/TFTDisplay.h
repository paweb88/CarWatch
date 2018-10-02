#ifndef TFTDISPLAY_H_
#define TFTDISPLAY_H_
#include "DisplayInterface.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "GfxUi.h"
#include "iPhoneApplicationList.h"
#include <map>

typedef enum
{
    IMG_LOGO,
    IMG_BLE_ON,
    IMG_BLE_OFF,
    IMG_PHONE,
    IMG_NEXT,
    IMG_PREV,
    IMG_FB,
    IMG_MSG,
    IMG_LINKEDIN,
    IMG_INSTAGRAM,
    IMG_GMAIL
} icon_type_def;

typedef void (*PairButtonCallback)();

class TFTDisplay
{
  public:
    void init();
    void mainScreen();
    void showNotification(notification_def *notification);
    void setNotificationList(std::vector<notification_def> notificationList);
    void setBLEConnectionState(conn_state_def state);
    void showPairButton(PairButtonCallback pairButtonCallback);
    void getTouch();

  private:
    conn_state_def bleState;
    PairButtonCallback pairButtonCallback = NULL;
    void renderHeader();
    void drawLongString(String str);
    void renderNotification(iphone_application_def notifyType, String title, String msg);

    TFT_eSPI *tft;
    TFT_eSPI_Button *pairButton = NULL;
    GfxUi *ui;
    boolean screenClear = false;

    const std::map <icon_type_def, std::string> imgMap {
        {IMG_LOGO, "/audi_logo.jpg"},
        {IMG_BLE_ON, "/bluetooth_on.jpg"},
        {IMG_BLE_OFF, "/bluetooth_off.jpg"},
        {IMG_PHONE, "/phone_icon.jpg"},
        {IMG_NEXT, "/next.jpg"},
        {IMG_PREV, "/prev.jpg"},
        {IMG_FB, "/facebook_icon.jpg"},
        {IMG_MSG, "/message_icon.jpg"},
        {IMG_LINKEDIN, "/linkedin_icon.jpg"},
        {IMG_INSTAGRAM, "/instagram_icon.jpg"},
        {IMG_GMAIL, "/gmail_icon.jpg"}
    };
};
#endif