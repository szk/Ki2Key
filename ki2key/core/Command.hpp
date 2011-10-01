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

#ifndef COMMAND_HPP__
#define COMMAND_HPP__

#include "Common.hpp"

enum CommandType
{
    CMD_KEY,
    CMD_MOUSE,

    // unused
    CMD_X_VALUE, // changing x position provides keydown/up events by units.
    CMD_Y_VALUE, // changing y position provides keydown/up events by units.
    CMD_DISTANCE_VALUE, // changing distance provides keydown/up events by units.
    CMD_NUM,
};

#define CMD_MOUSECLICK_LEFT    1
#define CMD_MOUSECLICK_RIGHT   2
#define CMD_MOUSECLICK_MIDDLE  3
#define CMD_MOUSECLICK_XBUTTON 4

#define CMD_MOUSEWHEEL_UP      5
#define CMD_MOUSEWHEEL_DOWN    6

// modifier keys
#define CMD_MOD_SHIFT   0x0001
#define CMD_MOD_CTRL    0x0002
#define CMD_MOD_ALT     0x0004
#define CMD_MOD_RESERVE 0x0008

#define CMD_TYPE_KEY "key"
#define CMD_TYPE_MOUSE "mouse"

class Command
{
public:
    Command(const Str name_ = INIT_COMMAND, const CommandType = CMD_KEY,
            const uInt32 code_ = 0, const uInt16 mod_ = 0);
    virtual ~Command(void);

    void init(const Str& name_, const uInt32 code_);
    const Str& get_name(void) const;
    const CommandType get_type(void) const;
    const uInt32 get_code(void) const;

private:
    Str name;
    CommandType type;
    uInt32 code;
    uInt16 mod;
};

#endif
