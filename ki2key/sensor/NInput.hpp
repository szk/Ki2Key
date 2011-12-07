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

#ifndef NINPUT_HPP__
#define NINPUT_HPP__

#include "Common.hpp"
#include "User.hpp"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnFPSCalculator.h>
#include <XnUSB.h>

class NInput
{
public:
    NInput(UsrList& users_);
    virtual ~NInput(void);

    const Int32 init(void);
    const Int32 update(void);
    const Int32 finish(void);

    const XnRGB24Pixel* get_rgb_map(void);
    const uInt32 get_rgb_width(void);
    const uInt32 get_rgb_height(void);
    const uInt32 get_rgb_offset_x(void);
    const uInt32 get_rgb_offset_y(void);

    const XnLabel* get_scene_map(void);
    const XnDepthPixel* get_depth_map(void);
    const uInt32 get_depth_width(void);
    const uInt32 get_depth_height(void);
    const uInt32 get_depth_offset_x(void);
    const uInt32 get_depth_offset_y(void);

    void set_mirror_mode(const bool flag_);
    void start_sensing_gesture(void);
    void stop_sensing_gesture(void);

    // for callbacks
    void usr_begin(XnUserID id_);
    void usr_finish(XnUserID id_);
    void gst_recognized(const XnPoint3D* pos_, const XnChar* name_);
    void gst_beginning(const XnPoint3D* pos_, const XnChar* name_);
    void hnd_begin(const XnUserID hand_id_, const XnPoint3D* pos_);
    void hnd_continue(const XnUserID hand_id_, const XnPoint3D* pos_);
    void hnd_finish(const XnUserID hand_id_);

protected:
    void add_ordinal_gestures(void);
    void remove_ordinal_gestures(void);

    const XnUserID get_user_by_pt(uInt32 x_, uInt32 y_);
    void debug(void);

private:
    xn::Context context;
    xn::DepthGenerator gen_depth;
    xn::ImageGenerator gen_image;
    xn::UserGenerator gen_user;
    xn::GestureGenerator gen_gesture;
    xn::HandsGenerator gen_hands;
    xn::DepthMetaData depthMD;
    xn::ImageMetaData imageMD;
    xn::SceneMetaData sceneMD;

    UsrList& users;
    XnPoint3D last_gst_beginning_pos;
};

#endif
