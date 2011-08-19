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

#ifndef WINTARGET_MODE_HPP__
#define WINTARGET_MODE_HPP__

#include "Common.hpp"
#include "WinGUIMode.hpp"

class WinTargetMode : public WinGUIMode
{
public:
    WinTargetMode(WinListView& elist_, WinMsgSender& sender_, Core& core_)
        : WinGUIMode(elist_, sender_, core_), hwnd_selected(NULL)
        {}
    virtual ~WinTargetMode(void) {}

    virtual void init(void) {}
    virtual void begin(HWND hwnd_)
        {
            gui_end = false;
            elist.set_last_item(_T("Select Target"));

            RECT rect;
            GetWindowRect(hwnd_, &rect);
            SetWindowPos(hwnd_, HWND_TOPMOST, rect.left, rect.top, 0, 0,
                         SWP_NOSIZE);
            SetCapture(hwnd_);
#ifdef UNDEF
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_NORMAL);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_IBEAM);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_SIZENESW);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_SIZENS);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_SIZENWSE);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_SIZEWE);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_HAND);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_APPSTARTING);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_NO);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_SIZEALL);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_UP);
            SetSystemCursor(LoadCursor(NULL, IDC_CROSS), OCR_WAIT);
#endif
        }

    virtual LRESULT proc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
        {
            switch (msg_)
            {
            case WM_MOUSEMOVE:
                if(!gui_end)
                {
                    // get a window by position
                    POINT pt;
                    GetCursorPos(&pt);
                    HWND hwnd_pt = WindowFromPoint(pt);
                    if (hwnd_pt)
                    {
                        ScreenToClient(hwnd_pt, &pt);
                        HWND hwnd_pt_child = ChildWindowFromPoint(hwnd_pt, pt);
                        if (hwnd_pt_child) { hwnd_pt = hwnd_pt_child; }
                    }

                    // if window is changed
                    if (hwnd_pt != hwnd_selected)
                    {
                        // clear pre-highlight
                        if (hwnd_selected) { bordering(hwnd_selected, 3); }

                        // changing window
                        hwnd_selected = hwnd_pt;
                        bordering(hwnd_selected, 3);

                        // if there is a window
                        TCHAR pszBuf[256];
                        HWND hDlg = GetParent(hwnd_);
                        if (hwnd_selected)
                        { GetClassName(hwnd_selected, pszBuf, 256); }
                        else { pszBuf[0] = '\0'; }
//                         SetDlgItemText(hDlg, IDC_CLASS, pszBuf);
                        OutputDebugStr("%S", pszBuf);
                    }
                }
                break;

            case WM_LBUTTONDOWN:
            {
                OutputDebugStr("buttondown\n");
                // selection finished
                gui_end = true;
//                 ReleaseCapture();
//                 SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
                POINT pt;
                GetCursorPos(&pt);
                HWND target_wnd = WindowFromPoint(pt);
                Str proc_name, class_name;
                uInt32 win_id;
                if (sender.get_tgt_info(target_wnd, proc_name, class_name,
                                        win_id))
                {
                    OutputDebugStr("clicked: %d, %d: %S %S %d\n", pt.x, pt.y,
                                   proc_name.c_str(), class_name.c_str(),
                                   win_id);
                    Str target_str = proc_name + _T(" / ") + class_name;
                    core.edit_action(elist.get_last_row(0),
                                     ACT_TARGET_ID, proc_name);
                    core.edit_action(elist.get_last_row(0),
                                     ACT_TARGET_CLASS, class_name);
                    elist.set_last_item(&(target_str.at(0)));
                    return 0;
                }
            }
            break;
            // cancel selection
            case WM_MOUSEWHEEL:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_MENUCHAR:
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_SYSDEADCHAR:
            case WM_SYSCOMMAND:
                gui_end = true;
                return 0;
            default: break;
            }
            return 0;
        }

    virtual void finish(HWND hwnd_)
        {
            bordering(hwnd_selected, 3);

            RECT rect;
            GetWindowRect(hwnd_, &rect);
            SetWindowPos(hwnd_, HWND_NOTOPMOST, rect.left, rect.top, 0, 0,
                         SWP_NOSIZE);
            OutputDebugStr("tgt_finish\n");
//             SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
            ReleaseCapture();
            InvalidateRect(elist.get_hwnd(), NULL, TRUE);
            UpdateWindow(elist.get_hwnd());
        }

protected:
    void bordering(HWND hwnd_, const Int32 width_)
        {
            // XXX need to be usable
#ifdef UNDEF
            if (hwnd_ == NULL || !IsWindow(hwnd_)) { return; }

            RECT rc;
            HDC hdc = GetWindowDC(hwnd_);
            GetWindowRect(hwnd_, &rc);
            OffsetRect(&rc, -rc.left, -rc.top);

            if (!IsRectEmpty(&rc))
            {
                PatBlt(hdc, rc.left, rc.top, rc.right - rc.left, width_,
                       DSTINVERT);
                PatBlt(hdc, rc.left, rc.bottom - width_, width_,
                       -(rc.bottom - rc.top - 2 * width_), DSTINVERT);
		PatBlt(hdc, rc.right - width_, rc.top + width_, width_,
                       rc.bottom - rc.top - 2 * width_, DSTINVERT);
		PatBlt(hdc, rc.right, rc.bottom - width_,
                       -(rc.right - rc.left), width_, DSTINVERT);
            }
            ReleaseDC(hwnd_, hdc);
#endif
        }

    HWND hwnd_selected;
};

#endif
