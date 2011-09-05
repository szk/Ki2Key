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
#include <windows.h>
#include <windowsx.h>
#include "WinAdvSet.hpp"
#include "resource.h"

WinAdvSet::WinAdvSet(void)
    : hwnd(0), hins(0), finish(false)
{
}

WinAdvSet::~WinAdvSet(void)
{
}

void WinAdvSet::init(HINSTANCE hins_)
{
    OutputDebugStr("initialize win advset\n");
    hins = hins_;
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = prx_wndproc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hins;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("AdvancedDlg");
    wcex.hIconSm = NULL;
    RegisterClassEx(&wcex);
}

HWND WinAdvSet::create(int width_, int height_, BYTE type_,
                       DWORD flags_, HWND parent_hwnd_)
{
    hwnd = CreateWindow(_T("AdvancedDlg"), _T("Advanced Settings"),
                        DS_MODALFRAME | WS_VISIBLE, CW_USEDEFAULT,
                        CW_USEDEFAULT, width_, height_, parent_hwnd_, NULL,
                        hins, (WNDPROC)prx_wndproc);

    hwnd_btn_ok = CreateWindow(_T("BUTTON"), _T("OK"),
                               WS_CHILD | WS_VISIBLE, 106, 182, 90, 24,
                               hwnd, (HMENU)1, hins, NULL);
    hwnd_btn_cancel = CreateWindow(_T("BUTTON"), _T("Cancel"),
                                   WS_CHILD | WS_VISIBLE, 206, 182, 90, 24,
                                   hwnd, (HMENU)2, hins, NULL);

    hwnd_gstcaption = CreateWindow(_T("STATIC"), _T("Gesture: "),
                                   WS_CHILD | WS_VISIBLE, 8, 8, 64, 20,
                                   hwnd, (HMENU)0, hins, NULL);
    hwnd_gst = CreateWindow(_T("STATIC"), _T("---"), WS_CHILD | WS_VISIBLE,
                            66, 8, 300, 20, hwnd, (HMENU)0, hins, NULL);
    hwnd_tgtcaption = CreateWindow(_T("STATIC"), _T("Target: "),
                                   WS_CHILD | WS_VISIBLE, 8, 28, 64, 20,
                                   hwnd, (HMENU)0, hins, NULL);
    hwnd_tgt = CreateWindow(_T("STATIC"), _T("---"), WS_CHILD | WS_VISIBLE,
                            66, 28, 300, 20, hwnd, (HMENU)0, hins, NULL);
    hwnd_cmdcaption = CreateWindow(_T("STATIC"), _T("Command: "),
                                   WS_CHILD | WS_VISIBLE, 8, 48, 64, 20, hwnd,
                                   (HMENU)0, hins, NULL);
    hwnd_cmd = CreateWindow(_T("STATIC"), _T("---"), WS_CHILD | WS_VISIBLE,
                            66, 48, 300, 20, hwnd, (HMENU)0, hins, NULL);

    hwnd_sndbox = CreateWindow(_T("BUTTON"), _T("Command sending mode"),
                               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                               10, 70, 284, 50, hwnd, (HMENU)0, hins, NULL);
    hwnd_rb_sndonce = CreateWindow(_T("BUTTON") , _T("Once"),
                                   WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                   8, 16, 90, 24, hwnd_sndbox,
                                   (HMENU)3, hins, NULL);
    hwnd_rb_sndrepeat = CreateWindow(_T("BUTTON") , _T("Repeat"),
                                     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                     100, 16, 90, 24, hwnd_sndbox,
                                     (HMENU)4, hins, NULL);
    hwnd_rb_sndhold = CreateWindow(_T("BUTTON") , _T("Hold"),
                                   WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                   192, 16, 90, 24, hwnd_sndbox,
                                   (HMENU)5, hins, NULL);
    hwnd_clsbox = CreateWindow(_T("BUTTON"), _T("Target selection mode"),
                               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                               10, 124, 284, 50, hwnd, (HMENU)0, hins, NULL);
    hwnd_rb_classuse = CreateWindow(_T("BUTTON") , _T("Use class"),
                                    WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                    8, 16, 90, 24, hwnd_clsbox,
                                    (HMENU)6, hins, NULL);
    hwnd_rb_classavoid = CreateWindow(_T("BUTTON") , _T("Avoid class"),
                                      WS_CHILD | WS_VISIBLE
                                      | BS_AUTORADIOBUTTON,
                                      100, 16, 90, 24, hwnd_clsbox,
                                      (HMENU)7, hins, NULL);
    // Setting fonts
    LOGFONT lf = { 0 };
    lf.lfHeight = 13;
    lf.lfWeight = FW_DONTCARE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    wcscpy_s(lf.lfFaceName, _T("Tahoma"));
    HFONT h_fn = CreateFontIndirect(&lf);
    SetWindowFont(hwnd_btn_ok, h_fn, TRUE);
    SetWindowFont(hwnd_btn_cancel, h_fn, TRUE);
    SetWindowFont(hwnd_gstcaption, h_fn, TRUE);
    SetWindowFont(hwnd_gst, h_fn, TRUE);
    SetWindowFont(hwnd_tgtcaption, h_fn, TRUE);
    SetWindowFont(hwnd_tgt, h_fn, TRUE);
    SetWindowFont(hwnd_cmdcaption, h_fn, TRUE);
    SetWindowFont(hwnd_cmd, h_fn, TRUE);
    SetWindowFont(hwnd_sndbox, h_fn, TRUE);
    SetWindowFont(hwnd_rb_sndonce, h_fn, TRUE);
    SetWindowFont(hwnd_rb_sndrepeat, h_fn, TRUE);
    SetWindowFont(hwnd_rb_sndhold, h_fn, TRUE);
    SetWindowFont(hwnd_clsbox, h_fn, TRUE);
    SetWindowFont(hwnd_rb_classuse, h_fn, TRUE);
    SetWindowFont(hwnd_rb_classavoid, h_fn, TRUE);

    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(LONG_PTR)prx_wndproc);
    SetProp(hwnd, _T("AdvSettings"), this);

    return hwnd;
}

const bool WinAdvSet::show(HWND hwnd_, Core& core_, const Str& gesture_name_)
{
    if (hwnd == 0) { return 1; }
    MSG msg;
    EnableWindow(hwnd_, FALSE);
    init_setting(core_.get_action(gesture_name_));

    while (!finish)
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        switch (result)
        {
        case 0: break;
        case 1: apply_setting(core_, gesture_name_);
        case 2: // FALL THROUGH
        default:
            DestroyWindow(hwnd);
        }
    }

    result = 0;
    finish = false;

    EnableWindow(hwnd_, TRUE);
    BringWindowToTop(hwnd_);

    return true;
}

LRESULT CALLBACK WinAdvSet::prx_wndproc(HWND hwnd_, UINT msg_,
                                        WPARAM wp_, LPARAM lp_)
{
    WinAdvSet* target = static_cast<WinAdvSet*>(::GetProp(hwnd_,
                                                          _T("AdvSettings")));
    if (target == NULL) { return ::DefWindowProc(hwnd_, msg_, wp_, lp_); }
    return target->wndproc(hwnd_, msg_, wp_, lp_);
}

LRESULT WinAdvSet::wndproc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    switch (msg_)
    {
    case WM_COMMAND:
        switch (LOWORD(wp_))
        {
        case 1: // OK
            result = 1;
            break;
        case 2: // Cancel
            result = 2;
            break;
        }

        return 0;

    case WM_DESTROY:
        RemoveProp(hwnd, _T("AdvSettings"));
        finish = true;
        break;
    }
    return DefWindowProc(hwnd_, msg_, wp_, lp_);
}

void WinAdvSet::init_setting(const Action& act_)
{
    SetWindowText(hwnd_gst, act_.get_gesture().c_str());
    SetWindowText(hwnd_tgt, act_.get_target_name().c_str());

    if (act_.get_cmd_size())
    { SetWindowText(hwnd_cmd, act_.get_cmd(0).get_name().c_str()); }
    else { SetWindowText(hwnd_cmd, _T("---")); }

    Button_SetCheck(hwnd_rb_sndonce, BST_UNCHECKED);
    Button_SetCheck(hwnd_rb_sndrepeat, BST_UNCHECKED);
    Button_SetCheck(hwnd_rb_sndhold, BST_UNCHECKED);
    Button_SetCheck(hwnd_rb_classuse, BST_UNCHECKED);
    Button_SetCheck(hwnd_rb_classavoid, BST_UNCHECKED);

    if (act_.get_send_type() == ACT_SEND_HOLD)
    { Button_SetCheck(hwnd_rb_sndhold, BST_CHECKED); }
    else if (act_.get_send_type() == ACT_SEND_REPEAT)
    { Button_SetCheck(hwnd_rb_sndrepeat, BST_CHECKED); }
    else // SEND_ONCE or other (default)
    { Button_SetCheck(hwnd_rb_sndonce, BST_CHECKED); }

    if (act_.is_avoid_class())
    { Button_SetCheck(hwnd_rb_classavoid, BST_CHECKED); }
    else { Button_SetCheck(hwnd_rb_classuse, BST_CHECKED); }
}

void WinAdvSet::apply_setting(Core& core_, const Str& gesture_name_)
{
    // Check keystroke sending radiobuttons
    if (Button_GetCheck(hwnd_rb_sndonce))
    { core_.edit_action(gesture_name_, ACT_SEND_TYPE, _T(SEND_ONCE)); }
    else if (Button_GetCheck(hwnd_rb_sndrepeat))
    { core_.edit_action(gesture_name_, ACT_SEND_TYPE, _T(SEND_REPEAT)); }
    else if (Button_GetCheck(hwnd_rb_sndhold))
    { core_.edit_action(gesture_name_, ACT_SEND_TYPE, _T(SEND_HOLD)); }
    // Check class using radiobuttons
    if (Button_GetCheck(hwnd_rb_classuse))
    { core_.edit_action(gesture_name_, ACT_AVOID_CLASS, _T(NO)); }
    else if (Button_GetCheck(hwnd_rb_classavoid))
    { core_.edit_action(gesture_name_, ACT_AVOID_CLASS, _T(YES)); }
}
