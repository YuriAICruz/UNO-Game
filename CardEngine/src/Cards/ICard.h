#pragma once
#include <memory>
#include <string>

#include "../../framework.h"
#include "ActionTypes/actionType.h"

namespace cards
{
    class ENGINE_API ICard
    {
    protected:
        int number = -1;
        char color = 0;
        std::unique_ptr<actions::actionType> action;

    public:
        virtual ~ICard() = default;

        int Number() const
        {
            return number;
        }

        char Color() const
        {
            return color;
        }

        bool operator==(const ICard& other) const
        {
            return equal(other);
        }

        bool operator!=(const ICard& other) const
        {
            return !equal(other);
        }

        virtual bool equal(const ICard& other) const = 0;

        virtual bool sameColor(const ICard& other) const
        {
            return color == other.color;
        }

        virtual bool sameNumber(const ICard& other) const
        {
            return number == other.number;
        }

        virtual bool sameType(const ICard& other) const
        {
            return actionType()->isEqual(typeid(*other.action));
        }

        virtual bool hasAction() const = 0;

        virtual actions::actionType* actionType() const
        {
            return action.get();
        }

        virtual std::string typeName() const = 0;
    };
}
