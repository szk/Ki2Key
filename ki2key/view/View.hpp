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

#ifndef VIEW_HPP__
#define VIEW_HPP__

#include "Common.hpp"
#include "sensor/User.hpp"

#define GRID_SPACING 160

class View
{
public:
    View(const uInt32 scn_width_, const uInt32 scn_height_);
    virtual ~View(void);

    virtual void init(void);
    virtual void display(UsrList& users_, const bool sensor_output_ = true);

    virtual void update_image(void);
    const bool is_mirrored(void);

    const bool set_rgb(const RGB24* rgb_map_,
                       const uInt32 width_, const uInt32 height_,
                       const uInt32 rgb_offset_x_, const uInt32 rgb_offset_y_);
    const bool set_scene(const Depth16* scene_map_, const Depth16* depth_map_,
                         const uInt32 width_, const uInt32 height_,
                         const uInt32 scn_offset_x_,
                         const uInt32 scn_offset_y_);

    const bool set_color(Float32 r_, Float32 g_, Float32 b_);
    void add_rotate(Float32 x_, Float32 y_, Float32 z_);
    void set_rotate(Float32 x_, Float32 y_, Float32 z_);

protected:
    bool mirror;
    uInt32 texture_width, texture_height, rgb_width, rgb_height, scn_width,
        scn_height, rgb_offset_x, rgb_offset_y, scn_offset_x, scn_offset_y,
        usr_num_pts[MAX_USERS];
    RGB32* texture;
    const RGB24* rgb_map;
    Float32 colors[MAX_USERS][3];
    const Depth16* scene_map,* depth_map;
    Float32 rot_x, rot_y, rot_z, depth_hg[MAX_DEPTH], usr_depth_ave[MAX_USERS],
        grid_para[(S_AREA_WIDTH / GRID_SPACING + 1)
                  * (S_AREA_HEIGHT / GRID_SPACING + 1) * 3 * 2],
        grid_depth[(S_AREA_WIDTH / GRID_SPACING + 1)
                   * (S_AREA_HEIGHT / GRID_SPACING + 1) * 3 * 2];
    Pos48* tmpl_vert,* depth_vert;
};

#endif
