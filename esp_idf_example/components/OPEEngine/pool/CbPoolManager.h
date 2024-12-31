#pragma once
// std lib
#include <cstring>
#include <cstdio>
// esp-idf
#include "esp_log.h"
// OPEEngine
#include "OPEEngineConfig.h"
#include "OPEEngine_types.h"
#include "CbPool.h"
#include "DataWatchStackCtrlBlock.h"
#include "SubscriberCtrlBlock.h"
#include "CbWrapperDefined.h"

namespace opee
{
    template <size_t DWMaxCnt>
    class CbPoolManager
    {
        private:
            static constexpr const char* TAG = "CbPoolManager"; ///< Class tag for debug logs.
            static constexpr const opee_uint8_t DW_STK_GUARD_BYTE = 0xFFU;

            inline static CbPool<OPEEconfigCB_POOL_SZ> cb_pool;                    ///< Callback memory pool, contains all data watch stacks, which contain individual callbacks
            inline static DataWatchStackCtrlBlock dw_stk_control_blocks[DWMaxCnt]; ///< DataWatch Stack control blocks (contains context for each DataWatch stack)
            inline static opee_size_t dw_count = 0U;                               ///< Total count of allocated DataWatch stacks
            inline static opee_size_t allocator_ofs = 0U; ///< DataWatch stack allocator offset (points to next free element in cb_poo to allocate DataWatch stack)

            template <size_t DWStkSz>
            static constexpr bool check_cb_pool_overflow(const opee_size_t next_allocator_ofs)
            {
                return (next_allocator_ofs < OPEEconfigCB_POOL_SZ);
            }

            template <size_t CbWrprMaxSz>
            static constexpr bool check_dw_stk_overflow(const DataWatchStackCtrlBlock dw_stk_ctrl_blk)
            {
                return ((dw_stk_ctrl_blk.cb_pool_addr_ofs + dw_stk_ctrl_blk.stk_ptr_ofs + CbWrprMaxSz) < (dw_stk_ctrl_blk.cb_pool_addr_ofs + dw_stk_ctrl_blk.stk_sz));
            }

        public:
            CbPoolManager()
            {
            }

            void reset()
            {
                dw_count = 0U;
                allocator_ofs = 0U;

                for (opee_ssize_t i = 0; i < DWMaxCnt; i++)
                    dw_stk_control_blocks[i] = DataWatchStackCtrlBlock();

                memset(cb_pool.begin(), 0U, OPEEconfigCB_POOL_SZ);
            }

            template <size_t DWStkSz>
            OPEEngineRes_t allocate_dw_stk(opee_size_t& dw_stk)
            {

                // compilation check: does does DWStkSz exceed exceed max OPEEconfigMAX_DATA_WATCH_STK_SZ?
                DataWatchStackCtrlBlock::check_dw_stk_sz<DWStkSz>();

                // runtime check: will allocating this DWStk exceed OPEEconfigCB_POOL_SZ?
                if (!check_cb_pool_overflow<DWStkSz>(DWStkSz + allocator_ofs))
                    return OPEE_CB_POOL_FULL;

                // ensure max amount of DW objects have not been exceeded
                if (dw_count >= DWMaxCnt)
                    return OPEE_MAX_DWSTK_CNT_EXCEEDED;

                // verify the entirety of desired space is free (== 0)
                for (opee_ssize_t i = allocator_ofs; i < DWStkSz; i++)
                    if (cb_pool[i] != 0U)
                        return OPEE_CB_POOL_RGN_NOT_EMPTY;

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

                return OPEE_OK;
            }

            template <typename TArg, typename TCb, size_t CbWrprMaxSz>
            OPEEngineRes_t store_cb(SubscriberCtrlBlock* subscribers, opee_uint8_t& sub_count, const opee_size_t dw_stk, CbWrapperDefined<TArg, TCb>* cb_wrpr)
            {
                const constexpr size_t data_sz = sizeof(CbWrapperDefined<TArg, TCb>);

                // compilation check: does cb size in memory exceed CbWrprMaxSz?
                SubscriberCtrlBlock::check_cb_wrpr_sz<data_sz, CbWrprMaxSz>();

                // runtime check: will allocating this CbWrpr exceed respective DWStkSz?
                if (!check_dw_stk_overflow<CbWrprMaxSz>(dw_stk_control_blocks[dw_stk]))
                    return OPEE_DWSTK_FULL;

                if (dw_stk >= dw_count)
                    return OPEE_INVALID_DWSTK_IDX;

                ESP_LOGI(TAG, "CbWrapper Sz: %dbytes", data_sz);
                const opee_size_t cb_pool_addr_ofs = dw_stk_control_blocks[dw_stk].cb_pool_addr_ofs + dw_stk_control_blocks[dw_stk].stk_ptr_ofs;

                // verify the entirety of desired space is free (== 0)
                for (opee_ssize_t i = cb_pool_addr_ofs; i < (cb_pool_addr_ofs + CbWrprMaxSz); i++)
                    if (cb_pool[i] != 0)
                        return OPEE_CB_POOL_RGN_NOT_EMPTY;

                new (cb_pool.begin() + cb_pool_addr_ofs) CbWrapperDefined<TArg, TCb>(*cb_wrpr);

                // checksum is all elements of serialized calback data XOR'd together
                const opee_uint8_t checksum = SubscriberCtrlBlock::create_checksum(cb_pool_addr_ofs, data_sz, cb_pool);

                subscribers[sub_count++] = SubscriberCtrlBlock(
                        cb_pool_addr_ofs, data_sz, checksum, &cb_pool, reinterpret_cast<CbWrapperGeneric*>(cb_pool.begin() + cb_pool_addr_ofs)); // save callback context

                if (subscribers[sub_count - 1].cb_wrpr == nullptr)
                    return OPEE_CB_WRPR_CREATION_FAILED;

                // store checksum as guard byte at cb wrapper boundry
                cb_pool[cb_pool_addr_ofs + CbWrprMaxSz - 1] = checksum;

                dw_stk_control_blocks[dw_stk].stk_ptr_ofs += CbWrprMaxSz; // increment respective stack pointer to next free chunk

                return OPEE_OK;
            }

            friend class PoolManagerTests;
    };
} // namespace opee