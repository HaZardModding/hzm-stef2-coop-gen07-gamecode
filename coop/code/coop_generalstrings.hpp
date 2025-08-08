//--------------------------------------------------------------
// COOP Gen7 - GENERAL STRINGS USED ->ONLY<- FOR COOP FUNCTIONS
//--------------------------------------------------------------

#pragma once


constexpr auto _COOP_COOP_allowesOnlyBlue = "Coop Mod allowes only blue team.\n";
constexpr auto _COOP_INFO_coopCommandOnly = "^3This command is only usable during Coop.\n";

constexpr auto _COOP_NAME_CLASS_technician = "Technician";
constexpr auto _COOP_NAME_CLASS_medic = "Medic";
constexpr auto _COOP_NAME_CLASS_heavyWeapons = "HeavyWeapons";

constexpr auto _COOP_INFO_TEST_maptype = "TEST HZM Coop Gen7 - Current map type is[%s]\n";
constexpr auto _COOP_INFO_TEST_maptypedetect = "TEST HZM Coop Gen7 - Detected category %s\n";

constexpr auto _COOP_INFO_INIT_game = "==== HaZardModding Coop Mod Gen7 GAME - initialising ====\n";
constexpr auto _COOP_INFO_INIT_gamedone = "==== HaZardModding Coop Mod Gen7 GAME - init complete ====\n";
constexpr auto _COOP_INFO_INIT_world = "==== HaZardModding Coop Mod Gen7 WORLD - %s ====\n";
constexpr auto _COOP_INFO_INIT_status = "==== HaZardModding Coop Mod Gen7 STAUS - %s ====\n";

constexpr auto _COOP_INFO_INIT_server_config = "Server config '%s.cfg' remapped to '%s.cfg'\n";

constexpr auto _COOP_INFO_usedCommand_noclip = "^8Command: ^5noclip ^8toggled.\n";
constexpr auto _COOP_INFO_usedCommand_stuck1 = "^5Coop^2: You have been moved to your spawn location.\n";
constexpr auto _COOP_INFO_usedCommand_stuck1_deu = "^5Coop^2: Sie wurden zu Ihrem Spawnpunkt geschoben.\n";
constexpr auto _COOP_INFO_usedCommand_stuck2 = "^5Coop^2: You have been moved to spawn location %d.\n";
constexpr auto _COOP_INFO_usedCommand_stuck2_deu = "^5Coop^2: Sie wurden zu Spawnpunkt %d geschoben.\n";
constexpr auto _COOP_INFO_usedCommand_transport1 = "^5Coop:^8 Your mobile ^3Heisenberg Compensator is recalibrating^8, please wait!\n";
constexpr auto _COOP_INFO_usedCommand_transport1_deu = "^5Coop:^8 Ihr mobiler ^3Heisenberg Kompensator rekalibriert^8 gerade, bitte warten!\n";
constexpr auto _COOP_INFO_usedCommand_transport2 = "^5Coop^2: No Teammember found, to serve as Transport target.\n";
constexpr auto _COOP_INFO_usedCommand_transport2_deu = "^5Coop^2: Kein Teammitglied als Transport Ziel gefunden.\n";
constexpr auto _COOP_INFO_usedCommand_transport3 = "^5Coop^2: Player does not allow, direct Transports.\n";
constexpr auto _COOP_INFO_usedCommand_transport3_deu = "^5Coop^2: Spieler erlaubt keine direkt Transporte.\n";
constexpr auto _COOP_INFO_usedCommand_transport4 = "^5Coop^2: Your Transport inhibitor is active!!!\n";
constexpr auto _COOP_INFO_usedCommand_transport4_deu = "^5Coop^2: Ihr Transport Inhibitor ist aktiviert!!!\n";
constexpr auto _COOP_INFO_usedCommand_transport5 = "^5Coop^2: Can't transport to your self.\n";
constexpr auto _COOP_INFO_usedCommand_transport5_deu = "^5Coop^2: Transport zu sich selbst nicht erlaubt.\n";
constexpr auto _COOP_INFO_usedCommand_notransport1 = "^5Coop^2: Transport inhibitor activated.\n";
constexpr auto _COOP_INFO_usedCommand_notransport1_deu = "^5Coop^2: Transport Inhibitor aktiviert.\n";
constexpr auto _COOP_INFO_usedCommand_notransport2 = "^5Coop^2: Transport inhibitor turned off.\n";
constexpr auto _COOP_INFO_usedCommand_notransport2_deu = "^5Coop^2: Transport Inhibitor ausgeschaltet.\n";
constexpr auto _COOP_INFO_usedCommand_targetnames1 = "^5Info^8: Show Targetnames - ^1disabled.\n";
constexpr auto _COOP_INFO_usedCommand_targetnames2 = "^5Info^8: Show Targetnames - ^2enabled.\n";
constexpr auto _COOP_INFO_usedCommand_levelend1 = "^5Info^8: Function 'coop_endLevel' not found in script.\n";
constexpr auto _COOP_INFO_usedCommand_levelend2 = "^5Info^8: Level End thread executed.\n";
constexpr auto _COOP_INFO_usedCommand_drop1 = "^5Can't drop this particular Weapon.\n";
constexpr auto _COOP_INFO_usedCommand_drop1_deu = "^5Diese Waffe kann nicht ablegt werden.\n";
constexpr auto _COOP_INFO_usedCommand_drop2 = "^5Your Weapon was dropped.\n";
constexpr auto _COOP_INFO_usedCommand_drop2_deu = "^5Ihre Waffe wurde ablegt.\n";
constexpr auto _COOP_INFO_usedCommand_info1 = "===Your Informations ===\n";
constexpr auto _COOP_INFO_usedCommand_info1_deu = "===Ihre Informationen ===\n";
constexpr auto _COOP_INFO_usedCommand_block1 = "^2Player marked!\n";
constexpr auto _COOP_INFO_usedCommand_block1_deu = "^2Spieler makiert!\n";
constexpr auto _COOP_INFO_usedCommand_block2 = "^2No valid target found!\n";
constexpr auto _COOP_INFO_usedCommand_block2_deu = "^2Kein valides Ziel gefunden!\n";
constexpr auto _COOP_INFO_usedCommand_mapname = "Current Level is:^5 %s.bsp\n";
constexpr auto _COOP_INFO_usedCommand_mapname_deu = "Aktuelles Level ist:^5 %s.bsp\n";
constexpr auto _COOP_INFO_usedCommand_class1 = "^3You can't change your class anymore.\n";
constexpr auto _COOP_INFO_usedCommand_class1_deu = "^3Ihre Klasse kann nicht mehr wechseln.\n";
constexpr auto _COOP_INFO_usedCommand_class2 = "^5Your current class is^5: ";
constexpr auto _COOP_INFO_usedCommand_class2_deu = "^5Ihre aktuelle Klasse ist^5: ";
constexpr auto _COOP_INFO_usedCommand_class3 = "^3Invalid Classenname!^2 Valid: [^5t^2] $$Technician$$ [^5m^2] $$Medic$$ [^5h^2] $$HeavyWeapons$$\n";
constexpr auto _COOP_INFO_usedCommand_class3_deu = "^3Invalider Klassenname!^2 Valide: [^5t^2] $$Technician$$ [^5m^2] $$Medic$$ [^5h^2] $$HeavyWeapons$$\n";

constexpr auto _COOP_INFO_spawnspotNumberedNotFound = "Spawnspot (in script/bsp) with number %d was not found...\n";

constexpr auto _COOP_INFO_radarFollowMarker_off = "^2Green ^5locaction ^5marker ^5on Radar ^1disabled\n";
constexpr auto _COOP_INFO_radarFollowMarker_off_deu = "^2Gruene ^5Positions ^5Markerkierung ^5auf Radar ^1deaktiviert\n";
constexpr auto _COOP_INFO_radarFollowMarker_on = "^5Locaction marked ^2green ^5on Radar of^8";
constexpr auto _COOP_INFO_radarFollowMarker_on_deu = "^5Position ^2gruen ^5markiert auf Radar von^8";


constexpr auto _COOP_INFO_adminLogin_needAdminUse = "You need to !login as Admin to use '%s'!\n";
constexpr auto _COOP_INFO_adminAuto = "Automatically logged you in as Host.";
constexpr auto _COOP_INFO_adminLoggedInHost = "^3You are now logged in (Host auto-!login).\n";
constexpr auto _COOP_INFO_adminLogin_needLatestCoop = "You need the latest HZM Coop Mod to use !login.\n";
constexpr auto _COOP_INFO_adminLogin_auth = "globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n";
constexpr auto _COOP_INFO_adminLogin_authCvarToShort = "Error: 'coop_admin' is empty or shorter than 3 digits - Aborting";
constexpr auto _COOP_INFO_adminLogin_authCvarToLong = "Error: 'coop_admin' is longer than 10 digits - Aborting";
constexpr auto _COOP_INFO_adminLogin_authSuccess = "Login succsessful - Accsess granted!";
constexpr auto _COOP_INFO_adminLogin_authFailure = "Login failed - Accsess denied!";
constexpr auto _COOP_INFO_adminLogin_useLogout = "^3You are already logged in - use ^2!logout ^3 to log out.\n";
constexpr auto _COOP_INFO_adminLogin_loginStartedEnter = "Login Started - Please enter the code.";
constexpr auto _COOP_INFO_adminLogin_logoutDone = "^3You are now logged out.\n";
constexpr auto _COOP_INFO_adminLogin_logoutAlready = "^3You are already logged out.\n";

constexpr auto _COOP_INFO_VERSION_pleaseUpdate = "Please update your Co-Op Mod!";
constexpr auto _COOP_INFO_VERSION_pleaseUpdate_deu = "Bitte Co-Op Mod aktualisieren!";


constexpr auto _COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW = "%s - too few parameters\n";

constexpr auto _COOP_WARNING_FILE_failed = "HZM Coop Gen7 - WARNING: Failed to load %s\n";

constexpr auto _COOP_ERROR_fatal = "HZM Coop Gen7 - FATAL EEROR: %s\n";
