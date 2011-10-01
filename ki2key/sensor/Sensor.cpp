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
#include "Sensor.hpp"

Sensor::Sensor(UsrMap& users_)
    : active(false), wait_for_signal(false), mirror(false), dev(users_)
{
}

Sensor::~Sensor(void)
{
}

const bool Sensor::init(void)
{
    if (!dev.init()) { return false; }
    OutputDebugStr("sensor initializing succeeded");
    active = true;

    return true;
}

const Int32 Sensor::tick_with_wait(void)
{
    if (!active) { return 0; }
    dev.update();
    return 0;
}

const bool Sensor::finish(void)
{
    dev.finish();
    active = false;
    return true;
}

void Sensor::set_wait_for_signal(const bool on_)
{
    if (on_ == wait_for_signal) { return; }
    if (on_) { dev.start_sensing_gesture(); return; }
    dev.stop_sensing_gesture();
}

void Sensor::set_mirror_mode(const bool flag_)
{
    if (flag_ != mirror)
    {
        dev.set_mirror_mode(flag_);
        mirror = flag_;
    }
}

const bool Sensor::is_active(void)
{
    return active;
}

const RGB24* Sensor::get_image_map(void)
{
    return (const RGB24*)(dev.get_rgb_map());
}

const uInt32 Sensor::get_image_width(void)
{
    return dev.get_rgb_width();
}

const uInt32 Sensor::get_image_height(void)
{
    return dev.get_rgb_height();
}

const uInt32 Sensor::get_image_offset_x(void)
{
    return dev.get_rgb_offset_x();
}

const uInt32 Sensor::get_image_offset_y(void)
{
    return dev.get_rgb_offset_y();
}

const Depth16* Sensor::get_scene_map(void)
{
    return dev.get_scene_map();
}

const Depth16* Sensor::get_depth_map(void)
{
    return dev.get_depth_map();
}

const uInt32 Sensor::get_scene_width(void)
{
    return dev.get_depth_width();
}

const uInt32 Sensor::get_scene_height(void)
{
    return dev.get_depth_height();
}

const uInt32 Sensor::get_scene_offset_x(void)
{
    return dev.get_depth_offset_x();
}

const uInt32 Sensor::get_scene_offset_y(void)
{
    return dev.get_depth_offset_y();
}
