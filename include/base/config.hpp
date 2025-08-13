/**
 * @file config.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief tnrw项目的基本配置信息
 * @version 0.1.0-1
 * @date 2025-08-10
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __BASE_CONFIG_HPP__
#define __BASE_CONFIG_HPP__

#ifdef TNRW_VERSION_MAJOR
#undef TNRW_VERSION_MAJOR
#endif // TNRW_VERSION_MAJOR
#define TNRW_VERSION_MAJOR 0

#ifdef TNRW_VERSION_MINOR
#undef TNRW_VERSION_MINOR
#endif // TNRW_VERSION_MINOR
#define TNRW_VERSION_MINOR 1

#ifdef TNRW_VERSION_PATCH
#undef TNRW_VERSION_PATCH
#endif // TNRW_VERSION_PATCH
#define TNRW_VERSION_PATCH 0

#endif // __BASE_CONFIG_HPP__