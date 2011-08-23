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

#ifndef WINLISTVIEW_HPP__
#define WINLISTVIEW_HPP__

#include "Common.hpp"
#include <windows.h>

#define ITEM_LEN 256

class WinGUIMode;
class WinGestureMode;
class WinCommandMode;
class WinTargetMode;

class WinListView
{
public:
    WinListView(void);
    virtual ~WinListView(void);

    HWND WINAPI create_lvwindow(HWND hwnd_p_, HMENU id_);
    void WINAPI add_item(const Str& gesture_, const Str& target_,
                         const Str& command_);
    void WINAPI del_selected_item(void);
    UserRequest CALLBACK lvn_proc(WPARAM wp_, LPARAM lp_);
    LRESULT CALLBACK draw_proc(HWND hwnd_, LPARAM lp_, std::stack<WinGUIMode*> gui_mode_);

    Str get_selected_row(UINT col_);
    Str get_last_row(UINT col_);
    void WINAPI set_last_item(const Str& str_);
    void WINAPI revert(void);
    HWND get_hwnd(void);

protected:
    static LRESULT CALLBACK prx_listproc(HWND hwnd_, UINT msg_, WPARAM wp_,
                                         LPARAM lp_);
    static LRESULT CALLBACK prx_headerproc(HWND hwnd_, UINT msg_, WPARAM wp_,
                                           LPARAM lp_);
    LRESULT ListViewProc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_);
    LRESULT HeaderProc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_);

    const bool update_selection(LPARAM lp_);

    HWND hwnd, hwnd_header, hwnd_parent;
    WNDPROC org_listproc, org_headerproc;
    TCHAR szText[ITEM_LEN]; // Text to modify
    int item_id, subitem_id; // Currently selected item and subitem index
};

#endif
