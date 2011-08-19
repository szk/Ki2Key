/*
 * Copyright (c) 2011, Tatsuhiko Suzuki
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdafx.h"
#include "ki2key.h"
#include <commctrl.h>
#include <richedit.h>
#include <shellapi.h>
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "resource.h"

#include "GLView.hpp"
#include "WinListView.hpp"
#include "WinMsgSender.hpp"
#include "core/Setting.hpp"
#include "core/Core.hpp"

#include "mode/WinTargetMode.hpp"
#include "mode/WinCommandMode.hpp"
#include "mode/WinGestureMode.hpp"

#define MAX_LOADSTRING 100
#define T_BUFFER 128
#define MAX_GESTURES 64

// for GUI
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 350
#define SCENEVIEW_WIDTH 320
#define SCENEVIEW_HEIGHT 240
#define WINDOW_FONT _T("Tahoma")

#define IDC_COMBO1 0x1000
#define IDC_LIST1 0x1001
#define IDC_BTN_BINDADD 0x2001
#define IDC_BTN_BINDDEL 0x2002
#define IDC_BTN_BINDADV 0x2003

#define WM_NOTIFYICON (WM_USER + 100)

NOTIFYICONDATA tasktray_icon;
LPTSTR sensor_mode[] = { _T("OFF"),
                         _T("ON: Automatic"), };
#define CB_SENSOR_OFF 0
#define CB_SENSOR_ON 1

const Str INIT_GESTURE = _T("N/A");
const Str INIT_TARGET = _T("Undefined");
const Str INIT_COMMAND = _T("None");

// Global Variables:
// current instance
HINSTANCE hins;
// The title bar text
TCHAR szTitle[MAX_LOADSTRING];

// the main window class name
TCHAR szWindowClass[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK base_proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK adv_proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK about_proc(HWND, UINT, WPARAM, LPARAM);

Str lasterr_str;

Core core;
GLView glwin(640, 480);
WinListView elist;
WinMsgSender sender;
Setting setting;

HWND hwnd_base, hwnd_cbcaption, hwnd_combo, hwnd_lview, hwnd_sview,
    hwnd_bindadd, hwnd_bindadv, hwnd_binddel;

WinTargetMode* tgt_mode = NULL;
WinGestureMode* gst_mode = NULL;
WinCommandMode* cmd_mode = NULL;

std::stack<WinGUIMode*> gui_mode;

void OutputDebugStr( LPCSTR pszFormat, ...)
{
    va_list argp;
    char pszBuf[256];
    va_start(argp, pszFormat);
    vsprintf_s(pszBuf, pszFormat, argp);
    va_end(argp);
    OutputDebugStringA((LPCSTR)pszBuf);
}

int APIENTRY _tWinMain(HINSTANCE hins_, HINSTANCE hpinst_,
                       LPTSTR lpcl_, int cmd_)
{
    UNREFERENCED_PARAMETER(hpinst_);
    UNREFERENCED_PARAMETER(lpcl_);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hins_, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    StringCchCat(szTitle, MAX_LOADSTRING, _T(" "));
    StringCchCat(szTitle, MAX_LOADSTRING, _T(APPVERSION));
    LoadString(hins_, IDC_KI2KEY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hins_);

    // Perform application initialization:
    if (!InitInstance (hins_, cmd_)) { return FALSE; }

    hAccelTable = LoadAccelerators(hins_, MAKEINTRESOURCE(IDC_KI2KEY));

    if (!core.init(&glwin, &setting, &sender))
    {
        MessageBox(NULL, (LPCWSTR)lasterr_str.c_str(),
                   _T("core init error"), MB_OK);
        return false;
    }

    for (size_t i = 0; core.get_action_size() > i; ++i)
    {
        const Action& act = core.get_action(i);
        Str gesture_str = act.get_gesture();
        const Str& target_name_str = act.get_target_name();
        const Str& target_class_str = act.get_target_class();
        Str target_str = target_name_str + Str(_T(" / ")) + target_class_str;
        Str cmd_str;
        for (size_t i = 0; act.get_cmd_size() > i; ++i)
        {
            const Command& cmd = act.get_cmd(i);
            OutputDebugStr("%S/%d ", cmd.get_name().c_str(), cmd.get_code());
            cmd_str = cmd.get_name();
        }

        elist.add_item(gesture_str, target_str, cmd_str);
    }

    tgt_mode = new WinTargetMode(elist, sender, core);
    gst_mode = new WinGestureMode(elist, sender, core);
    cmd_mode = new WinCommandMode(elist, sender, core);

// Main message loop:
    bool finish = false;
    while (!finish)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                switch (msg.message)
                {
                case WM_MOUSEWHEEL: // Through the event, this is dirty XXX
                    if (gui_mode.size() != 0)
                    {
                        SendMessage(hwnd_base, msg.message, msg.wParam,
                                    msg.lParam);
                        continue;
                    }
                    break;
                case WM_KEYDOWN:
                    switch (msg.wParam)
                    {
                    case VK_LEFT:
                    case VK_RIGHT:
                    case VK_UP:
                    case VK_DOWN:
                        if (gui_mode.size() != 0)
                        {
                            SendMessage(hwnd_base, msg.message, msg.wParam,
                                        msg.lParam);
                            continue;
                        }
                        break;
#ifdef DUMMY_DEVICE
                    case VK_RETURN:
                        core.set_device(DEV_START);
                        core.set_device(DEV_USER_ON);
                        break;
                    case VK_ESCAPE: core.set_device(DEV_END);
                        break;
                    case VK_SPACE: core.set_device(DEV_FORWARD);
                        break;
                    case VK_SHIFT: core.set_device(DEV_BACK);
                        break;
                    case 0x45:
                        core.set_device(DEV_TILE_ON);
                        core.set_device(DEV_HAND_ON); break; // E
                    case 0x51:
                        core.set_device(DEV_HAND_OFF);
                        core.set_device(DEV_TILE_OFF); break; // Q
                    case 0x57: core.set_device(DEV_UP); break; // W
                    case 0x41: core.set_device(DEV_LEFT); break; // A
                    case 0x53: core.set_device(DEV_DOWN); break; // S
                    case 0x44: core.set_device(DEV_RIGHT); break; // D
#endif // DUMMY_DEVICE
                    }
                    break;
                case WM_CLOSE:
                case WM_QUIT:
                    finish = true;
                    break;
                default: break;
                }
                DispatchMessage(&msg);
            }
        }
        if (core.tick_with_wait() != 0)
        { MessageBox(NULL, _T("tick is null"), _T("datum"), MB_OK); }
        Sleep(1);
    }
    core.finish();
    glwin.destroy_glwindow();

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hins_)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = base_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hins_;
    wcex.hIcon = LoadIcon(hins_, MAKEINTRESOURCE(IDI_KI2KEY));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hins_, int cmd_)
{
    hins = hins_; // Store instance handle in our global variable

    hwnd_base = CreateWindow(szWindowClass, szTitle,
                             WS_DLGFRAME | WS_CAPTION | WS_SYSMENU
                             | WS_MINIMIZEBOX,
                             CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                             NULL, NULL, hins, NULL);

    if (!hwnd_base) { return FALSE; }

    // 3D Monitor window
    hwnd_sview = glwin.create_glwindow(_T("minimal"), 8, 8, SCENEVIEW_WIDTH,
                                       SCENEVIEW_HEIGHT,
                                       PFD_TYPE_RGBA, 0, hins, hwnd_base);
    // sensor settings menu
    hwnd_cbcaption = CreateWindow(_T("STATIC"), _T("Sensor mode: "),
                                  WS_VISIBLE | WS_CHILD, 8, 260, 70, 24,
                                  hwnd_base, (HMENU)IDC_STATIC, hins, 0);
    hwnd_combo = CreateWindowEx(0, WC_COMBOBOX, 0, WS_VISIBLE | WS_CHILD
                                | WS_VSCROLL | CBS_DROPDOWNLIST
                                | CBS_HASSTRINGS, 78, 256, 250, 96,
                                hwnd_base, (HMENU)IDC_COMBO1, hins, 0);
    for (size_t i = 0; 2 > i; ++i)
    { SendMessage(hwnd_combo, CB_ADDSTRING, 0L, (LPARAM)(sensor_mode[i])); }
    SendMessage(hwnd_combo, CB_SETCURSEL, CB_SENSOR_ON, 0L);

    // Bindings list
    hwnd_lview = elist.create_lvwindow(hwnd_base, (HMENU)IDC_LIST1);

    // Editing bindings buttons
    hwnd_bindadd = CreateWindow(_T("BUTTON") , _T("Add"), WS_CHILD
                                | WS_VISIBLE | BS_PUSHBUTTON, 336, 292, 90, 24,
                                hwnd_base, (HMENU)IDC_BTN_BINDADD, hins, NULL);
    hwnd_binddel = CreateWindow(_T("BUTTON") , _T("Delete"), WS_CHILD
                                | WS_VISIBLE | BS_PUSHBUTTON, 436, 292, 90, 24,
                                hwnd_base, (HMENU)IDC_BTN_BINDDEL, hins, NULL);
    hwnd_bindadv = CreateWindow(_T("BUTTON") , _T("Advanced"), WS_CHILD
                                | WS_VISIBLE | BS_PUSHBUTTON, 536, 292, 90, 24,
                                hwnd_base, (HMENU)IDC_BTN_BINDADV, hins, NULL);

    // Setting fonts
    LOGFONT lf;
    memset(&lf, NULL, sizeof(lf));
    lf.lfHeight = 13;
    lf.lfWeight = FW_DONTCARE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    wcscpy_s(lf.lfFaceName, WINDOW_FONT);
    HFONT h_fn = CreateFontIndirect(&lf);
    SendMessage(hwnd_lview, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));
    SendMessage(hwnd_combo, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));
    SendMessage(hwnd_cbcaption, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));
    SendMessage(hwnd_bindadd, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));
    SendMessage(hwnd_binddel, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));
    SendMessage(hwnd_bindadv, WM_SETFONT, (WPARAM)h_fn, MAKELPARAM(true, 0));

    tasktray_icon.cbSize = sizeof(NOTIFYICONDATA);
    tasktray_icon.uID    = 108;
    tasktray_icon.hWnd   = hwnd_base;
    tasktray_icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tasktray_icon.hIcon  = LoadIcon(GetModuleHandle(NULL),
                                    MAKEINTRESOURCE(IDI_SMALL));
    tasktray_icon.uCallbackMessage = WM_NOTIFYICON;
    lstrcpy(tasktray_icon.szTip, _T(APPNAME));
    for (size_t tasktray_try = 5; !Shell_NotifyIcon(NIM_ADD, &tasktray_icon)
             && tasktray_try > 0; tasktray_try--)
    { Sleep(100); }

    ShowWindow(hwnd_base, cmd_);
    UpdateWindow(hwnd_base);

    return TRUE;
}

// for changing string and cell color
LRESULT CALLBACK lvcd_proc(LPARAM lp_)
{
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lp_;

    switch(lplvcd->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        SetWindowLong( hwnd_base, DWL_MSGRESULT,(long)CDRF_NOTIFYITEMDRAW);
        return CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:
        SetWindowLong( hwnd_base, DWL_MSGRESULT,(long)CDRF_NOTIFYITEMDRAW);
        return CDRF_NOTIFYSUBITEMDRAW;
        break;

    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
    {
        const bool mode_ready = (gui_mode.size() != 0);
        switch(lplvcd->iSubItem)
        {
        case 0:
            if (mode_ready
                && typeid(WinGestureMode) == typeid(*(gui_mode.top())))
            { lplvcd->clrText = LV_BK_RGB; lplvcd->clrTextBk = LV_GST_RGB; }
            else
            { lplvcd->clrText = LV_GST_RGB; lplvcd->clrTextBk = LV_BK_RGB; }
            return CDRF_NEWFONT;
            break;
        case 1:
            if (mode_ready
                && typeid(WinTargetMode) == typeid(*(gui_mode.top())))
            { lplvcd->clrText = LV_BK_RGB; lplvcd->clrTextBk = LV_TGT_RGB; }
            else
            { lplvcd->clrText = LV_TGT_RGB; lplvcd->clrTextBk = LV_BK_RGB; }
            return CDRF_NEWFONT;
            break;
        case 2:
            if (mode_ready
                && typeid(WinCommandMode) == typeid(*(gui_mode.top())))
            { lplvcd->clrText = LV_BK_RGB; lplvcd->clrTextBk = LV_CMD_RGB; }
            else
            { lplvcd->clrText = LV_CMD_RGB; lplvcd->clrTextBk = LV_BK_RGB; }
            return CDRF_NEWFONT;
            break;
        }
    }
    }
    return CDRF_DODEFAULT;
}

LRESULT CALLBACK base_proc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    if (msg_ == WM_NOTIFY)
    {
        if (LPNMLISTVIEW pnm = reinterpret_cast<LPNMLISTVIEW>(lp_))
        {
            if (pnm->hdr.hwndFrom == hwnd_lview
                && pnm->hdr.code == NM_CUSTOMDRAW)
            { return lvcd_proc(lp_); }
        }
    }

    if (gui_mode.size() != 0)
    {
        if (gui_mode.top()->is_end() == false)
        { return gui_mode.top()->proc(hwnd_, msg_, wp_, lp_); }
        else
        {
            WinGUIMode* mode = gui_mode.top();
            gui_mode.pop();
            mode->finish(hwnd_);
            return 0;
        }
    }

    switch (msg_)
    {
    case WM_MOUSEWHEEL:
        OutputDebugStr("base wheel");
        break;
    case WM_CREATE:
        InitCommonControls();
        break;
    case WM_INITDIALOG:
        break;
    case WM_SYSCOMMAND:
        if (wp_ != SC_KEYMENU)
        { return DefWindowProc(hwnd_, msg_, wp_, lp_); }
        if (wp_ == SC_MINIMIZE)
        {
            OutputDebugStr("minimized");
            ShowWindow(hwnd_, SW_HIDE);
        }
        break;
    case WM_COMMAND:
        // Parse the menu selections:
        switch (LOWORD(wp_))
        {
        case IDM_ABOUT:
            DialogBox(hins, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd_, about_proc);
            break;
        case IDM_EXIT:
            DestroyWindow(hwnd_);
            break;
        case IDC_COMBO1:
            if (HIWORD(wp_) == CBN_SELCHANGE)
            {
                switch (SendMessage(hwnd_combo, CB_GETCURSEL, 0, 0))
                {
                case CB_SENSOR_OFF: core.activate_sensor(false); break;
                case CB_SENSOR_ON: core.activate_sensor(true); break;
                }
            }
            break;
        case IDC_BTN_BINDADD:
            elist.add_item(core.add_action(), INIT_TARGET, INIT_COMMAND);
            break;
        case IDC_BTN_BINDDEL:
            if (!core.remove_action(elist.get_selected_row(0))) { break; }
            elist.del_selected_item();
            break;
        case IDC_BTN_BINDADV:
            MessageBox(hwnd_, _T("Under construction"), _T("Advanced"), MB_OK);
            break;
        default:
            return DefWindowProc(hwnd_, msg_, wp_, lp_);
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd_, &ps);
        EndPaint(hwnd_, &ps);
        break;
    }
    case WM_SIZE:
        return 0;
        break;

    case WM_NOTIFY:
    {
        LPNMHDR lpnmHdr = (LPNMHDR)lp_;
        if (lpnmHdr->idFrom == IDC_LIST1)
        {
            bool pm = false;
            switch (elist.lvn_proc(wp_,lp_))
            {
            case UR_GUI_SET_GESTURE: gui_mode.push(gst_mode); pm = true; break;
            case UR_GUI_SET_TARGET: gui_mode.push(tgt_mode); pm = true; break;
            case UR_GUI_SET_COMMAND: gui_mode.push(cmd_mode); pm = true; break;
            case UR_GUI_DO_ACTION:
                sender.activate_alt(core.get_action(elist.get_selected_row(0)));
                break;
            }
            if (pm) { gui_mode.top()->begin(hwnd_); }
        }
    }

    case WM_NOTIFYICON:
        switch(lp_)
        {
        case WM_RBUTTONDOWN:
            ShellAbout(hwnd_, _T(APPNAME), _T(CREDIT),
                       LoadIcon(hins, MAKEINTRESOURCE(IDI_KI2KEY)));
            break;
        case WM_LBUTTONDBLCLK:
            ShowWindow(hwnd_, SW_RESTORE);
            break;
        }
        break;
    case WM_DESTROY:
        elist.revert();
        // kill tasktray button
        for (size_t tasktray_try = 5;
             !Shell_NotifyIcon(NIM_DELETE, &tasktray_icon) && tasktray_try > 0;
             tasktray_try--)
        { Sleep(100); }
        ShowWindow(hwnd_, SW_HIDE);
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd_, msg_, wp_, lp_);
}

// Message handler for advanced window procedure
INT_PTR CALLBACK adv_proc(HWND hDlg, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    UNREFERENCED_PARAMETER(lp_);
    switch (msg_)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wp_) == IDOK || LOWORD(wp_) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wp_));
            return (INT_PTR)TRUE;
        }
        break;
    default:
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK about_proc(HWND hDlg, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    UNREFERENCED_PARAMETER(lp_);
    switch (msg_)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wp_) == IDOK || LOWORD(wp_) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wp_));
            return (INT_PTR)TRUE;
        }
        break;
    default:
        break;
    }
    return (INT_PTR)FALSE;
}
