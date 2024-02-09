/*
    src/resource/resource.h
 */
/*
    The primary purpose of afxres.h is to define resource IDs and control IDs used in MFC-based applications.
    Even though this is not MFC based application and rather I would call it a pure WinApi-based application
    At compile time I was getting error(2104) about undefined keys/keywords like DS_SETFONT, IDC_STATIC, DS_MODALFRAME etc
    By including the following file(afxres.h) those compile time errors(2104) got removed. 
    There is this document about this compile time error...
    https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/resource-compiler-error-rc2104?view=msvc-170     
 */
#include <afxres.h>

#ifndef CHARACTER_PAINTER_RESOURCE_H
#define CHARACTER_PAINTER_RESOURCE_H

#define ID_CURSOR_PEN_CURSOR  200 // these numbers are arbitrary
#define ID_CURSOR_ERASER_CURSOR 201 // these numbers are arbitrary  

#define ID_MENU_BAR 250 // aka ID_MAIN_MENU, this number is arbitrary

#define ID_MENU_BACKGROUND_COLOR 300 // these numbers are arbitrary
#define ID_MENU_BACKGROUND_COLOR_BLACK 301
#define ID_MENU_BACKGROUND_COLOR_GREEN 302
#define ID_MENU_BACKGROUND_COLOR_RED 303
#define ID_MENU_BACKGROUND_COLOR_WHITE 304

#define ID_MENU_FOREGROUND_COLOR    400 // these numbers are arbitrary
#define ID_MENU_FOREGROUND_COLOR_BLACK  401
#define ID_MENU_FOREGROUND_COLOR_BLUE   402
#define ID_MENU_FOREGROUND_COLOR_GREEN  403   
#define ID_MENU_FOREGROUND_COLOR_RED    404
#define ID_MENU_FOREGROUND_COLOR_WHITE  405

#define ID_MENU_FILE        500
#define ID_MENU_FILE_SAVE   501
#define ID_MENU_FILE_EXIT   502

#define ID_MENU_FILE_SAVE_DIALOG_CONTROL              1000
#define ID_MENU_FILE_SAVE_DIALOG_CONTROL_EDIT_CONTROL 1001

#endif

