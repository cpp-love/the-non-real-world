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
    tnrw::no_nan_inf_f  value1{0.f};
    tnrw::no_nan_inf_f  value2{0.1f};
    tnrw::no_nan_inf_f  value3{-0.1f};
    tnrw::no_nan_inf_f  value4{-0.6f};
    tnrw::has_nan_inf_f value5{std::numeric_limits<float>::quiet_NaN()};
    tnrw::has_nan_inf_f value6{std::numeric_limits<float>::infinity()};
    TNRW_ASSERT_MSG(tnrw::no_nan_inf_f{value5} != tnrw::no_nan_inf_f{value6}); // 可以，但不要这样做
    TNRW_ASSERT_MSG(value1 != tnrw::no_nan_inf_f{value6});                     // 也可以，但不推荐这么做
    TNRW_ASSERT_MSG(value1 == value2);
    TNRW_ASSERT_MSG(value1 == value3);
    TNRW_ASSERT_MSG(value4 != value3);
    TNRW_ASSERT_MSG(value4 < value1);
    TNRW_ASSERT_MSG(tnrw::has_nan_inf_f(value1) != value5);
    TNRW_ASSERT_MSG(tnrw::has_nan_inf_f(value1) != value6);
    TNRW_ASSERT_MSG(tnrw::has_nan_inf_f(value1) == tnrw::has_nan_inf_f(value2));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return 0;
}