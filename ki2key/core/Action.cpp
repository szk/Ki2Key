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
#include "Action.hpp"

const Command invalid_cmd;

Action::Action(const Str gesture_, const Str tgt_name_,
               const Str tgt_class_, const Str cmd_name_,
               const uInt32 opt_value_)
    : gesture(gesture_), tgt_name(tgt_name_), tgt_class(tgt_class_),
      avoid_class(true), send_type(ACT_SEND_ONCE)
{
    add_cmd(Command(cmd_name_, opt_value_));
}

Action::~Action(void)
{
}

const bool Action::set_item(const ActionItem itm_, const Str& content_,
                            const uInt32 opt_value_)
{
    if (content_.empty()) { return false; }
    switch (itm_)
    {
    case ACT_GESTURE: gesture = content_; break;
    case ACT_TARGET_ID: tgt_name = content_; break;
    case ACT_TARGET_CLASS: tgt_class = content_; break;
    case ACT_CMD: // TODO: to make sustainable command input
        clear_cmd();
        add_cmd(Command(content_, opt_value_));
        break;
    case ACT_SEND_TYPE:
        set_send_type(content_);
        break;
    case ACT_AVOID_CLASS:
        if (content_ == _T(YES)) { avoid_class = true; }
        else { avoid_class = false; }
        break;
    case ACT_NUM:
    default:
        break;
    }
    return true;
}

void Action::set_avoid_class(const bool avoid_)
{
    avoid_class = avoid_;
}

void Action::set_send_type(const CommandSendType send_type_)
{
    send_type = send_type_;
}

void Action::set_send_type(const Str& send_type_str_)
{
    send_type = ACT_SEND_ONCE;
    if (send_type_str_ == _T(SEND_REPEAT)) { send_type = ACT_SEND_REPEAT; }
    else if (send_type_str_ == _T(SEND_HOLD)) { send_type = ACT_SEND_HOLD; }
}

const bool Action::add_cmd(const Command& cmd_)
{
    cmds.push_back(cmd_);
    return true;
}

const bool Action::del_cmd(size_t id_)
{
    if (cmds.size() >= id_) { return false; }
    cmds.erase(cmds.begin() + id_);
    return true;
}

const bool Action::clear_cmd(void)
{
    cmds.clear();
    return true;
}

const Str& Action::get_gesture(void) const
{
    return gesture;
}

const Str& Action::get_target_name(void) const
{
    return tgt_name;
}

const Str& Action::get_target_class(void) const
{
    return tgt_class;
}

const Command& Action::get_cmd(size_t id_) const
{
    if (cmds.size() < id_) { return invalid_cmd; }
    return cmds[id_];
}

const size_t Action::get_cmd_size(void) const
{
    return cmds.size();
}

const bool Action::is_avoid_class(void) const
{
    return avoid_class;
}

const CommandSendType Action::get_send_type(void) const
{
    return send_type;
}

void Action::debug(void) const
{
    OutputDebugStr("G: %S %S %S %d\n", gesture.c_str(), tgt_name.c_str(),
                   tgt_class.c_str(), cmds[0].get_code());
}
