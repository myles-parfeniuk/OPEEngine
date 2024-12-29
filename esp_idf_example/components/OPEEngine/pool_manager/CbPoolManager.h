#pragma once
// std lib
#include <cstring>
#include <cstdio>
// esp-idf
#include "esp_log.h"
// OPEEngine
#include "OPEEngineConfig.h"
#include "DataWatchStackCtrlBlock.h"
#include "SubscriberCtrlBlock.h"
#include "CbWrapperDefined.h"

template <size_t DWMaxCnt>
class CbPoolManager
{
    private:
        static constexpr const char* TAG = "CbPoolManager"; ///< Class tag for debug logs.
        static constexpr const uint8_t DW_STK_GUARD_BYTE = 0xFFU;

        inline static uint8_t cb_pool[OPEEconfigCB_POOL_SZ] = {0U};            ///< Callback memory pool, contains all data watch stacks, which contain individual callbacks
        inline static DataWatchStackCtrlBlock dw_stk_control_blocks[DWMaxCnt]; ///< DataWatch Stack control blocks (contains context for each DataWatch stack)
        inline static uint16_t dw_count = 0U;                                  ///< Total count of allocated DataWatch stacks
        inline static uint16_t allocator_ofs = 0U; ///< DataWatch stack allocator offset (points to next free element in cb_poo to allocate DataWatch stack)

        template <size_t DWStkSz>
        static constexpr bool check_cb_pool_overflow(const uint16_t next_allocator_ofs)
        {
            return (next_allocator_ofs < OPEEconfigCB_POOL_SZ);
        }

        template <size_t CbWrprMaxSz>
        static constexpr bool check_dw_stk_overflow(const DataWatchStackCtrlBlock dw_stk_ctrl_blk)
        {
            return ((dw_stk_ctrl_blk.cb_pool_addr_ofs + dw_stk_ctrl_blk.stk_ptr_ofs + CbWrprMaxSz) < (dw_stk_ctrl_blk.cb_pool_addr_ofs + dw_stk_ctrl_blk.stk_sz));
        }

        void reset()
        {
            dw_count = 0U;
            allocator_ofs = 0U;

            for (int i = 0; i < DWMaxCnt; i++)
                dw_stk_control_blocks[i] = DataWatchStackCtrlBlock();

            memset(cb_pool, 0U, OPEEconfigCB_POOL_SZ);
        }

    public:
        CbPoolManager()
        {
        }

        template <size_t DWStkSz>
        bool allocate_dw_stk(uint16_t& dw_stk)
        {

            // compilation check: does does DWStkSz exceed exceed max OPEEconfigMAX_DATA_WATCH_STK_SZ?
            DataWatchStackCtrlBlock::check_dw_stk_sz<DWStkSz>();

            // runtime check: will allocating this DWStk exceed OPEEconfigCB_POOL_SZ?
            if (check_cb_pool_overflow<DWStkSz>(DWStkSz + allocator_ofs))
            {
                // ensure max amount of DW objects have not been exceeded
                if (dw_count < DWMaxCnt)
                {
                    // verify the entirety of desired space is free (== 0)
                    for (int i = allocator_ofs; i < DWStkSz; i++)
                        if (cb_pool[i] != 0U)
                            return false;

                    // store context for respective dw stack
                    dw_stk_control_blocks[dw_count] = DataWatchStackCtrlBlock(allocator_ofs, 0, DWStkSz);

                    // increment the datawatch count and allocator offset for next DW registration
                    allocator_ofs += DWStkSz;
                    // save dw_stk # to reference for cb allocation
                    dw_stk = dw_count;

                    // insert DwStk guard bytes
                    cb_pool[dw_stk_control_blocks[dw_count].cb_pool_addr_ofs + DWStkSz - 1] = DW_STK_GUARD_BYTE;
                    cb_pool[dw_stk_control_blocks[dw_count].cb_pool_addr_ofs + DWStkSz - 2] = DW_STK_GUARD_BYTE;

                    dw_count++;

                    return true;
                }
            }

            return false;
        }

        template <typename TArg, typename TCb, size_t CbWrprMaxSz>
        bool store_cb(SubscriberCtrlBlock* subscribers, uint8_t& sub_count, const uint16_t dw_stk, CbWrapperDefined<TArg, TCb>* cb_wrpr)
        {
            // plus one for checksum / guard byte
            const constexpr size_t bytes2allocate = sizeof(CbWrapperDefined<TArg, TCb>);

            // compilation check: does cb size in memory exceed CbWrprMaxSz?
            SubscriberCtrlBlock::check_cb_wrpr_sz<bytes2allocate, CbWrprMaxSz>();

            // runtime check: will allocating this CbWrpr exceed respective DWStkSz?
            if (check_dw_stk_overflow<CbWrprMaxSz>(dw_stk_control_blocks[dw_stk]))
            {

                if (dw_stk < dw_count) // was dw object correctly registered?
                {
                    ESP_LOGI(TAG, "CbWrapper Sz: %dbytes", bytes2allocate);
                    const uint16_t cb_pool_addr_ofs = dw_stk_control_blocks[dw_stk].cb_pool_addr_ofs + dw_stk_control_blocks[dw_stk].stk_ptr_ofs;

                    new (cb_pool + cb_pool_addr_ofs) CbWrapperDefined<TArg, TCb>(*cb_wrpr);

                    const uint16_t data_sz = bytes2allocate;

                    // checksum is first and last elements of serialized calback XOR'd together
                    const uint8_t checksum = cb_pool[cb_pool_addr_ofs] ^ cb_pool[cb_pool_addr_ofs + bytes2allocate - 1];

                    subscribers[sub_count++] = SubscriberCtrlBlock(cb_pool_addr_ofs, data_sz, checksum,
                            reinterpret_cast<CbWrapperGeneric*>(cb_pool + cb_pool_addr_ofs)); // save pointer to cb from cb_pool

                    // store checksum as guard byte at cb wrapper boundry
                    cb_pool[cb_pool_addr_ofs + CbWrprMaxSz - 1] = checksum;

                    dw_stk_control_blocks[dw_stk].stk_ptr_ofs += CbWrprMaxSz; // increment respective stack pointer to next free chunk

                    return true;
                }
            }

            return false;
        }

        friend class PoolManagerTests;
};