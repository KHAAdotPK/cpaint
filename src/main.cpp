/*
    src/main.c
 */

// https://stackoverflow.com/questions/39261826/change-the-color-of-the-title-bar-caption-of-a-win32-application
// https://learn.microsoft.com/en-us/windows/win32/inputdev/using-mouse-input?source=recommendations
// https://learn.microsoft.com/en-us/windows/win32/inputdev/using-mouse-input?source=recommendations

//__declspec( dllimport ) void func(void);

#include "header.h"

bool bInside = true;
//class MouseTrackEvents trackMouse;
HWND hWnd, hFileSaveDialog;
LPWSTR longName = L"Character Painter";
LPWSTR shortName = L"CPaint";
POINT ptCursor;
// client area rectangle
RECT rcClient;
short mouseX_old = 0, mouseY_old = 0;

//unsigned char pixels[DEFAULT_CPAINT_WINDOW_HEIGHT*DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH];
unsigned char *pixels = NULL, *pixels_edge_bounded = NULL;
unsigned char* pixels_background = NULL;
unsigned char* pixels_foreground = NULL, *pixels_foreground_edge_bounded = NULL;
//unsigned char* pixels_ptr = NULL;
wchar_t* png_file_name = NULL;

size_t height, width;

LRESULT CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                        // Retrieve the entered text from the edit control
                        /*
                        HWND hwndEdit = GetDlgItem(hwndDlg, IDC_EDIT1);
                        int length = GetWindowTextLength(hwndEdit);
                        if (length > 0)
                        {
                            wchar_t* buffer = new wchar_t[length + 1];
                            GetWindowText(hwndEdit, buffer, length + 1);
                            userInput = buffer;
                            delete[] buffer;
                        }
                         */
                    HWND file_save_dialog_control_edit_control = GetDlgItem(hwndDlg, ID_MENU_FILE_SAVE_DIALOG_CONTROL_EDIT_CONTROL);
                    int png_file_name_length = GetWindowTextLength(file_save_dialog_control_edit_control);
                    if (png_file_name_length > 0)
                    {   
                        png_file_name = new wchar_t[png_file_name_length + 1]; // For NULL character                     
                        GetWindowText(file_save_dialog_control_edit_control, png_file_name, png_file_name_length + 1);
                                                
                        //MessageBox(hwndDlg, png_file_name, L"Entered Text", MB_OK | MB_ICONINFORMATION);
                        
                        /* Call PNG API */
                        png_init(pixels, DEFAULT_CPAINT_WINDOW_HEIGHT, DEFAULT_CPAINT_WINDOW_WIDTH, DEFAULT_CPAINT_BIT_DEPTH, png_file_name);
                        //png_init(pixels_edge_bounded, height, width, DEFAULT_CPAINT_BIT_DEPTH, png_file_name);
                        
                        /*
                            Each time when done using the png_file_name, deallocate it and assign NULL to the pointer
                         */                            
                        delete[] png_file_name;
                        png_file_name = NULL;                                                 
                    }
                }
                EndDialog(hwndDlg, wParam);
                return TRUE;

                case IDCANCEL:
                    EndDialog(hwndDlg, wParam);
                return TRUE;
            }
            break;
    }

    return FALSE;
}

//https://stackoverflow.com/questions/71030019/in-winapi-how-do-i-know-when-the-mouse-has-moved-outside-the-current-window
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    
    switch (nCode)
    {
        case HC_ACTION:
        {
            switch (wParam)
            {
                case WM_MOUSEMOVE:
                {
                    MOUSEHOOKSTRUCT *pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
                    ptCursor = pMouseStruct->pt;
                    ScreenToClient(hWnd, &ptCursor);
                    bool bInside = PtInRect(&rcClient, ptCursor);

                    if (!bInside)
                    {
                        SendMessage(hWnd, WM_MOUSELEAVE, 0, 0);
                        //bInside = false;
                    }
                    /*else 
                    {
                        //bInside = true;
                    }*/

                    //SetWindowTextW(hWnd, L" HC_ACTION********************");
                }
                break;

                /*
                case WM_LBUTTONDOWN:
                {                                    
                }
                break;

                case WM_RBUTTONUP:
                {
                }
                break;
                 */

            }
                //SetWindowTextW(hWnd, L" HC_ACTION********************");  
        }
        break;

        case HC_NOREMOVE:
        {   

            SetWindowTextW(hWnd, L"HC_NOREMOVE********************");         
        }
        break;
    }
    
    /*
    if (wParam == WM_MOUSEMOVE) {
        // Notify me.
        SetWindowTextW(hWnd, L"Moved********************"); 
    }

    if (wParam == WM_MOUSELEAVE) {

        SetWindowTextW(hWnd, L"Left********************"); 
    }
     */

    //return 0;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    //RECT rcClient;                 // client area rectangle
    
    // Register the window class.
    //const wchar_t CLASS_NAME[]  = L"Character Painter";
    WNDCLASS wc = { };
        
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = longName;
    wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(ID_CURSOR_PEN_CURSOR)); // class cursor 

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
                                0,                                    // Optional window styles.
                                longName,                             // Window class
                                longName,                             // Window text
                                // Window style
                                WS_OVERLAPPEDWINDOW  & ~(WS_MAXIMIZEBOX /*| WS_MINIMIZEBOX*/),
                                // Position
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                // Size
                                DEFAULT_CPAINT_WINDOW_WIDTH /*+ 16*/, DEFAULT_CPAINT_WINDOW_HEIGHT /*+ 39*/,
                                NULL,       // Parent window    
                                NULL,       // Menu
                                hInstance,  // Instance handle
                                NULL        // Additional application data
                              );

    if (hwnd == NULL)
    {
        return 0;
    }

    hWnd = hwnd;

    SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) & ~WS_SIZEBOX);

    /*
    HMENU hMenuBackground = LoadMenu(hInstance, MAKEINTRESOURCE(ID_MENU_COLOR));
    SetMenu(hwnd, hMenuBackground);
    */

    displayMessage();

    ShowWindow(hwnd, nCmdShow);

    /*
    // Retrieve the screen coordinates of the client area, 
    // and convert them into client coordinates. 
    GetClientRect(hwnd, &rcClient);

    
    // Create the message string
    wchar_t message[256];
    swprintf(message, L"rcClient: left=%d, top=%d, right=%d, bottom=%d", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

    // Display the message box
    MessageBox(hwnd, message, L"Client Area", MB_OK);
     */

    //https://stackoverflow.com/questions/71030019/in-winapi-how-do-i-know-when-the-mouse-has-moved-outside-the-current-window 	
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);

    //DialogBox(NULL, MAKEINTRESOURCE(ID_MENU_FILE_SAVE_DIALOG_CONTROL), NULL, DialogProc);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        /*
        // Check if the mouse is inside the client area of the window.
        GetCursorPos(&ptCursor);
        //ScreenToClient(hwnd, &ptCursor);
        bInside = PtInRect(&rcClient, ptCursor);
         */

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

// Window procedure callback function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    short mouseX, mouseY;
    WCHAR szTitle[500];

    switch (uMsg)
    {
        case WM_CLOSE:
            // Handle the window close message
            DestroyWindow(hwnd);
        break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                // BACKGROUND COLOR CHANGE 
                case ID_MENU_BACKGROUND_COLOR_BLACK:
                {
                    PIXEL pixel = {0x00, 0x00, 0x00/*, 0x00*/};

                    //set_back_or_foreground_color(pixels, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //set_back_or_foreground_color(pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);
                    set_back_or_foreground_color(pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //memcpy(pixels, pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
                    memcpy(pixels, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

                    //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    {
                        //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                        if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0*/) )
                        {
                            for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                            {
                                *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + j);
                            }
                        }
                    }
                   
                    /*
                        The InvalidateRect function is called with NULL as the second parameter, which indicates that the entire client area of the window should be invalidated.
                        The third parameter, FALSE, indicates that the invalidated area should not be erased before the WM_PAINT message is sent.
                    */
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                }
                break; 
                case ID_MENU_BACKGROUND_COLOR_GREEN:
                {
                    PIXEL pixel = {0x00, 0xFF, 0x00/*, 0x00*/};

                    //set_back_or_foreground_color(pixels, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //set_back_or_foreground_color(pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);
                    set_back_or_foreground_color(pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //memcpy(pixels, pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
                    memcpy(pixels, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

                    //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    {
                        //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                        if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0*/) )
                        {
                            for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                            {
                                *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + j);
                            }
                        }
                    }
                    
                    /*
                        The InvalidateRect function is called with NULL as the second parameter, which indicates that the entire client area of the window should be invalidated.
                        The third parameter, FALSE, indicates that the invalidated area should not be erased before the WM_PAINT message is sent.
                    */
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                } 
                break; 
                case ID_MENU_BACKGROUND_COLOR_RED:
                {
                    PIXEL pixel = {0x00, 0x00, 0xFF/*, 0x00*/};

                    //set_back_or_foreground_color(pixels, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //set_back_or_foreground_color(pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);
                    set_back_or_foreground_color(pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //memcpy(pixels, pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
                    memcpy(pixels, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

                    //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    {
                        //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                        if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0*/) )
                        {
                            for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                            {
                                *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + j);
                            }
                        }
                    }

                    /*
                        The InvalidateRect function is called with NULL as the second parameter, which indicates that the entire client area of the window should be invalidated.
                        The third parameter, FALSE, indicates that the invalidated area should not be erased before the WM_PAINT message is sent.
                    */
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);                    
                }
                break;
                case ID_MENU_BACKGROUND_COLOR_WHITE:
                {
                    PIXEL pixel = {0xFF, 0xFF, 0xFF/*, 0x00*/};

                    //set_back_or_foreground_color(pixels, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //set_back_or_foreground_color(pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);
                    //set_back_or_foreground_color(pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH), pixel);
                    set_back_or_foreground_color(pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH), pixel);

                    //memcpy(pixels, pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
                    memcpy(pixels, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

                    //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                    {
                        //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                        if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0*/) )
                        {
                            for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                            {
                                *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + j);
                            }
                        }
                    }

                    /*
                        The InvalidateRect function is called with NULL as the second parameter, which indicates that the entire client area of the window should be invalidated.
                        The third parameter, FALSE, indicates that the invalidated area should not be erased before the WM_PAINT message is sent.
                    */
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);                    
                }
                break;             
                case ID_MENU_FILE_EXIT:
                {
                    if (pixels != NULL)
                    {
                        free(pixels);
                        free(pixels_background);
                        free(pixels_foreground);

                        pixels = NULL;
                        pixels_background = NULL;
                        pixels_foreground = NULL;
                    }

                    // Send WM_DESTROY message
                    DestroyWindow(hwnd);
                }
                // FILE SAVE 
                case ID_MENU_FILE_SAVE:
                {
                    ShowWindow(hFileSaveDialog, SW_SHOW);
                    //DialogBox(NULL, MAKEINTRESOURCE(ID_MENU_FILE_SAVE_DIALOG_CONTROL), NULL, DialogProc);
                    //MessageBox(hwnd, L"Saved to a default named PNG file.", L"Client Area", MB_OK);                    
                }
                break;
                break;
                // FOREGROUND COLOR CHANGE
                case ID_MENU_FOREGROUND_COLOR_BLACK:
                {
                    defaultForegroundPixelColor = {0x00, 0x00, 0x00/*, 0x00*/};
                }
                break;
                case ID_MENU_FOREGROUND_COLOR_BLUE:
                {                    
                    defaultForegroundPixelColor = {0xFF, 0x00, 0x00/*, 0x00*/};                    
                }
                break;
                case ID_MENU_FOREGROUND_COLOR_GREEN:
                {
                    defaultForegroundPixelColor = {0x00, 0xFF, 0x00/*, 0x00*/};                    
                }
                break;
                case ID_MENU_FOREGROUND_COLOR_RED:
                {
                    defaultForegroundPixelColor = {0x00, 0x00, 0xFF/*, 0x00*/};
                }
                break;
                case ID_MENU_FOREGROUND_COLOR_WHITE:
                {
                    defaultForegroundPixelColor = {0xFF, 0xFF, 0xFF/*, 0xFF*/};                    
                }
                break;                
            } 
        } 
        break;   

        case WM_CREATE:
        { 
            // Create the dialog as a child window of the main window
            hFileSaveDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_MENU_FILE_SAVE_DIALOG_CONTROL), hwnd, DialogProc);
            HMENU hMainMenuBar = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_MENU_BAR));

            GetClientRect(hwnd, &rcClient);
            //height = rcClient.bottom - rcClient.top;
            //width = rcClient.right - rcClient.left;

            height = DEFAULT_CPAINT_WINDOW_HEIGHT;
            width = DEFAULT_CPAINT_WINDOW_WIDTH;

            /*
            char message[100];

            FormatMessage(
                            FORMAT_MESSAGE_FROM_STRING,
                            "Width: %1",
                            0,
                            0,
                            (LPWSTR)message,
                            sizeof(message) / sizeof(TCHAR),
                            (va_list*)&width);
            //sprintf(message, "Width = %l, height = %l", width, height);
            MessageBox(NULL, (LPCWSTR)message, (LPCWSTR)"Size of Window", MB_OK | MB_ICONINFORMATION);
             */
            
            /*
            pixels = (unsigned char*)malloc(DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
            pixels_background = (unsigned char*)malloc(DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
            pixels_foreground = (unsigned char*)malloc(DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
             */
            pixels = (unsigned char*)malloc(height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            pixels_background = (unsigned char*)malloc(height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            pixels_foreground = (unsigned char*)malloc(height*(width*DEFAULT_CPAINT_BIT_DEPTH));

            pixels_edge_bounded = (unsigned char*)malloc(height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            pixels_foreground_edge_bounded = (unsigned char*)malloc(height*(width*DEFAULT_CPAINT_BIT_DEPTH));

            /*
            // Initialize the pixels array with zeros
            memset(pixels, 0x00, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
            memset(pixels_background, 0x00, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
            memset(pixels_foreground, 0x01, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
             */
            memset(pixels, 0x00, height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            memset(pixels_background, 0x00, height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            memset(pixels_foreground, 0x01, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

            memset(pixels_edge_bounded, 0x00, height*(width*DEFAULT_CPAINT_BIT_DEPTH));
            memset(pixels_foreground_edge_bounded, 0x01, height*(width*DEFAULT_CPAINT_BIT_DEPTH));

            SetMenu(hwnd, hMainMenuBar);
            /*
                The save dialog box  
             */
            //SetWindowPos(hFileSaveDialog, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_HIDEWINDOW);
            SetWindowPos(hFileSaveDialog, NULL, rcClient.left, rcClient.top, 330, 240, SWP_HIDEWINDOW);
        }
        break;            

        case WM_DESTROY:
        {    
            if (pixels != NULL)
            {
                free(pixels);
                free(pixels_background);
                free(pixels_foreground);

                free(pixels_edge_bounded);

                pixels = NULL;
                pixels_background = NULL;
                pixels_foreground = NULL;

                pixels_edge_bounded = NULL;
            }

            // Handle the window destroy message
            PostQuitMessage(0);
        }
        break;

        case WM_LBUTTONDOWN:
        {
            mouseX_old = LOWORD(lParam);
            mouseY_old = HIWORD(lParam);
        }
        break;

        case WM_MOUSELEAVE:
        { 
            bInside = false;

            wsprintf(szTitle, L"%s: %d, %d - Outside", shortName, ptCursor.x, ptCursor.y);
            SetWindowTextW(hwnd, szTitle);          
        }
        break;

        case WM_MOUSEMOVE:
        {            
            mouseX = LOWORD(lParam) /*ptCursor.x*/;
            mouseY = HIWORD(lParam) /*ptCursor.y*/;  
            
            if (!(mouseX < 0 || mouseY < 0))
            {                
                //unsigned int pixelIndex = (mouseY*DEFAULT_CPAINT_WINDOW_WIDTH + mouseX)*DEFAULT_CPAINT_BIT_DEPTH;
                /*
                    Index into array which holds the pixels data... 
                    next DEFAULT_CPAINT_BIT_DEPTH many bytes hold the pixel data for the pixel at this index.
                    This is a displacement into the array which holds the pixel data.
                 */
                unsigned int pixelIndex = ((mouseY /*+ EDGE_BOUND(height, (rcClient.bottom - rcClient.top))*/)*width + (mouseX /*+ EDGE_BOUND(width, (rcClient.right - rcClient.left))*/))*DEFAULT_CPAINT_BIT_DEPTH;
                //unsigned int pixelIndex = ((mouseY + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)))*width + (mouseX + EDGE_BOUND(width, (rcClient.right - rcClient.left))))*DEFAULT_CPAINT_BIT_DEPTH;
                
                if (wParam & MK_LBUTTON) 
                {                    
                    pixels_foreground[pixelIndex + 2] = defaultForegroundPixelColor.r;
                    pixels_foreground[pixelIndex + 1] = defaultForegroundPixelColor.g;
                    pixels_foreground[pixelIndex + 0] = defaultForegroundPixelColor.b;

                    /*
                        pixels_foreground_edge_bounded[((mouseY + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)))*width + (mouseX + EDGE_BOUND(width, (rcClient.right - rcClient.left))))*DEFAULT_CPAINT_BIT_DEPTH + 2] = defaultForegroundPixelColor.r;
                        pixels_foreground_edge_bounded[((mouseY + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)))*width + (mouseX + EDGE_BOUND(width, (rcClient.right - rcClient.left))))*DEFAULT_CPAINT_BIT_DEPTH + 1] = defaultForegroundPixelColor.g;
                        pixels_foreground_edge_bounded[((mouseY + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)))*width + (mouseX + EDGE_BOUND(width, (rcClient.right - rcClient.left))))*DEFAULT_CPAINT_BIT_DEPTH + 0] = defaultForegroundPixelColor.b;
                     */

                    pixels_foreground_edge_bounded[((((size_t)(((double)mouseY)/(rcClient.bottom - rcClient.top)))*height)*width + ((size_t)(((double)mouseX)/(rcClient.right - rcClient.left)))*width)*DEFAULT_CPAINT_BIT_DEPTH + 2] = defaultForegroundPixelColor.r;
                    pixels_foreground_edge_bounded[((((size_t)(((double)mouseY)/(rcClient.bottom - rcClient.top)))*height)*width + ((size_t)(((double)mouseX)/(rcClient.right - rcClient.left)))*width)*DEFAULT_CPAINT_BIT_DEPTH + 1] = defaultForegroundPixelColor.g;
                    pixels_foreground_edge_bounded[((((size_t)(((double)mouseY)/(rcClient.bottom - rcClient.top)))*height)*width + ((size_t)(((double)mouseX)/(rcClient.right - rcClient.left)))*width)*DEFAULT_CPAINT_BIT_DEPTH + 0] = defaultForegroundPixelColor.b;

                    /*
                        (mouseX/(rcClient.right - rcClient.left))*width
                        (mouseY/(rcClient.bottom - rcClient.top))*height
                     */
                    
                    /*
                        The additional 8 bits in a 32-bit bitmap format are typically used for an alpha channel, 
                        which allows for transparency information to be stored alongside the color information for each pixel.
                        This can be useful for creating images with transparency effects, such as semi-transparent overlays or shadows.
                        The alpha channel is not being used, so the additional 8 bits are likely being set to zero
                    */
                    /*pixels_foreground[pixelIndex + 3] = defaultForegroundPixelColor.a;*/

                    //height = rcClient.bottom - rcClient.top;
                    //width = rcClient.right - rcClient.left;
                    wsprintf(szTitle, L"%s: mouseX(%u, %u), mouseY(%d, %u) - (Inside, LBUTTON) - pixelIndex(%d, (%u, %u)) | (rcClient.bottom - rcClient.top): %u, (rcClient.right - rcClient.left): %u EDGE_BOUND(%u, %u)", shortName, mouseX, (size_t)((((double)mouseX)/(rcClient.right - rcClient.left))*width), mouseY, (size_t)((((double)mouseY)/(rcClient.bottom - rcClient.top))*height), pixelIndex, 0, DEFAULT_CPAINT_WINDOW_HEIGHT*DEFAULT_CPAINT_WINDOW_WIDTH/**DEFAULT_CPAINT_BIT_DEPTH*/, (rcClient.bottom - rcClient.top), (rcClient.right - rcClient.left), EDGE_BOUND(height, (rcClient.bottom - rcClient.top)), EDGE_BOUND(width, (rcClient.right - rcClient.left)));

                    if (!bInside)
                    {
                        bInside = true;

                        mouseX_old = mouseX;
                        mouseY_old = mouseY;
                    }
                    else
                    {
                        /*
                            //draw_line(pixels_foreground, DEFAULT_CPAINT_WINDOW_HEIGHT, DEFAULT_CPAINT_WINDOW_WIDTH, mouseX_old, mouseY_old, mouseX, mouseY, defaultForegroundPixelColor);
                         */
                        draw_line(pixels_foreground, height, width, mouseX_old /*+ EDGE_BOUND(width, (rcClient.right - rcClient.left))*/, mouseY_old /*+ EDGE_BOUND(height, (rcClient.bottom - rcClient.top))*/, mouseX /*+ EDGE_BOUND(width, (rcClient.right - rcClient.left))*/, mouseY /*+ EDGE_BOUND(height, (rcClient.bottom - rcClient.top))*/, defaultForegroundPixelColor);

                         /*   
                            //draw_line(pixels_foreground_edge_bounded, height, width, mouseX_old + EDGE_BOUND(width, (rcClient.right - rcClient.left)), mouseY_old + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)), mouseX + EDGE_BOUND(width, (rcClient.right - rcClient.left)), mouseY + EDGE_BOUND(height, (rcClient.bottom - rcClient.top)), defaultForegroundPixelColor);
                         */
                        draw_line(pixels_foreground_edge_bounded, height, width, (size_t)((((double)mouseX_old)/(rcClient.right - rcClient.left))*width), (size_t)((((double)mouseY_old)/(rcClient.bottom - rcClient.top))*height), (size_t)((((double)mouseX)/(rcClient.right - rcClient.left))*width), (size_t)((((double)mouseY)/(rcClient.bottom - rcClient.top))*height), defaultForegroundPixelColor);

                        mouseX_old = mouseX;
                        mouseY_old = mouseY;
                    }
                }
                else
                {    
                    //wsprintf(szTitle, L"%s: mouseX(%d), mouseY(%d) - (Inside, LBUTTON) - pixelIndex(%d, (%u, %u))", shortName, mouseX, mouseY, pixelIndex, 0, DEFAULT_CPAINT_WINDOW_HEIGHT*DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH);
                    wsprintf(szTitle, L"%s: mouseX(%d), mouseY(%d) - Inside, pixelIndex(%d, (%u, %u)) | Height: %u, Width: %u EDGE_BOUND(%u, %u)", shortName, mouseX, mouseY, pixelIndex, 0, DEFAULT_CPAINT_WINDOW_HEIGHT*DEFAULT_CPAINT_WINDOW_WIDTH/**DEFAULT_CPAINT_BIT_DEPTH*/, height, width, EDGE_BOUND(height, (rcClient.bottom - rcClient.top)), EDGE_BOUND(width, (rcClient.right - rcClient.left)));
                }

                SetWindowTextW(hwnd, szTitle);

                //memcpy(pixels, pixels_background, DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH));
                memcpy(pixels, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));
                //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                {
                    //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                    if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0*/) )
                    {
                        for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                        {
                            *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + j);
                        }
                    }
                }

                memcpy(pixels_edge_bounded, pixels_background, height*(width*DEFAULT_CPAINT_BIT_DEPTH));
                //for (size_t i = 0; i < (DEFAULT_CPAINT_WINDOW_HEIGHT*(DEFAULT_CPAINT_WINDOW_WIDTH*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                for (size_t i = 0; i < (height*(width*DEFAULT_CPAINT_BIT_DEPTH))/DEFAULT_CPAINT_BIT_DEPTH; i++)
                {
                    //if ( /*!*/(/**(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 0) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 1) == 0 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 2) == 0 &&*/ *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground)[i] + 3) == 0) )
                    if ( !(*(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground_edge_bounded)[i] + 0) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground_edge_bounded)[i] + 1) == 0x01 && *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground_edge_bounded)[i] + 2) == 0x01  /*&& *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground_edge_bounded)[i] + 3) == 0*/) )
                    {
                        for (unsigned char j = 0; j < DEFAULT_CPAINT_BIT_DEPTH; j++)
                        {
                            *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_edge_bounded)[i] + j) = *(((unsigned char(*)[DEFAULT_CPAINT_BIT_DEPTH])pixels_foreground_edge_bounded)[i] + j);
                        }
                    }
                }

                /*
                    The InvalidateRect function is called with NULL as the second parameter, which indicates that the entire client area of the window should be invalidated.
                    The third parameter, FALSE, indicates that the invalidated area should not be erased before the WM_PAINT message is sent.
                */
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }  
        }
        break;

        case WM_PAINT:
        {           
            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(bmi));
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            //bmi.bmiHeader.biWidth = DEFAULT_CPAINT_WINDOW_WIDTH;
            bmi.bmiHeader.biWidth = width;
            //bmi.bmiHeader.biHeight = (-1)*DEFAULT_CPAINT_WINDOW_HEIGHT;  // negative for top-down DIB
            bmi.bmiHeader.biHeight = (-1)*height;  // negative for top-down DIB
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = DEFAULT_CPAINT_BIT_DEPTH*8;
            bmi.bmiHeader.biCompression = BI_RGB;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // All painting occurs here, between BeginPaint and EndPaint.

            // Create a compatible device context for the bitmap
            HDC hdcMem = CreateCompatibleDC(hdc);
            //HBITMAP hBitmap = CreateCompatibleBitmap(hdc, DEFAULT_CPAINT_WINDOW_WIDTH, DEFAULT_CPAINT_WINDOW_HEIGHT);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);

            SelectObject(hdcMem, hBitmap);

            // Copy the pixel data to the bitmap
            //SetDIBits(hdcMem, hBitmap, 0, DEFAULT_CPAINT_WINDOW_HEIGHT, pixels, &bmi, DIB_RGB_COLORS);
            SetDIBits(hdcMem, hBitmap, 0, height, pixels, &bmi, DIB_RGB_COLORS);

            // Draw the bitmap
            //BitBlt(hdc, 0, 0, DEFAULT_CPAINT_WINDOW_WIDTH, DEFAULT_CPAINT_WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);
            BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

            // Clean up resources
            DeleteObject(hBitmap);
            DeleteDC(hdcMem);

            //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        // return 0;        
        break;
                
        default:
            // Let the default window procedure handle other messages
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}



