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
#include "User.hpp"

User::User(uInt32 id_)
    : tile_cmd_finished(false), tile_cmd_continuing(false), id(id_),
      trail_id(0), top_mode(NULL)
{
    OutputDebugStr("user%d available\n", id);
    push_mode(new IRMode);
}

User::~User(void)
{
    OutputDebugStr("user%d destroying\n", id);
    while (pop_mode());
}

const uInt32 User::get_id(void) const
{
    return id;
}

void User::update(IRMode* ir_mode_)
{
    if (ir_mode_ == NULL) { return; }
    ir_mode_->tick();
    if (typeid(IRTileMode) == typeid(*(ir_mode_)))
    {
        IRTileMode* tile = dynamic_cast<IRTileMode*>(ir_mode_);
        if (!tile->get_cmd().empty())
        {
            tile_cmd_continuing = true;
            tile_cmd = tile->get_cmd();
        }
        if (tile->is_cmd_finished())
        {
            tile_cmd = tile->get_cmd();
            if (tile_cmd.empty()) { return; }
            tile->clear_cmd();
            tile_cmd_finished = true;
            tile_cmd_continuing = false;
        }
    }

    IRMode* next_mode = ir_mode_->get_base();
    update(next_mode);
}

const bool User::push_mode(IRMode* ir_mode_)
{
    if (top_mode != NULL) { ir_mode_->set_base(top_mode); }
    top_mode = ir_mode_;
    top_mode->init();
    // if tile mode was pushed
    if (typeid(IRTileMode) == typeid(*(ir_mode_)))
    {
        tile_z = dynamic_cast<const IRTileMode*>
            (ir_mode_)->get_tile_center().get_z();
    }

    return true;
}

const bool User::pop_mode(void)
{
    if (top_mode == NULL) { return false; }
    IRMode* child = top_mode->get_base();
    if (child != NULL)
    {
        if (typeid(IRUserMode) == typeid(*(top_mode))) { tile_z = 0; }
        DEL(top_mode);
        top_mode = child;
        return true;
    }

    return false;
}

const bool User::erase_mode(const type_info& ti_, const uInt32 id_)
{
    while (pop_mode())
    {
        if (typeid(IRUserMode) == typeid(*(top_mode))) { return true; }
    }
    return false;
}

IRMode* User::get_mode(const type_info& ti_, uInt32 id_) const
{
    for (IRMode* ir = top_mode; ir != NULL; ir = top_mode->get_base())
    {
        if (ti_ == typeid(*ir) && ir->get_id() == id_) { return ir; }
    }
    return NULL;
}

IRMode* User::get_top_mode(void) const
{
    return top_mode;
}

const bool User::is_tile_cmd_continuing(void) const
{
    return tile_cmd_continuing;
}

const bool User::is_tile_cmd_finished(void) const
{
    return tile_cmd_finished;
}

const Str& User::get_tile_cmd(void) const
{
    return tile_cmd;
}

void User::clear_tile_cmd(void)
{
    tile_cmd.clear();
    tile_cmd_finished = false;
    tile_cmd_continuing = false;
}

const Float32 User::get_tile_z(void) const
{
    return tile_z;
}

void User::usr_status(void)
{
    crawl_status(top_mode);
    OutputDebugStr("\n");
}

const bool User::crawl_status(IRMode* mode_)
{
//     OutputDebugStr("a\n");
    OutputDebugStr("a %s ", typeid((*mode_)).name());
    if (mode_->get_base() == NULL) { return false; }
    crawl_status(mode_->get_base());
    return true;
}
