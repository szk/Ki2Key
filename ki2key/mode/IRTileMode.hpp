#ifndef IRTILEMODE_HPP__
#define IRTILEMODE_HPP__

#include "Common.hpp"
#include "IRMode.hpp"
#include "IRUserMode.hpp"

class IRTileMode : public IRMode
{
public:
    IRTileMode(const uInt32 user_id_, const Pos3D& center_, const Pos3D& opt_)
        : IRMode(user_id_), tile_center(center_), tile_orig_center(center_),
          max_back(0), max_front(0), conf_trail_width(0), conf_trail_height(0),
          poked(false), cmd_finished(false)
        {
            tile_size.set(100, 100, 0);
//             Pos3D t_size = opt_ - center_;
//             tile_size.set(abs(t_size.get_x()), abs(t_size.get_y()), 0);
        }
    virtual ~IRTileMode(void) {}

    virtual void init(void) {}
    virtual void tick(void)
        {
            if (typeid(IRUserMode) == typeid(*base_mode)) {}

            // is poked?
            if (trail_start.get_z() <= 0 && trail_end.get_z() > 0)
            {
                if (abs(trail_start.get_x()) < tile_size.get_x() / 2
                    && abs(trail_start.get_y()) < tile_size.get_y() / 2)
                { poked = true; return; }
                return;
            }
            if (!poked) { return; }

            if (trail_start.get_z() > 0 && trail_end.get_z() <= 0)
            {
                poked = false;
                cmd_finished = true;
                tile_center = tile_orig_center;
                return;
            }
            cmd_finished = false;
            const Str& cmd = check_trail();
            if (cmd != GST_STAY) { tile_cmd += cmd; }
        }

    // call from View
    const Pos3D& get_tile_size(void) const { return tile_size; }
    const Pos3D& get_tile_center(void) const { return tile_center; }
    const Pos3D& get_tile_orig_center(void) const { return tile_orig_center; }
    const bool is_cmd_finished(void) const { return cmd_finished; }
    const Str& get_cmd(void) const { return tile_cmd; }
    void clear_cmd(void) { cmd_finished = false; tile_cmd.clear(); }
    const bool is_poked(void) const { return poked; }
    void set_tile_size(const Pos3D& size_) { tile_size = size_; }
    void set_tile_center(const Pos3D& pos_) { tile_center = pos_; }

    void set_trail(Pos3D& start_, Pos3D& end_)
        {
            trail_start = start_;
            trail_end = end_;
        }

protected:
    const Str& check_trail(void)
        {
            Pos3D c;
            if (abs(trail_start.get_y()) < tile_size.get_y() / 2)
            { // horizontal
                if (trail_start.get_x() > tile_size.get_x() / 2)
                {
                    c.set(tile_size.get_x(), 0, 0);
                    tile_center += c;

                    return GST_LEFT;
                }
                else if (trail_start.get_x() < -tile_size.get_x() / 2)
                {
                    c.set(-tile_size.get_x(), 0, 0);
                    tile_center += c;
                    return GST_RIGHT;
                }
            }
            else if (abs(trail_start.get_x()) < tile_size.get_x() / 2)
            { // vertical
                if (trail_start.get_y() > tile_size.get_y() / 2)
                {
                    c.set(0, tile_size.get_y(), 0);
                    tile_center += c;
                    return GST_DOWN;
                }
                else if (trail_start.get_y() < -tile_size.get_y() / 2)
                {
                    c.set(0, -tile_size.get_y(), 0);
                    tile_center += c;
                    return GST_UP;
                }
            }
            return GST_STAY;
        }

    Pos3D tile_center, tile_orig_center, tile_size, trail_start, trail_end;
    Float32 max_back, max_front, conf_trail_width, conf_trail_height;
    Str tile_cmd;
    bool poked, cmd_finished;
};

#endif
