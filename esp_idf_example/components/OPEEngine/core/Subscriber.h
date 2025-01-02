#pragma once
// OPEEngine
#include "SubCtrlBlk.h"

namespace opee
{
    template <typename TArg>
    class Subscriber
    {
        public:
            Subscriber()
                : _ctrl_blk(nullptr)
                , arg2p_addr(0U)
            {
            }

            Subscriber(SubCtrlBlk* _ctrl_blk, opee_uintptr_t arg2p_addr)
                : _ctrl_blk(_ctrl_blk)
                , arg2p_addr(arg2p_addr)
            {
            }

            void notify(TArg arg2p)
            {
                volatile TArg* _arg2p = reinterpret_cast<volatile TArg*>(arg2p_addr);

                *_arg2p = arg2p;
                _ctrl_blk->_cb_wrpr->invoke(arg2p_addr, 0U);
            }

            bool is_muted()
            {
                return _ctrl_blk->muted;
            }

            void mute()
            {
                _ctrl_blk->muted = true;
            }

            void un_mute()
            {
                _ctrl_blk->muted = false;
            }

            bool validate_checksum()
            {
                return SubCtrlBlk::validate_checksum(*_ctrl_blk);
            }

        private:
            SubCtrlBlk* _ctrl_blk;
            opee_uintptr_t arg2p_addr;
    };
} // namespace opee