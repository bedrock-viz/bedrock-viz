#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    class NamedObject {
    public:
        std::string name;
        NamedObject(std::string name)
            : name{std::move(name)}
        {
        }
    };

    class BaseObjectNoId: public NamedObject {
    public:
        using ColorType = int32_t;
    protected:
        ColorType color_;
        bool is_color_set_;
    public:

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