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

#ifndef COMMON_HPP__
#define COMMON_HPP__

#include <stdio.h>
#include <math.h>
#include <float.h>

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <stack>

typedef char Char;
typedef unsigned char uChar;
typedef short Int16;
typedef unsigned short uInt16;
typedef int Int32;
typedef unsigned int uInt32;
typedef float Float32;

#ifdef _WIN32_WINDOWS
typedef std::wstring Str;
typedef std::wstringstream Strstream;
#else
typedef std::string Str;
typedef std::stringstream Strstream;
#endif // _WIN32_WINDOWS

#define CREDIT "Copyright (c) 2011, Tatsuhiko Suzuki\r\nhttp://vram.org/"
#define APPNAME "Ki2Key"
#define APPVERSION "alpha 1"
#define SETTING_FILENAME "ki2key.xml"

#define S_AREA_WIDTH 640
#define S_AREA_HEIGHT 480
#define MAX_DEPTH 10000
#define MAX_USERS 15
#define GESTURE_TIMEOUT 10

#define TILE_WIDTH 50
#define TILE_HEIGHT 50

#define LV_BK_RGB RGB(255, 255, 255)
#define LV_GST_RGB RGB(128, 0, 96)
#define LV_TGT_RGB RGB(0, 128, 0)
#define LV_CMD_RGB RGB(0, 0, 128)

extern const Str INIT_GESTURE;
extern const Str INIT_TARGET;
extern const Str INIT_COMMAND;

extern const Str GST_UP;
extern const Str GST_DOWN;
extern const Str GST_RIGHT;
extern const Str GST_LEFT;
extern const Str GST_STAY;

typedef struct RGB24
{
    unsigned char red; // 8bit
    unsigned char green;
    unsigned char blue;
} RGB24;

typedef struct RGB32
{
    unsigned char red; // 8bit
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} RGB32;

typedef struct Pos48
{
    Int16 x; // 16bit
    Int16 y;
    Int16 z;
} Pos48;

enum UserRequest
{
    UR_GUI_SET_GESTURE,
    UR_GUI_SET_TARGET,
    UR_GUI_SET_COMMAND,
    UR_GUI_DO_ACTION,
    UR_CAM_GESTURE,
    UR_CAM_VALUE,
    UR_NONE,
    UR_NUM,
};

typedef unsigned short Depth16; // 16bit

extern Str lasterr_str;

#define ERR_DEV_SENSOR 0x00000001
#define ERR_DEV_VIEW   0x00000002
#define ERR_DEV_SENDER 0x00000004
#define ERR_DEV_KEY    0x00000008
#define ERR_OS_SENSOR  0x00000010
#define ERR_OS_VIEW    0x00000020
#define ERR_OS_SENDER  0x00000040
#define ERR_OS_KEY     0x00000080

#define PI 3.14159265358979323846
#define RADIAN(i) (((i) * PI) / 180)
#define DIGREE(i) (((i) * 180) / PI)

#define DEL(i) { if ((i) != NULL) { delete (i); (i) = NULL; } }
#define DEL_ARRAY(i) { if ((i) != NULL) { delete[] (i); (i) = NULL; } }

void OutputDebugStr( LPCSTR pszFormat, ...);

class Pos3D
{
public:
    Pos3D(Float32 x_ = 0, Float32 y_ = 0, Float32 z_ = 0)
        { v[0] = x_, v[1] = y_, v[2] = z_; }
    virtual ~Pos3D(void) {}

    inline void set(Float32 x_, Float32 y_, Float32 z_)
        { v[0] = x_; v[1] = y_; v[2] = z_; }
    inline const Float32* get_v(void) const { return v; }

    inline const Float32 get_x(void) const { return v[0]; }
    inline const Float32 get_y(void) const { return v[1]; }
    inline const Float32 get_z(void) const { return v[2]; }

    inline const bool operator==(const Pos3D& rhs_) const
        { return rhs_.v[0] == v[0] && rhs_.v[1] == v[1] && rhs_.v[2] == v[2]; }
    inline const bool operator!=(const Pos3D& rhs_) const
        { return rhs_.v[0] != v[0] || rhs_.v[1] != v[1] || rhs_.v[2] != v[2]; }

    inline Pos3D& operator+=(const Pos3D& rhs_)
        {
            v[0] += rhs_.v[0]; v[1] += rhs_.v[1]; v[2] += rhs_.v[2];
            return *this;
        }
    inline Pos3D& operator-=(const Pos3D& rhs_)
        {
            v[0] -= rhs_.v[0]; v[1] -= rhs_.v[1]; v[2] -= rhs_.v[2];
            return *this;
        }
    inline Pos3D operator+(const Pos3D& rhs_) const
        { Pos3D val(*this); val += rhs_; return val; }
    inline Pos3D operator-(const Pos3D& rhs_) const
        { Pos3D val(*this); val -= rhs_; return val; }
    inline void debug() const
        { OutputDebugStr("%f, %f, %f", v[0], v[1], v[2]); }

private:
    Float32 v[3];
};

#endif
