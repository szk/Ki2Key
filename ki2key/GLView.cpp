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
#include "GLView.hpp"

GLView::GLView(const uInt32 scn_width_, const uInt32 scn_height_)
    : View(scn_width_, scn_height_), dragging(false)
{
}

GLView::~GLView()
{
}

void GLView::init(void)
{
    View::init();
    bg_color.set(0, 0.2f, 0.2f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glLoadIdentity();
    glClearColor(bg_color.get_x(), bg_color.get_y(), bg_color.get_z(), 1.0);
    glPointSize(2.0);
    glLineWidth(5.0);

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    OutputDebugStr("OpenGL initialized\n");
}

void GLView::display(UsrMap& users_)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glOrtho(-S_AREA_WIDTH / 4, S_AREA_WIDTH / 4,
            S_AREA_HEIGHT / 4, -S_AREA_HEIGHT / 4, -5000, 5000);

    glRotated(rot_y, 1, 0, 0);
    glRotated(rot_x + 180, 0, 1, 0);

    glScaled(0.5, 0.5, 0.5);

    draw_rgb(Pos3D(0, 0, S_AREA_WIDTH / 2));

    if (!users_.empty()) { bg_color.set(0.0, 0.5, 0.5); }
    glClearColor(bg_color.get_x(), bg_color.get_y(), bg_color.get_z(), 1.0f);

    Float32 users_depth_ave = 0, i = 0;
    for (UsrMap::iterator itr = users_.begin(); users_.end() != itr; ++itr)
    {
        users_depth_ave += usr_depth_ave[itr->get_id()];
        ++i;
    }
    users_depth_ave /= i;
    draw_shape(Pos3D(0, 0, 0), users_depth_ave);

    for (UsrMap::iterator itr = users_.begin(); users_.end() != itr; ++itr)
    { draw_controller(*itr, users_depth_ave); }
    draw_grid();
    glFlush();
    SwapBuffers(hdc);
}

// This method is called before the init() method.
HWND GLView::create_glwindow(LPCWSTR title_, int x_, int y_,
                             int width_, int height_, BYTE type_,
                             DWORD flags_, HINSTANCE hins_, HWND parent_hwnd_)
{
    int pf;
    PIXELFORMATDESCRIPTOR pfd;

    hwnd = CreateWindow(_T(APPNAME), title_, WS_VISIBLE | WS_CHILD | WS_BORDER,
                        x_, y_, width_, height_, parent_hwnd_, NULL, hins_,
                        NULL);

    if (hwnd == NULL)
    {
        LPTSTR Error = 0;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER
                       | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL, GetLastError(), 0, (LPTSTR)&Error, 0, NULL);
        MessageBox(NULL, Error, _T("CreateWindow() failed"),
                   MB_OK|MB_ICONWARNING);
        return NULL;
    }

    wndproc = (WNDPROC)(LONG_PTR)GetWindowLong(hwnd, GWL_WNDPROC);
    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(LONG_PTR)prx_wndproc);
    SetProp(hwnd, _T("GLViewWindow"), this);

    // popup menu
    hmenu = CreatePopupMenu();
    AppendMenu(hmenu, MF_UNCHECKED, VIEW_POINTCLOUD, TEXT("Pointcloud"));
    AppendMenu(hmenu, MF_UNCHECKED, VIEW_MIRROR, TEXT("Mirror"));
    memset(&menu_ii, 0, sizeof(menu_ii));
    menu_ii.cbSize = sizeof(menu_ii);
    menu_ii.fMask = MIIM_STATE;

    // OpenGL Context
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
        | flags_;
    pfd.iPixelType = type_;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;

    hdc = GetDC(hwnd);

    pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0)
    {
        MessageBox(NULL, _T("ChoosePixelFormat() failed: ")
                   _T("Cannot find a suitable pixel format."),
                   _T("Error"), MB_OK);
        return 0;
    }

    if (SetPixelFormat(hdc, pf, &pfd) == FALSE)
    {
        MessageBox(NULL, _T("SetPixelFormat() failed: ")
                   _T("Cannot set format specified."), _T("Error"), MB_OK);
        return 0;
    }

    DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);

    return hwnd;
}

void GLView::destroy_glwindow(void)
{
    RemoveProp(hwnd, _T("GLViewWindow"));
    wglMakeCurrent(NULL, NULL);
    glDeleteTextures(1, &tex_id);
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
}

void GLView::draw_controller(const User& usr_, const Float32 depth_ave_)
{
    glPushMatrix();
    glTranslatef(0, 0, -depth_ave_);

    for (const IRMode* ir = usr_.get_top_mode(); ir != NULL;
         ir = ir->get_base())
    {
        if (typeid(IRTileMode) == typeid(*(ir)))
        {
            const IRTileMode* ir_tile = dynamic_cast<const IRTileMode*>(ir);
            draw_tiles(ir_tile->get_tile_orig_center(), ir_tile,
                       ir_tile->get_cmd());
        }
        else if (typeid(IRHandMode) == typeid(*(ir)))
        { draw_hand(Pos3D(0, 0, 0), dynamic_cast<const IRHandMode*>(ir)); }
    }

    glPopMatrix();
    return;
}

void GLView::draw_grid(void)
{
    glPushMatrix();
    glTranslatef(0, 0, -S_AREA_WIDTH / 2);

    // set drawing mode
    glBlendFunc(GL_ONE, GL_ONE);

    glLineWidth(1.0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.05f, 0.05f, 0.05f);
    glVertexPointer(3, GL_FLOAT, 0, grid_para);
    glDrawArrays(GL_LINES, 0, (S_AREA_WIDTH / GRID_SPACING + 1)
                 * (S_AREA_HEIGHT / GRID_SPACING + 1) * 2);

    glPushMatrix();
    for (size_t i = 0; i < S_AREA_WIDTH; ++i)
    {
        i += GRID_SPACING;
        glTranslatef(0, 0, static_cast<Float32>(GRID_SPACING));
        glVertexPointer(3, GL_FLOAT, 0, grid_depth);
        glDrawArrays(GL_LINES, 0, (S_AREA_WIDTH / GRID_SPACING + 1)
                     * (S_AREA_HEIGHT / GRID_SPACING + 1) * 2);
    }
    glPopMatrix();

    // revert
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_COLOR_ARRAY);
    glLineWidth(5.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPopMatrix();
}

void GLView::draw_tile(const Pos3D& center_, const Pos3D& size_,
                       const RGB32& rgb_)
{
    // topleft
    Pos3D tl(-size_.get_x() / 2 + center_.get_x(),
             -size_.get_y() / 2 + center_.get_y(), 0);
    // bottomright
    Pos3D br(size_.get_x() / 2 + center_.get_x(),
             size_.get_y() / 2 + center_.get_y(), 0);

    Float32 tile_v[] = { tl.get_x(), tl.get_y(), center_.get_z(),
                         br.get_x(), tl.get_y(), center_.get_z(),
                         br.get_x(), br.get_y(), center_.get_z(),
                         tl.get_x(), br.get_y(), center_.get_z() };

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_DEPTH_TEST);

    glLineWidth(2.0);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glVertexPointer(3, GL_FLOAT, 0, tile_v);
    glColor4ubv(reinterpret_cast<const GLubyte*>(&(rgb_)));
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDrawArrays(GL_QUADS, 0, 4);
//     glDisable(GL_DEPTH_TEST);
}

void GLView::draw_tiles(const Pos3D& offset_, const IRTileMode* tile_,
                        const Str& tile_cmd_)
{
    RGB32 tile_color = { 255, 128, 255, 128 };
    if (tile_->is_poked()) { tile_color.blue = 128; }

    glPushMatrix();
    // draw center
    glTranslatef(offset_.get_x(), offset_.get_y(), offset_.get_z());
    draw_tile(Pos3D(0, 0, 0), tile_->get_tile_size(), tile_color);

    // draw suggestion tile
    // draw cmd tile
    Pos3D drawn_tile = Pos3D(0, 0, 0);
    for (Str::const_iterator itr = tile_cmd_.begin(); itr != tile_cmd_.end();
         ++itr)
    {
        Pos3D c;
        if ((*itr) == GST_UP[0])
        { c.set(0, -tile_->get_tile_size().get_y(), 0); }
        else if ((*itr) == GST_DOWN[0])
        { c.set(0, tile_->get_tile_size().get_y(), 0); }
        else if ((*itr) == GST_RIGHT[0])
        { c.set(-tile_->get_tile_size().get_x(), 0, 0); }
        else if ((*itr) == GST_LEFT[0])
        { c.set(tile_->get_tile_size().get_x(), 0, 0); }
        drawn_tile += c;
        draw_tile(drawn_tile, tile_->get_tile_size(), tile_color);
    }
    // revert
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_COLOR_ARRAY);
    glLineWidth(5.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPopMatrix();
}

void GLView::draw_hand(const Pos3D& offset_, const IRHandMode* hand_)
{
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_COLOR_ARRAY);
    GLsizei seq = static_cast<GLsizei>(hand_->get_trail_seq());

    glPushMatrix();
    glTranslatef(offset_.get_x(), offset_.get_y(), offset_.get_z());

    glVertexPointer(3, GL_FLOAT, 0, hand_->get_trail_vert());
    // glColorPointer(4, GL_FLOAT, 0,
    //                &(hand_->get_trail_color()[(TRAIL_TICK - seq) * 4]));
    glColorPointer(4, GL_FLOAT, 0, hand_->get_trail_color());
    glDrawArrays(GL_LINE_STRIP, 0, seq);

    glVertexPointer(3, GL_FLOAT, 0, hand_->get_trail_vert());
    // glColorPointer(4, GL_FLOAT, 0, &(hand_->get_trail_color()[seq * 4]));
    glColorPointer(4, GL_FLOAT, 0, hand_->get_trail_color());
    glDrawArrays(GL_LINE_STRIP, seq, TRAIL_TICK - seq);

    if (hand_->get_trail_seq() != 0)
    {
        glBegin(GL_LINE_STRIP);
        glColor3fv(&(hand_->get_trail_color()[0]));
        glVertex3fv(&(hand_->get_trail_vert()[0]));
        glColor3fv(&(hand_->get_trail_color()[(TRAIL_TICK - 1) * 4]));
        glVertex3fv(&(hand_->get_trail_vert()[(TRAIL_TICK - 1) * 3]));
        glEnd();
    }
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
}

void GLView::draw_rgb(const Pos3D& offset_)
{
    glDisable(GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb_width, rgb_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, (const RGB24*)(rgb_map));

    glColor4f(1.0, 1.0, 1.0, 0.5);
    // Display the OpenGL texture map
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glPushMatrix();
    glTranslatef(offset_.get_x(), offset_.get_y(), offset_.get_z());
    draw_plane(static_cast<Float32>(rgb_width),
               static_cast<Float32>(rgb_height));
    glPopMatrix();
    glEnable(GL_BLEND);
}

void GLView::draw_shape(const Pos3D& offset_, const Float32 depth_ave_)
{
    if (pointcloud)
    {
        glEnable(GL_DEPTH_TEST);
        glPushMatrix();
        glTranslatef(offset_.get_x(), offset_.get_y(),
                     offset_.get_z() - depth_ave_);
        glDisable(GL_TEXTURE_2D);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, rgb_map);
        glVertexPointer(3, GL_SHORT, 0, depth_vert);
        glDrawArrays(GL_POINTS, 0, scn_width * scn_height);
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        draw_rgba(offset_);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void GLView::draw_rgba(const Pos3D& offset_)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, (const RGB32*)(texture));

    glColor4f(1.0, 1.0, 1.0, 0.5);
    // Display the OpenGL texture map
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glPushMatrix();
    glTranslatef(offset_.get_x(), offset_.get_y(), offset_.get_z());
    draw_plane(static_cast<Float32>(texture_width),
               static_cast<Float32>(texture_height));
    glPopMatrix();
}

void GLView::check_error(void)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        if (err == GL_INVALID_ENUM)
        { OutputDebugStr("invalid_enum\n", err); }
        if (err == GL_INVALID_VALUE)
        { OutputDebugStr("invalid_value\n", err); }
        if (err == GL_INVALID_OPERATION)
        { OutputDebugStr("invalid_operation\n", err); }
        if (err == GL_STACK_OVERFLOW)
        { OutputDebugStr("stack_overflow\n", err); }
        if (err == GL_STACK_UNDERFLOW)
        { OutputDebugStr("stack_underflow\n", err); }
        if (err == GL_OUT_OF_MEMORY)
        { OutputDebugStr("out_of_memory\n", err); }
    }
}

LRESULT CALLBACK GLView::prx_wndproc(HWND hwnd_, UINT msg_,
                                     WPARAM wp_, LPARAM lp_)
{
    GLView* target = static_cast<GLView*>(::GetProp(hwnd_, _T("GLViewWindow")));
    if(target == NULL) { return ::DefWindowProc(hwnd_,  msg_, wp_, lp_); }
    return target->glwndproc(hwnd_, msg_, wp_, lp_);
}

LRESULT GLView::glwndproc(HWND hwnd_, UINT msg_, WPARAM wp_, LPARAM lp_)
{
    POINT p;
    switch(msg_)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_MENUCHAR:
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
        break;
    case WM_MOUSEMOVE:
        if (!is_dragging()) { break; }
        GetCursorPos(&p);
        ptr_move(p.x, p.y);
        break;
    case WM_LBUTTONUP:
        GetCursorPos(&p);
        btn_up(p.x, p.y);
        ReleaseCapture();
        break;
    case WM_LBUTTONDOWN:
        GetCursorPos(&p);
        btn_down(p.x, p.y);
        SetCapture(hwnd);
        break;
    case WM_RBUTTONUP:
        break;
    case WM_RBUTTONDOWN:
        GetCursorPos(&p);
        {
            size_t mode = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN
                                         | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                         p.x, p.y, 0, hwnd, NULL);
            toggle_view(static_cast<ViewMode>(mode));
        }
        break;
    case WM_MOUSEWHEEL:
        OutputDebugStr("view wheel");
        break;
    default: break;
    }

    return ::DefWindowProc(hwnd_, msg_, wp_, lp_);
//     CallWindowProc(wndproc, hwnd, msg, wp, lp );
}

void GLView::toggle_view(const ViewMode vm_)
{
    switch (vm_)
    {
    case VIEW_POINTCLOUD:
        if (pointcloud) { menu_ii.fState = MFS_UNCHECKED; pointcloud = false; }
        else { menu_ii.fState = MFS_CHECKED; pointcloud = true; }
        break;

    case VIEW_MIRROR:
        if (mirror) { menu_ii.fState = MFS_UNCHECKED; mirror = false; }
        else { menu_ii.fState = MFS_CHECKED; mirror = true; }
        break;
    }

    SetMenuItemInfo(hmenu, vm_, TRUE, &menu_ii);
}

void GLView::draw_plane(const Float32 tex_width_, const Float32 tex_height_)
{
    const Float32 x_ratio = scn_width / tex_width_,
        y_ratio = scn_height / tex_height_;
    glBegin(GL_QUADS);
    // upper left
    glTexCoord2f(0, 0);
    glVertex3f(-S_AREA_WIDTH / 2, -S_AREA_HEIGHT / 2, 0);
    // upper right
    glTexCoord2f(x_ratio, 0);
    glVertex3f(S_AREA_WIDTH / 2, -S_AREA_HEIGHT / 2, 0);
    // bottom right
    glTexCoord2f(x_ratio, y_ratio);
    glVertex3f(S_AREA_WIDTH / 2, S_AREA_HEIGHT / 2, 0);
    // bottom left
    glTexCoord2f(0, y_ratio);
    glVertex3f(-S_AREA_WIDTH / 2, S_AREA_HEIGHT / 2, 0);
    glEnd();
}

const bool GLView::ptr_move(Int32 x_, Int32 y_)
{
    if (dragging)
    {
        add_rotate(static_cast<Float32>(x_) - ptr_x,
                   static_cast<Float32>(y_) - ptr_y, 0);
        ptr_x = x_; ptr_y = y_;
    }

    return true;
}

const bool GLView::btn_down(Int32 x_, Int32 y_)
{
    dragging = true;
    ptr_x = x_;
    ptr_y = y_;
    return false;
}

const bool GLView::btn_up(Int32 x_, Int32 y_)
{
    dragging = false;
    return true;
}

const bool GLView::is_dragging(void)
{
    return dragging;
}
