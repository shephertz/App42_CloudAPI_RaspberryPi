//
//  App42API.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 07/10/13.
//
//

#include "App42API.h"
#include "App42FileManager.h"

static string apiKey="";
static string secretKey="";
static string loggedInUser = "";
bool isTraceEnabled = false;
bool isEventServiceEnabled = false;
bool isAppStateEventTrackingEnabled = false;

static string installId = "";
static string userSessionId = "";
static string fbAccesToken = "";
static string dbName = "";
static vector<App42ACL> defaultApp42ACL;

vector<App42ACL> App42API::getDefaultApp42ACL()
{
    return defaultApp42ACL;
}

void App42API::setDefaultApp42ACL(vector<App42ACL> _defaultApp42ACL)
{
    defaultApp42ACL = _defaultApp42ACL;
}

string App42API::getInstallId()
{
    printf("getInstallId...installId = %s",installId.c_str());
    return installId;
}

void App42API::setInstallId(string _installId)
{
    installId=_installId;
    printf("setInstallId...installId = %s",installId.c_str());
}

string App42API::getDbName()
{
    return dbName;
}
void App42API::setDbName(string _dbName)
{
    dbName=_dbName;
}

string App42API::getUserSessionId()
{
    return userSessionId;
}
void App42API::setUserSessionId(string _userSessionId)
{
    userSessionId=_userSessionId;
}

void App42API::removeSession()
{
    userSessionId = nullptr;
    loggedInUser = nullptr;
}

string App42API::getFbAccesToken()
{
    return fbAccesToken;
}
void App42API::setFbAccesToken(string _fbAccesToken)
{
    fbAccesToken=_fbAccesToken;
}


string App42API::getLoggedInUser()
{
    return loggedInUser;
}
void App42API::setLoggedInUser(string _loggedInUser)
{
    loggedInUser=_loggedInUser;
}

void App42API::setIsTraceEnabled(bool _isTraceEnabled)
{
    isTraceEnabled = _isTraceEnabled;
}

void App42API::enableEventService(bool isEnable)
{
    isEventServiceEnabled = isEnable;
}

void App42API::enableAppStateEventTracking(bool isEnable)
{
    isAppStateEventTrackingEnabled = isEnable;
    isEventServiceEnabled = isEnable;
}

void App42API::Initialize(string _apiKey, string _secretKey)
{
    apiKey = _apiKey;
    secretKey = _secretKey;
    isTraceEnabled = false;
    string l_installId = Util::getInstallationId();
    printf("l_installId = %s",l_installId.c_str());
    setInstallId(l_installId);
}

/*UserService * App42API::BuildUserService()
{
    UserService *userService = UserService::Initialize(apiKey, secretKey);
    return userService;
}


GameService * App42API::BuildGameService()
{
    GameService *gameService = GameService::Initialize(apiKey, secretKey);
    return gameService;
}

ScoreBoardService * App42API::BuildScoreBoardService()
{
    ScoreBoardService *scoreBoardService = ScoreBoardService::Initialize(apiKey, secretKey);
    return scoreBoardService;
}

ScoreService * App42API::BuildScoreService()
{
    ScoreService *scoreService = ScoreService::Initialize(apiKey, secretKey);
    return scoreService;
}

RewardService * App42API::BuildRewardService()
{
    RewardService *rewardService = RewardService::Initialize(apiKey, secretKey);
    return rewardService;
}
*/

StorageService * App42API::BuildStorageService()
{
    StorageService *storageService = StorageService::Initialize(apiKey, secretKey);
    return storageService;
}

PushNotificationService * App42API::BuildPushNotificationService()
{
    PushNotificationService *pushNotificationService = PushNotificationService::Initialize(apiKey, secretKey);
    return pushNotificationService;
}
/*
EmailService * App42API::BuildEmailService()
{
    EmailService *emailService = EmailService::Initialize(apiKey, secretKey);
    return emailService;
}

UploadService * App42API::BuildUploadService()
{
    UploadService *uploadService = UploadService::Initialize(apiKey, secretKey);
    return uploadService;
}

CustomCodeService * App42API::BuildCustomCodeService()
{
    CustomCodeService *customCodeService = CustomCodeService::Initialize(apiKey, secretKey);
    return customCodeService;
}

SocialService * App42API::BuildSocialService()
{
    SocialService *socialService = SocialService::Initialize(apiKey, secretKey);
    return socialService;
}

AvatarService * App42API::BuildAvatarService()
{
    AvatarService *avatarService = AvatarService::Initialize(apiKey, secretKey);
    return avatarService;
}

SessionService * App42API::BuildSessionService()
{
    SessionService *sessionService = SessionService::Initialize(apiKey, secretKey);
    return sessionService;
}

BuddyService* App42API::BuildBuddyService()
{
    BuddyService *buddyService = BuddyService::Initialize(apiKey, secretKey);
    return buddyService;
}

TimerService* App42API::BuildTimerService()
{
    TimerService *timerService = TimerService::Initialize(apiKey, secretKey);
    return timerService;
}

GeoService* App42API::BuildGeoService()
{
    GeoService *geoService = GeoService::Initialize(apiKey, secretKey);
    return geoService;
}
*/
EventService* App42API::BuildEventService()
{
    EventService *eventService = EventService::Initialize(apiKey, secretKey);
    return eventService;
}
