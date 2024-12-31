#pragma once
// OPEEngine
#include "SubscriberCtrlBlock.h"

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

            Subscriber(SubscriberCtrlBlock* _ctrl_blk, uintptr_t arg2p_addr)
                : _ctrl_blk(_ctrl_blk)
                , arg2p_addr(arg2p_addr)
            {
            }

            void notify(TArg arg2p)
            {
                _ctrl_blk->cb_wrpr->invoke(arg2p_addr, 0U);
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

        private:
            SubscriberCtrlBlock* _ctrl_blk;
            uintptr_t arg2p_addr;
    };
} // namespace opee