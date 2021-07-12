#pragma once

extern "C" {
#include <linux/input.h>
}

#include <unordered_map>

static const std::unordered_map<std::string, uint16_t> key_conversion_table =
{
    {"ESC", KEY_ESC},
    {"1", KEY_1},
    {"2", KEY_2},
    {"3", KEY_3},
    {"4", KEY_4},
    {"5", KEY_5},
    {"6", KEY_6},
    {"7", KEY_7},
    {"8", KEY_8},
    {"9", KEY_9},
    {"0", KEY_0},
    {"MINUS", KEY_MINUS},
    {"EQUAL", KEY_EQUAL},
    {"BACKSPACE", KEY_BACKSPACE},
    {"TAB", KEY_TAB},
    {"Q", KEY_Q},
    {"W", KEY_W},
    {"E", KEY_E},
    {"R", KEY_R},
    {"T", KEY_T},
    {"Y", KEY_Y},
    {"U", KEY_U},
    {"I", KEY_I},
    {"O", KEY_O},
    {"P", KEY_P},
    {"LEFTBRACE", KEY_LEFTBRACE},
    {"RIGHTBRACE", KEY_RIGHTBRACE},
    {"ENTER", KEY_ENTER},
    {"LEFTCTRL", KEY_LEFTCTRL},
    {"A", KEY_A},
    {"S", KEY_S},
    {"D", KEY_D},
    {"F", KEY_F},
    {"G", KEY_G},
    {"H", KEY_H},
    {"J", KEY_J},
    {"K", KEY_K},
    {"L", KEY_L},
    {"SEMICOLON", KEY_SEMICOLON},
    {"APOSTROPHE", KEY_APOSTROPHE},
    {"GRAVE", KEY_GRAVE},
    {"LEFTSHIFT", KEY_LEFTSHIFT},
    {"BACKSLASH", KEY_BACKSLASH},
    {"Z", KEY_Z},
    {"X", KEY_X},
    {"C", KEY_C},
    {"V", KEY_V},
    {"B", KEY_B},
    {"N", KEY_N},
    {"M", KEY_M},
    {"COMMA", KEY_COMMA},
    {"DOT", KEY_DOT},
    {"SLASH", KEY_SLASH},
    {"RIGHTSHIFT", KEY_RIGHTSHIFT},
    {"KPASTERISK", KEY_KPASTERISK},
    {"LEFTALT", KEY_LEFTALT},
    {"SPACE", KEY_SPACE},
    {"CAPSLOCK", KEY_CAPSLOCK},
    {"F1", KEY_F1},
    {"F2", KEY_F2},
    {"F3", KEY_F3},
    {"F4", KEY_F4},
    {"F5", KEY_F5},
    {"F6", KEY_F6},
    {"F7", KEY_F7},
    {"F8", KEY_F8},
    {"F9", KEY_F9},
    {"F10", KEY_F10},
    {"NUMLOCK", KEY_NUMLOCK},
    {"SCROLLLOCK", KEY_SCROLLLOCK},
    {"KP7", KEY_KP7},
    {"KP8", KEY_KP8},
    {"KP9", KEY_KP9},
    {"KPMINUS", KEY_KPMINUS},
    {"KP4", KEY_KP4},
    {"KP5", KEY_KP5},
    {"KP6", KEY_KP6},
    {"KPPLUS", KEY_KPPLUS},
    {"KP1", KEY_KP1},
    {"KP2", KEY_KP2},
    {"KP3", KEY_KP3},
    {"KP0", KEY_KP0},
    {"KPDOT", KEY_KP0},
    {"ZENKAKUHANKAKU", KEY_ZENKAKUHANKAKU},
    {"102ND", KEY_102ND},
    {"F11", KEY_F11},
    {"F12", KEY_F12},
    {"RO", KEY_RO},
    {"KATAKANA", KEY_KATAKANA},
    {"HIRAGANA", KEY_HIRAGANA},
    {"HENKAN", KEY_HENKAN},
    {"KATAKANAHIRAGANA", KEY_KATAKANAHIRAGANA},
    {"MUHENKAN", KEY_MUHENKAN},
    {"KPJPCOMMA", KEY_KPJPCOMMA},
    {"KPENTER", KEY_KPENTER},
    {"RIGHTCTRL", KEY_RIGHTCTRL},
    {"KPSLASH", KEY_KPSLASH},
    {"SYSRQ", KEY_SYSRQ},
    {"RIGHTALT", KEY_RIGHTALT},
    {"LINEFEED", KEY_LINEFEED},
    {"HOME", KEY_HOME},
    {"UP", KEY_UP},
    {"PAGEUP", KEY_PAGEUP},
    {"LEFT", KEY_LEFT},
    {"RIGHT", KEY_RIGHT},
    {"END", KEY_END},
    {"DOWN", KEY_DOWN},
    {"PAGEDOWN", KEY_PAGEDOWN},
    {"INSERT", KEY_INSERT},
    {"DELETE", KEY_DELETE},
    {"MACRO", KEY_MACRO},
    {"MUTE", KEY_MUTE},
    {"VOLUMEDOWN", KEY_VOLUMEDOWN},
    {"VOLUMEUP", KEY_VOLUMEUP},
    {"POWER", KEY_POWER},
    {"KPEQUAL", KEY_KPEQUAL},
    {"KPPLUSMINUS", KEY_KPPLUSMINUS},
    {"PAUSE", KEY_PAUSE},
    {"SCALE", KEY_SCALE},
    {"KPCOMMA", KEY_KPCOMMA},
    {"HANGEUL", KEY_HANGEUL},
    {"HANGUEL", KEY_HANGEUL},
    {"HANJA", KEY_HANJA},
    {"YEN", KEY_YEN},
    {"LEFTMETA", KEY_LEFTMETA},
    {"RIGHTMETA", KEY_LEFTMETA},
    {"COMPOSE", KEY_COMPOSE},
    {"STOP", KEY_STOP},
    {"AGAIN", KEY_AGAIN},
    {"PROPS", KEY_PROPS},
    {"UNDO", KEY_UNDO},
    {"FRONT", KEY_FRONT},
    {"COPY", KEY_COPY},
    {"OPEN", KEY_OPEN},
    {"PASTE", KEY_PASTE},
    {"FIND", KEY_FIND},
    {"CUT", KEY_CUT},
    {"HELP", KEY_HELP},
    {"MENU", KEY_MENU},
    {"CALC", KEY_CALC},
    {"SETUP", KEY_SETUP},
    {"SLEEP", KEY_SLEEP},
    {"WAKEUP", KEY_WAKEUP},
    {"FILE", KEY_FILE},
    {"SENDFILE", KEY_SENDFILE},
    {"DELETEFILE", KEY_DELETEFILE},
    {"XFER", KEY_XFER},
    {"PROG1", KEY_PROG1},
    {"PROG2", KEY_PROG2},
    {"WWW", KEY_WWW},
    {"MSDOS", KEY_MSDOS},
    {"COFFEE", KEY_COFFEE},
    {"SCREENLOCK", KEY_COFFEE},
    {"DIRECTION", KEY_DIRECTION},
    {"CYCLEWINDOWS", KEY_CYCLEWINDOWS},
    {"MAIL", KEY_MAIL},
    {"BOOKMARKS", KEY_BOOKMARKS},
    {"COMPUTER", KEY_COMPUTER},
    {"BACK", KEY_BACK},
    {"FORWARD", KEY_FORWARD},
    {"CLOSECD", KEY_CLOSECD},
    {"EJECTCD", KEY_EJECTCD},
    {"EJECTCLOSECD", KEY_EJECTCLOSECD},
    {"NEXTSONG", KEY_NEXTSONG},
    {"PLAYPAUSE", KEY_PLAYPAUSE},
    {"PREVIOUSSONG", KEY_PREVIOUSSONG},
    {"STOPCD", KEY_STOPCD},
    {"RECORD", KEY_RECORD},
    {"REWIND", KEY_REWIND},
    {"PHONE", KEY_PHONE},
    {"ISO", KEY_ISO},
    {"CONFIG", KEY_CONFIG},
    {"HOMEPAGE", KEY_HOMEPAGE},
    {"REFRESH", KEY_REFRESH},
    {"EXIT", KEY_EXIT},
    {"MOVE", KEY_MOVE},
    {"EDIT", KEY_EDIT},
    {"SCROLLUP", KEY_SCROLLUP},
    {"SCROLLDOWN", KEY_SCROLLDOWN},
    {"KPLEFTPAREN", KEY_KPLEFTPAREN},
    {"KPRIGHTPAREN", KEY_KPRIGHTPAREN},
    {"NEW", KEY_NEW},{"REDO", KEY_REDO},
    {"F13", KEY_F13},{"F14", KEY_F14},
    {"F15", KEY_F15},{"F16", KEY_F16},
    {"F17", KEY_F17},{"F18", KEY_F18},
    {"F19", KEY_F19},{"F20", KEY_F20},
    {"F21", KEY_F21},{"F22", KEY_F22},
    {"F23", KEY_F23},{"F24", KEY_F24},
    {"PLAYCD", KEY_PLAYCD},
    {"PAUSECD", KEY_PAUSECD},
    {"PROG3", KEY_PROG3},
    {"PROG4", KEY_PROG4},
    {"DASHBOARD", KEY_DASHBOARD},
    {"SUSPEND", KEY_SUSPEND},
    {"CLOSE", KEY_CLOSE},
    {"PLAY", KEY_PLAY},
    {"FASTFORWARD", KEY_FASTFORWARD},
    {"BASSBOOST", KEY_BASSBOOST},
    {"PRINT", KEY_PRINT},
    {"HP", KEY_HP},
    {"CAMERA", KEY_CAMERA},
    {"SOUND", KEY_SOUND},
    {"QUESTION", KEY_QUESTION},
    {"EMAIL", KEY_EMAIL},
    {"CHAT", KEY_CHAT},
    {"SEARCH", KEY_SEARCH},
    {"CONNECT", KEY_CONNECT},
    {"FINANCE", KEY_FINANCE},
    {"SPORT", KEY_SPORT},
    {"SHOP", KEY_SHOP},
    {"ALTERASE", KEY_ALTERASE},
    {"CANCEL", KEY_CANCEL},
    {"BRIGHTNESSDOWN", KEY_BRIGHTNESSDOWN},
    {"BRIGHTNESSUP", KEY_BRIGHTNESSUP},
    {"MEDIA", KEY_MEDIA},
    {"SWITCHVIDEOMODE", KEY_SWITCHVIDEOMODE},
    {"KBDILLUMTOGGLE", KEY_KBDILLUMTOGGLE},
    {"KBDILLUMDOWN", KEY_KBDILLUMDOWN},
    {"KBDILLUMUP", KEY_KBDILLUMUP},
    {"SEND", KEY_SEND},
    {"REPLY", KEY_REPLY},
    {"FORWARDMAIL", KEY_FORWARDMAIL},
    {"SAVE", KEY_SAVE},
    {"DOCUMENTS", KEY_DOCUMENTS},
    {"BATTERY", KEY_BATTERY},
    {"BLUETOOTH", KEY_BLUETOOTH},
    {"WLAN", KEY_WLAN},
    {"UWB", KEY_UWB},
    {"UNKNOWN", KEY_UNKNOWN},
    {"VIDEO_NEXT", KEY_VIDEO_NEXT},
    {"VIDEO_PREV", KEY_VIDEO_PREV},
    {"BRIGHTNESS_CYCLE", KEY_BRIGHTNESS_CYCLE},
    {"BRIGHTNESS_ZERO", KEY_BRIGHTNESS_ZERO},
    {"DISPLAY_OFF", KEY_DISPLAY_OFF},
    {"WIMAX", KEY_WIMAX},
    {"RFKILL", KEY_RFKILL},
    {"BTN_MISC", BTN_MISC},
    {"BTN_0", BTN_0},
    {"BTN_1", BTN_1},
    {"BTN_2", BTN_2},
    {"BTN_3", BTN_3},
    {"BTN_4", BTN_4},
    {"BTN_5", BTN_5},
    {"BTN_6", BTN_6},
    {"BTN_7", BTN_7},
    {"BTN_8", BTN_8},
    {"BTN_9", BTN_9},
    {"BTN_MOUSE", BTN_MOUSE},
    {"BTN_LEFT", BTN_LEFT},
    {"BTN_RIGHT", BTN_RIGHT},
    {"BTN_MIDDLE", BTN_MIDDLE},
    {"BTN_SIDE", BTN_SIDE},
    {"BTN_EXTRA", BTN_EXTRA},
    {"BTN_FORWARD", BTN_FORWARD},
    {"BTN_BACK", BTN_BACK},
    {"BTN_TASK", BTN_TASK},
    {"BTN_JOYSTICK", BTN_JOYSTICK},
    {"BTN_TRIGGER", BTN_TRIGGER},
    {"BTN_THUMB", BTN_THUMB},
    {"BTN_THUMB2", BTN_THUMB2},
    {"BTN_TOP", BTN_TOP},
    {"BTN_TOP2", BTN_TOP2},
    {"BTN_PINKIE", BTN_PINKIE},
    {"BTN_BASE", BTN_BASE},
    {"BTN_BASE2", BTN_BASE2},
    {"BTN_BASE3", BTN_BASE3},
    {"BTN_BASE4", BTN_BASE4},
    {"BTN_BASE5", BTN_BASE5},
    {"BTN_BASE6", BTN_BASE6},
    {"BTN_DEAD", BTN_DEAD},
    {"BTN_GAMEPAD", BTN_GAMEPAD},
    {"BTN_A", BTN_A},
    {"BTN_B", BTN_B},
    {"BTN_C", BTN_C},
    {"BTN_X", BTN_X},
    {"BTN_Y", BTN_Y},
    {"BTN_Z", BTN_Z},
    {"BTN_TL", BTN_TL},
    {"BTN_TR", BTN_TR},
    {"BTN_TL2", BTN_TL2},
    {"BTN_TR2", BTN_TR2},
    {"BTN_SELECT", BTN_SELECT},
    {"BTN_START", BTN_START},
    {"BTN_MODE", BTN_MODE},
    {"BTN_THUMBL", BTN_THUMBL},
    {"BTN_THUMBR", BTN_THUMBR},
    {"BTN_DIGI", BTN_DIGI},
    {"BTN_TOOL_PEN", BTN_TOOL_PEN},
    {"BTN_TOOL_RUBBER", BTN_TOOL_RUBBER},
    {"BTN_TOOL_BRUSH", BTN_TOOL_RUBBER},
    {"BTN_TOOL_PENCIL", BTN_TOOL_PENCIL},
    {"BTN_TOOL_AIRBRUSH", BTN_TOOL_AIRBRUSH},
    {"BTN_TOOL_FINGER", BTN_TOOL_FINGER},
    {"BTN_TOOL_MOUSE", BTN_TOOL_MOUSE},
    {"BTN_TOOL_LENS", BTN_TOOL_LENS},
    {"BTN_TOUCH", BTN_TOUCH},
    {"BTN_STYLUS", BTN_STYLUS},
    {"BTN_STYLUS2", BTN_STYLUS2},
    {"BTN_TOOL_DOUBLETAP", BTN_TOOL_DOUBLETAP},
    {"BTN_TOOL_TRIPLETAP", BTN_TOOL_TRIPLETAP},
    {"BTN_TOOL_QUADTAP", BTN_TOOL_QUADTAP},
    {"BTN_WHEEL", BTN_WHEEL},
    {"BTN_GEAR_DOWN", BTN_GEAR_DOWN},
    {"BTN_GEAR_UP", BTN_GEAR_UP},
    {"OK", KEY_OK},
    {"SELECT", KEY_SELECT},
    {"GOTO", KEY_GOTO},
    {"CLEAR", KEY_CLEAR},
    {"POWER2", KEY_POWER2},
    {"OPTION", KEY_OPTION},
    {"INFO", KEY_INFO},
    {"TIME", KEY_TIME},
    {"VENDOR", KEY_VENDOR},
    {"ARCHIVE", KEY_ARCHIVE},
    {"PROGRAM", KEY_PROGRAM},
    {"CHANNEL", KEY_CHANNEL},
    {"FAVORITES", KEY_FAVORITES},
    {"EPG", KEY_EPG},
    {"PVR", KEY_PVR},
    {"MHP", KEY_MHP},
    {"LANGUAGE", KEY_LANGUAGE},
    {"TITLE", KEY_TITLE},
    {"SUBTITLE", KEY_SUBTITLE},
    {"ANGLE", KEY_ANGLE},
    {"ZOOM", KEY_ZOOM},
    {"MODE", KEY_MODE},
    {"KEYBOARD", KEY_KEYBOARD},
    {"SCREEN", KEY_SCREEN},
    {"PC", KEY_PC},
    {"TV", KEY_TV},
    {"TV2", KEY_TV2},
    {"VCR", KEY_VCR},
    {"VCR2", KEY_VCR2},
    {"SAT", KEY_SAT},
    {"SAT2", KEY_SAT2},
    {"CD", KEY_CD},
    {"TAPE", KEY_TAPE},
    {"RADIO", KEY_RADIO},
    {"TUNER", KEY_TUNER},
    {"PLAYER", KEY_PLAYER},
    {"TEXT", KEY_TEXT},
    {"DVD", KEY_DVD},
    {"AUX", KEY_AUX},
    {"MP3", KEY_MP3},
    {"AUDIO", KEY_AUDIO},
    {"VIDEO", KEY_VIDEO},
    {"DIRECTORY", KEY_DIRECTORY},
    {"LIST", KEY_LIST},
    {"MEMO", KEY_MEMO},
    {"CALENDAR", KEY_CALENDAR},
    {"RED", KEY_RED},
    {"GREEN", KEY_GREEN},
    {"YELLOW", KEY_YELLOW},
    {"BLUE", KEY_BLUE},
    {"CHANNELUP", KEY_CHANNELUP},
    {"CHANNELDOWN", KEY_CHANNELDOWN},
    {"FIRST", KEY_FIRST},{"LAST", KEY_LAST},
    {"AB", KEY_AB},
    {"NEXT", KEY_NEXT},
    {"RESTART", KEY_RESTART},
    {"SLOW", KEY_SLOW},
    {"SHUFFLE", KEY_SHUFFLE},
    {"BREAK", KEY_BREAK},
    {"PREVIOUS", KEY_PREVIOUS},
    {"DIGITS", KEY_DIGITS},
    {"TEEN", KEY_TEEN},
    {"TWEN", KEY_TWEN},
    {"VIDEOPHONE", KEY_VIDEOPHONE},
    {"GAMES", KEY_GAMES},
    {"ZOOMIN", KEY_ZOOMIN},
    {"ZOOMOUT", KEY_ZOOMOUT},
    {"ZOOMRESET", KEY_ZOOMRESET},
    {"WORDPROCESSOR", KEY_WORDPROCESSOR},
    {"EDITOR", KEY_EDITOR},
    {"SPREADSHEET", KEY_SPREADSHEET},
    {"GRAPHICSEDITOR", KEY_GRAPHICSEDITOR},
    {"PRESENTATION", KEY_PRESENTATION},
    {"DATABASE", KEY_DATABASE},
    {"NEWS", KEY_NEWS},
    {"VOICEMAIL", KEY_VOICEMAIL},
    {"ADDRESSBOOK", KEY_ADDRESSBOOK},
    {"MESSENGER", KEY_MESSENGER},
    {"DISPLAYTOGGLE", KEY_DISPLAYTOGGLE},
    {"SPELLCHECK", KEY_SPELLCHECK},
    {"LOGOFF", KEY_LOGOFF},
    {"DOLLAR", KEY_DOLLAR},
    {"EURO", KEY_EURO},
    {"FRAMEBACK", KEY_FRAMEBACK},
    {"FRAMEFORWARD", KEY_FRAMEFORWARD},
    {"CONTEXT_MENU", KEY_CONTEXT_MENU},
    {"MEDIA_REPEAT", KEY_MEDIA_REPEAT},
    {"10CHANNELSUP", KEY_10CHANNELSUP},
    {"10CHANNELSDOWN", KEY_10CHANNELSDOWN},
    {"DEL_EOL", KEY_DEL_EOL},
    {"DEL_EOS", KEY_DEL_EOS},
    {"INS_LINE", KEY_INS_LINE},
    {"DEL_LINE", KEY_DEL_LINE},
    {"FN", KEY_FN},
    {"FN_ESC", KEY_FN_ESC},
    {"FN_F1", KEY_FN_F1},
    {"FN_F2", KEY_FN_F2},
    {"FN_F3", KEY_FN_F3},
    {"FN_F4", KEY_FN_F4},
    {"FN_F5", KEY_FN_F5},
    {"FN_F6", KEY_FN_F6},
    {"FN_F7", KEY_FN_F7},
    {"FN_F8", KEY_FN_F8},
    {"FN_F9", KEY_FN_F9},
    {"FN_F10", KEY_FN_F10},
    {"FN_F11", KEY_FN_F11},
    {"FN_F12", KEY_FN_F12},
    {"FN_1", KEY_FN_1},
    {"FN_2", KEY_FN_2},
    {"FN_D", KEY_FN_D},
    {"FN_E", KEY_FN_E},
    {"FN_F", KEY_FN_F},
    {"FN_S", KEY_FN_S},
    {"FN_B", KEY_FN_B},
    {"BRL_DOT1", KEY_BRL_DOT1},
    {"BRL_DOT2", KEY_BRL_DOT2},
    {"BRL_DOT3", KEY_BRL_DOT3},
    {"BRL_DOT4", KEY_BRL_DOT4},
    {"BRL_DOT5", KEY_BRL_DOT5},
    {"BRL_DOT6", KEY_BRL_DOT6},
    {"BRL_DOT7", KEY_BRL_DOT7},
    {"BRL_DOT8", KEY_BRL_DOT8},
    {"BRL_DOT9", KEY_BRL_DOT9},
    {"BRL_DOT10", KEY_BRL_DOT10},
    {"NUMERIC_0", KEY_NUMERIC_0},
    {"NUMERIC_1", KEY_NUMERIC_1},
    {"NUMERIC_2", KEY_NUMERIC_2},
    {"NUMERIC_3", KEY_NUMERIC_3},
    {"NUMERIC_4", KEY_NUMERIC_4},
    {"NUMERIC_5", KEY_NUMERIC_5},
    {"NUMERIC_6", KEY_NUMERIC_6},
    {"NUMERIC_7", KEY_NUMERIC_7},
    {"NUMERIC_8", KEY_NUMERIC_8},
    {"NUMERIC_9", KEY_NUMERIC_9},
    {"NUMERIC_STAR", KEY_NUMERIC_STAR},
    {"NUMERIC_POUND", KEY_NUMERIC_POUND},
    {"CAMERA_FOCUS", KEY_CAMERA_FOCUS},
    {"WPS_BUTTON", KEY_WPS_BUTTON},
    {"TOUCHPAD_TOGGLE", KEY_TOUCHPAD_TOGGLE},
    {"TOUCHPAD_ON", KEY_TOUCHPAD_ON},
    {"TOUCHPAD_OFF", KEY_TOUCHPAD_OFF},
    {"BTN_TRIGGER_HAPPY", BTN_TRIGGER_HAPPY},
    {"BTN_TRIGGER_HAPPY1", BTN_TRIGGER_HAPPY1},
    {"BTN_TRIGGER_HAPPY2", BTN_TRIGGER_HAPPY2},
    {"BTN_TRIGGER_HAPPY3", BTN_TRIGGER_HAPPY3},
    {"BTN_TRIGGER_HAPPY4", BTN_TRIGGER_HAPPY4},
    {"BTN_TRIGGER_HAPPY5", BTN_TRIGGER_HAPPY5},
    {"BTN_TRIGGER_HAPPY6", BTN_TRIGGER_HAPPY6},
    {"BTN_TRIGGER_HAPPY7", BTN_TRIGGER_HAPPY7},
    {"BTN_TRIGGER_HAPPY8", BTN_TRIGGER_HAPPY8},
    {"BTN_TRIGGER_HAPPY9", BTN_TRIGGER_HAPPY9},
    {"BTN_TRIGGER_HAPPY10", BTN_TRIGGER_HAPPY10},
    {"BTN_TRIGGER_HAPPY11", BTN_TRIGGER_HAPPY11},
    {"BTN_TRIGGER_HAPPY12", BTN_TRIGGER_HAPPY12},
    {"BTN_TRIGGER_HAPPY13", BTN_TRIGGER_HAPPY13},
    {"BTN_TRIGGER_HAPPY14", BTN_TRIGGER_HAPPY14},
    {"BTN_TRIGGER_HAPPY15", BTN_TRIGGER_HAPPY15},
    {"BTN_TRIGGER_HAPPY16", BTN_TRIGGER_HAPPY16},
    {"BTN_TRIGGER_HAPPY17", BTN_TRIGGER_HAPPY17},
    {"BTN_TRIGGER_HAPPY18", BTN_TRIGGER_HAPPY18},
    {"BTN_TRIGGER_HAPPY19", BTN_TRIGGER_HAPPY19},
    {"BTN_TRIGGER_HAPPY20", BTN_TRIGGER_HAPPY20},
    {"BTN_TRIGGER_HAPPY21", BTN_TRIGGER_HAPPY21},
    {"BTN_TRIGGER_HAPPY22", BTN_TRIGGER_HAPPY22},
    {"BTN_TRIGGER_HAPPY23", BTN_TRIGGER_HAPPY23},
    {"BTN_TRIGGER_HAPPY24", BTN_TRIGGER_HAPPY24},
    {"BTN_TRIGGER_HAPPY25", BTN_TRIGGER_HAPPY25},
    {"BTN_TRIGGER_HAPPY26", BTN_TRIGGER_HAPPY26},
    {"BTN_TRIGGER_HAPPY27", BTN_TRIGGER_HAPPY27},
    {"BTN_TRIGGER_HAPPY28", BTN_TRIGGER_HAPPY28},
    {"BTN_TRIGGER_HAPPY29", BTN_TRIGGER_HAPPY29},
    {"BTN_TRIGGER_HAPPY30", BTN_TRIGGER_HAPPY30},
    {"BTN_TRIGGER_HAPPY31", BTN_TRIGGER_HAPPY31},
    {"BTN_TRIGGER_HAPPY32", BTN_TRIGGER_HAPPY32},
    {"BTN_TRIGGER_HAPPY33", BTN_TRIGGER_HAPPY33},
    {"BTN_TRIGGER_HAPPY34", BTN_TRIGGER_HAPPY34},
    {"BTN_TRIGGER_HAPPY35", BTN_TRIGGER_HAPPY35},
    {"BTN_TRIGGER_HAPPY36", BTN_TRIGGER_HAPPY36},
    {"BTN_TRIGGER_HAPPY37", BTN_TRIGGER_HAPPY37},
    {"BTN_TRIGGER_HAPPY38", BTN_TRIGGER_HAPPY38},
    {"BTN_TRIGGER_HAPPY39", BTN_TRIGGER_HAPPY39},
    {"BTN_TRIGGER_HAPPY40", BTN_TRIGGER_HAPPY40},
    /* Aliases */
    {"CTRL", KEY_LEFTCTRL},
    {"META", KEY_LEFTMETA},
    {"ALT", KEY_LEFTALT},
    {"SHIFT", KEY_LEFTSHIFT},
    {"PRINTSCR", KEY_SYSRQ},
    {"MIC_MUTE", KEY_F20}
};


