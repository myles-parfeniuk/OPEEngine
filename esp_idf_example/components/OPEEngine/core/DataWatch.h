#pragma once

// OPEEngine
#include "OPEEngine_types.h"
#include "CbHelper.h"
#include "ForwardUtil.h"
#include "SubCtrlBlk.h"
#include "Subscriber.h"

namespace opee
{
    template <typename TArg, opee_size_t DWStkSz, opee_size_t CbMaxCnt>
    class DataWatch
    {

        private:
            SubCtrlBlk subscribers[CbMaxCnt];
            opee_uint8_t sub_count = 0;
            TArg data;
            TArg arg2p;
            opee_size_t dw_stk;
            CbPoolManager<OPEEconfigMAX_DATA_WATCH_CNT>& pool_manager;
            OPEEngineRes_t dw_stk_alloc_res = OPEE_DW_STK_NOT_INIT;

        public:
            DataWatch(TArg init_data)
                : data(init_data)
                , arg2p(init_data)
                , pool_manager(CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::get_manager())
            {
                dw_stk_alloc_res = pool_manager.template allocate_dw_stk<DWStkSz, CbMaxCnt>(dw_stk, subscribers);
            }

            template <opee_size_t CbWrprMaxSz, typename TLambda>
            OPEEngineRes_t subscribe(TLambda&& lambda, Subscriber<TArg>* sub_interface = nullptr)
            {
                using TCb = ForwardUtil::decay_t<TLambda>; // get the actual type of the lambda by stripping it of references with decay

                // ensure respective DWStk allocated correctly
                if (dw_stk_alloc_res != OPEE_OK)
                    return dw_stk_alloc_res;

                // check if the max amount of subscribers has been reached
                if (sub_count < CbMaxCnt)
                {
                    CbWrprDefined<TArg, TCb> cb_wrpr(ForwardUtil::forward<TLambda>(lambda)); // create a temp wrapper object on stack to store callback

                    OPEEngineRes_t OPEEres = pool_manager.template store_cb<TArg, TCb, CbWrprMaxSz>(subscribers, sub_count, dw_stk, &cb_wrpr);

                    if ((sub_interface != nullptr) && (OPEEres == OPEE_OK))
                    {
                        const opee_uintptr_t _arg2p = reinterpret_cast<opee_uintptr_t>(&arg2p);
                        *sub_interface = Subscriber<TArg>(&subscribers[sub_count - 1], _arg2p);
                    }

                    return OPEEres;
                }

                return OPEE_MAX_SUB_CNT_EXCEEDED;
            }

            OPEEngineRes_t set(TArg arg, bool execute_callbacks = true)
            {
                OPEEngineRes_t OPEEres = OPEE_ALL_CB_MUTED;

                arg2p = arg;
                if (execute_callbacks && (sub_count > 0))
                {
                    const opee_uintptr_t _arg2p = reinterpret_cast<opee_uintptr_t>(&arg2p);
                    const opee_uintptr_t _data = reinterpret_cast<opee_uintptr_t>(&data);

                    OPEEres = CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::queue_cbs(subscribers, sub_count, _arg2p, _data);
                }

                if (OPEEres == OPEE_ALL_CB_MUTED)
                {
                    data = arg2p;
                    return OPEE_OK;
                }
                else
                {
                    return OPEEres;
                }
            }

            TArg get()
            {
                return data;
            }
    };
} // namespace opee
