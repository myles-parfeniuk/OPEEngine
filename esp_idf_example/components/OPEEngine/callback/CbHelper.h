#pragma once
// OPEEngine
#include "OPEEngine_freeRTOS_hook.h"
#include "OPEEngineConfig.h"
#include "OPEEngine_types.h"
#include "SubCtrlBlk.h"
#include "CbPoolManager.h"

namespace opee
{
    template <opee_size_t DWMaxCnt>
    class CbHelper
    {
        public:
            static void init()
            {
                static bool init = false;

                if (!init)
                {
                    xTaskCreate(cb_task, "cb_tsk", OPEEconfigCB_TASK_STACK_SZ, NULL, OPEEconfigCB_TASK_PRIO, &task_cb_hdl);
                    init = true;
                }
            }

            static void cb_task(void* arg)
            {
                cb_queue_item_t item2rx;

                while (1)
                {
                    // block until any callbacks are queued
                    if (xQueueReceive(queue_cb_hdl, &item2rx, portMAX_DELAY) == pdTRUE)
                    {
                        if (item2rx.arg2p_addr != 0)
                            item2rx.cb->invoke(item2rx.arg2p_addr, item2rx.data_addr);
                    }
                }
            }

            static opee_ssize_t find_last_cb_to_execute(SubCtrlBlk* subscribers, opee_uint8_t sub_count)
            {
                for (opee_ssize_t i = sub_count - 1; i >= 0; i--)
                    if (!subscribers[i].muted)
                        return i;

                return -1;
            }

            static OPEEngineRes_t queue_cbs(SubCtrlBlk* subscribers, opee_uint8_t sub_count, opee_uintptr_t arg2p_addr, opee_uintptr_t data_addr)
            {
                opee_ssize_t last_cb_2_execute = find_last_cb_to_execute(subscribers, sub_count);

                if (last_cb_2_execute != -1)
                {
                    for (opee_ssize_t i = 0; i < sub_count; i++)
                        if (subscribers[i]._cb_wrpr != nullptr)
                        {
                            // skip this callback if muted
                            if (subscribers[i].muted)
                                continue;

                            // only pass data address to last subscriber on list such that data is updated after last callback execution
                            cb_queue_item_t item2queue = {subscribers[i]._cb_wrpr, arg2p_addr, (i != last_cb_2_execute) ? 0 : data_addr};
                            if (xQueueSend(queue_cb_hdl, &item2queue, 0UL) != pdTRUE)
                                return OPEE_CB_QUEUE_FULL;
                        }
                }
                else
                {
                    return OPEE_ALL_CB_MUTED;
                }

                return OPEE_OK;
            }

            static CbPoolManager<DWMaxCnt>& get_manager()
            {
                return manager;
            }

        private:
            typedef struct cb_queue_item_t
            {
                    CbWrprGeneric* cb;
                    opee_uintptr_t arg2p_addr;
                    opee_uintptr_t data_addr;
            } cb_queue_item_t;

            inline static CbPoolManager<DWMaxCnt> manager;
            inline static TaskHandle_t task_cb_hdl = NULL;
            inline static QueueHandle_t queue_cb_hdl = xQueueCreate(OPEEconfigCB_QUEUE_SZ, sizeof(cb_queue_item_t));

            // prevent creation and copies of this class, it's just a container for methods
            CbHelper() = default;
            CbHelper(const CbHelper&) = delete;
            CbHelper& operator=(const CbHelper&) = delete;
    };
} // namespace opee