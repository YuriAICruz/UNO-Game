#pragma once
#include <map>
#include <memory>
#include <typeinfo>

class bootstrapper
{
    struct baseBinder
    {
    };

    template <typename Base, typename... Args>
    struct binder : public baseBinder
    {
        template <typename T>
        binder* to()
        {
            create = [](Args... args)
            {
                std::shared_ptr<Base> base = std::make_shared<T>(args...);
                auto c = T(args...);
                return base;
            };
            return this;
        }

        binder* asSingleton()
        {
            isSingleton = true;
            return this;
        }

        std::shared_ptr<Base> getValue(Args... args)
        {
            if (isSingleton)
            {
                if (singleton == nullptr)
                {
                    singleton = create(args...);
                }
                return singleton;
            }
            return create(args...);
        }

    private:
        std::shared_ptr<Base> singleton;
        std::shared_ptr<Base> (*create)(Args... args);
        bool isSingleton = false;
    };

public:
    template <typename T, typename... Args>
    binder<T, Args...>* bind()
    {
        auto hash = typeid(T).hash_code();
        binders.emplace(hash, std::make_unique<binder<T, Args...>>());
        return static_cast<binder<T, Args...>*>(binders.at(hash).get());
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args... args)
    {
        binder<T, Args...>* b = static_cast<binder<T, Args...>*>(binders.at(typeid(T).hash_code()).get());
        return b->getValue(args...);
    }

private:
    std::map<size_t, std::unique_ptr<baseBinder>> binders;
};
