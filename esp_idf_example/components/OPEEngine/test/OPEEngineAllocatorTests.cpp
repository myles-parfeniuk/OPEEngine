//third party includes
#include "unity.h"
//in-house includes
#include "../callback/CbHelper.h"
#include "../OPEEngineConfig.h"
#include "OPEEngineTestHelper.hpp"

class OPEEngineAllocatorTests
{
    public:
        static const constexpr uint16_t DWMaxCnt = 1;
        static const constexpr uint16_t DWStkSz = 128;
        inline static uint16_t dw_stk = 0;

        static bool allocator_init_val_test()
        {
            const constexpr char* TEST_TAG = "allocator_init_val_tests";

            CbAllocator<DWMaxCnt>& allocator = CbHelper<DWMaxCnt>::get_allocator();

            if (allocator.allocator_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: Allocator offset did not initially point to 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: allocator offset check.");
            }

            if (allocator.dw_count != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DataWatch count non-zero before any stacks allocated.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch count check.");
            }

            for (int i = 0; i < OPEEconfigCB_POOL_SZ; i++)
                if (allocator.cb_pool[i] != 0U)
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

            CbAllocator<DWMaxCnt>& allocator = CbHelper<DWMaxCnt>::get_allocator();

            if (allocator.dw_stk_control_blocks[0].cb_pool_addr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset did not initially point to 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk addr offset check.");
            }

            if (allocator.dw_stk_control_blocks[0].stk_ptr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset did not initially point to 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
            }

            if (allocator.dw_stk_control_blocks[0].stk_sz != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack size not initially 0 bytes.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
            }

            return true;
        }

        static bool allocate_dw_stk_test()
        {
            const constexpr char* TEST_TAG = "allocate_dw_stk_test";

            CbAllocator<DWMaxCnt>& allocator = CbHelper<DWMaxCnt>::get_allocator();
            allocator.template allocate_dw_stk<DWStkSz>(dw_stk);

            // first DataWatch stack allocated should correspond to number 0
            if (dw_stk != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk reference number/index of first allocated DWStk not 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk index check.");
            }

            // starting offset of first DataWatch stacks should be 0
            if (allocator.dw_stk_control_blocks[0].cb_pool_addr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk addr offset of first allocated DWStk not 0.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk addr offset check.");
            }

            // stack pointer offset should still point to 0, no callbacks have been stored
            if (allocator.dw_stk_control_blocks[0].stk_ptr_ofs != 0)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk stack pointer offset non-zero before any callbacks stored.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk stack pointer offset check.");
            }

            // stack size should be DWStkSz
            if (allocator.dw_stk_control_blocks[0].stk_sz != DWStkSz)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: DWStk size within respective dw_stk_control_block not equal to assigned value.");
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DWStk size check.");
            }

            // allocator offset should be incremented by DWStkSz to point to next free start address for next stack to allocate
            if (allocator.allocator_ofs != DWStkSz)
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
};

TEST_CASE("allocator_init_val_test", "[DWStkAllocatorTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, OPEEngineAllocatorTests::allocator_init_val_test(), "FAILED allocator_init_val_test");
}

TEST_CASE("dw_stk_ctrl_blks_init_val_test", "[DWStkAllocatorTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, OPEEngineAllocatorTests::dw_stk_ctrl_blks_init_val_test(), "FAILED dw_stk_ctrl_blks_init_val_test");
}

TEST_CASE("allocate_dw_stk_test", "[DWStkAllocatorTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, OPEEngineAllocatorTests::allocate_dw_stk_test(), "FAILED allocate_dw_stk_test");
}
