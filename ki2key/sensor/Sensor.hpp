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

#ifndef SENSOR_HPP__
#define SENSOR_HPP__

#include "Common.hpp"
#ifndef DUMMY_DEVICE
#include "NInput.hpp"
#else
enum DevCmd
{
    DEV_UP, DEV_DOWN, DEV_RIGHT, DEV_LEFT,
    DEV_FORWARD, DEV_BACK,
    DEV_WAIT, DEV_START, DEV_END,
    DEV_USER_ON, DEV_USER_OFF,
    DEV_HAND_ON, DEV_HAND_OFF,
    DEV_TILE_ON, DEV_TILE_OFF,
};
#include "Device.hpp"
#endif

class Sensor
{
public:
    Sensor(UsrMap& users_);
    virtual ~Sensor(void);

    const bool init(void);
    const Int32 tick_with_wait(void);
    const bool finish(void);
    void set_wait_for_signal(const bool on_);
    void set_mirror_mode(const bool flag_);

    const bool is_active(void);
    const RGB24* get_image_map(void);
    const uInt32 get_image_width(void);
    const uInt32 get_image_height(void);
    const uInt32 get_image_offset_x(void);
    const uInt32 get_image_offset_y(void);

    const Depth16* get_scene_map(void);
    const Depth16* get_depth_map(void);
    const uInt32 get_scene_width(void);
    const uInt32 get_scene_height(void);
    const uInt32 get_scene_offset_x(void);
    const uInt32 get_scene_offset_y(void);

#ifdef DUMMY_DEVICE
    void set_device(const DevCmd cmd_) { dev.act(cmd_); }
#endif

protected:
    bool active, wait_for_signal, mirror;
private:
#ifndef DUMMY_DEVICE
    NInput dev;
#else
    Device dev;
#endif // DUMMY DEVICE
};

#endif
