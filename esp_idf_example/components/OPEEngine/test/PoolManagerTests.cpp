// third-party
#include "unity.h"
// OPEEngine
#include "../callback/CbHelper.h"
#include "../OPEEngineConfig.h"
#include "OPEEngineTestHelper.hpp"

class PoolManagerTests
{
    public:
        static const constexpr uint16_t TEST_OPEEconfigMAX_DATA_WATCH_CNT = 5;

        template <size_t DWStkSz>
        static bool allocate_dw_stk_helper(const char* TEST_TAG, uint16_t& dw_stk_idx)
        {
            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            uint16_t dw_stk_idx_before_allocation = dw_stk_idx;
            uint16_t allocator_ofs_before_allocation = pool_manager.allocator_ofs;

            if (!pool_manager.template allocate_dw_stk<DWStkSz>(dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: allocate_dw_stk() did not return successfully");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: allocate_dw_stk() allocated %dbytes successfully", DWStkSz);
            }

            // DataWatch stack idx should correspond to order in which they were allocated (should increase with each allocation)
            uint16_t expected_dw_stk_idx = (dw_stk_idx == 0) ? 0 : (dw_stk_idx - 1);
            if (dw_stk_idx_before_allocation != expected_dw_stk_idx)
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "FAIL: DWStk reference number/index does not match expected. Actual: %d Expected: %d", dw_stk_idx, expected_dw_stk_idx);
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk index check. Actual: %d Expected: %d", dw_stk_idx, expected_dw_stk_idx);
            }

            // starting offset of each DataWatch stack should increase with each allocation by size of previous DWStk allocation
            uint16_t expected_cb_pool_ofs =
                    (dw_stk_idx == 0) ? 0 : (pool_manager.dw_stk_control_blocks[dw_stk_idx - 1].cb_pool_addr_ofs + pool_manager.dw_stk_control_blocks[dw_stk_idx - 1].stk_sz);

            if (pool_manager.dw_stk_control_blocks[dw_stk_idx].cb_pool_addr_ofs != expected_cb_pool_ofs)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset does not match expected value. Actual: %d, Expected: %d",
                        pool_manager.dw_stk_control_blocks[dw_stk_idx].cb_pool_addr_ofs, expected_cb_pool_ofs);
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "PASS: DWStk addr offset check. Actual: %d, Expected: %d", pool_manager.dw_stk_control_blocks[dw_stk_idx].cb_pool_addr_ofs, expected_cb_pool_ofs);
            }

            // stack pointer offset should still point to 0, no callbacks have been stored
            if (pool_manager.dw_stk_control_blocks[dw_stk_idx].stk_ptr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset non-zero before any callbacks stored.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
            }

            // stack size should be DWStkSz
            if (pool_manager.dw_stk_control_blocks[dw_stk_idx].stk_sz != DWStkSz)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk size within respective dw_stk_control_block not equal to assigned value.");
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
            }

            // allocator offset should be incremented by DWStkSz to point to next free start address for next stack to allocate
            if (pool_manager.allocator_ofs != (allocator_ofs_before_allocation + DWStkSz))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocator offset not incremented for next DWStk allocation. Actual: %d, Expected: %d",
                        pool_manager.allocator_ofs, (allocator_ofs_before_allocation + DWStkSz));
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "PASS: Allocator offset check. Actual: %d, Expected: %d", pool_manager.allocator_ofs, (allocator_ofs_before_allocation + DWStkSz));
            }

            uint8_t guard_byte_0 =
                    pool_manager.cb_pool[pool_manager.dw_stk_control_blocks[dw_stk_idx].cb_pool_addr_ofs + pool_manager.dw_stk_control_blocks[dw_stk_idx].stk_sz - 1];

            uint8_t guard_byte_1 =
                    pool_manager.cb_pool[pool_manager.dw_stk_control_blocks[dw_stk_idx].cb_pool_addr_ofs + pool_manager.dw_stk_control_blocks[dw_stk_idx].stk_sz - 2];

            // check guard bytes
            if ((guard_byte_0 != CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE) ||
                    (guard_byte_1 != CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk guard bytes not detected. Actual: %d %d Expected: %d %d", guard_byte_0, guard_byte_1,
                        CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE, CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE);

                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk guard byte check. Actual: %d %d Expected: %d %d", guard_byte_0, guard_byte_1,
                        CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE, CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::DW_STK_GUARD_BYTE);
            }

            return true;
        }

        template <size_t CbWrprMaxSz, typename TArg, typename TLambda>
        static bool store_cb_helper(TLambda&& lambda, const char* TEST_TAG, SubscriberCtrlBlock* subscribers, uint8_t& sub_count, const uint16_t dw_stk)
        {
            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            using TCb = std::decay_t<TLambda>; // get the actual type of the lambda by stripping it of references with decay

            CbWrapperDefined<TArg, TCb> cb_wrpr(std::forward<TLambda>(lambda)); // create a temp wrapper object on stack to store callback

            size_t bytes2allocate = sizeof(CbWrapperDefined<TArg, TCb>);
            uint8_t sub_count_before_allocation = sub_count;
            uint16_t dw_stk_stack_pointer_ofs_before_allocation = pool_manager.dw_stk_control_blocks[dw_stk].stk_ptr_ofs;

            if (!pool_manager.template store_cb<TArg, TCb, CbWrprMaxSz>(subscribers, sub_count, dw_stk, &cb_wrpr))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: store_cb() failed to return successfully.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: store_cb() returned successfully.");
            }

            if (sub_count != sub_count_before_allocation + 1)
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "FAIL: sub count did not increment after cb allocation. Actual: %d Expected: %d", sub_count, (sub_count_before_allocation + 1));
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: sub count incrementaction check. Actual: %d Expected: %d", sub_count, (sub_count_before_allocation + 1));
            }

            uint16_t actual_dw_stk_ptr_ofs = pool_manager.dw_stk_control_blocks[dw_stk].stk_ptr_ofs;
            uint16_t expected_dw_stk_ptr_ofs = dw_stk_stack_pointer_ofs_before_allocation + CbWrprMaxSz;

            if (actual_dw_stk_ptr_ofs != expected_dw_stk_ptr_ofs)
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "FAIL: DwStk[%d] incorrect stack pointer. Actual: %d Expected: %d", dw_stk, actual_dw_stk_ptr_ofs, expected_dw_stk_ptr_ofs);
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "PASS: DwStk[%d] stack pointer check. Actual: %d Expected: %d", dw_stk, actual_dw_stk_ptr_ofs, expected_dw_stk_ptr_ofs);
            }

            if (subscribers[sub_count - 1].data_sz != bytes2allocate)
            {
                OPEEngineTestHelper::print_test_msg(
                        TEST_TAG, "FAIL: unexpected CbWrpr size. Actual: %dbytes Expected: %dbytes", subscribers[sub_count - 1].data_sz, bytes2allocate);
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: CbWrpr size check. Actual: %dbytes Expected: %dbytes", subscribers[sub_count - 1].data_sz, bytes2allocate);
            }

            uint16_t cb_pool_addr_ofs = (pool_manager.dw_stk_control_blocks[dw_stk].stk_ptr_ofs + pool_manager.dw_stk_control_blocks[dw_stk].cb_pool_addr_ofs) - CbWrprMaxSz;
            uint16_t actual_checksum = pool_manager.create_checksum(cb_pool_addr_ofs, bytes2allocate);
            uint16_t expected_checksum = subscribers[sub_count - 1].checksum;

            // checksum validation
            if (!pool_manager.validate_checksum(subscribers[sub_count - 1], dw_stk))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: checksum validation failed. Actual: %d Expected: %d", actual_checksum, expected_checksum);
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: checksum validation check. Actual: %d Expected: %d", actual_checksum, expected_checksum);
            }

            return true;
        }

        static bool pool_manager_init_val_test()
        {
            const constexpr char* TEST_TAG = "pool_manager_init_val_tests";

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            if (pool_manager.allocator_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocator offset did not initially point to 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: allocator offset check.");
            }

            if (pool_manager.dw_count != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DataWatch count non-zero before any stacks allocated.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch count check.");
            }

            for (int i = 0; i < OPEEconfigCB_POOL_SZ; i++)
                if (pool_manager.cb_pool[i] != 0U)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Non-zero value detected in cb_pool before any callbacks stored.");
                    return false;
                }

            OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: cb_pool empty check.");

            return true;
        }

        static bool dw_stk_ctrl_blks_init_val_test()
        {
            const constexpr char* TEST_TAG = "dw_stk_ctrl_blks_init_val_test";

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            for (int i = 0; i < TEST_OPEEconfigMAX_DATA_WATCH_CNT; i++)
            {
                if (pool_manager.dw_stk_control_blocks[i].cb_pool_addr_ofs != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset did not initially point to 0.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk addr offset check.");
                }

                if (pool_manager.dw_stk_control_blocks[i].stk_ptr_ofs != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset did not initially point to 0.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
                }

                if (pool_manager.dw_stk_control_blocks[i].stk_sz != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack size not initially 0 bytes.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
                }
            }

            return true;
        }

        static bool allocate_single_dw_stk_test()
        {
            const constexpr char* TEST_TAG = "allocate_single_dw_stk_test";
            const constexpr uint16_t DWStkSz = 64;

            uint16_t dw_stk_idx = 0;
            if (!allocate_dw_stk_helper<DWStkSz>(TEST_TAG, dw_stk_idx))
                return false;

            return true;
        }

        static bool pool_manager_vals_after_reset_test()
        {
            const constexpr char* TEST_TAG = "pool_manager_vals_after_reset_test";

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            pool_manager.reset();

            if (pool_manager.allocator_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocator offset did not initially point to 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: allocator offset check.");
            }

            if (pool_manager.dw_count != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DataWatch count non-zero before any stacks allocated.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch count check.");
            }

            for (int i = 0; i < OPEEconfigCB_POOL_SZ; i++)
                if (pool_manager.cb_pool[i] != 0U)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Non-zero value detected in cb_pool before any callbacks stored.");
                    return false;
                }

            OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: cb_pool empty check.");

            return true;
        }

        static bool dw_stk_ctrl_blks_vals_after_reset_test()
        {
            const constexpr char* TEST_TAG = "dw_stk_ctrl_blks_vals_after_reset_test";

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();

            pool_manager.reset();

            for (int i = 0; i < TEST_OPEEconfigMAX_DATA_WATCH_CNT; i++)
            {
                if (pool_manager.dw_stk_control_blocks[i].cb_pool_addr_ofs != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset did not initially point to 0.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk addr offset check.");
                }

                if (pool_manager.dw_stk_control_blocks[i].stk_ptr_ofs != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset did not initially point to 0.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
                }

                if (pool_manager.dw_stk_control_blocks[i].stk_sz != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack size not initially 0 bytes.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
                }
            }

            return true;
        }

        static bool allocate_multi_dw_stk_test()
        {
            const constexpr char* TEST_TAG = "allocate_multi_dw_stk_test";
            enum DWStkSz_t
            {
                DWStkSz_0 = 128, // Value at index 0
                DWStkSz_1 = 64,  // Value at index 1
                DWStkSz_2 = 128, // Value at index 2
                DWStkSz_3 = 32,  // Value at index 3
                DWStkSz_4 = 48   // Value at index 4
            };

            uint16_t dw_stk_idx = 0U;

            for (int i = 0; i < TEST_OPEEconfigMAX_DATA_WATCH_CNT; i++)
            {
                switch (i)
                {
                    case 0:
                        if (!allocate_dw_stk_helper<DWStkSz_0>(TEST_TAG, dw_stk_idx))
                            return false;
                        break;

                    case 1:
                        if (!allocate_dw_stk_helper<DWStkSz_1>(TEST_TAG, dw_stk_idx))
                            return false;
                        break;

                    case 2:
                        if (!allocate_dw_stk_helper<DWStkSz_2>(TEST_TAG, dw_stk_idx))
                            return false;
                        break;

                    case 3:
                        if (!allocate_dw_stk_helper<DWStkSz_3>(TEST_TAG, dw_stk_idx))
                            return false;
                        break;

                    case 4:
                        if (!allocate_dw_stk_helper<DWStkSz_4>(TEST_TAG, dw_stk_idx))
                            return false;
                        break;

                    default:

                        break;
                }
            }

            return true;
        }

        static bool attempt_cb_pool_overflow_test()
        {
            const constexpr char* TEST_TAG = "attempt_cb_pool_overflow_test";
            const constexpr uint16_t DWStkSz = 2 * (OPEEconfigCB_POOL_SZ / TEST_OPEEconfigMAX_DATA_WATCH_CNT); // should be guaranteed to cause invalid write

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();
            uint16_t dw_stk_idx = 0U;
            uint16_t total_mem_occupied = 0U;
            bool allocation_success = true;

            for (int i = 0; i < TEST_OPEEconfigMAX_DATA_WATCH_CNT; i++)
            {
                allocation_success = pool_manager.template allocate_dw_stk<DWStkSz>(dw_stk_idx);
                total_mem_occupied += DWStkSz;

                if (total_mem_occupied < OPEEconfigCB_POOL_SZ)
                {
                    if (!allocation_success)
                    {
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: allocate_dw_stk() did not return successfully on valid allocation parameters  (requested %d/%dbytes).",
                                total_mem_occupied, OPEEconfigCB_POOL_SZ);
                        return false;
                    }
                    else
                    {
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: allocate_dw_stk() successfully allocated %d/%dbytes", total_mem_occupied, OPEEconfigCB_POOL_SZ);
                    }
                }
                else
                {
                    if (allocation_success)
                    {
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: allocate_dw_stk() returned successfully on invalid allocation parameters,  allocated %d/%dbytes ?",
                                total_mem_occupied, OPEEconfigCB_POOL_SZ);
                        return false;
                    }
                    else
                    {
                        OPEEngineTestHelper::print_test_msg(TEST_TAG,
                                "PASS: allocate_dw_stk() did not return successfully on invalid allocation parameters (requested %d/%dbytes).", total_mem_occupied,
                                OPEEconfigCB_POOL_SZ);
                    }
                }
            }

            pool_manager.reset();
            return true;
        }

        static bool allocate_cb_test()
        {
            const constexpr char* TEST_TAG = "allocate_cb_test";

            class DummyClassForCapture
            {
                public:
                    DummyClassForCapture()
                    {
                    }

                    void set_data_a(uint32_t new_data_a)
                    {
                        data_b = !data_b;
                        data_a = new_data_a;
                    }

                private:
                    uint32_t data_a = 0U;
                    bool data_b = false;
            };

            if (!allocate_multi_dw_stk_test())
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: could not allocate DWStks to store callbacks.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: successfully allocated DWStks to store callbacks.");
            }

            uint8_t sub_count = 0U;
            SubscriberCtrlBlock sub_list;

            DummyClassForCapture capture_me;
            // capture list item has to be used in callback or it will be optimized out
            if (!store_cb_helper<16, uint32_t>([&capture_me](uint32_t new_data) { capture_me.set_data_a(new_data); }, TEST_TAG, &sub_list, sub_count, 0U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store callback.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            return true;
        }

        static bool allocate_multi_cb_test()
        {
            const constexpr char* TEST_TAG = "allocate_multi_cb_test";

            if (!allocate_multi_dw_stk_test())
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: could not allocate DWStks to store callbacks.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: successfully allocated DWStks to store callbacks.");
            }

            uint8_t sub_count_dw_stk_0 = 0U;
            SubscriberCtrlBlock sub_list_dw_stk_0[3];
            uint8_t sub_count_dw_stk_1 = 0U;
            SubscriberCtrlBlock sub_list_dw_stk_1[3];
            uint8_t sub_count_dw_stk_2 = 0U;
            SubscriberCtrlBlock sub_list_dw_stk_2[3];
            uint8_t sub_count_dw_stk_3 = 0U;
            SubscriberCtrlBlock sub_list_dw_stk_3[2];
            uint8_t sub_count_dw_stk_4 = 0U;
            SubscriberCtrlBlock sub_list_dw_stk_4[2];

            if (!store_cb_helper<16, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_0, sub_count_dw_stk_0, 0U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<32, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_0, sub_count_dw_stk_0, 0U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, bool>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_0, sub_count_dw_stk_0, 0U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_2 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_1, sub_count_dw_stk_1, 1U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_1.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_1, sub_count_dw_stk_1, 1U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_1.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, bool>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_1, sub_count_dw_stk_1, 1U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_2 on dw_stk_1.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_2, sub_count_dw_stk_2, 2U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_2.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<32, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_2, sub_count_dw_stk_2, 2U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_2.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, bool>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_2, sub_count_dw_stk_2, 2U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_2 on dw_stk_2.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_3, sub_count_dw_stk_3, 3U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_3.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<12, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_3, sub_count_dw_stk_3, 3U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_3.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<16, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_4, sub_count_dw_stk_4, 4U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_4.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<24, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list_dw_stk_4, sub_count_dw_stk_4, 4U))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_4.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            return true;
        }

        static bool attempt_dw_stk_overflow_test()
        {
            const constexpr char* TEST_TAG = "attempt_dw_stk_overflow_test";

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();
            const constexpr uint16_t DWStkSz = 64U;
            const constexpr uint16_t CbWrprMaxSz = DWStkSz / 4U;
            uint16_t dw_stk_idx = 0U;

            if (!allocate_dw_stk_helper<DWStkSz>(TEST_TAG, dw_stk_idx))
                return false;

            uint8_t sub_count = 0U;
            SubscriberCtrlBlock sub_list[5];

            if (!store_cb_helper<CbWrprMaxSz, uint32_t>([](uint32_t new_data) {}, TEST_TAG, sub_list, sub_count, dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_0 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<CbWrprMaxSz, uint64_t>([](uint32_t new_data) {}, TEST_TAG, sub_list, sub_count, dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_1 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (!store_cb_helper<CbWrprMaxSz, bool>([](uint32_t new_data) {}, TEST_TAG, sub_list, sub_count, dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Could not store cb_2 on dw_stk_0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Successfully stored callback.");
            }

            if (store_cb_helper<CbWrprMaxSz, char*>([](char* new_data) {}, TEST_TAG, sub_list, sub_count, dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocated for cb in cb_pool past respective DWStk guard bytes.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Failed to store callback on overflow conditions.");
            }

            if (store_cb_helper<CbWrprMaxSz, uintptr_t>([](uintptr_t new_data) {}, TEST_TAG, sub_list, sub_count, dw_stk_idx))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocated for cb in cb_pool past respective DWStk guard bytes.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Failed to store callback on overflow conditions.");
            }

            pool_manager.reset();

            return true;
        }
};

TEST_CASE("pool_manager_init_val_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::pool_manager_init_val_test(), "FAILED pool_manager_init_val_test");
}

TEST_CASE("dw_stk_ctrl_blks_init_val_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::dw_stk_ctrl_blks_init_val_test(), "FAILED dw_stk_ctrl_blks_init_val_test");
}

TEST_CASE("allocate_single_dw_stk_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::allocate_single_dw_stk_test(), "FAILED allocate_single_dw_stk_test");
}

TEST_CASE("pool_manager_vals_after_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::pool_manager_vals_after_reset_test(), "FAILED pool_manager_vals_after_reset_test");
}

TEST_CASE("dw_stk_ctrl_blks_vals_after_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::dw_stk_ctrl_blks_vals_after_reset_test(), "FAILED dw_stk_ctrl_blks_vals_after_reset_test");
}

TEST_CASE("allocate_multi_dw_stk_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::allocate_multi_dw_stk_test(), "FAILED allocate_multi_dw_stk_test");
}

TEST_CASE("pool_manager_vals_after_dw_stk_allocation_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::pool_manager_vals_after_reset_test(), "FAILED pool_manager_vals_after_dw_stk_allocation_reset_test");
}

TEST_CASE("dw_stk_ctrl_blks_vals_after_dw_stk_allocation_after_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::dw_stk_ctrl_blks_vals_after_reset_test(), "FAILED dw_stk_ctrl_blks_vals_after_dw_stk_allocation_after_reset_test");
}

TEST_CASE("attempt_cb_pool_overflow_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::attempt_cb_pool_overflow_test(), "FAILED attempt_cb_pool_overflow_test");
}

TEST_CASE("allocate_cb_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::allocate_cb_test(), "FAILED allocate_cb_test");
}

TEST_CASE("pool_manager_vals_after_cb_allocation_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::pool_manager_vals_after_reset_test(), "FAILED pool_manager_vals_after_cb_allocation_reset_test");
}

TEST_CASE("dw_stk_ctrl_blks_vals_after_cb_allocation_after_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::dw_stk_ctrl_blks_vals_after_reset_test(), "FAILED dw_stk_ctrl_blks_vals_after_cb_allocation_after_reset_test");
}

TEST_CASE("allocate_multi_cb_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::allocate_multi_cb_test(), "FAILED allocate_multi_cb_test");
}

TEST_CASE("pool_manager_vals_after_multi_cb_allocation_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::pool_manager_vals_after_reset_test(), "FAILED pool_manager_vals_after_multi_cb_allocation_reset_test");
}

TEST_CASE("dw_stk_ctrl_blks_vals_after_multi_cb_allocation_after_reset_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::dw_stk_ctrl_blks_vals_after_reset_test(), "FAILED dw_stk_ctrl_blks_vals_after_multi_cb_allocation_after_reset_test");
}

TEST_CASE("attempt_dw_stk_overflow_test", "[PoolManagerTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, PoolManagerTests::attempt_dw_stk_overflow_test(), "FAILED attempt_dw_stk_overflow_test");
}