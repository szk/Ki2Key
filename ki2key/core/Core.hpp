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

#ifndef CORE_HPP__
#define CORE_HPP__

#include "Common.hpp"

#include "view/View.hpp"
#include "MsgSender.hpp"
#include "Setting.hpp"

#include "sensor/Sensor.hpp"
#include "mode/IRHandMode.hpp"
#include "mode/IRTileMode.hpp"

class Core
{
public:
    Core(void);
    virtual ~Core(void);

    const bool init(View* view_, Setting* setting_, MsgSender* sender_);
    const Int32 tick_with_wait(void);
    const Int32 finish(void);

    const bool activate_sensor(const bool on_);

    // on setting
    const Str add_action(void);
    const bool edit_action(const Str& gesture_, const ActionItem itm_,
                           const Str& content_, const uInt32 opt_value_ = 0);
    const size_t get_action_size(void) const;
    const Action& get_action(const size_t idx_) const;
    const Action& get_action(const Str& gesture_) const;
    const bool remove_action(const Str& gesture_);

    void clear_cmd(void);
    Str pop_cmd(void);
    const bool is_cmd_empty(void);

#ifdef DUMMY_DEVICE
    void set_device(const DevCmd cmd_) { sensor.set_device(cmd_); }
#endif
private:
    Sensor sensor;
    View* view;
    MsgSender* sender;
    Setting* setting;
    ActMap acts;
    Action act_invalid;
    UsrMap users;
    std::stack<Str> cmd_stack;
    Str empty_str;
};

#endif
