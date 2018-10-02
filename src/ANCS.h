#ifndef ANCS_DICTIONARY_H_
#define ANCS_DICTIONARY_H_
namespace ANCS
{
    typedef enum
    {
        CategoryIDOther = 0,
        CategoryIDIncomingCall = 1,
        CategoryIDMissedCall = 2,
        CategoryIDVoicemail = 3,
        CategoryIDSocial = 4,
        CategoryIDSchedule = 5,
        CategoryIDEmail = 6,
        CategoryIDNews = 7,
        CategoryIDHealthAndFitness = 8,
        CategoryIDBusinessAndFinance = 9,
        CategoryIDLocation = 10,
        CategoryIDEntertainment = 11
    } category_id_t;

    typedef enum
    {
        EventIDNotificationAdded = 0,
        EventIDNotificationModified = 1,
        EventIDNotificationRemoved = 2
    } event_id_t;

    typedef enum
    {
        EventFlagSilent = (1 << 0),
        EventFlagImportant = (1 << 1),
        EventFlagPreExisting = (1 << 2),
        EventFlagPositiveAction = (1 << 3),
        EventFlagNegativeAction = (1 << 4)
    } event_flags_t;

    typedef enum
    {
        CommandIDGetNotificationAttributes = 0,
        CommandIDGetAppAttributes = 1,
        CommandIDPerformNotificationAction = 2
    } command_id_t;

    typedef enum
    {
        NotificationAttributeIDAppIdentifier = 0,
        NotificationAttributeIDTitle = 1,    // (Needs to be followed by a 2-bytes max length parameter)
        NotificationAttributeIDSubtitle = 2, // (Needs to be followed by a 2-bytes max length parameter)
        NotificationAttributeIDMessage = 3,  // (Needs to be followed by a 2-bytes max length parameter)
        NotificationAttributeIDMessageSize = 4,
        NotificationAttributeIDDate = 5,
        NotificationAttributeIDPositiveActionLabel = 6,
        NotificationAttributeIDNegativeActionLabel = 7
    } notification_attribute_id_t;
} // namespace ANCS
#endif