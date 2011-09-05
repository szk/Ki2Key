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

#ifndef WINADVSET_HPP__
#define WINADVSET_HPP__

#include "Common.hpp"
#include <windows.h>
#include "core/Core.hpp"

class WinAdvSet
{
public:
    WinAdvSet(void);
    virtual ~WinAdvSet(void);

    void init(HINSTANCE hins_);
    HWND create(int width_, int height_, BYTE type_,
                DWORD flags_, HWND parent_hwnd_);
    const bool show(HWND hwnd_, Core& core_, const Str& gesture_name_);

private:
    static LRESULT CALLBACK prx_wndproc(HWND hwnd_, UINT msg_, WPARAM wp_,
                                        LPARAM lp_);
    LRESULT wndproc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_);
    void init_setting(const Action& act_);
    void apply_setting(Core& core_, const Str& gesture_name_);
    HWND hwnd, hwnd_gstcaption, hwnd_gst, hwnd_tgtcaption, hwnd_tgt,
        hwnd_cmdcaption, hwnd_cmd, hwnd_sndbox, hwnd_clsbox,
        hwnd_rb_sndonce, hwnd_rb_sndrepeat, hwnd_rb_sndhold,
        hwnd_rb_classuse, hwnd_rb_classavoid,
        hwnd_btn_ok, hwnd_btn_cancel;
    HINSTANCE hins;
    DWORD result;
    bool finish;
};

#endif
