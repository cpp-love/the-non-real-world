/**
 * @file config.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief tnrw项目的基本配置信息
 * @version 0.1.0-2
 * @date 2025-08-16
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_BASE_CONFIG_HPP
#define TNRW_BASE_CONFIG_HPP

#include <cstdint>

namespace tnrw {

    using version_type = std::uint16_t;               ///< tnrw版本类型
    using level_identifier_type = std::uint16_t;      ///< 关卡标识符类型
    constexpr version_type version_major_offset = 11; ///< tnrw主版本偏移量
    constexpr version_type version_major = 0;         ///< tnrw主版本号
    constexpr version_type version_minor_offset = 6;  ///< tnrw次版本偏移量
    constexpr version_type version_minor = 1;         ///< tnrw次版本号
    constexpr version_type version_patch_offset = 0;  ///< tnrw修订版本偏移量
    constexpr version_type version_patch = 0;         ///< tnrw修订版本号
    constexpr version_type version = (version_major << version_major_offset)
                                     + (version_minor << version_minor_offset)
                                     + (version_patch << version_patch_offset); ///< tnrw版本号
} // namespace tnrw

#endif // TNRW_BASE_CONFIG_HPP
