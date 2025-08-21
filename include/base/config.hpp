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

#include <cstdint>

#ifndef __BASE_CONFIG_HPP__
#define __BASE_CONFIG_HPP__

#ifndef TNRW_VERSION_TYPE
#define TNRW_VERSION_TYPE std::uint16_t
#endif // TNRW_VERSION_TYPE

#ifndef TNRW_GAME_LEVEL_TYPE
#define TNRW_GAME_LEVEL_TYPE std::uint16_t
#endif // TNRW_GAME_LEVEL_TYPE

#ifndef TNRW_VERSION_MAJOR_OFFSET
#define TNRW_VERSION_MAJOR_OFFSET (11)
#endif // TNRW_VERSION_MAJOR_OFFSET

#ifdef TNRW_VERSION_MAJOR
#undef TNRW_VERSION_MAJOR
#endif // TNRW_VERSION_MAJOR
#define TNRW_VERSION_MAJOR (0)

#ifndef TNRW_VERSION_MINOR_OFFSET
#define TNRW_VERSION_MINOR_OFFSET (6)
#endif // TNRW_VERSION_MINOR_OFFSET

#ifdef TNRW_VERSION_MINOR
#undef TNRW_VERSION_MINOR
#endif // TNRW_VERSION_MINOR
#define TNRW_VERSION_MINOR (1)

#ifndef TNRW_VERSION_PATCH_OFFSET
#define TNRW_VERSION_PATCH_OFFSET (0)
#endif // TNRW_VERSION_PATCH_OFFSET

#ifdef TNRW_VERSION_PATCH
#undef TNRW_VERSION_PATCH
#endif // TNRW_VERSION_PATCH
#define TNRW_VERSION_PATCH (0)

#ifdef TNRW_VERSION
#undef TNRW_VERSION
#endif // TNRW_VERSION
namespace tnrw {
    namespace Details {
        constexpr TNRW_VERSION_TYPE __tnrw_version__ =
            static_cast<TNRW_VERSION_TYPE>((TNRW_VERSION_MAJOR << TNRW_VERSION_MAJOR_OFFSET)
                                           + (TNRW_VERSION_MINOR << TNRW_VERSION_MINOR_OFFSET)
                                           + (TNRW_VERSION_PATCH << TNRW_VERSION_PATCH_OFFSET));
    }
} // namespace tnrw
#define TNRW_VERSION tnrw::Details::__tnrw_version__

#endif // __BASE_CONFIG_HPP__