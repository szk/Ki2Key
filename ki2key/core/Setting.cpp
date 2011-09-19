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
#include "Setting.hpp"

const Str SETTING_BASE(_T(APPNAME));
// For Actions
const Str SETTING_ACTION(_T("Action"));
// For Children of actions
const Str SETTING_GESTURE(_T("Gesture"));
const Str SETTING_TARGET(_T("Target"));
const Str SETTING_TARGET_NAME(_T("name")); // attribute
const Str SETTING_TARGET_CLASS(_T("Class"));
const Str SETTING_TARGET_CLASS_ENABLED(_T("use")); // attribute
const Str SETTING_COMMAND(_T("Command"));
const Str SETTING_COMMAND_CODE(_T("code")); // attribute
const Str SETTING_SENDING_TYPE(_T("send")); // attribute

// For sensors
const Str SETTING_SENSOR(_T("Sensor"));
const Str SETTING_SENSOR_DISPLAY(_T("pixels")); // attribute
const Str SETTING_SENSOR_FPS(_T("fps")); // atribute

Setting::Setting(void)
{
}

Setting::~Setting(void)
{
}

const bool Setting::load(const Str& filename_, ActMap& acts_)
{
    acts_.clear();
    if (!doc.read(filename_, SETTING_BASE)) { return false; }

    doc.set_root_el();
    for (doc.set_first_child(); doc.is_correct_node();
         doc.set_next_sibling())
    {
        if (doc.cmp_el(SETTING_ACTION))
        {
            Str gesture, target_name, target_class, cmd_name, send_type;
            Int32 cmd_code = 0;
            bool class_use = true;

            send_type = doc.get_att(SETTING_SENDING_TYPE);
            doc.push_node();
            for (doc.set_first_child(); doc.is_correct_node();
                 doc.set_next_sibling())
            {
                if (doc.cmp_el(SETTING_GESTURE))
                { gesture = doc.get_content_txt(); }
                if (doc.cmp_el(SETTING_TARGET))
                {
                    target_name = doc.get_att(SETTING_TARGET_NAME);
                    doc.push_node();
                    for (doc.set_first_child(); doc.is_correct_node();
                         doc.set_next_sibling())
                    {
                        if (doc.cmp_el(SETTING_TARGET_CLASS))
                        {
                            target_class = doc.get_content_txt();
                            class_use = doc.cmp_att(SETTING_TARGET_CLASS_ENABLED,
                                                      _T(YES));
                        }
                    }
                    doc.pop_node();
                }
                if (doc.cmp_el(SETTING_COMMAND))
                {
                    cmd_name = doc.get_content_txt();
                    cmd_code = doc.get_att_int(SETTING_COMMAND_CODE);
                }

            }
            OutputDebugStr("load: %S, %S, %S, %S, %d adv: %d %S,\n", gesture.c_str(),
                           target_name.c_str(), target_class.c_str(),
                           cmd_name.c_str(), cmd_code, class_use, send_type);
            // FIXME: Add advanced option
            ActPair act = ActPair(gesture, Action(gesture, target_name,
                                                  target_class, cmd_name,
                                                  cmd_code));
            // advanced option
            act.second.set_class_enabled(class_use);
            act.second.set_send_type(send_type);
            acts_.insert(act);
            doc.pop_node();
        }
    }
    return true;
}

const bool Setting::save(const Str& filename_, const ActMap& acts_)
{
    XML xml(SETTING_BASE);

    for (ActMap::const_iterator itr = acts_.begin(); acts_.end() != itr; ++itr)
    {
        const Action& act = itr->second;
        xml.push_node(); // begin Action
        xml.add_element(SETTING_ACTION);
        xml.push_node(); // begin Gesture
        xml.add_element(SETTING_GESTURE);
        xml.add_text(act.get_gesture());
        xml.pop_node(); // end Gesture

        xml.push_node(); // begin Target
        xml.add_element(SETTING_TARGET);
        xml.set_attribute(SETTING_TARGET_NAME, act.get_target_name());
        xml.add_element(SETTING_TARGET_CLASS);

        Str class_enabled = _T(NO);
        if (act.is_class_enabled()) { class_enabled = _T(YES); }
        xml.set_attribute(SETTING_TARGET_CLASS_ENABLED, class_enabled);

        xml.add_text(act.get_target_class());
        xml.pop_node(); // end Target

        Str send_str;
        switch (act.get_send_type())
        {
        case ACT_SEND_ONCE: send_str = _T(SEND_ONCE); break;
        case ACT_SEND_REPEAT: send_str = _T(SEND_REPEAT); break;
        case ACT_SEND_HOLD: send_str = _T(SEND_HOLD); break;
        }
        OutputDebugStr("sendtype: %S\n", send_str.c_str());
        xml.set_attribute(SETTING_SENDING_TYPE, send_str);

        for (size_t i = 0; act.get_cmd_size() > i; ++i)
        {
            const Command& cmd = act.get_cmd(i);
            xml.push_node(); // begin Command
            xml.add_element(SETTING_COMMAND);
            xml.add_text(cmd.get_name());
            xml.set_attribute(SETTING_COMMAND_CODE, cmd.get_code());
            xml.pop_node(); // end Command
        }
        xml.pop_node(); // end Action
    }

    OutputDebugStr("Act finished: %s\n",
                   xml.get_error_description().c_str());

    return xml.write(filename_);
}
