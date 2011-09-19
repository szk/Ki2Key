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
#include "WinMsgSender.hpp"
#include <Winuser.h>
#include <windows.h>
#include <commctrl.h>
#include <Psapi.h>
#include <tlhelp32.h>

WinMsgSender::WinMsgSender(void)
    : MsgSender()
{
}

WinMsgSender::~WinMsgSender(void)
{
}

const bool WinMsgSender::init(void)
{
    return 0;
}

void WinMsgSender::activate(const Action& act_)
{
    BYTE kstats[256];
    DWORD stid, dtid;

    HWND target_wnd = FindWindow(act_.get_target_name().c_str(), NULL);

    if (act_.is_class_enabled())
    {
        target_wnd = FindWindowEx(target_wnd, NULL,
                                  act_.get_target_class().c_str(), NULL);
    }

    // if button, combobox or something have same class and ID, please decide
    // from captions or size.
    // ID = GetWindowWord(hwnd_, GWW_ID);

    // attach
    stid = GetCurrentThreadId();
    dtid = GetWindowThreadProcessId(target_wnd, NULL);
    AttachThreadInput(stid, dtid, TRUE);

    TCHAR window_txt[64];
    GetWindowText(target_wnd, window_txt, 64);
    OutputDebugStr("activate on %S, class: %S %d, cmd: %S, thread proc: %d\n",
                   window_txt, act_.get_target_class().c_str(),
                   act_.is_class_enabled(), act_.get_cmd(0).get_name().c_str(), dtid);

    HWND exist_wnd = GetFocus();
    SetFocus(target_wnd);
    // save current key state
    GetKeyboardState(kstats);
    // send input
    send_press(act_);

    if (act_.get_send_type() != ACT_SEND_HOLD) { send_release(act_); }

    // revert key state
    SetKeyboardState(kstats);

    // if you want to add modifier key, consider this way.
//    kstats[VK_CONTROL] = ctrlstat;

    // detouch
    AttachThreadInput(stid, dtid, FALSE);
    SetFocus(exist_wnd);
}

void WinMsgSender::deactivate(const Action& act_)
{
    if (act_.get_send_type() != ACT_SEND_HOLD) { return; }

    BYTE kstats[256];
    DWORD stid, dtid;
    HWND target_wnd = FindWindow(act_.get_target_name().c_str(), NULL);

    if (act_.is_class_enabled())
    {
        target_wnd = FindWindowEx(target_wnd, NULL,
                                  act_.get_target_class().c_str(), NULL);
    }

    // if button, combobox or something have same class and ID, please decide
    // from captions or size.
    // ID = GetWindowWord(hwnd_, GWW_ID);

    // attach
    stid = GetCurrentThreadId();
    dtid = GetWindowThreadProcessId(target_wnd, NULL);
    AttachThreadInput(stid, dtid, TRUE);

    TCHAR window_txt[64];
    GetWindowText(target_wnd, window_txt, 64);
    OutputDebugStr("activate on %S, class: %S %d, cmd: %S, thread proc: %d\n",
                   window_txt, act_.get_target_class().c_str(),
                   act_.is_class_enabled(), act_.get_cmd(0).get_name().c_str(), dtid);

    HWND exist_wnd = GetFocus();
    SetFocus(target_wnd);
    // save current key state
    GetKeyboardState(kstats);
    // send input
    send_release(act_);
    // revert key state
    SetKeyboardState(kstats);

    // if you want to add modifier key, consider this way.
//    kstats[VK_CONTROL] = ctrlstat;

    // detouch
    AttachThreadInput(stid, dtid, FALSE);
    SetFocus(exist_wnd);
}

const bool WinMsgSender::get_tgt_info(HWND hwnd_, Str& proc_name_,
                                      Str& class_name_, uInt32& win_id_)
{
    DWORD proc_id = NULL;
    GetWindowThreadProcessId(hwnd_, &proc_id);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) { return false; }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    for (BOOL result = Process32First(hSnapshot, &pe); TRUE == result;
         result = Process32Next(hSnapshot, &pe))
    {
        if (pe.th32ProcessID == proc_id) { proc_name_ = pe.szExeFile; break; }
    }

    // max classname length is 256
    TCHAR class_buf[256 + 1];
    GetClassName(hwnd_, class_buf, 256);
    class_name_ = class_buf;
    win_id_ = GetWindowLong(hwnd_, GWL_ID);
    OutputDebugStr("id: %d\n", GetWindowLong(hwnd_, GWL_ID));

    return true;
}

void WinMsgSender::send_press(const Action& act_)
{
    // if you want to add modifier key, consider this way.
//    BYTE ctrlstat = kstats[VK_CONTROL];

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = act_.get_cmd(0).get_code();
    input.ki.wScan = MapVirtualKey(act_.get_cmd(0).get_code(), 0);
    input.ki.dwFlags = 0;//KEYEVENTF_KEYDOWN;
    input.ki.time = 0;
    input.ki.dwExtraInfo = GetMessageExtraInfo();

    SendInput(1, &input, sizeof(INPUT));

    // or postmessage or sendmessage(you can customize)
//     PostMessage(target_wnd, WM_KEYDOWN, (WPARAM)VkKeyScan('a'), 0);
    // or
//     PostMessage(target_wnd, WM_CHAR, 'a', 0);
}

void WinMsgSender::send_release(const Action& act_)
{
    // if you want to add modifier key, consider this way.
//    BYTE ctrlstat = kstats[VK_CONTROL];

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = act_.get_cmd(0).get_code();
    input.ki.wScan = MapVirtualKey(act_.get_cmd(0).get_code(), 0);
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = GetMessageExtraInfo();

    SendInput(1, &input, sizeof(INPUT));
    // or postmessage or sendmessage(you can customize)
//     PostMessage(target_wnd, WM_KEYDOWN, (WPARAM)VkKeyScan('a'), 0);
    // or
//     PostMessage(target_wnd, WM_CHAR, 'a', 0);
}
