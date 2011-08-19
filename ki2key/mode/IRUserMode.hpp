#ifndef IRUSERMODE_HPP__
#define IRUSERMODE_HPP__

#include "Common.hpp"
#include "IRMode.hpp"

class IRUserMode : public IRMode
{
public:
    IRUserMode(uInt32 id_) : IRMode(id_)  {}
    virtual ~IRUserMode(void) {}

    virtual void init(void) {}
    virtual void tick(void)
        {
            if (base_mode == NULL)
            { OutputDebugStr("IRUser Fatal Error"); return; }
//             OutputDebugStr("user:");
        }

    void set_com(const Pos3D& pos_) { com = pos_; }
    const Pos3D& get_com(void) const { return com; }

protected:
    Pos3D com;
};

#endif
