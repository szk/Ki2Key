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

#ifndef XML_HPP_
#define XML_HPP_

#include "Common.hpp"
#include "tinyxml/tinyxml.h"

typedef std::map<const Str, const Str> StrMap;

class XML
{
public:
    XML(void);
    XML(const Str& root_tag_);
    virtual ~XML(void);

    const bool read(const Str& filename_, const Str& root_tag_);
    const bool read(const char *content_, size_t size_,
                    const Str& new_filename_, const Str& root_tag_);
    const bool write(const Str& filename_);

    const bool is_correct(void);
    const bool is_correct_node(void);
    const bool is_comment_node(void);
    Str& get_error_description(void);

    // node migration
    const bool set_next_sibling(void);
    const bool set_root_el(void);
    const bool set_first_child(void);

    // node reading
    const Str get_text(void);
    const Str get_content_txt(void);
    const Str get_att(const Str name_);
    const Int32 get_att_int(const Str name_);
    const Float32 get_att_float(const Str name_);
    const StrMap& get_att_map(void);
    const Str get_el(void);

    const bool cmp_text(const Str str_);
    const bool cmp_att(const Str name_, const Str att_);
    const bool cmp_el(const Str str_);

    const bool push_node(void);
    const bool pop_node(void);

    // node writing
    const bool add_element(const Str& name_);
    const bool add_text(const Str& content_);
    const bool set_attribute(const Str& att_, const Str& value_);
    const bool set_attribute(const Str& att_, const Int32 value_);
    const bool set_attribute(const Str& att_, const uInt32 value_);
    const bool set_attribute(const Str& att_, const Float32 value_);

    // debug
    void print(void);

protected:
    void create_init_file(const Str& filename_, const Str& root_tag_);
    std::stack<TiXmlNode*> node_stack;
    TiXmlDocument doc;
    TiXmlNode* node;

    StrMap tmp_am;
    Str last_err_desc;
};

#endif
