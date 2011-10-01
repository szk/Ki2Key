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

#ifndef WINCOMMAND_MODE_HPP__
#define WINCOMMAND_MODE_HPP__

#include "Common.hpp"
#include "WinGUIMode.hpp"

class WinCommandMode : public WinGUIMode
{
public:
    WinCommandMode(WinListView& elist_, WinMsgSender& sender_, Core& core_)
        : WinGUIMode(elist_, sender_, core_)
        {
        }
    virtual ~WinCommandMode(void) {}

    virtual void init(void) {}
    virtual void begin(HWND hwnd_)
        {
            gui_end = false;
            elist.set_last_item(_T("Press a Key"));
            SetCapture(hwnd_);

            RECT clientrc, cliprc;
            POINT topleft, rightbottom;
            GetClientRect(hwnd_, &clientrc);
            topleft.x = clientrc.left; topleft.y = clientrc.top;
            rightbottom.x = clientrc.right; rightbottom.y = clientrc.bottom;
            ClientToScreen(hwnd_, &topleft);
            ClientToScreen(hwnd_, &rightbottom);
            cliprc.left = topleft.x; cliprc.top  = topleft.y;
            cliprc.right = rightbottom.x; cliprc.bottom  = rightbottom.y;
            ClipCursor(&cliprc);
        }

#define CMD_NAME_SIZE 64
    virtual LRESULT proc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
        {
            TCHAR cmd_name[CMD_NAME_SIZE];
            uInt32 code = 0;
            switch (msg_)
            {
            case WM_LBUTTONDOWN:
                gui_end = true;
                _tcsncpy_s(cmd_name, _T("Left Click"), CMD_NAME_SIZE);
                core.edit_action(elist.get_last_row(0), ACT_CMD_MOUSE,
                                 cmd_name, CMD_MOUSECLICK_LEFT);
                break;
            case WM_MBUTTONDOWN:
                gui_end = true;
                _tcsncpy_s(cmd_name, _T("Middle Click"), CMD_NAME_SIZE);
                core.edit_action(elist.get_last_row(0), ACT_CMD_MOUSE,
                                 cmd_name, CMD_MOUSECLICK_MIDDLE);
                break;
            case WM_RBUTTONDOWN:
                gui_end = true;
                _tcsncpy_s(cmd_name, _T("Right Click"), CMD_NAME_SIZE);
                core.edit_action(elist.get_last_row(0), ACT_CMD_MOUSE,
                                 cmd_name, CMD_MOUSECLICK_RIGHT);
                break;
            case WM_MOUSEWHEEL:
                gui_end = true;
                if (GET_WHEEL_DELTA_WPARAM(wp_) > 0)
                {
                    _tcsncpy_s(cmd_name, _T("Wheel Up"), CMD_NAME_SIZE);
                    code = CMD_MOUSEWHEEL_UP;
                }
                else
                {
                    _tcsncpy_s(cmd_name, _T("Wheel Down"), CMD_NAME_SIZE);
                    code = CMD_MOUSEWHEEL_DOWN;
                }
                core.edit_action(elist.get_last_row(0), ACT_CMD_MOUSE,
                                 cmd_name, code);
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_MENUCHAR:
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_SYSDEADCHAR:
            case WM_SYSCOMMAND:
                gui_end = true;
                GetKeyNameText(static_cast<LONG>(lp_), cmd_name, CMD_NAME_SIZE);
                core.edit_action(elist.get_last_row(0), ACT_CMD_KEY,
                                 cmd_name, static_cast<const uInt32>(wp_));
                break;
            default:
                break;
            }
            if (gui_end) { show_cmdname(cmd_name); }
            return 0;
        }

    virtual void finish(HWND hwnd_)
        {
            OutputDebugStr("cmd_finish\n");
            ClipCursor(NULL);
            ReleaseCapture();
            InvalidateRect(elist.get_hwnd(), NULL, TRUE);
            UpdateWindow(elist.get_hwnd());
        }

protected:
    void show_cmdname(const TCHAR* name_)
        {
            if (_tcsnlen(name_, CMD_NAME_SIZE) != CMD_NAME_SIZE)
            { elist.set_last_item(name_); }
            else { elist.set_last_item(INIT_COMMAND); }
        }
};

#endif
