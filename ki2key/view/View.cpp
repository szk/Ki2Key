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
#include "View.hpp"

View::View(const uInt32 scn_width_, const uInt32 scn_height_)
    : mirror(false), texture(NULL), texture_width(0), texture_height(0),
      rgb_width(0), rgb_height(0), scn_width(scn_width_),
      scn_height(scn_height_), rgb_map(NULL), depth_map(NULL), scene_map(NULL),
      rot_x(0), rot_y(0), rot_z(0)
{
    size_t i = 0;
    Float32 offset_x, offset_y;

    size_t v_grid = S_AREA_WIDTH / GRID_SPACING;
    size_t h_grid = S_AREA_HEIGHT / GRID_SPACING;

    Float32 usr_colors[][3] =
        { {0,1,1}, {0,0,1}, {0,1,0}, {1,1,0}, {1,0,0},
          {1,.5,0}, {.5,1,0}, {0,.5,1}, {.5,0,1}, {1,0,.5},
          {.5,1,1}, {.5,.5,1}, {.5,1,.5}, {1,1,.5}, {1,.5,.5} };

     for (size_t i = 0; MAX_USERS > i; ++i)
     {
         colors[i][0] = usr_colors[i][0];
         colors[i][1] = usr_colors[i][1];
         colors[i][2] = usr_colors[i][2];
     }

    // plane line
    for (size_t y = 0; y <= h_grid; ++y)
    {
        for (size_t x = 0; x <= v_grid; ++x)
        {
            offset_x = x * GRID_SPACING - S_AREA_WIDTH / 2.0f;
            offset_y = y * GRID_SPACING - S_AREA_HEIGHT / 2.0f;

            grid_para[i * 6] = offset_x; // x
            grid_para[i * 6 + 1] = offset_y; // y
            grid_para[i * 6 + 2] = S_AREA_WIDTH; // z

            grid_para[i * 6 + 3] = offset_x; // x
            grid_para[i * 6 + 4] = offset_y; // y
            grid_para[i * 6 + 5] = 0; // z
            ++i;
        }
    }

    i = 0;
    // vertical line of depth
    for (size_t x = 0; x <= v_grid; ++x)
    {
        offset_x = x * GRID_SPACING - S_AREA_WIDTH / 2.0f;
        offset_y = S_AREA_HEIGHT / 2.0f;

        grid_depth[i * 6] = offset_x; // x
        grid_depth[i * 6 + 1] = offset_y; // y
        grid_depth[i * 6 + 2] = 0; // z

        grid_depth[i * 6 + 3] = offset_x; // x
        grid_depth[i * 6 + 4] = -offset_y; // y
        grid_depth[i * 6 + 5] = 0; // z
        ++i;
    }
    // horizontal line of depth
    for (size_t y = 0; y <= h_grid; ++y)
    {
        offset_x = S_AREA_WIDTH / 2.0f;
        offset_y = y * GRID_SPACING - S_AREA_HEIGHT / 2.0f;

        grid_depth[i * 6] = offset_x; // x
        grid_depth[i * 6 + 1] = offset_y; // y
        grid_depth[i * 6 + 2] = 0; // z

        grid_depth[i * 6 + 3] = -offset_x; // x
        grid_depth[i * 6 + 4] = offset_y; // y
        grid_depth[i * 6 + 5] = 0; // z

        ++i;
    }
    // init user depth
    for (size_t i = 0; MAX_USERS > i; ++i)
    { usr_depth_ave[i] = 0; usr_num_pts[i] = 0; }
}

View::~View(void)
{
    DEL_ARRAY(texture);
    DEL_ARRAY(tmpl_vert);
    DEL_ARRAY(depth_vert);
}

void View::init(void)
{
    texture_width = (((uInt16)(scn_width-1) / 512) + 1) * 512;
    texture_height = (((uInt16)(scn_height-1) / 512) + 1) * 512;
    texture = new RGB32[texture_width * texture_height];

    depth_vert = new Pos48[scn_width * scn_height];
    tmpl_vert = new Pos48[scn_width * scn_height];
    Pos48* v_ptr = tmpl_vert;
    for (size_t y = 0; y < scn_height; ++y)
    {
        for (size_t x = 0; x < scn_width; ++x)
        {
            v_ptr[y * scn_width + x].x = static_cast<Int16>(x - scn_width / 2);
            v_ptr[y * scn_width + x].y = static_cast<Int16>(y - scn_height / 2);
            v_ptr[y * scn_width + x].z = static_cast<Int16>(0);
        }
    }

    memmove(depth_vert, tmpl_vert, scn_width * scn_height * sizeof(Pos48));
    return;
}

void View::display(UsrList& users_, const bool sensor_output_)
{
}

void View::update_image(void)
{
    const Depth16* depth_ptr = depth_map;
    // Calculate the accumulative histogram (the yellow display...)
    memset(depth_hg, 0, MAX_DEPTH * sizeof(Float32));

    // update depth histgram
    uInt32 num_pts = 0;
    for (uInt32 y = 0; y < scn_height; ++y)
    {
        for (uInt32 x = 0; x < scn_width; ++x, ++depth_ptr)
        {
            if (*depth_ptr != 0)
            {
                depth_hg[*depth_ptr]++;
                num_pts++;
            }
        }
    }

    for (Int32 i = 1; i < MAX_DEPTH; i++) { depth_hg[i] += depth_hg[i - 1]; }
    if (num_pts)
    {
        for (Int32 i = 1; i < MAX_DEPTH; i++)
        {
            depth_hg[i] = static_cast<Float32>(256 * (1.0f -
                                                      (depth_hg[i] / num_pts)));
        }
    }

    memset(texture, 0, texture_width * texture_height * sizeof(RGB32));

    // refresh vertex
    memmove(depth_vert, tmpl_vert, scn_width * scn_height * sizeof(Pos48));
    depth_ptr = depth_map;
    const uInt16* scn_label = scene_map;

    for (size_t i = 0; MAX_USERS > i; ++i)
    {
        usr_depth_ave[i] = 0;
        usr_num_pts[i] = 0;
    }

    // check if we need to draw depth frame to texture
    const Depth16* depth_row = depth_map;
    const RGB24* rgb_ptr = rgb_map;
    RGB32* texture_row = texture + scn_offset_y * texture_width;
    Pos48* d_ptr = depth_vert;

    for (uInt32 y = 0; y < scn_height; ++y)
    {
        RGB32* texel = texture_row + scn_offset_x;

        for (uInt32 x = 0; x < scn_width;
             ++x, ++depth_ptr, ++rgb_ptr, ++texel, ++scn_label, ++d_ptr)
        {
            if ((*depth_ptr) != 0)
            {
                if ((*scn_label) != 0)
                {
                    uInt32 col_id = *scn_label % MAX_USERS;
                    int hg = static_cast<Int32>(depth_hg[*depth_ptr]);
                    texel->red = static_cast<uChar>(hg * colors[col_id][0]);
                    texel->green = static_cast<uChar>(hg * colors[col_id][1]);
                    texel->blue = static_cast<uChar>(hg * colors[col_id][2]);
                    texel->alpha = 128;
                    (*d_ptr).z = (*depth_ptr);
                    usr_depth_ave[(*scn_label)] += (*depth_ptr);
                    ++(usr_num_pts[(*scn_label)]);
                }
                else
                {
                    // quarantine vertex
                    (*d_ptr).z = (Int16)-40000;
                    // make invisible texel
                    texel->alpha = 0;
                }
            }
            else { (*d_ptr).z = (Int16)-40000; } // quarantine vertex
        }
        depth_row += scn_width;
        texture_row += texture_width;
    }
    for (size_t i = 0; MAX_USERS > i; ++i)
    { usr_depth_ave[i] /= (Float32)usr_num_pts[i]; }
}

const bool View::is_mirrored(void)
{
    return mirror;
}

const bool View::set_rgb(const RGB24* map_,
                         const uInt32 width_, const uInt32 height_,
                         const uInt32 offset_x_, const uInt32 offset_y_)
{
    rgb_map = map_;
    rgb_width = width_;
    rgb_height = height_;

    rgb_offset_x = offset_x_;
    rgb_offset_y = offset_y_;

    return true;
}

const bool View::set_scene(const Depth16* scene_map_,
                           const Depth16* depth_map_,
                           const uInt32 width_, const uInt32 height_,
                           const uInt32 scn_offset_x_,
                           const uInt32 scn_offset_y_)
{
    scene_map = scene_map_;
    depth_map = depth_map_;
    scn_width = width_;
    scn_height = height_;

    scn_offset_x = scn_offset_x_;
    scn_offset_y = scn_offset_y_;
    return true;
}

const bool View::set_color(Float32 r_, Float32 g_, Float32 b_)
{
    return true;
}

void View::add_rotate(Float32 x_, Float32 y_, Float32 z_)
{
    rot_x += x_; rot_y += y_; rot_z += z_;
    if (rot_x >= 90) { rot_x = 90; }
    if (rot_x <= -90) { rot_x = -90; }
    if (rot_y >= 90) { rot_y = 90; }
    if (rot_y <= -90) { rot_y = -90; }
}

void View::set_rotate(Float32 x_, Float32 y_, Float32 z_)
{
    rot_x = x_; rot_y = y_; rot_z = z_;
}
