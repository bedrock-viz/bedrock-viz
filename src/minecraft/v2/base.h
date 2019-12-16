#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    class BaseObjectNoId {
    public:
        using ColorType = int32_t;
    protected:
        ColorType color_;
        bool is_color_set_;
    public:
        std::string name;

        BaseObjectNoId(std::string name);

        [[nodiscard]]
        ColorType color() const { return this->color_; }
        void color(const ColorType& rgb);

        [[nodiscard]]
        bool is_color_set() const { return this->is_color_set_; }
    };

    class BaseObject: public BaseObjectNoId {
    public:
        using IdType = int32_t;
        IdType id;

        BaseObject(const IdType& id, std::string name)
            : BaseObjectNoId{std::move(name)}
            , id{id}
        {
        }
    };
}