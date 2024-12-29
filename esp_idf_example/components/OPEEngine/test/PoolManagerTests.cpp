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

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();
            uint16_t dw_stk_idx = 0;

            pool_manager.template allocate_dw_stk<DWStkSz>(dw_stk_idx);

            // first DataWatch stack allocated should correspond to number 0
            if (dw_stk_idx != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk reference number/index of first allocated DWStk not 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk index check.");
            }

            // starting offset of first DataWatch stack should be 0
            if (pool_manager.dw_stk_control_blocks[0].cb_pool_addr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset of first allocated DWStk not 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk addr offset check.");
            }

            // stack pointer offset should still point to 0, no callbacks have been stored
            if (pool_manager.dw_stk_control_blocks[0].stk_ptr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset non-zero before any callbacks stored.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
            }

            // stack size should be DWStkSz
            if (pool_manager.dw_stk_control_blocks[0].stk_sz != DWStkSz)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk size within respective dw_stk_control_block not equal to assigned value.");
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
            }

            // allocator offset should be incremented by DWStkSz to point to next free start address for next stack to allocate
            if (pool_manager.allocator_ofs != DWStkSz)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocator offset not incremented for next DWStk allocation.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Allocator offset check.");
            }

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
            const constexpr char* TEST_TAG = "dw_stk_ctrl_blks_vals_after_reset_test";
            enum DWStkSz_t
            {
                DWStkSz_0 = 32,  // Value at index 0
                DWStkSz_1 = 64,  // Value at index 1
                DWStkSz_2 = 128, // Value at index 2
                DWStkSz_3 = 32,  // Value at index 3
                DWStkSz_4 = 32   // Value at index 4
            };

            CbPoolManager<TEST_OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager = CbHelper<TEST_OPEEconfigMAX_DATA_WATCH_CNT>::get_manager();
            uint16_t dw_stk_idx = 0U;
            uint16_t total_mem_occupied = 0U;

            for (int i = 0; i < TEST_OPEEconfigMAX_DATA_WATCH_CNT; i++)
            {
                switch (i)
                {
                    case 0:
                        pool_manager.template allocate_dw_stk<DWStkSz_0>(dw_stk_idx);
                        break;

                    case 1:
                        pool_manager.template allocate_dw_stk<DWStkSz_1>(dw_stk_idx);
                        break;

                    case 2:
                        pool_manager.template allocate_dw_stk<DWStkSz_2>(dw_stk_idx);
                        break;

                    case 3:
                        pool_manager.template allocate_dw_stk<DWStkSz_3>(dw_stk_idx);
                        break;

                    case 4:
                        pool_manager.template allocate_dw_stk<DWStkSz_4>(dw_stk_idx);
                        break;

                    default:

                    break; 
                }

                // DataWatch stack idx should correspond to order in which they were allocated (should increase with each allocation)
                if (dw_stk_idx != i)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk reference number/index does not match expected. Actual: %d Expected: %d", dw_stk_idx, i);
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk index check. Actual: %d Expected: %d", dw_stk_idx, i);
                }

                // starting offset of each DataWatch stack should increase with each allocation by size of previous DWStk allocation
                uint16_t expected_cb_pool_ofs = (i == 0) ? 0 : (pool_manager.dw_stk_control_blocks[i - 1].cb_pool_addr_ofs + pool_manager.dw_stk_control_blocks[i - 1].stk_sz);
                if (pool_manager.dw_stk_control_blocks[i].cb_pool_addr_ofs != expected_cb_pool_ofs)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset does not match expected value. Actual: %d, Expected: %d",
                            pool_manager.dw_stk_control_blocks[i].cb_pool_addr_ofs, expected_cb_pool_ofs);
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(
                            TEST_TAG, "PASS: DWStk addr offset check. Actual: %d, Expected: %d", pool_manager.dw_stk_control_blocks[i].cb_pool_addr_ofs, expected_cb_pool_ofs);
                }

                // stack pointer offset should still point to 0, no callbacks have been stored
                if (pool_manager.dw_stk_control_blocks[i].stk_ptr_ofs != 0)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset non-zero before any callbacks stored.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
                }

                // stack size should be DWStkSz
                if (pool_manager.dw_stk_control_blocks[i].stk_sz != (i == 0 ? DWStkSz_0 : (i == 1 ? DWStkSz_1 : (i == 2 ? DWStkSz_2 : (i == 3 ? DWStkSz_3 : (i == 4 ? DWStkSz_4 : DWStkSz_0))))))
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk size within respective dw_stk_control_block not equal to assigned value.");
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
                }

                total_mem_occupied += (i == 0 ? DWStkSz_0 : (i == 1 ? DWStkSz_1 : (i == 2 ? DWStkSz_2 : (i == 3 ? DWStkSz_3 : (i == 4 ? DWStkSz_4 : DWStkSz_0)))));

                if (pool_manager.allocator_ofs != total_mem_occupied)
                {
                    OPEEngineTestHelper::print_test_msg(
                            TEST_TAG, "FAIL: Allocator offset not incremented for next DWStk allocation. Actual: %d, Expected: %d", pool_manager.allocator_ofs, total_mem_occupied);
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: Allocator offset check. Actual: %d, Expected: %d", pool_manager.allocator_ofs, total_mem_occupied);
                }
            }

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
