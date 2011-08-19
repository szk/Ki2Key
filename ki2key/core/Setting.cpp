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
const Str SETTING_COMMAND(_T("Command"));
const Str SETTING_COMMAND_CODE(_T("code")); // attribute

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
    if (!loaded_doc.read(filename_, SETTING_BASE)) { return false; }

    loaded_doc.set_root_el();
    for (loaded_doc.set_first_child(); loaded_doc.is_correct_node();
         loaded_doc.set_next_sibling())
    {
        OutputDebugStr("%S\n", loaded_doc.get_el().c_str());

        if (loaded_doc.cmp_el(SETTING_ACTION))
        {
            Str gesture, target_name, target_class, cmd_name;
            Int32 cmd_code = 0;

            loaded_doc.push_node();
            for (loaded_doc.set_first_child(); loaded_doc.is_correct_node();
                 loaded_doc.set_next_sibling())
            {
                if (loaded_doc.cmp_el(SETTING_GESTURE))
                {
                    gesture = loaded_doc.get_content_txt();
//                     OutputDebugStr("Gesture: %S\n", gesture.c_str());
                }
                if (loaded_doc.cmp_el(SETTING_TARGET))
                {
                    target_name = loaded_doc.get_att(SETTING_TARGET_NAME);
//                     OutputDebugStr("Target name: %S\n", target_name.c_str());
                    loaded_doc.push_node();
                    for (loaded_doc.set_first_child();
                         loaded_doc.is_correct_node();
                         loaded_doc.set_next_sibling())
                    {
                        if (loaded_doc.cmp_el(SETTING_TARGET_CLASS))
                        {
                            target_class = loaded_doc.get_content_txt();
//                             OutputDebugStr("Class: %S\n", target_class.c_str());
                        }
                    }
                    loaded_doc.pop_node();
                }
                if (loaded_doc.cmp_el(SETTING_COMMAND))
                {
                    cmd_name = loaded_doc.get_content_txt();
                    cmd_code = loaded_doc.get_att_int(SETTING_COMMAND_CODE);
//                     OutputDebugStr("command name: %S %d\n", cmd_name.c_str(),
//                                    cmd_code);
                }

            }
            OutputDebugStr("load: %S, %S, %S, %S, %d\n", gesture.c_str(),
                           target_name.c_str(), target_class.c_str(),
                           cmd_name.c_str(), cmd_code);

            acts_.insert(ActPair(gesture,
                                       Action(gesture, target_name,
                                              target_class, cmd_name,
                                              cmd_code)));
            loaded_doc.pop_node();
        }
    }

    OutputDebugStr("load finished\n");
    return true;
}

const bool Setting::save(const Str& filename_, const ActMap& acts_)
{
    XML xml(SETTING_BASE);

    for (ActMap::const_iterator itr = acts_.begin(); acts_.end() != itr; ++itr)
    {
        xml.push_node(); // begin Action
        const Action& act = itr->second;
        xml.add_element(SETTING_ACTION);
        xml.push_node(); // begin Gesture
        xml.add_element(SETTING_GESTURE);
        xml.add_text(act.get_gesture());
        xml.pop_node(); // end Gesture

        xml.push_node(); // begin Target
        xml.add_element(SETTING_TARGET);
        xml.set_attribute(SETTING_TARGET_NAME, act.get_target_name());
        xml.add_element(SETTING_TARGET_CLASS);
        xml.add_text(act.get_target_class());
        xml.pop_node(); // end Target

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
