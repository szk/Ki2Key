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
#include "NInput.hpp"

#include "mode/IRMode.hpp"
#include "mode/IRUserMode.hpp"
#include "mode/IRHandMode.hpp"
#include "mode/IRTileMode.hpp"

#define DISPLAY_MODE_OVERLAY 1
#define DISPLAY_MODE_DEPTH 2
#define DISPLAY_MODE_IMAGE 3
#define DEFAULT_DISPLAY_MODE DISPLAY_MODE_DEPTH

#define CHECK_RC(rc, what) \
    if (rc != XN_STATUS_OK) \
    { \
        lasterr_str = Str(_T(what)) + Str(_T(" failed: "))           \
            + Str(reinterpret_cast<const TCHAR*>(xnGetStatusString(rc)));     \
        return rc; \
    }

using namespace xn;

#define GESTURE_TO_USE "Wave"

// user callbacks
void XN_CALLBACK_TYPE cb_usr_found(UserGenerator& generator_, XnUserID id_,
                                   void* ninput_)
{ static_cast<NInput*>(ninput_)->usr_begin(id_); }

void XN_CALLBACK_TYPE cb_usr_lost(UserGenerator& generator_, XnUserID id_,
                                  void* ninput_)
{ static_cast<NInput*>(ninput_)->usr_finish(id_); }

// gestures callbacks
void XN_CALLBACK_TYPE cb_gst_beginning(GestureGenerator& generator_,
                                       const XnChar* gesture_,
                                       const XnPoint3D* pos_,
                                       XnFloat progress_, void* ninput_)
{
    static_cast<NInput*>(ninput_)->gst_beginning(pos_, gesture_);
}

void XN_CALLBACK_TYPE cb_gst_recognized(GestureGenerator& generator_,
                                        const XnChar* gesture_,
                                        const XnPoint3D* idpos_,
                                        const XnPoint3D* endpos_, void* ninput_)
{
    // use endpos to specify user.
    static_cast<NInput*>(ninput_)->gst_recognized(endpos_, gesture_);
}

// hands callbacks
void XN_CALLBACK_TYPE cb_hnd_begin(HandsGenerator& generator_,
                                   XnUserID hand_id_, const XnPoint3D* pos_,
                                   XnFloat time_, void* ninput_)
{
    ((NInput*)ninput_)->hnd_begin(hand_id_, pos_);
}

void XN_CALLBACK_TYPE cb_hnd_continue(HandsGenerator& generator_,
                                      XnUserID hand_id_, const XnPoint3D* pos_,
                                      XnFloat time_, void* ninput_)
{
    ((NInput*)ninput_)->hnd_continue(hand_id_, pos_);
}

void XN_CALLBACK_TYPE cb_hnd_finish(HandsGenerator& generator_,
                                    XnUserID hand_id_, XnFloat time_,
                                    void* ninput_)
{
    ((NInput*)ninput_)->hnd_finish(hand_id_);
}


// add xnGetFloor

NInput::NInput(UsrMap& users_)
    : users(users_)
{
}

NInput::~NInput(void)
{
}

const Int32 NInput::init(void)
{
    XnStatus nRetVal = XN_STATUS_OK;
    EnumerationErrors errors;

    // hardware initializing
    nRetVal = context.Init();
    if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
    {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        lasterr_str = reinterpret_cast<TCHAR*>(strError);
        return (nRetVal);
    }
    else if (nRetVal != XN_STATUS_OK)
    {
        lasterr_str = Str(_T("Open failed: "))
            + Str(reinterpret_cast<const TCHAR*>(xnGetStatusString(nRetVal)));
        return (nRetVal);
    }

    // mumble
    XnLicense license;
    xnOSStrCopy(license.strVendor, "PrimeSense", XN_MAX_NAME_LENGTH);
    xnOSStrCopy(license.strKey, "0KOIk2JeIBYClPWVnMoRKn5cdY4=",
                XN_MAX_LICENSE_LENGTH);
    CHECK_RC(context.AddLicense(license), "Added license");

    // base initializing
    CHECK_RC(gen_depth.Create(context), "Find depth generator");
    CHECK_RC(gen_image.Create(context), "Find image generator");

    // recognition initializing
    CHECK_RC(gen_user.Create(context), "Find user generator");
    CHECK_RC(gen_gesture.Create(context), "Find gesture generator");
    CHECK_RC(gen_hands.Create(context), "Find hand generator");

    // user initializing
    XnCallbackHandle usr_cbh;
    gen_user.RegisterUserCallbacks(cb_usr_found, cb_usr_lost, this, usr_cbh);

    // gestures initializing
    XnCallbackHandle gesture_cb;
    gen_gesture.RegisterGestureCallbacks(cb_gst_recognized, cb_gst_beginning,
                                         this, gesture_cb);

    // hands initializing
    XnCallbackHandle hand_cb;
    gen_hands.RegisterHandCallbacks(cb_hnd_begin, cb_hnd_continue,
                                    cb_hnd_finish, this, hand_cb);

    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "Start generating all");

    // The first getting of metadatas
    gen_depth.GetMetaData(depthMD);
    gen_image.GetMetaData(imageMD);
    gen_user.GetUserPixels(0, sceneMD);

    // gesture
    add_ordinal_gestures();
    CHECK_RC(nRetVal, "Start generating gesture");

    // Hybrid mode isn't supported in this sample
    if (imageMD.FullXRes() != depthMD.FullXRes()
        || imageMD.FullYRes() != depthMD.FullYRes())
    {
        lasterr_str =
            _T("The device depth and image resolution must be equal!\n");
        return 1;
    }

    // RGB is the only image format supported.
    if (imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
    {
        lasterr_str = _T("The device image format must be RGB24\n");
        return 1;
    }

    // Calibrate rgb and depth
    gen_depth.GetAlternativeViewPointCap().SetViewPoint(gen_image);

    XnFieldOfView fov;
    gen_depth.GetFieldOfView(fov);
    OutputDebugStr("max_depth: %d fov: %f/%f\n", gen_depth.GetDeviceMaxDepth(),
                   fov.fVFOV, fov.fHFOV);
    return true;
}

const Int32 NInput::update(void)
{
    XnStatus rc = XN_STATUS_OK;

    // Read a new frame
    rc = context.WaitAnyUpdateAll();
    if (rc != XN_STATUS_OK)
    {
        lasterr_str = Str(_T("Read failed: "))
            + Str(reinterpret_cast<const TCHAR*>(xnGetStatusString(rc)));
        return 1;
    }

    gen_depth.GetMetaData(depthMD);
    gen_image.GetMetaData(imageMD);
    gen_user.GetUserPixels(0, sceneMD);

    XnUserID aUsers[MAX_USERS];
    XnUInt16 nUsers = MAX_USERS;
    gen_user.GetUsers(aUsers, nUsers);

    return 0;
}

const Int32 NInput::finish(void)
{
    context.Shutdown();
    users.clear();
    return 0;
}

const XnRGB24Pixel* NInput::get_rgb_map(void)
{
    return imageMD.RGB24Data();
}

const uInt32 NInput::get_rgb_width(void)
{
    return depthMD.FullXRes();
}

const uInt32 NInput::get_rgb_height(void)
{
    return depthMD.FullYRes();
}

const uInt32 NInput::get_rgb_offset_x(void)
{
    return imageMD.XOffset();
}

const uInt32 NInput::get_rgb_offset_y(void)
{
    return imageMD.YOffset();
}

const XnDepthPixel* NInput::get_depth_map(void)
{
    return depthMD.Data();
}

const uInt32 NInput::get_depth_width(void)
{
    return depthMD.FullXRes();
}

const uInt32 NInput::get_depth_height(void)
{
    return depthMD.FullYRes();
}

const XnLabel* NInput::get_scene_map(void)
{
    return sceneMD.Data();
}

const uInt32 NInput::get_depth_offset_x(void)
{
    return depthMD.XOffset();
}

const uInt32 NInput::get_depth_offset_y(void)
{
    return depthMD.YOffset();
}

void NInput::set_mirror_mode(const bool flag_)
{
    context.SetGlobalMirror(flag_);
}

void NInput::start_sensing_gesture(void)
{
    add_ordinal_gestures();
}

void NInput::stop_sensing_gesture(void)
{
    remove_ordinal_gestures();
}

void NInput::usr_begin(XnUserID user_id_)
{
    OutputDebugStr("appear: user %d\n", user_id_);
    if (user_id_ >= MAX_USERS) { return; }
    users.push_back(user_id_);
    users.back().push_mode(new IRUserMode(user_id_));
}

void NInput::usr_finish(XnUserID user_id_)
{
    OutputDebugStr("disappear: user %d\n", user_id_);
    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        if (user_id_ == itr->get_id())
        {
            users.erase(itr);
            break;
        }
    }
}

/// Gestures recognition
void NInput::gst_recognized(const XnPoint3D* pos_, const XnChar* name_)
{
    XnPoint3D proj_pos;
    gen_depth.ConvertRealWorldToProjective(1, pos_, &proj_pos);
    uInt16 usr_id = get_user_by_pt(static_cast<uInt32>(proj_pos.X),
                                   static_cast<uInt32>(proj_pos.Y));
    OutputDebugStr("Gesture recognized: %s user: %d\n", name_, usr_id);

    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        if (usr_id == itr->get_id())
        {
            if (strncmp(name_, GESTURE_TO_USE, 4)) { continue; }
            // cleanup existing tiles and hands
            while (typeid(IRUserMode) != typeid(*(itr->get_top_mode())))
            { itr->pop_mode(); }

            // push new tile
            itr->push_mode(new IRTileMode(usr_id,
                                          Pos3D(proj_pos.X - S_AREA_WIDTH / 2,
                                                proj_pos.Y - S_AREA_HEIGHT / 2,
                                                proj_pos.Z)));
        }
        else
        {
            // reset other users
            while (typeid(IRUserMode) != typeid(*(itr->get_top_mode())))
            { itr->pop_mode(); }
        }
    }

    gen_hands.StopTrackingAll();
    gen_hands.StartTracking(*pos_);
}

void NInput::gst_beginning(const XnPoint3D* pos_, const XnChar* name_)
{
    OutputDebugStr("<%s>", name_);
}

/// Hand recognition
void NInput::hnd_begin(const XnUserID hand_id_, const XnPoint3D* pos_)
{
    XnPoint3D proj_pos;
    gen_depth.ConvertRealWorldToProjective(1, pos_, &proj_pos);
    uInt16 usr_id = get_user_by_pt(static_cast<uInt32>(proj_pos.X),
                                   static_cast<uInt32>(proj_pos.Y));
    OutputDebugStr("user%d: Enable hand %d: %f, %f, %f\n",
                   usr_id, hand_id_, pos_->X, pos_->Y, pos_->Z);
    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        if (usr_id == itr->get_id())
        { itr->push_mode(new IRHandMode(hand_id_)); }
    }
    debug();
}

void NInput::hnd_continue(const XnUserID hand_id_, const XnPoint3D* pos_)
{
    XnPoint3D proj_pos;
    gen_depth.ConvertRealWorldToProjective(1, pos_, &proj_pos);
    uInt16 usr_id = get_user_by_pt(static_cast<uInt32>(proj_pos.X),
                                   static_cast<uInt32>(proj_pos.Y));
    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        for (IRMode* ir = itr->get_top_mode(); ir != NULL;
             ir = ir->get_base())
        {
            if (typeid(IRHandMode) == typeid(*(ir)))
            {
                IRHandMode* mode = dynamic_cast<IRHandMode*>(ir);
                mode->set_pos(proj_pos.X - S_AREA_WIDTH / 2,
                              proj_pos.Y - S_AREA_HEIGHT / 2,
                              proj_pos.Z);
            }
        }
    }
}

void NInput::hnd_finish(const XnUserID hand_id_)
{
    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        itr->usr_status();
        OutputDebugStr("hand_finished:%d, user:%d\n", hand_id_, itr->get_id());
/*
        if (hand_id_ == itr->get_id())
        {
            while (typeid(IRHandMode) != typeid(*(itr->get_top_mode())))
            { itr->pop_mode(); }
            }
*/
    }
}

/// Gestures controls
void NInput::add_ordinal_gestures(void)
{
    gen_gesture.AddGesture(GESTURE_TO_USE, NULL);
}

void NInput::remove_ordinal_gestures(void)
{
    gen_gesture.RemoveGesture(GESTURE_TO_USE);
}

const XnUserID NInput::get_user_by_pt(uInt32 x_, uInt32 y_)
{
    return sceneMD[x_ + y_ * S_AREA_WIDTH];
}

void NInput::debug(void)
{
    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        OutputDebugStr("usr%d:", itr->get_id());
        itr->usr_status();
    }
    OutputDebugStr("\n");
}