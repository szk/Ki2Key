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

#ifndef WINGUIMODE_HPP__
#define WINGUIMODE_HPP__

#include "Common.hpp"
#include "Mode.hpp"

class WinGUIMode : public Mode
{
public:
    WinGUIMode(WinListView& elist_, WinMsgSender& sender_, Core& core_)
        : elist(elist_), sender(sender_), core(core_), gui_end(false)
        {}
    virtual ~WinGUIMode(void) {}

    virtual void init(void) {}
    virtual void begin(HWND hwnd_) { OutputDebugStr("WinBegin"); }

    virtual LRESULT proc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
        { OutputDebugStr("WinProc"); return 0; }

    virtual void finish(HWND hwnd_) { OutputDebugStr("WinFinish"); }
    const bool is_end(void) { return gui_end; }

protected:
    WinListView& elist;
    WinMsgSender& sender;
    Core& core;
    bool gui_end;
};

#endif
