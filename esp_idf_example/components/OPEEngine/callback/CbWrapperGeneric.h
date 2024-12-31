#pragma once
// std lib
#include <cstdint>

namespace opee
{
    // base class for callback wrapper
    class CbWrapperGeneric
    {
        public:
            virtual void invoke(uintptr_t arg2p_addr, uintptr_t data_addr) = 0;
            virtual ~CbWrapperGeneric() = default;
    };
} // namespace opee