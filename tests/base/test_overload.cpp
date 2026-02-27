/**
 * @file test_overload.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::makeOverloaded` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2026-02-16
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#include "base/assert_msg.hpp"
#include "base/overload.hpp"
#include <functional>
#include <memory>
#include <print>

struct func_obj {
    double operator()(double val) const noexcept { return val * value; }
    double value;
};

struct object {
    [[nodiscard]] double get_value(bool arg) const noexcept { return arg ? value : 0.; }
    double               value;
};

[[nodiscard]] constexpr int func() { return 0; }

int                         main() {
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    std::function<void(int)> func2([](int value) { std::println("value: {}", value); });
    auto                     lambda = [&func2](auto value) {
        std::println("call from lambda");
        func2(value);
    };
    [[maybe_unused]] auto lambda2 = [] {};
    func_obj              fobj(2.);

    // 不好的定义：func 与 lambda2 的参数重复，调用 overloaded_result() 将会导致错误
    // auto overloaded_result = tnrw::make_overloaded(&Object::getValue, func, func2, lambda, lambda2, fobj);
    // 好的定义
    auto overloaded_result = tnrw::make_overloaded(&object::get_value, func, func2, lambda, fobj);
    // 缺点：会抛弃 [[nodiscard]] 属性标记
    // 下面的代码没有警告
    overloaded_result(); // 匹配 func
    overloaded_result(2); // 匹配 func2，不匹配 lambda，因为模板比普通函数的优先级低
    overloaded_result(true);            // 匹配 lambda 的 bool 实例化
    double res = overloaded_result(2.); // 匹配 fobj
    object obj(res);
    res = overloaded_result(obj, true);  // 匹配 &Object::getValue
    res = overloaded_result(&obj, true); // 还是匹配 &Object::getValue
    res = overloaded_result(std::reference_wrapper{obj}, true); // 还是匹配 &Object::getValue
    res = overloaded_result(std::make_unique<object>(obj), true); // 还是匹配 &Object::getValue
    auto lambda3 = [](object obj, bool arg) {
        std::println("call from lambda3");
        return obj.get_value(!arg);
    };
    auto overloaded_result2 = tnrw::make_overloaded(&object::get_value, lambda3);
    res = overloaded_result2(
        obj,
        false); // 匹配 lambda3!!! 因为 &Object::getValue 经包装后成为了模板函数对象，调用优先级比普通可调用对象低
    TNRW_ASSERT_MSG(res == 4, "断言不会触发");
    res = overloaded_result2(&obj, true); // 这次才匹配 &Object::getValue
    TNRW_ASSERT_MSG(res == 4, "断言不会触发");

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}