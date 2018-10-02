#include "iPhoneApplicationList.h"

bool iPhoneApplicationList::isAllowedApplication(std::string appName) {
    return allowedApplication.count(appName) > 0;
};

iphone_application_def iPhoneApplicationList::getApplicationId(std::string appName) {
    allowedApplicationIterator = allowedApplication.find(appName);
    if (allowedApplicationIterator != allowedApplication.end()) {
        return allowedApplicationIterator->second;
    }
};

std::string iPhoneApplicationList::getDisplayName(iphone_application_def appId) {
    applicationNameIterator = applicationName.find(appId);
    if (applicationNameIterator != applicationName.end()) {
        return applicationNameIterator->second;
    }
};