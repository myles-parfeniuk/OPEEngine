#pragma once
// third-party
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
// in-house
#include "OPEEngineConfig.h"
#include "SubscriberCtrlBlock.h"
#include "CbPoolManager.h"

template <size_t DWMaxCnt>
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

        static bool queue_cbs(SubscriberCtrlBlock* subscribers, uint8_t sub_count, uintptr_t arg2p_addr, uintptr_t data_addr)
        {
            for (int i = 0; i < sub_count; i++)
                if (subscribers[i].cb_wrpr != nullptr)
                {
                    // only pass data address to last subscriber on list such that data is updated after last callback execution
                    cb_queue_item_t item2queue = {subscribers[i].cb_wrpr, arg2p_addr, (i != (sub_count - 1)) ? 0 : data_addr};
                    if (xQueueSend(queue_cb_hdl, &item2queue, 0UL) != pdTRUE)
                        return false;
                }

            return true;
        }

        static CbPoolManager<DWMaxCnt>& get_manager()
        {
            return manager;
        }

    private:
        typedef struct cb_queue_item_t
        {
                CbWrapperGeneric* cb;
                uintptr_t arg2p_addr;
                uintptr_t data_addr;
        } cb_queue_item_t;

        inline static CbPoolManager<DWMaxCnt> manager;
        inline static TaskHandle_t task_cb_hdl = NULL;
        inline static QueueHandle_t queue_cb_hdl = xQueueCreate(OPEEconfigCB_QUEUE_SZ, sizeof(cb_queue_item_t));

        // prevent creation and copies of this class, it's just a container for methods
        CbHelper() = default;
        CbHelper(const CbHelper&) = delete;
        CbHelper& operator=(const CbHelper&) = delete;
};