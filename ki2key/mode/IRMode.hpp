#ifndef IRMODE_HPP__
#define IRMODE_HPP__

#include "Common.hpp"
#include "Mode.hpp"

#define IR_NULL_ID -1

class IRMode : public Mode
{
public:
    IRMode(const uInt32 id_ = IR_NULL_ID) : base_mode(NULL), id(id_) {}
    virtual ~IRMode(void) {}

    virtual void init(void) {}
    virtual void tick(void) {}
    virtual const bool set_base(IRMode* ir_mode_)
        {
            base_mode = ir_mode_;
            if (ir_mode_ == NULL) { return false; }
            return true;
        }
    IRMode* get_base(void) const { return base_mode; }
    const uInt32 get_id(void) const { return id; }

protected:
    IRMode* base_mode;
    uInt32 id;
};

#endif
