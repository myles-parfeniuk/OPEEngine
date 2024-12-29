#pragma once
// std lib 
#include <cstdint>

// base class for callback wrapper
class CbWrapperGeneric
{
    public:
        virtual void invoke(uintptr_t arg2p_addr, uintptr_t data_addr) = 0;
        virtual ~CbWrapperGeneric() = default;
};