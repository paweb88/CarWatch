#include "TFTDisplay.h"

void TFTDisplay::init() {
    SPIFFS.begin();
    tft = new TFT_eSPI();
    tft->begin();
    tft->setRotation(3);
    uint16_t calData[5] = {388, 3494, 291, 3500, 1};
    tft->setTouch(calData);
    ui = new GfxUi(tft);
    tft->setFreeFont(&FreeMono9pt7b);
};

void TFTDisplay::setBLEConnectionState(conn_state_def state) {
    bleState = state;
    screenClear = false;
    mainScreen();
};

void TFTDisplay::renderHeader() {
    tft->fillRect(0, 0, 320, 35, TFT_WHITE);
    tft->drawFastHLine(0, 35, 320, TFT_BLACK);
    tft->setFreeFont(&FreeMonoBold12pt7b);
    tft->setTextDatum(0);
    tft->setTextColor(TFT_BLACK, TFT_WHITE);
    switch (bleState) {
        case BLE_DISCONNECTED:
            ui->drawJpeg(imgMap.at(IMG_BLE_OFF).c_str(), 10, 2);
            tft->drawString("Not connected", 45, 8);
            break;
        case BLE_PAIRING:
            ui->drawJpeg(imgMap.at(IMG_BLE_OFF).c_str(), 10, 2);
            tft->drawString("Pairing...", 45, 8);
            break;
        case BLE_CONNECTED:
            ui->drawJpeg(imgMap.at(IMG_BLE_ON).c_str(), 10, 2);
            tft->drawString("Connected", 45, 8);
            break;
    }
};

void TFTDisplay::drawLongString(String string) {
    const unsigned int len = string.length();
    uint8_t posy = 136, step= 16;
    tft->setTextDatum(MC_DATUM);
    if (len > 25) {
        String tmpS = "";
        for(uint16_t i = 1; i <= len; i++) {
            tmpS += string.charAt(i-1);
            if (i % 25 == 0) {
                tft->drawString(tmpS, 160, posy);
                posy+=step;
                tmpS = "";
            }
            if (i > 100) {
                tmpS += "...";
                break;
            }
        }
        tft->drawString(tmpS, 160, posy);
    } else {
        tft->drawString(string, 160, posy);
    }
}

void TFTDisplay::renderNotification(iphone_application_def notifyType, String title, String msg){
    tft->fillRoundRect(20,50,280,180,5,TFT_WHITE);
    tft->drawRoundRect(20,50,280,180,5,TFT_DARKGREY);
    String iconFile;
    switch (notifyType) {
        case APP_SMS:
            iconFile = imgMap.at(IMG_MSG).c_str();
            break;
        case APP_PHONE:
            iconFile = imgMap.at(IMG_PHONE).c_str();
            break;
        case APP_FACETIME:
            iconFile = imgMap.at(IMG_PHONE).c_str();
            break;
        case APP_FACEBOOK:
            iconFile = imgMap.at(IMG_FB).c_str();
            break;
        case APP_INSTAGRAM:
            iconFile = imgMap.at(IMG_INSTAGRAM).c_str();
            break;
        case APP_GMAIL:
            iconFile = imgMap.at(IMG_GMAIL).c_str();
            break;
    }
    ui->drawJpeg(iconFile.c_str(), 128, 55);
    tft->setFreeFont(&FreeMonoBold9pt7b);
    tft->setTextColor(TFT_BLACK, TFT_WHITE);
    tft->setTextDatum(MC_DATUM);
    tft->drawString(title.c_str(), 160, 119);
    tft->setFreeFont(&FreeMono9pt7b);
    if (notifyType == APP_PHONE || notifyType == APP_FACETIME){
        tft->drawString("is calling", 160, 136);
    } else {
        drawLongString(msg);
    }
};

void TFTDisplay::showPairButton(PairButtonCallback callback) {
    pairButton = new TFT_eSPI_Button();
    pairButton->initButton(tft, 160, 120, 60, 40, TFT_BLUE, TFT_DARKGREY, TFT_WHITE, "Pair", 1);
    pairButtonCallback = callback;
}

void TFTDisplay::getTouch() {
    uint16_t x = 0, y =0;
    boolean pressed = tft->getTouch(&x, &y);
    if (pressed) {
        if (pairButton != NULL && pairButton->contains(x,y)) {
            pairButtonCallback();
        }
    }
}

void TFTDisplay::showNotification(notification_def *notification){
    renderNotification(notification->type, notification->title.c_str(), notification->message.c_str());
    screenClear = false;
}

void TFTDisplay::mainScreen() {
    if (screenClear) return;
    tft->fillScreen(TFT_WHITE);  
    ui->drawJpeg(imgMap.at(IMG_LOGO).c_str(), 0, 0);
    renderHeader();
    if (bleState == BLE_DISCONNECTED && pairButton != NULL) {
        pairButton->drawButton();
    }
    screenClear = true;
}

void TFTDisplay::setNotificationList(std::vector<notification_def> notificationList){

}