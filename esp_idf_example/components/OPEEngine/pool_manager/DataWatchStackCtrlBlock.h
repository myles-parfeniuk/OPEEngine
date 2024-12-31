#pragma once
// std lib
#include <cstdint>

namespace opee
{
    class DataWatchStackCtrlBlock
    {
        public:
            DataWatchStackCtrlBlock()
                : cb_pool_addr_ofs(0U)
                , stk_ptr_ofs(0U)
                , stk_sz(0U)
            {
            }

            DataWatchStackCtrlBlock(uint16_t cb_pool_addr_ofs, uint16_t stk_ptr_ofs, uint16_t stk_sz)
                : cb_pool_addr_ofs(cb_pool_addr_ofs)
                , stk_ptr_ofs(stk_ptr_ofs)
                , stk_sz(stk_sz)
            {
            }

            template <size_t DWStkSz>
            static constexpr void check_dw_stk_sz()
            {
                static_assert(DWStkSz <= OPEEconfigMAX_DATA_WATCH_STK_SZ, "Max DataWatch stack size exceeded. Increase OPEEconfigMAX_DW_STK_SZ or decrease DataWatch stack size.");
            }

            uint16_t cb_pool_addr_ofs; ///< start address of respective DataWatch stack (as offset from start address of cb_pool)
            uint16_t stk_ptr_ofs;      ///< stack pointer offset (points to next free element in respective DataWatch stack)
            uint16_t stk_sz;           ///< total allocated stack size for respective DataWatch stack
    };
} // namespace opee