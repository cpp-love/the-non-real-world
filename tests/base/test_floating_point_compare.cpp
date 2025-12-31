/**
 * @file test_floating_point_compare.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::FastFloat` 和 `tnrw::SafeFloat` 的测试用例和使用示例
 * @version 0.1.0-1
 * @date 2025-11-28
 * 
 * @copyright cpp-love
 * 
 */

#include "base/assert_msg.hpp"
#include "base/floating_point_compare.hpp"

int main() {

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    tnrw::FastFloatf value1{0.f};
    tnrw::FastFloatf value2{0.1f};
    tnrw::FastFloatf value3{-0.1f};
    tnrw::FastFloatf value4{-0.6f};
    tnrw::SafeFloatf value5{std::numeric_limits<float>::quiet_NaN()};
    tnrw::SafeFloatf value6{std::numeric_limits<float>::infinity()};
    assert_msg(tnrw::FastFloatf{value5} != tnrw::FastFloatf{value6}); // 可以，但不要这样做
    assert_msg(value1 != tnrw::FastFloatf{value6});                   // 也可以，但不推荐这么做
    assert_msg(value1 == value2);
    assert_msg(value1 == value3);
    assert_msg(value4 != value3);
    assert_msg(value4 < value1);
    assert_msg(tnrw::SafeFloatf(value1) != value5);
    assert_msg(tnrw::SafeFloatf(value1) != value6);
    assert_msg(tnrw::SafeFloatf(value1) == tnrw::SafeFloatf(value2));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return 0;
}