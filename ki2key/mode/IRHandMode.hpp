#ifndef IRHANDMODE_HPP__
#define IRHANDMODE_HPP__

#include "Common.hpp"
#include "IRMode.hpp"
#include "IRUserMode.hpp"
#include "IRTileMode.hpp"

class IRHandMode : public IRMode
{
public:
    IRHandMode(uInt32 id_) : IRMode(id_), trail_seq(0)
        {
            for (size_t i = 0; i < TRAIL_TICK * 3; ++i) { trail_vert[i] = 0; }
            for (size_t i = 0; i < TRAIL_TICK * 4; ++i)
            { trail_color[i] = 1.0; /* (Float32) TRAIL_TICK / i;*/ }
        }
    virtual ~IRHandMode(void) {}

    virtual void init(void) { ir_tile = dynamic_cast<IRTileMode*>(base_mode); }
    virtual void tick(void)
        {
            if (ir_tile == NULL) { return; }
            ir_tile->set_trail(trail_start - ir_tile->get_tile_center(),
                               trail_end - ir_tile->get_tile_center());
            return;
        }

    const Pos3D& get_trail_start(void) const { return trail_start; }
    const Pos3D& get_trail_end(void) const { return trail_end; }
    const Float32* get_trail_vert(void) const { return trail_vert; }
    const Float32* get_trail_color(void) const { return trail_color; }
    const size_t get_trail_seq(void) const { return trail_seq; }

    void set_pos(Float32 x_, Float32 y_, Float32 z_)
        {
            trail_end = trail_start;
            trail_start.set(x_, y_, z_);

            trail_vert[trail_seq * 3] = x_;
            trail_vert[trail_seq * 3 + 1] = y_;
            trail_vert[trail_seq * 3 + 2] = z_;

            if (++trail_seq >= TRAIL_TICK) { trail_seq = 0; }
        }

protected:
    IRTileMode* ir_tile;
    Pos3D trail_start, trail_end;
    size_t trail_seq;
    Float32 trail_vert[TRAIL_TICK * 3], trail_color[TRAIL_TICK * 4];
};

#endif
