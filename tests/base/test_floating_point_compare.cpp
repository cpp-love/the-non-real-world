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
    tnrw::fast_floatf value1{0.f};
    tnrw::fast_floatf value2{0.1f};
    tnrw::fast_floatf value3{-0.1f};
    tnrw::fast_floatf value4{-0.6f};
    tnrw::safe_floatf value5{std::numeric_limits<float>::quiet_NaN()};
    tnrw::safe_floatf value6{std::numeric_limits<float>::infinity()};
    ASSERT_MSG(tnrw::fast_floatf{value5} != tnrw::fast_floatf{value6}); // 可以，但不要这样做
    ASSERT_MSG(value1 != tnrw::fast_floatf{value6});                    // 也可以，但不推荐这么做
    ASSERT_MSG(value1 == value2);
    ASSERT_MSG(value1 == value3);
    ASSERT_MSG(value4 != value3);
    ASSERT_MSG(value4 < value1);
    ASSERT_MSG(tnrw::safe_floatf(value1) != value5);
    ASSERT_MSG(tnrw::safe_floatf(value1) != value6);
    ASSERT_MSG(tnrw::safe_floatf(value1) == tnrw::safe_floatf(value2));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return 0;
}