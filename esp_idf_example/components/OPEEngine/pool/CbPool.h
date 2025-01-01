#pragma once

// OPEEngine
#include "OPEEngine_types.h"

namespace opee
{
    template <size_t CbPoolSz>
    class CbPool
    {
        private:
            opee_uint8_t cb_pool[CbPoolSz];

        public:
            opee_uint8_t* begin()
            {
                return cb_pool;
            }

            // overloaded [] operator for const access (read-only)
            opee_uint8_t operator[](opee_size_t i) const
            {
                if (i < CbPoolSz)
                    return cb_pool[i];
                else
                    return 0U;
            }

            // overloaded [] operator for non-const access (read/write)
            opee_uint8_t& operator[](opee_size_t i)
            {
                if (i < CbPoolSz)
                    return cb_pool[i];
                else
                    return cb_pool[CbPoolSz - 1]; // if invalid return max addr (in hope that it is unused)
            }
    };
} // namespace opee