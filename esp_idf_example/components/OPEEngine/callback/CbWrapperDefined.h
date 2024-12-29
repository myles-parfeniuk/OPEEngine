#pragma once
// std lib 
#include <cstring>
// OPEEngine 
#include "CbWrapperGeneric.h"

// template derived class that wraps a callback
template <typename TArg, typename TCb>
class CbWrapperDefined : public CbWrapperGeneric
{
    private:
        TCb cb;

    public:
        // constructor to set the callback
        CbWrapperDefined(TCb cb)
            : cb(cb)
        {
        }

        void invoke(uintptr_t arg2p_addr, uintptr_t data_addr) override
        {
            volatile TArg* arg2p_ptr = reinterpret_cast<volatile TArg*>(arg2p_addr);
            volatile TArg* data_ptr;

            cb(*arg2p_ptr);

            // last callback in salvo callbacks requested by data watch object if data_addr != 0
            if (data_addr != 0)
            {
                // overwrite the data with the new data after all callbacks are finished executing
                data_ptr = reinterpret_cast<volatile TArg*>(data_addr);
                *data_ptr = *arg2p_ptr;
            }
        }
};
