#pragma once

enum OPEEngineRes_t
{
    OPEE_OK = 0,
    OPEE_CB_POOL_FULL,
    OPEE_DW_STK_NOT_INIT,
    OPEE_MAX_DWSTK_CNT_EXCEEDED,
    OPEE_DWSTK_FULL,
    OPEE_INVALID_DWSTK_IDX,
    OPEE_CB_POOL_RGN_NOT_EMPTY,
    OPEE_CB_WRPR_CREATION_FAILED,
    OPEE_MAX_SUB_CNT_EXCEEDED
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