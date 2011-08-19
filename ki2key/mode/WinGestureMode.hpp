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

#ifndef WINGESTURE_MODE_HPP__
#define WINGESTURE_MODE_HPP__

#include "Common.hpp"
#include "WinGUIMode.hpp"

class WinGestureMode : public WinGUIMode
{
public:
    WinGestureMode(WinListView& elist_, WinMsgSender& sender_, Core& core_)
        : WinGUIMode(elist_, sender_, core_), sec_timer(0), collect_cmd(false)
        {}
    virtual ~WinGestureMode(void) {}

    virtual void init(void) {}
    virtual void begin(HWND hwnd_)
        {
            gui_end = collect_cmd = false;
            core.clear_cmd();
            editing_gesture = elist.get_last_row(0);
            elist.set_last_item(_T("Starting.."));
            SetCapture(hwnd_);
            timer_id = SetTimer(hwnd_, 1, 1000, 0);
            sec_timer = 0;
        }

    virtual LRESULT proc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
        {
            switch (msg_)
            {
            case WM_TIMER:
                if (!core.is_cmd_empty())
                {
                    Str last_str = core.pop_cmd();
                    core.edit_action(editing_gesture, ACT_GESTURE, last_str);
                    elist.set_last_item(&(last_str.at(0)));
                    // cleanup command stack
                    core.clear_cmd();
                    gui_end = collect_cmd = true;
                    return 0;
                }
                if (GESTURE_TIMEOUT > sec_timer++)
                {
                    // update timer
                    TCHAR buf[5];
                    _snwprintf_s(buf, 5, _T("%d"), GESTURE_TIMEOUT - sec_timer);
                    elist.set_last_item(buf);
                    break;
                }
                /* FALL THROUGH */
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_MOUSEWHEEL:
#ifndef DUMMY_DEVICE
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_CHAR:
#endif
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_MENUCHAR:
            case WM_SYSCHAR:
            case WM_SYSDEADCHAR:
            case WM_SYSCOMMAND:
                gui_end = true;
                break;
            default: break;
            }
            return 0;
        }

    virtual void finish(HWND hwnd_)
        {
            OutputDebugStr("gst_finish\n");
            sec_timer = 0;
            if (!collect_cmd) { elist.set_last_item(&(editing_gesture.at(0))); }
            editing_gesture.clear();
            KillTimer(hwnd_, timer_id);
            ReleaseCapture();
            InvalidateRect(elist.get_hwnd(), NULL, TRUE);
            UpdateWindow(elist.get_hwnd());
        }

protected:
    UINT_PTR timer_id;
    uInt32 sec_timer;
    bool collect_cmd;
    Str editing_gesture;
};

#endif
