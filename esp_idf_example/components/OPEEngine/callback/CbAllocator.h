#pragma once
// std lib includes
#include <cstring>
#include <cstdio>
// esp-idf includes
#include "esp_log.h"
// OPEEngine includes
#include "CbWrapperDefined.h"
#include "OPEEngineConfig.h"

template <size_t DWMaxCnt>
class CbAllocator
{
    private:
        typedef struct dwstk_cb_t
        {
                uint16_t cb_pool_addr_ofs; ///< start address of respective DataWatch stack (as offset from start address of cb_pool)
                uint16_t stk_ptr_ofs;      ///< stack pointer offset (points to next free element in respective DataWatch stack)
                uint16_t stk_sz;           ///< total allocated stack size for respective DataWatch stack
        } dwstk_cb_t;

        inline static constexpr const char* TAG = "CbAllocator";  ///< Class tag for debug logs.
        inline static uint8_t cb_pool[OPEEconfigCB_POOL_SZ];      ///< Callback memory pool, contains all data watch stacks, which contain individual callbacks
        inline static dwstk_cb_t dw_stk_control_blocks[DWMaxCnt]; ///< DataWatch Stack control blocks (contains context for each DataWatch stack)
        inline static uint16_t dw_count = 0U;                     ///< Total count of allocated DataWatch stacks
        inline static uint16_t allocator_ofs = 0U;                ///< DataWatch stack allocator offset (points to next free element in cb_poo to allocate DataWatch stack)

        template <size_t DWStkSz>
        static constexpr void check_dw_stk_sz()
        {
            static_assert(DWStkSz <= OPEEconfigMAX_DATA_WATCH_STK_SZ, "Max DataWatch stack size exceeded. Increase OPEEconfigMAX_DW_STK_SZ or decrease DataWatch stack size.");
        }

        template <size_t Bytes2Allocate, size_t CbWrprMaxSz>
        static constexpr void check_cb_wrpr_sz()
        {
            static_assert(Bytes2Allocate <= CbWrprMaxSz, "CbWrapper size exceeds TCbMaxSz, increase on subscribe call.");
        }

        template <size_t DWStkSz>
        static constexpr bool check_cb_pool_overflow()
        {
            return ((DWStkSz + allocator_ofs) < OPEEconfigCB_POOL_SZ);
        }

        template <size_t CbWrprMaxSz>
        static constexpr bool check_dw_stk_overflow(const uint16_t dw_stk)
        {
            return ((dw_stk_control_blocks[dw_stk].stk_ptr_ofs + CbWrprMaxSz) < (dw_stk_control_blocks[dw_stk].cb_pool_addr_ofs + dw_stk_control_blocks[dw_stk].stk_sz));
        }

    public:
        CbAllocator()
        {
        }

        template <typename TArg, size_t DWStkSz>
        bool allocate_dw_stk(uint16_t& dw_stk)
        {

            // compilation check: does does DWStkSz exceed exceed max OPEEconfigMAX_DATA_WATCH_STK_SZ?
            check_dw_stk_sz<DWStkSz>();

            // runtime check: will allocating this DWStk exceed OPEEconfigCB_POOL_SZ?
            if (check_cb_pool_overflow<DWStkSz>())
            {
                if (dw_count < DWMaxCnt)
                {
                    // store context for respective dw stack
                    dw_stk_control_blocks[dw_count].cb_pool_addr_ofs = allocator_ofs;
                    dw_stk_control_blocks[dw_count].stk_ptr_ofs = allocator_ofs; // stack pointer starts at bottom of respective DWStk and grows with each cb subscribed
                    dw_stk_control_blocks[dw_count].stk_sz = DWStkSz;

                    // increment the datawatch count and allocator offset for next DW registration
                    allocator_ofs += DWStkSz;
                    // save dw_stk # to reference for cb allocation
                    dw_stk = dw_count;

                    dw_count++;

                    return true;
                }
            }

            return false;
        }

        template <typename TArg, typename TCb, size_t CbWrprMaxSz>
        bool store_cb(CbWrapperGeneric** subscribers, uint8_t& sub_count, const uint16_t dw_stk, CbWrapperDefined<TArg, TCb, CbWrprMaxSz>* cb_wrpr)
        {
            const constexpr size_t bytes2allocate = sizeof(CbWrapperDefined<TArg, TCb, CbWrprMaxSz>);

            // compilation check: does cb size in memory exceed CbWrprMaxSz?
            check_cb_wrpr_sz<bytes2allocate, CbWrprMaxSz>();

            // runtime check: will allocating this CbWrpr exceed respective DWStkSz?
            if (check_dw_stk_overflow<CbWrprMaxSz>(dw_stk))
            {

                if (dw_stk < dw_count) // was dw object correctly registered?
                {
                    printf("CbWrapper Sz: %dbytes\n\r", bytes2allocate);

                    new (cb_pool + dw_stk_control_blocks[dw_stk].stk_ptr_ofs) CbWrapperDefined<TArg, TCb, CbWrprMaxSz>(*cb_wrpr);

                    subscribers[sub_count++] = reinterpret_cast<CbWrapperGeneric*>(cb_pool + dw_stk_control_blocks[dw_stk].stk_ptr_ofs); // save pointer to cb from cb_pool

                    dw_stk_control_blocks[dw_stk].stk_ptr_ofs += CbWrprMaxSz; // increment respective stack pointer to next free chunk

                    return true;
                }
            }

            return false;
        }
};