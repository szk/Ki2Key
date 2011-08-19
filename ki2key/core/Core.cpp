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
#include "Core.hpp"

const Str GST_UP(_T("U"));
const Str GST_DOWN(_T("D"));
const Str GST_RIGHT(_T("R"));
const Str GST_LEFT(_T("L"));
const Str GST_STAY(_T(""));

Core::Core(void)
    : sensor(users), view(NULL), setting(NULL), sender(NULL)
{
}

Core::~Core(void)
{
}

const bool Core::init(View* view_, Setting* setting_, MsgSender* sender_)
{
    view = view_;
    setting = setting_;
    sender = sender_;

    if (view == NULL || setting == NULL || sender == NULL) { return false; }
    if (!setting->load(_T(SETTING_FILENAME), acts))
    { OutputDebugStr("Setting file is generated\n"); }
    activate_sensor(true);
    view->init();
    view->set_rgb(sensor.get_image_map(),
                  sensor.get_image_width(), sensor.get_image_height(),
                  sensor.get_image_offset_x(), sensor.get_image_offset_y());
    view->set_scene(sensor.get_scene_map(), sensor.get_depth_map(),
                    sensor.get_scene_width(), sensor.get_scene_height(),
                    sensor.get_scene_offset_x(), sensor.get_scene_offset_y());
    return true;
}

const Int32 Core::tick_with_wait(void)
{
    // update sensor information
    sensor.set_mirror_mode(view->is_mirrored());
    sensor.tick_with_wait();

    for (UsrMap::iterator itr = users.begin(); users.end() != itr; ++itr)
    {
        itr->update(itr->get_top_mode());
        if (itr->is_tile_cmd_continuing())
        {
            sensor.set_wait_for_signal(false);
            ActMap::iterator entry = acts.find(itr->get_tile_cmd());
            if (entry != acts.end()) { sender->activate(entry->second); }
            continue;
        }
        sensor.set_wait_for_signal(true);
        if (itr->is_tile_cmd_finished())
        {
            OutputDebugStr("cmd: %S\n", itr->get_tile_cmd().c_str());
            cmd_stack.push(itr->get_tile_cmd());
            itr->clear_tile_cmd();
        }
    }

    // update image
    view->set_rgb(sensor.get_image_map(),
                  sensor.get_image_width(), sensor.get_image_height(),
                  sensor.get_image_offset_x(), sensor.get_image_offset_y());
    view->set_scene(sensor.get_scene_map(), sensor.get_depth_map(),
                    sensor.get_scene_width(), sensor.get_scene_height(),
                    sensor.get_scene_offset_x(), sensor.get_scene_offset_y());

    view->update_image();
    view->display(users);
    return 0;
}

const Int32 Core::finish(void)
{
    setting->save(_T(SETTING_FILENAME), acts);
    OutputDebugStr("Core::finish");
    return sensor.finish();
}

const bool Core::activate_sensor(const bool on_)
{
    if (on_)
    {
        if (sensor.is_active()) { return true; }
        else { return sensor.init(); }
    }
    return sensor.finish();
}

const Str Core::add_action(void)
{
    Strstream ss;
    size_t i = 0;
    do
    {
        ss.str(_T(""));
        ss << INIT_GESTURE << _T(" #") << i++;
    } while (acts.find(ss.str()) != acts.end());

    Action act;
    act.set_item(ACT_GESTURE, ss.str(), 0);
    acts.insert(ActPair(act.get_gesture(), act));

    return act.get_gesture();
}

const bool Core::edit_action(const Str& gesture_, const ActionItem itm_,
                             const Str& content_, const uInt32 opt_value_)
{
    ActMap::iterator act_itr = acts.find(gesture_);
    if (ACT_GESTURE == itm_)
    {
        // The user want to edit the gesture.
        // 'gesture_' means a old identifier and 'content_' means a new gesture.
        if (acts.end() != act_itr)
        {
            Action current_act = act_itr->second;
            acts.erase(act_itr);
            current_act.set_item(ACT_GESTURE, content_, 0);
            acts.insert(ActPair(current_act.get_gesture(), current_act));
            return true;
        }
        return false;
    }
    if (act_itr == acts.end()) { return false; } // the Action is not found
    Action& found_act = act_itr->second;
    found_act.set_item(itm_, content_, opt_value_);
    return false;
}

const size_t Core::get_action_size(void) const
{
    return acts.size();
}

const Action& Core::get_action(const size_t idx_) const
{
    size_t i = 0;
    for (ActMap::const_iterator itr = acts.begin(); itr != acts.end(); ++itr)
    {
        if (idx_ == i++) { return itr->second; }
    }
    return act_invalid;
}

const Action& Core::get_action(const Str& gesture_) const
{
    if (acts.find(gesture_) == acts.end()) { return act_invalid; }
    return (acts.find(gesture_))->second;
}

const bool Core::remove_action(const Str& gesture_)
{
    return acts.erase(gesture_) != 0;
}

void Core::clear_cmd(void)
{
    for (; !cmd_stack.empty(); cmd_stack.pop());
}

Str Core::pop_cmd(void)
{
    if (is_cmd_empty()) { return empty_str; }
    Str top = cmd_stack.top();
    cmd_stack.pop();
    return top;
}

const bool Core::is_cmd_empty(void)
{
    return cmd_stack.empty();
}
