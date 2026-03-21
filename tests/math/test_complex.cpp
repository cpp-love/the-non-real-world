/**
 * @file test_complex.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `tnrw::math::complex` 模板类的测试用例或使用示例
 * @version 0.1.0-1
 * @date 2026-03-21
 * 
 * @copyright cpp-love
 * 
 */

#include "base/assert_msg.hpp"
#include "math/complex.hpp"
#include <utility>

struct integer {
    long long value{};

    integer  &operator+=(integer rhs) {
        value += rhs.value;
        return *this;
    }
    integer &operator-=(integer rhs) {
        value -= rhs.value;
        return *this;
    }
    integer &operator*=(integer rhs) {
        value *= rhs.value;
        return *this;
    }
    integer &operator/=(integer rhs) {
        value /= rhs.value;
        return *this;
    }
};

integer operator+(integer lhs, integer rhs) { return integer{lhs.value + rhs.value}; }

integer operator-(integer lhs, integer rhs) { return integer{lhs.value - rhs.value}; }

integer operator*(integer lhs, integer rhs) { return integer{lhs.value * rhs.value}; }

integer operator/(integer lhs, integer rhs) { return integer{lhs.value / rhs.value}; }

bool    operator==(integer lhs, integer rhs) { return lhs.value == rhs.value; }

int     main() {

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    tnrw::math::complex<int> complex(7, 2);
    complex += 114;
    TNRW_ASSERT_MSG(complex.real() == 121, "114 + 7 = 121");
    TNRW_ASSERT_MSG(complex.imag() == 2, "2 = 2");
    complex = tnrw::math::complex<double>(114.1);
    TNRW_ASSERT_MSG(complex.real() == 114, "int(114.1) = 114");
    TNRW_ASSERT_MSG(complex.imag() == 0, "int(0.) = 0");

    tnrw::math::complex<integer> complex2(integer{.value = 2}, integer{.value = 3});
    tnrw::math::complex<integer> complex3(integer{.value = 1}, integer{.value = 2});
    TNRW_ASSERT_MSG(complex2 * complex3
                            == tnrw::math::complex<integer>(integer{.value = -4}, integer{.value = 7}),
                        "(2 + 3i) * (1 + 2i) = -4 + 7i");

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}