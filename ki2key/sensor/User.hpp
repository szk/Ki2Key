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

#ifndef USER_HPP__
#define USER_HPP__

#define TRAIL_TICK 8
#define TRAIL_END_DEPTH 500

#include "Common.hpp"
#include "mode/IRMode.hpp"
#include "mode/IRUserMode.hpp"
#include "mode/IRHandMode.hpp"
#include "mode/IRTileMode.hpp"

class User
{
public:
    User(uInt32 id_);
    virtual ~User(void);

    const uInt32 get_id(void) const;

    // control modes
    const bool push_mode(IRMode* ir_mode_);
    const bool pop_mode(void);
    const bool erase_mode(const type_info& ti_, const uInt32 id_);
    IRMode* get_mode(const type_info& ti_, uInt32 id_) const;
    IRMode* get_top_mode(void) const;

    // update tick of modes
    void update(IRMode* mode_);

    // assigning commands TODO: support not only the tile
    const bool is_tile_cmd_continuing(void) const;
    const bool is_tile_cmd_finished(void) const;
    const Str& get_tile_cmd(void) const;
    void clear_tile_cmd(void);

    // user status
    const Float32 get_tile_z(void) const;
    void usr_status(void);

private:
    const bool User::crawl_status(IRMode* mode_);
    // Buffers of IRTileMode's state XXX
    Str tile_cmd;
    bool tile_cmd_finished, tile_cmd_continuing;
    //
    uInt32 id, trail_id;
    Float32 tile_z;
    Pos3D trail_start, trail_end, gesture_pos;
    IRMode* top_mode;
};

typedef std::list<User> UsrMap;

#endif
