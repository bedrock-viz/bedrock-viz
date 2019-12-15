#pragma once

#include <string>

namespace mcpe_viz {
    class BaseObject {
    public:
        using ColorType = int32_t;
        using IdType = int32_t;
    protected:
        ColorType color_;
        bool is_color_set_;
    public:
        std::string name;
        IdType id;

        BaseObject(const IdType& id, std::string name);

        [[nodiscard]]
        ColorType color() const { return this->color_; }
        void color(const ColorType& rgb);

        [[nodiscard]]
        bool is_color_set() const { return this->is_color_set_; }
    };
}