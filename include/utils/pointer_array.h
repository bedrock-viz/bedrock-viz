#pragma once

#include <array>

namespace mcpe_viz
{
    template<typename T, size_t size>
    class PointerArray {
    private:
        std::array<T*, size> value_;
        PointerArray() = default;
    public:
        PointerArray(const PointerArray&) = delete;
        PointerArray(const PointerArray&&) = delete;
        PointerArray& operator=(const PointerArray&) = delete;
        PointerArray& operator=(const PointerArray&&) = delete;

        ~PointerArray()
        {
            for(auto& i: this->value_) {
                delete i;
            }
        }

        static std::array<T*, size>& value()
        {
            static PointerArray arr;
            return arr.value_;
        }
    };
}
