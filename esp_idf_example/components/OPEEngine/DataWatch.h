#pragma once

// std lib includes
#include <cstddef>
#include <utility>
#include <iostream>

// OPEEngine includes
#include "CbHelper.h"
#include "SubscriberCtrlBlock.h"

template <typename TArg, size_t DWStkSz, size_t CbMaxCnt, size_t DWMaxCnt>
class DataWatch
{

    private:
        SubscriberCtrlBlock subscribers[CbMaxCnt];
        uint8_t sub_count = 0;
        TArg data;
        TArg arg2p;
        uint16_t dw_stk;
        CbPoolManager<DWMaxCnt>& pool_manager;

    public:
        DataWatch(TArg init_data)
            : data(init_data)
            , arg2p(init_data)
            , pool_manager(CbHelper<DWMaxCnt>::get_manager())
        {
            pool_manager.template allocate_dw_stk<DWStkSz>(dw_stk);
        }

        template <size_t CbWrprMaxSz, typename TLambda>
        bool subscribe(TLambda&& lambda)
        {
            using TCb = std::decay_t<TLambda>; // get the actual type of the lambda by stripping it of references with decay

            // check if the max amount of subscribers has been reached
            if (sub_count < CbMaxCnt)
            {
                CbWrapperDefined<TArg, TCb> cb_wrpr(std::forward<TLambda>(lambda)); // create a temp wrapper object on stack to store callback

                if (pool_manager.template store_cb<TArg, TCb, CbWrprMaxSz>(subscribers, sub_count, dw_stk, &cb_wrpr))
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
