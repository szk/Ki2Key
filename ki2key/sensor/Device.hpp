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

#ifndef DEVICE_HPP__
#define DEVICE_HPP__

#include "Common.hpp"
#include "User.hpp"

class Device
{
public:
    Device(UsrMap& users_) : users(users_) {}
    virtual ~Device(void) {}

    const Int32 init(void) { return 0; }
    const Int32 update(void) { return 0; }
    const Int32 finish(void) { return 0; }

    const RGB24* get_rgb_map(void) { return NULL; }
    const uInt32 get_rgb_width(void) { return 0; }
    const uInt32 get_rgb_height(void) { return 0; }
    const uInt32 get_rgb_offset_x(void) { return 0; }
    const uInt32 get_rgb_offset_y(void) { return 0; }

    const Depth16* get_scene_map(void) { return NULL; }
    const Depth16* get_depth_map(void) { return NULL; }
    const uInt32 get_depth_width(void) { return 0; }
    const uInt32 get_depth_height(void) { return 0; }
    const uInt32 get_depth_offset_x(void) { return 0; }
    const uInt32 get_depth_offset_y(void) { return 0; }

    void set_mirror_mode(const bool flag_) {}
    void start_sensing_gesture(void) {}
    void stop_sensing_gesture(void) {}

    void act(const DevCmd cmd_)
        {
            switch (cmd_)
            {
            case DEV_UP: hnd_continue(0, -10, 0); break;
            case DEV_DOWN: hnd_continue(0, 10, 0); break;
            case DEV_RIGHT: hnd_continue(10, 0, 0); break;
            case DEV_LEFT: hnd_continue(-10, 0, 0); break;
            case DEV_FORWARD: hnd_continue(0, 0, -10); break;
            case DEV_BACK: hnd_continue(0, 0, 10); break;
            case DEV_WAIT: break;
            case DEV_START: break;
            case DEV_END:
                if (users.empty()) { return; }
                users.pop_back();
                break;
            case DEV_USER_ON:
                users.push_back(1);
                users.front().push_mode(new IRUserMode(1));
                break;
            case DEV_USER_OFF:
                users.pop_back();
                break;
            case DEV_HAND_ON:
                users.front().push_mode(new IRHandMode(1));
                break;
            case DEV_HAND_OFF:
                users.front().pop_mode();
                break;
            case DEV_TILE_ON:
                users.front().push_mode(new IRTileMode(1, Pos3D(0, 0, -10)));
                break;
            case DEV_TILE_OFF:
                users.front().pop_mode();
                break;
            default: break;
            }
        }

protected:
    void hnd_continue(Float32 x_, Float32 y_, Float32 z_)
        {
            for (UsrMap::iterator itr = users.begin(); users.end() != itr;
                 ++itr)
            {
                for (IRMode* ir = itr->get_top_mode(); ir != NULL;
                     ir = ir->get_base())
                {
                    if (typeid(IRHandMode) == typeid(*(ir)))
                    {
                        IRHandMode* hand = dynamic_cast<IRHandMode*>(ir);
                        abs_pos += Pos3D(x_, y_, z_);
                        hand->set_pos(abs_pos.get_x(),
                                      abs_pos.get_y(),
                                      abs_pos.get_z());
                    }
                }
            }
        }

    UsrMap& users;
    Pos3D abs_pos;
private:
};

#endif
