/**
 * @file test_observable.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::observable` 模板类的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2026-04-02
 * 
 * @copyright cpp-love
 * 
 */

#include <print>
#undef NDEBUG
#include "base/assert_msg.hpp"
#include "base/observable.hpp"

int add(int &val, int off) {
    val += off;
    return val;
}
int cadd(int val, int off) {
    val += off;
    return val;
}

int main() {
    tnrw::observable<int> obs;
    obs.observers().emplace_back([](const int &val) { std::println("value changed, now is: {}", val); });
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    int res = obs.invoke(add, 11);
    TNRW_ASSERT_MSG(res == 11 && obs.get_value() == 11, "0 + 11 = 11");
    res = obs.invoke(cadd, 11);
    TNRW_ASSERT_MSG(res == 22 && obs.get_value() == 11, "11 + 11 = 22");
    const tnrw::observable<int> cobs(obs);
    res = cobs.invoke(cadd, 11);
    TNRW_ASSERT_MSG(res == 22 && cobs.get_value() == 11, "11 + 11 = 22");
    res = obs.invoke(add, 11);
    TNRW_ASSERT_MSG(res == 22 && obs.get_value() == 22 && cobs.get_value() == 11, "11 + 11 = 22");
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}