#pragma once

using opee_uint8_t = uint8_t;
using opee_uint_t = uint16_t;
using opee_int_t = int16_t;
using opee_uintl_t = uint32_t;
using opee_intl_t = int32_t;
using opee_uintptr_t = uintptr_t;

enum OPEEngineRes_t
{
    OPEE_OK = 0,                 ///< Successful operation.
    OPEE_CB_POOL_FULL,           ///< CB pool is full, failure occurs if allocating a DWStk will result in CB Pool overflow, see OPEEconfigCB_POOL_SZ.
    OPEE_DW_STK_NOT_INIT,        ///< The DWStk for this DataWatch object was never correctly initialized.
    OPEE_MAX_DWSTK_CNT_EXCEEDED, ///< Max DataWatch objects have been exceeded, see OPEEconfigMAX_DATA_WATCH_CNT.
    OPEE_DWSTK_FULL,        ///< DWStk for this DataWatch object is full, failure occurs if allocating a callback will result in DWStk overflow, see DWStkSz on DataWatch object.
    OPEE_INVALID_DWSTK_IDX, ///< Invalid DWStk idx, there is no corresponding index for DWStk in CbPoolManager.
    OPEE_CB_POOL_RGN_NOT_EMPTY,   ///< Region in CB Pool where DWStk allocation was attempted is not empty.
    OPEE_CB_WRPR_CREATION_FAILED, ///< Null pointer was returned from CbWrpr constructor, callback storage aborted.
    OPEE_MAX_SUB_CNT_EXCEEDED,    ///< Max sub count for a given DataWatch object has been exceeded, see CbMaxCnt on DataWatch object.
    OPEE_CB_QUEUE_FULL            ///< CB Queue is full, callbacks could not be posted to cb_task() for execution.
};

constexpr const char* OPEEngineRes_to_str(OPEEngineRes_t res)
{
    switch (res)
    {
        case OPEE_OK:
            return "OPEE_OK";
        case OPEE_CB_POOL_FULL:
            return "OPEE_CB_POOL_FULL";
        case OPEE_MAX_DWSTK_CNT_EXCEEDED:
            return "OPEE_MAX_DWSTK_CNT_EXCEEDED";
        case OPEE_DWSTK_FULL:
            return "OPEE_DWSTK_FULL";
        case OPEE_INVALID_DWSTK_IDX:
            return "OPEE_INVALID_DWSTK_IDX";
        case OPEE_CB_POOL_RGN_NOT_EMPTY:
            return "OPEE_CB_POOL_RGN_NOT_EMPTY";
        case OPEE_CB_WRPR_CREATION_FAILED:
            return "OPEE_CB_WRPR_CREATION_FAILED";
        case OPEE_MAX_SUB_CNT_EXCEEDED:
            return "OPEE_MAX_SUB_CNT_EXCEEDED";
        case OPEE_DW_STK_NOT_INIT:
            return "OPEE_DW_STK_NOT_INIT";
        default:
            return "UNKNOWN_ERROR";
    }
}