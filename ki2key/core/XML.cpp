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
#include "XML.hpp"

using namespace std;

// dirty
std::wstring str2wstr(const std::string& str_)
{
    std::wstring wstr(str_.length(), L' ');
    std::copy(str_.begin(), str_.end(), wstr.begin());
    return wstr;
}

std::string wstr2str(const std::wstring& wstr_)
{
    std::string str(wstr_.length(), L' ');
    std::copy(wstr_.begin(), wstr_.end(), str.begin());
    return str;
}

#define S2W(i) str2wstr(i)
#define W2S(i) wstr2str(i)

XML::XML(void)
{
    set_root_el();
}

XML::XML(const Str& root_tag_)
{
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement* root_element = new TiXmlElement(W2S(root_tag_.c_str()));
    doc.LinkEndChild(decl);
    doc.LinkEndChild(root_element);
    set_root_el();
}

XML::~XML(void)
{
}

const bool XML::read(const Str& filename_, const Str& root_tag_)
{
    doc.LoadFile(W2S(filename_));
    if (is_correct()) { set_root_el(); return true; }
    // if there is no filename_ file, create new XML file
    create_init_file(filename_, root_tag_);
    return false;
}

const bool XML::read(const char* content_, size_t size_,
                     const Str& new_filename_, const Str& root_tag_)
{
    doc.Parse(content_);
    if (is_correct()) { set_root_el(); return true; }
    // if there is no filename_ file, create new XML file
    create_init_file(new_filename_, root_tag_);
    return false;
}

const bool XML::write(const Str& filename_)
{
    return doc.SaveFile(W2S(filename_).c_str());
}

const bool XML::is_correct(void)
{
    return !(doc.Error());
}

const bool XML::is_correct_node(void)
{
    return node != NULL;
}

const bool XML::is_comment_node(void)
{
    return node && node->Type() == TiXmlNode::TINYXML_COMMENT;
}

Str& XML::get_error_description(void)
{
    last_err_desc.clear();
    last_err_desc = Str(L"XML: Error found \"")
        + S2W(doc.Value()) + L"\" : " + S2W(doc.ErrorDesc());
    return last_err_desc;
}

const bool XML::set_next_sibling(void)
{
    if (node && (node = node->NextSibling()))
    {
        while (is_comment_node()) { set_next_sibling(); }
        return true;
    }
    return false;
}

const bool XML::set_root_el(void)
{
    if ((node = doc.RootElement())) { return true; }
    return false;
}

const bool XML::set_first_child(void)
{
    if (node && (node = node->FirstChild()))
    {
        while (is_comment_node()) { set_next_sibling(); }
        return true;
    }
    return false;
}

const Str XML::get_text(void)
{
    if (node && node->ToText() && node->ToText()->Value())
    { return S2W(node->ToText()->Value()); }
    return L"";
}

const Str XML::get_content_txt(void)
{
    push_node();
    set_first_child();
    const Str txt = get_text();
    pop_node();
    return txt;
}

const Str XML::get_att(const Str name_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    {
        if (element->Attribute(W2S(name_).c_str()))
        { return S2W(element->Attribute(W2S(name_).c_str())); }
    }
    return L"";
}

const Int32 XML::get_att_int(const Str name_)
{
    Strstream ss(get_att(name_));
    Int32 result = 0;
    ss >> result;
    return result;
}

const Float32 XML::get_att_float(const Str name_)
{
    Strstream ss(get_att(name_));
    Float32 result = 0;
    ss >> result;
    return result;
}

const StrMap& XML::get_att_map(void)
{
    TiXmlAttribute* att;
    tmp_am.clear();
    if (node == NULL) { return tmp_am; }
    TiXmlElement* element = node->ToElement();

    if (!element) { return tmp_am; }
    if ((att = element->FirstAttribute()))
    {
        while (att)
        {
            tmp_am.insert(pair<const Str, const Str>
                          (S2W(att->Name()), S2W(att->Value())));
            att = att->Next();
        }
    }

    return tmp_am;
}

const Str XML::get_el(void)
{
    if (node && node->Type() == TiXmlNode::TINYXML_ELEMENT)
    { return S2W(node->Value()); }
    return L"";
}

const bool XML::cmp_text(const Str str_)
{
    if (node && node->ToText()) { return str_ == S2W(node->ToText()->Value()); }
    return false;
}

const bool XML::cmp_att(const Str name_, const Str att_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    {
        if (element->Attribute(W2S(name_).c_str()))
        { return att_ == S2W(element->Attribute(W2S(name_).c_str())); }
    }
    return false;
}

const bool XML::cmp_el(const Str str_)
{
    if (node && str_ == S2W(node->Value())
        && node->Type() == TiXmlNode::TINYXML_ELEMENT)
    { return true; }
    return false;
}

const bool XML::push_node(void)
{
   if (node == NULL) { return set_root_el(); }
    node_stack.push(node);
    return true;
}

const bool XML::pop_node(void)
{
    if (node_stack.size() == 0) { return set_root_el(); }
    node = node_stack.top();
    if (node_stack.size() > 1)
    {
	node_stack.pop();
	return true;
    }
    return false;
}

const bool XML::add_element(const Str& name_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    {
        TiXmlElement* new_element = new TiXmlElement(W2S(name_));
        element->LinkEndChild(new_element);
        node = new_element;
        return true;
    }
    return false;
}

const bool XML::add_text(const Str& content_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element = node->ToElement())
    {
        TiXmlText* new_text = new TiXmlText(W2S(content_));
        element->LinkEndChild(new_text);
    }
    return false;
}

const bool XML::set_attribute(const Str& att_, const Str& value_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    { element->SetAttribute(W2S(att_), W2S(value_)); return true;}
    return false;
}

const bool XML::set_attribute(const Str& att_, const Int32 value_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    { element->SetAttribute(W2S(att_), value_); return true;}
    return false;
}

const bool XML::set_attribute(const Str& att_, const uInt32 value_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    { element->SetAttribute(W2S(att_), value_); return true;}
    return false;
}

const bool XML::set_attribute(const Str& att_, const Float32 value_)
{
    if (node == NULL) { return false; }
    TiXmlElement* element = node->ToElement();
    if (element)
    { element->SetDoubleAttribute(W2S(att_), value_); return true;}
    return false;
}

void XML::print(void)
{
    doc.Print();
}

void XML::create_init_file(const Str& filename_, const Str& root_tag_)
{
    TiXmlDocument new_doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement* root_element = new TiXmlElement(W2S(root_tag_.c_str()));
    new_doc.LinkEndChild(decl);
    new_doc.LinkEndChild(root_element);
    new_doc.SaveFile(W2S(filename_).c_str());
}
