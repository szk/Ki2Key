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

#ifndef GLVIEW_HPP__
#define GLVIEW_HPP__

#include "Common.hpp"
#include "view/View.hpp"
#include <windows.h>
#include <GL/gl.h>

enum ViewMode
{
    VIEW_CANCEL,
    VIEW_POINTCLOUD,
    VIEW_MIRROR,
    VIEW_NUM,
};

class GLView : public View
{
public:
    GLView(const uInt32 scn_width_, const uInt32 scn_height_);
    virtual ~GLView(void);

    virtual void init(void);
    virtual void display(UsrMap& users_, const bool sensor_output_ = true);
    HWND create(LPCWSTR title_, int x_, int y_,
                         int width_, int height_, BYTE type_, DWORD flags_,
                         HINSTANCE hins_, HWND parent_hwnd_);
    void destroy_glwindow(void);

protected:
    virtual void draw_controller(const class User& usr_,
                                 const Float32 depth_ave_);
    virtual void draw_tiles(const Pos3D& offset_, const IRTileMode* tile_,
                            const Str& panel_cmd_);
    virtual void draw_hand(const Pos3D& offset_,
                                 const class IRHandMode* hand_);
    virtual void draw_rgb(const Pos3D& offset_);
    virtual void draw_rgba(const Pos3D& offset_);
    virtual void draw_shape(const Pos3D& offset_, const Float32 depth_ave_);
    virtual void draw_grid(void);

    virtual void draw_tile(const Pos3D& center_, const Pos3D& size_,
                           const RGB32& rgb_);

    virtual void check_error(void);

private:
    static LRESULT CALLBACK prx_wndproc(HWND hWnd, UINT msg, WPARAM wp,
                                        LPARAM lp);
    LRESULT glwndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
    void toggle_view(const ViewMode vm_);
    void draw_plane(const Float32 tex_width_, const Float32 tex_height);
    // on 3d view
    const bool ptr_move(Int32 x_, Int32 y_);
    const bool btn_down(Int32 x_, Int32 y_);
    const bool btn_up(Int32 x_, Int32 y_);
    const bool is_dragging(void);

    HDC hdc;
    HGLRC hrc;
    HWND hwnd;
    HMENU hmenu;
    MENUITEMINFO menu_ii;
    uInt32 tex_id;
    GLuint vbo_id;
    bool dragging, pointcloud;
    Int32 ptr_x, ptr_y;
    Pos3D center_offset, bg_color;
};

#endif
