#pragma once

// std lib includes
#include <cstddef>
#include <utility>

// OPEEngine includes
#include "CbHelper.h"

// windows demo only includes
#include <iostream>

template <typename TArg, size_t DWStkSz, size_t CbMaxCnt, size_t DWMaxCnt>
class DataWatch
{

    private:
        CbWrapperGeneric* subscribers[CbMaxCnt] = {nullptr};
        uint8_t sub_count = 0;
        TArg data;
        TArg arg2p;
        uint16_t dw_stk;
        CbAllocater<DWMaxCnt>& allocater;

    public:
        DataWatch(TArg init_data)
            : data(init_data)
            , arg2p(init_data)
            , allocater(CbHelper<DWMaxCnt>::get_allocater())
        {
            allocater.template allocate_dw_stk<TArg, DWStkSz>(dw_stk); // argument passing here is gimped, check it
        }

        template <size_t CbWrprMaxSz, typename TLambda>
        bool subscribe(TLambda&& lambda)
        {
            using TCb = std::decay_t<TLambda>; // get the actual type of the lambda by stripping it of references with decay

            // check if the max amount of subscribers has been reached
            if (sub_count < CbMaxCnt)
            {
                CbWrapperDefined<TArg, TCb, CbWrprMaxSz> cb_wrpr(
                        std::forward<TLambda>(lambda)); // create a temp wrapper object on stack to store callback

                if (allocater.allocate_cb(subscribers, sub_count, dw_stk, &cb_wrpr))
                    return true;
            }

            return false;
        }

        void set(TArg arg)
        {

            arg2p = arg;
            const uintptr_t arg2p_ptr = reinterpret_cast<uintptr_t>(&arg2p);
            const uintptr_t data_ptr = reinterpret_cast<uintptr_t>(&data);
            CbHelper<DWMaxCnt>::queue_cbs(subscribers, sub_count, arg2p_ptr, data_ptr);
        }

        TArg get()
        {
            return data;
        }
};
