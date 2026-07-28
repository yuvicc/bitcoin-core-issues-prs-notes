#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

namespace yuvicc {

template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() noexcept = default;

    template <typename F>
    Function(F&& callable) {
        // remove the reference from F
        // create a type erased wrapper for the callable
        // store that wrapper
        using Callable = std::decay_t<F>;
        m_callable = std::make_unique<CallableModel<Callable>>(
            std::forward<F>(callable)
        );
    }

    R operator()(Args... args) const {
        // check if its an empty function
        if(!m_callable) {
            throw std::bad_function_call{};
        }
        // invoke type erased callable
        return m_callable->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(m_callable);
    }


private:
    struct CallableBase {
        virtual R invoke(Args&&... args) = 0;
        virtual ~CallableBase() = default;
    };

    template <typename F>
    struct CallableModel final : CallableBase {
        F callable;

        explicit CallableModel(F value)
            : callable(std::move(value)) {}
        
        R invoke(Args&&... args) override {
            return std::invoke(callable, std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<CallableBase> m_callable;

};

} // namespace yuvicc

int add(int a, int b) {
    return a + b;
}

int main() {
    yuvicc::Function<int(int,int)> f_add = [](int a, int b) {
        return a + b;
    };

    yuvicc::Function<int(int,int)> fun_add = add;

    std::cout << f_add(2, 3) << std::endl;
    std::cout << fun_add(4, 5);

    return 0;

}
