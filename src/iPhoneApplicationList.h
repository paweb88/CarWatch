#ifndef IPHONE_APPLICATION_H_
#define IPHONE_APPLICATION_H_
#include <map>
#include <string>

typedef enum
{
    APP_SMS,
    APP_INSTAGRAM,
    APP_GMAIL,
    APP_PHONE,
    APP_FACETIME,
    APP_FACEBOOK
} iphone_application_def;

class iPhoneApplicationList
{
  public:
    bool isAllowedApplication(std::string appName);
    iphone_application_def getApplicationId(std::string appName);
    std::string getDisplayName(iphone_application_def appId);
  private:
    std::map<std::string, iphone_application_def>::const_iterator allowedApplicationIterator;
    const std::map<std::string, iphone_application_def> allowedApplication {
        {"com.apple.MobileSMS", APP_SMS},
        {"com.burbn.instagram", APP_INSTAGRAM},
        {"com.google.Gmail", APP_GMAIL},
        {"com.apple.mobilephone", APP_PHONE},
        {"com.apple.facetime", APP_FACETIME},
        {"com.facebook.Facebook", APP_FACEBOOK}
        };
    std::map<iphone_application_def, std::string>::const_iterator applicationNameIterator;
    const std::map<iphone_application_def, std::string> applicationName {
        {APP_SMS, "Text Message"},
        {APP_INSTAGRAM, "Instagram"},
        {APP_GMAIL, "Gmail"},
        {APP_PHONE, "Call"},
        {APP_FACEBOOK, "Facebook"}
        };
};
#endif