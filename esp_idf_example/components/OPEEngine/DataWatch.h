#pragma once

// std lib
#include <cstddef>
#include <utility>
#include <iostream>
// OPEEngine
#include "OPEEngine_types.h"
#include "CbHelper.h"
#include "SubscriberCtrlBlock.h"
#include "Subscriber.h"

namespace opee
{
    template <typename TArg, size_t DWStkSz, size_t CbMaxCnt>
    class DataWatch
    {

        private:
            SubscriberCtrlBlock subscribers[CbMaxCnt];
            opee_uint8_t sub_count = 0;
            TArg data;
            TArg arg2p;
            opee_uint_t dw_stk;
            CbPoolManager<OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager;
            OPEEngineRes_t dw_stk_alloc_res = OPEE_DW_STK_NOT_INIT;

        public:
            DataWatch(TArg init_data)
                : data(init_data)
                , arg2p(init_data)
                , pool_manager(CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::get_manager())
            {
                dw_stk_alloc_res = pool_manager.template allocate_dw_stk<DWStkSz>(dw_stk);
            }

            template <size_t CbWrprMaxSz, typename TLambda>
            OPEEngineRes_t subscribe(TLambda&& lambda, Subscriber<TArg>* sub_interface = nullptr)
            {
                using TCb = std::decay_t<TLambda>; // get the actual type of the lambda by stripping it of references with decay

                // ensure respective DWStk allocated correctly
                if (dw_stk_alloc_res != OPEE_OK)
                    return dw_stk_alloc_res;

                // check if the max amount of subscribers has been reached
                if (sub_count < CbMaxCnt)
                {
                    CbWrapperDefined<TArg, TCb> cb_wrpr(std::forward<TLambda>(lambda)); // create a temp wrapper object on stack to store callback

                    OPEEngineRes_t res = pool_manager.template store_cb<TArg, TCb, CbWrprMaxSz>(subscribers, sub_count, dw_stk, &cb_wrpr);

                    if ((sub_interface != nullptr) && (res == OPEE_OK))
                    {
                        const opee_uintptr_t arg2p_ptr = reinterpret_cast<opee_uintptr_t>(&arg2p);
                        *sub_interface = Subscriber<TArg>(&subscribers[sub_count - 1], arg2p_ptr);
                    }

                    return res;
                }

                return OPEE_MAX_SUB_CNT_EXCEEDED;
            }

            OPEEngineRes_t set(TArg arg)
            {

                arg2p = arg;
                const opee_uintptr_t arg2p_ptr = reinterpret_cast<opee_uintptr_t>(&arg2p);
                const opee_uintptr_t data_ptr = reinterpret_cast<opee_uintptr_t>(&data);
                return CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::queue_cbs(subscribers, sub_count, arg2p_ptr, data_ptr);
            }

            TArg get()
            {
                return data;
            }
    };
} // namespace opee
