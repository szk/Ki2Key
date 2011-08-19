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

#ifndef ACTION_HPP__
#define ACTION_HPP__

#include "Common.hpp"
#include "Command.hpp"

extern const Command invalid_cmd;

enum ActionItem
{
    ACT_GESTURE,
    ACT_TARGET_ID,
    ACT_TARGET_CLASS,
    ACT_CMD,
    ACT_NUM,
};

class Action
{
public:
    Action(const Str gesture_ = INIT_GESTURE,
           const Str tgt_id_ = INIT_TARGET,
           const Str tgt_class_ = INIT_TARGET,
           const Str cmd_name_ = INIT_COMMAND,
           const uInt32 opt_value_ = 0);
    virtual ~Action(void);

    const bool set_item(const ActionItem itm_, const Str& content_,
                        const uInt32 opt_value_);
    const bool add_cmd(const Command& cmd_);
    const bool del_cmd(size_t id_);
    const bool clear_cmd(void);

    const Str& get_gesture(void) const;
    const Str& get_target_name(void) const;
    const Str& get_target_class(void) const;
    const Command& get_cmd(size_t id_) const;
    const size_t get_cmd_size(void) const;

    void debug(void) const;
private:
    Str gesture, tgt_name, tgt_class;
    std::vector<Command> cmds;
};

typedef std::pair<Str,Action> ActPair;
typedef std::map<Str,Action> ActMap;

#endif
