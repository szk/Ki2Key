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
#include "WinListView.hpp"

#include <windows.h>
#include <commctrl.h>

LPTSTR titles[] = { _T("Gesture"), _T("Process / Class"), _T("Command"), };
uInt32 title_width[] = { 50, 140, 100, };

WinListView::WinListView(void)
    : item_id(-1), subitem_id(0)
{
}

WinListView::~WinListView(void)
{
}

// ListVIew creation method
HWND WINAPI WinListView::create_lvwindow(HWND hwnd_p_, HMENU id_)
{
    HWND hwndStrListView;
    LVCOLUMN lvCol;

    hwnd_parent = hwnd_p_;
    // First create window through CreateWindowEx
    hwndStrListView = CreateWindowEx(0L, WC_LISTVIEW, NULL,
                                     WS_CHILD | WS_BORDER | LVS_REPORT
                                     | LVS_SHOWSELALWAYS, 336, 8, 290, 274,
                                     hwnd_parent, id_, GetModuleHandle(NULL),
                                     NULL);
    if (!hwndStrListView) { return NULL; }

    // Add columns
    for (int i = 0; i < 3 ; i++)
    {
        lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
        lvCol.cx = title_width[i];
        lvCol.cchTextMax = 10;
        lvCol.pszText = titles[i];

        ListView_InsertColumn(hwndStrListView, i, &lvCol);
    }

    // Set extended listview styles like FullRowSelect and GridLines
    ListView_SetExtendedListViewStyleEx(hwndStrListView, 0, LVS_EX_FULLROWSELECT
                                        | LVS_EX_GRIDLINES);
    // Subclass control(List control)
    org_listproc = (WNDPROC)SetWindowLong(hwndStrListView, GWL_WNDPROC,
                                          (LONG)(LONG_PTR)prx_listproc);
    // Subclass control(Header)
    hwnd_header = GetDlgItem(hwndStrListView, 0);
    org_headerproc = (WNDPROC)(GetWindowLong(hwnd_header, GWL_WNDPROC));
    (WNDPROC)SetWindowLong(hwnd_header, GWL_WNDPROC,
                           (LONG)(LONG_PTR)prx_headerproc);
    SetProp(hwnd_header, _T("LHeader"), this);

    // Display it
    ShowWindow(hwndStrListView, SW_SHOWDEFAULT);

    // Store its handle onto global variable
    hwnd = hwndStrListView;
    SetProp(hwnd, _T("ListView"), this);
    return hwndStrListView;
}

// Method for add items
void WINAPI WinListView::add_item(const Str& gesture_,
                                  const Str& target_, const Str& command_)
{
    if (gesture_.empty() || target_.empty() || command_.empty()) { return; }

    LVITEM lvItem; // Item information
    INT iLastIndex = 0; // Last item added
    Str gesture = gesture_, target = target_, command = command_;

    // Get total item count. The last one is the item count
    iLastIndex = ListView_GetItemCount(hwnd);

    // Set item properties
    lvItem.mask = LVIF_TEXT;
    lvItem.cchTextMax = lstrlen(&(gesture.at(0)));
    lvItem.pszText = &(gesture.at(0));
    lvItem.iItem = iLastIndex;
    lvItem.iSubItem = 0;

    // Insert a item
    ListView_InsertItem(hwnd, &lvItem);

    // autoresize last column (for scroll bar)
    ListView_SetColumnWidth(hwnd, 2, -2);

    // and set subitems text
    ListView_SetItemText(hwnd, iLastIndex, 1, &(target.at(0)));
    ListView_SetItemText(hwnd, iLastIndex, 2, &(command.at(0)));
}

void WINAPI WinListView::del_selected_item(void)
{
    while(1)
    {
        int nItem = ListView_GetNextItem(hwnd, -1,
                                         LVNI_ALL | LVNI_SELECTED);
        if (nItem == -1) { break; }
        ListView_DeleteItem(hwnd, nItem);
    }
}

// Message processing for parent window (main window)
UserRequest CALLBACK WinListView::lvn_proc(WPARAM wp_, LPARAM lp_)
{
    LPNMHDR lpnmHdr = (LPNMHDR)lp_;

    switch(lpnmHdr->code)
    {
    case NM_CLICK:
        update_selection(lp_);
        break;
    case NM_RCLICK:
        if (update_selection(lp_)) { return UR_GUI_DO_ACTION; }
        break;
    case NM_DBLCLK:
        if (!update_selection(lp_)) { return UR_NONE; }
        ListView_SetItemState(hwnd, item_id, 0, LVIS_SELECTED);
        if (subitem_id == 0) { return UR_GUI_SET_GESTURE; }
        else if (subitem_id == 1) { return UR_GUI_SET_TARGET; }
        else if (subitem_id == 2) { return UR_GUI_SET_COMMAND; }
        break;
    default:
        break;
    }

    return UR_NONE;
}

// for changing string and cell color
LRESULT CALLBACK WinListView::draw_proc(HWND hwnd_, LPARAM lp_,
                                        std::stack<WinGUIMode*> gui_mode_)
{
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lp_;

    switch(lplvcd->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        SetWindowLong(hwnd_, DWL_MSGRESULT,(long)CDRF_NOTIFYITEMDRAW);
        return CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:
        SetWindowLong(hwnd_, DWL_MSGRESULT,(long)CDRF_NOTIFYITEMDRAW);
        return CDRF_NOTIFYSUBITEMDRAW;
        break;

    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
    {
        const bool mode_ready = (gui_mode_.size() != 0);
        switch(lplvcd->iSubItem)
        {
        case 0:
            if (mode_ready
                && typeid(WinGestureMode) == typeid(*(gui_mode_.top())))
            { lplvcd->clrText = LV_BK_RGB; lplvcd->clrTextBk = LV_GST_RGB; }
            else
            { lplvcd->clrText = LV_GST_RGB; lplvcd->clrTextBk = LV_BK_RGB; }
            return CDRF_NEWFONT;
            break;
        case 1:
            if (mode_ready
                && typeid(WinTargetMode) == typeid(*(gui_mode_.top())))
            { lplvcd->clrText = LV_BK_RGB; lplvcd->clrTextBk = LV_TGT_RGB; }
            else
            { lplvcd->clrText = LV_TGT_RGB; lplvcd->clrTextBk = LV_BK_RGB; }
            return CDRF_NEWFONT;
            break;
        case 2:
            if (mode_ready
                && typeid(WinCommandMode) == typeid(*(gui_mode_.top())))
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

Str WinListView::get_selected_row(UINT col_)
{
    while(1)
    {
        int nItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
        if (nItem == -1) { break; }
        else
        {
            ListView_GetItemText(hwnd, nItem, col_, szText, ITEM_LEN);
            return Str(szText);
        }
    }
    return Str();
}

Str WinListView::get_last_row(UINT col_)
{
    ListView_GetItemText(hwnd, item_id, col_, szText, ITEM_LEN);
    return Str(szText);
}

void WINAPI WinListView::set_last_item(const Str& str_)
{
    Str item_name = str_;
    ListView_SetItemText(hwnd, item_id, subitem_id, &(item_name.at(0)));
}

// Function used to set original WndProc for listview
void WINAPI WinListView::revert(void)
{
    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)org_listproc);
}

HWND WinListView::get_hwnd(void)
{
    return hwnd;
}

LRESULT CALLBACK WinListView::prx_listproc(HWND hwnd_, UINT msg_,
                                           WPARAM wp_, LPARAM lp_)
{
    WinListView* found = static_cast<WinListView*>(GetProp(hwnd_,
                                                           _T("ListView")));
    if(found == NULL ) { return DefWindowProc(hwnd_, msg_, wp_, lp_); }
    return found->ListViewProc(hwnd_, msg_, wp_, lp_);
}

LRESULT CALLBACK WinListView::prx_headerproc(HWND hwnd_, UINT msg_,
                                             WPARAM wp_, LPARAM lp_)
{
    WinListView* found = static_cast<WinListView*>(GetProp(hwnd_,
                                                           _T("LHeader")));
    if (found == NULL ) { return DefWindowProc(hwnd_, msg_, wp_, lp_); }
    return found->HeaderProc(hwnd_, msg_, wp_, lp_);
}

// ListView subclass procedure
LRESULT WinListView::ListViewProc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    switch (msg_)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_MENUCHAR:
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
        SendMessage(hwnd_parent, msg_, wp_, lp_);
        break;
    case WM_PAINT:
        // autoresize last column (for scroll bar)
        ListView_SetColumnWidth(hwnd, 2, -2);
        break;
    case WM_NOTIFY:
        // Don't change the header width
        if (NMHDR* nmh = reinterpret_cast<NMHDR*>(lp_))
        {
            switch (nmh->code)
            {
            case HDN_BEGINTRACKA:
            case HDN_BEGINTRACKW:
            case HDN_DIVIDERDBLCLICKA:
            case HDN_DIVIDERDBLCLICKW:
                return TRUE;
                break;
            break;
            default: break;
            }
        }
        break;
    case WM_DESTROY:
        RemoveProp(hwnd_, _T("ListView"));
        break;
    }

    return CallWindowProc(org_listproc, hwnd_, msg_, wp_, lp_);
}

LRESULT WinListView::HeaderProc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    switch (msg_)
    {
    case WM_SETCURSOR:
        return TRUE;
        break;
    case WM_DESTROY:
        RemoveProp(hwnd_, _T("LHeader"));
        break;
    }

    return CallWindowProc(org_headerproc, hwnd_header, msg_, wp_, lp_);
}

const bool WinListView::update_selection(LPARAM lp_)
{
    NMITEMACTIVATE* pia = (NMITEMACTIVATE*)lp_;
    if (pia->iItem != -1 && pia->iSubItem != -1)
    {
        item_id = pia->iItem;
        subitem_id = pia->iSubItem;
        return true;
    }
    return false;
}
