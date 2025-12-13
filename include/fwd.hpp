/**
 * @file fwd.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief tnrw命名空间内容的前向声明头文件
 * @version 0.1.0-1
 * @date 2025-07-27
 * 
 * @copyright cpp-love
 * 
 */

#ifndef __TNRW_FWD_HPP__
#define __TNRW_FWD_HPP__

#include "base/fwd.hpp"
#include "ecs/fwd.hpp"
#include "maths/fwd.hpp"


/**
 * @brief "The Non-real World" 游戏的命名空间
 * @details 此游戏的所有API都在此命名空间内
 */
namespace tnrw {

    /// @brief tnrw命名空间内的自定义字面量命名空间
    inline namespace literals {}

} // namespace tnrw

#endif // __TNRW_FWD_HPP__