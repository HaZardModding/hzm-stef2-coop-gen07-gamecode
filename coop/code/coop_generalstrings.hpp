//--------------------------------------------------------------
// COOP Gen7 - GENERAL STRINGS USED ->ONLY<- FOR COOP FUNCTIONS
//--------------------------------------------------------------

#pragma once


constexpr auto _COOP_COOP_allowesOnlyBlue = "Coop Mod allowes only blue team.\n";

constexpr auto _COOP_INFO_TEST_maptype = "TEST HZM Coop Gen7 - Current map type is[%s]\n";
constexpr auto _COOP_INFO_TEST_maptypedetect = "TEST HZM Coop Gen7 - Detected category %s\n";

constexpr auto _COOP_INFO_INIT_game = "==== HaZardModding Coop Mod Gen7 GAME - initialising ====\n";
constexpr auto _COOP_INFO_INIT_gamedone = "==== HaZardModding Coop Mod Gen7 GAME - init complete ====\n";
constexpr auto _COOP_INFO_INIT_world = "==== HaZardModding Coop Mod Gen7 WORLD - %s ====\n";
constexpr auto _COOP_INFO_INIT_status = "==== HaZardModding Coop Mod Gen7 STAUS - %s ====\n";


constexpr auto _COOP_INFO_adminAuto = "Automatically logged you in as Host.";
constexpr auto _COOP_INFO_adminLoggedInHost = "^3You are now logged in (Host auto-!login).\n";
constexpr auto _COOP_INFO_adminNeeded = "^3You need to !login as Coop Admin to use this command.\n";
constexpr auto _COOP_INFO_coopCommandOnly = "^3This command is only usable during Coop.\n";

constexpr auto _COOP_INFO_radarFollowMarker_off = "^2Green ^5locaction ^5marker ^5on Radar ^1disabled\n";
constexpr auto _COOP_INFO_radarFollowMarker_off_deu = "^2Gruene ^5Positions ^5Markerkierung ^5auf Radar ^1deaktiviert";
constexpr auto _COOP_INFO_radarFollowMarker_on = "^5Locaction marked ^2green ^5on Radar of^8";
constexpr auto _COOP_INFO_radarFollowMarker_on_deu = "^5Position ^2gruen ^5markiert auf Radar von^8";

constexpr auto _COOP_INFO_adminLogin_auth = "globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n";
constexpr auto _COOP_INFO_adminLogin_authCvarToShort = "Error: 'coop_admin' is empty or shorter than 3 digits - Aborting";
constexpr auto _COOP_INFO_adminLogin_authCvarToLong = "Error: 'coop_admin' is longer than 10 digits - Aborting";
constexpr auto _COOP_INFO_adminLogin_authSuccess = "Login succsessful - Accsess granted!";
constexpr auto _COOP_INFO_adminLogin_authFailure = "Login failed - Accsess denied!";
constexpr auto _COOP_INFO_adminLogin_useLogout = "^3You are already logged in - use ^2!logout ^3 to log out.\n";
constexpr auto _COOP_INFO_adminLogin_loginStartedEnter = "Login Started - Please enter the code.";
constexpr auto _COOP_INFO_adminLogin_needLatestCoop = "You need the latest HZM Coop Mod to use !login.\n";
constexpr auto _COOP_INFO_adminLogin_logoutDone = "^3You are now logged out.\n";
constexpr auto _COOP_INFO_adminLogin_logoutAlready = "^3You are already logged out.\n";


constexpr auto _COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW = "%s - too few parameters\n";

constexpr auto _COOP_WARNING_FILE_failed = "HZM Coop Gen7 - WARNING: Failed to load %s\n";

constexpr auto _COOP_ERROR_fatal = "HZM Coop Gen7 - FATAL EEROR: %s\n";
