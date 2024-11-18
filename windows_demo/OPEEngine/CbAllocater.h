#pragma once
//std lib includes
#include <cstring>
#include <cstdio>

//OPEEngine includes
#include "CbWrapperDefined.h"
#include "OPEEngineConfig.h"

template<size_t DWMaxCnt>
class CbAllocater
{
private:
    inline static uint8_t cb_pool[OPEEconfigCB_POOL_SZ];
    inline static uint16_t dw_stk_ptr_ofs[DWMaxCnt];
    inline static uint16_t dw_count = 0; 
    inline static uint16_t allocater_ofs = 0; 

    template <size_t... DWStkSz>
    static constexpr void check_cb_pool_overflow()
    {
        static_assert((DWStkSz + ... + 0) < OPEEconfigCB_POOL_SZ, "CbPool size exceeded. Increase OPEEconfigCB_POOL_SZ or decrease DataWatch stack sizes.");

    }

    template <size_t Bytes2Allocate, size_t CbWrprMaxSz>
    static constexpr bool check_cb_max_sz_overflow()
    {
        static_assert(Bytes2Allocate < CbWrprMaxSz, "CbWrapper size exceeds TCbMaxSz, increase on subscribe call.");

        if (Bytes2Allocate > CbWrprMaxSz)
            return false;

        return true;
    }


public:
    CbAllocater() {}

    template <typename TArg, size_t DWStkSz>
    bool allocate_dw_stk(uint16_t &dw_stk)
    {


        //compilation check: does accumulation of all DataWatch stack sizes exceed OPEEconfigCB_POOL_SZ?
        check_cb_pool_overflow<DWStkSz>();

        if ((DWStkSz + allocater_ofs) < OPEEconfigCB_POOL_SZ)
        {
            if (dw_count < DWMaxCnt)
            {
                //store offset for respective dw stack 
                dw_stk_ptr_ofs[dw_count] = allocater_ofs; //stack pointer starts at bottom of respective DWStk and grows with each cb subscribed
                //increment the datawatch count and allocater offset for next DW registration
                allocater_ofs += DWStkSz;
                dw_stk = dw_count; //save dw_stk # to reference for cb allocation
                dw_count++;
                return true;
            }
        }
     
        return false; 
    }

    template <typename TArg, typename TCb, size_t CbWrprMaxSz>
    bool allocate_cb(CbWrapperGeneric** subscribers, uint8_t &sub_count, const uint16_t dw_stk, CbWrapperDefined<TArg, TCb, CbWrprMaxSz> *cb_wrpr)
    {
        const constexpr size_t bytes2allocate = sizeof(CbWrapperDefined<TArg, TCb, CbWrprMaxSz>);

        //compilation check: does cb size in memory exceed CbWrprMaxSz?
        if (check_cb_max_sz_overflow<bytes2allocate, CbWrprMaxSz>())
        {


            if (dw_stk < dw_count) //was dw object correctly registered?
            {
                printf("CbWrapper Sz: %dbytes\n\r", bytes2allocate);
                std::memcpy(cb_pool + dw_stk_ptr_ofs[dw_stk], cb_wrpr, bytes2allocate);

                subscribers[sub_count++] = reinterpret_cast<CbWrapperGeneric*>(cb_pool + dw_stk_ptr_ofs[dw_stk]); //save pointer to cb from cb_pool
                dw_stk_ptr_ofs[dw_stk] += CbWrprMaxSz; //increment respective stack pointer to next free chunk
                return true;
            }
        }
         

        return false; 
       

    }


};