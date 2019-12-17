#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    class Named {
    public:
        std::string name;
        explicit Named(std::string name)
            : name{std::move(name)}
        {
        }
    };

    class Colored {
    public:
        using ColorType = int32_t;
    protected:
        ColorType color_;
        bool is_color_set_;
    public:
        Colored();

        [[nodiscard]]
        ColorType color() const { return this->color_; }
        void color(const ColorType& rgb);

        [[nodiscard]]
        bool is_color_set() const { return this->is_color_set_; }
    };

    class WithId {
    public:
        using IdType = int32_t;
        IdType id;
        explicit WithId(const IdType& id)
            : id{id}
        {
        }
    };

    class WithData {
    public:
        using DataType = unsigned short;
        DataType data;

        explicit WithData(const DataType& data)
            : data{data}
        {
        }
    };


    class BaseObject: public Named, public Colored, public WithId {
    public:
        BaseObject(const IdType& id, std::string name)
            : Named{std::move(name)}
            , Colored{}
            , WithId{id}
        {
        }
    };
}