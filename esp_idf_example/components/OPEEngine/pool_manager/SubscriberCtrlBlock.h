#pragma once
// OPEEngine
#include "CbWrapperGeneric.h"
#include "DataWatchStackCtrlBlock.h"

namespace opee
{
    class SubscriberCtrlBlock
    {
        public:
            SubscriberCtrlBlock()
                : cb_pool_addr_ofs(0U)
                , data_sz(0U)
                , checksum(0U)
                , cb_wrpr(nullptr)
                , muted(true)
            {
            }

            SubscriberCtrlBlock(opee_uint_t dw_stk_addr_offs, opee_uint_t data_sz, opee_uint_t checksum, CbWrapperGeneric* cb_wrpr)
                : cb_pool_addr_ofs(dw_stk_addr_offs)
                , data_sz(data_sz)
                , checksum(checksum)
                , cb_wrpr(cb_wrpr)
                , muted(false)
            {
            }

            template <size_t Bytes2Allocate, size_t CbWrprMaxSz>
            static constexpr void check_cb_wrpr_sz()
            {
                static_assert(Bytes2Allocate <= CbWrprMaxSz, "CbWrapper size exceeds TCbMaxSz, increase on subscribe call.");
            }

            opee_uint_t cb_pool_addr_ofs; ///< start address of respective callback (as offset from start address of cb_pool)
            opee_uint_t data_sz;          ///< size of stored callback in bytes (excludes checksum byte)
            opee_uint8_t checksum;        ///< check sum byte
            CbWrapperGeneric* cb_wrpr;    ///< pointer to callback wrapper in cb_pool
            bool muted;                   ///< if true this callback will not be executed even if DataWatch::set is called
    };
} // namespace opee