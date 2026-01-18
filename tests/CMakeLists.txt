option(TNRW_BUILD_TESTS "Build The Non-\"real\" World tests" OFF)

# 添加测试
enable_testing()

# 默认不添加，如果启用TNRW_BUILD_TESTS则默认添加
if(TNRW_BUILD_TESTS)
    option(TNRW_BUILD_BASE_TESTS "Build the tests in the tests/base directory" ON)
    option(TNRW_BUILD_ECS_TESTS "Build the tests in the tests/ecs directory" ON)
    option(TNRW_BUILD_MATH_TESTS "Build the tests in the tests/math directory" ON)
else()
    option(TNRW_BUILD_BASE_TESTS "Build the tests in the tests/base directory" OFF)
    option(TNRW_BUILD_ECS_TESTS "Build the tests in the tests/ecs directory" OFF)
    option(TNRW_BUILD_MATH_TESTS "Build the tests in the tests/math directory" OFF)
endif()

if(TNRW_BUILD_BASE_TESTS)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    # 设置链接文件
    add_executable(test_assert_msg ${CMAKE_CURRENT_SOURCE_DIR}/base/test_assert_msg.cpp)
    add_executable(test_string_convert ${CMAKE_CURRENT_SOURCE_DIR}/base/test_string_convert.cpp)
    add_executable(test_floating_point_compare ${CMAKE_CURRENT_SOURCE_DIR}/base/test_floating_point_compare.cpp)
    add_executable(test_SFML_formatter ${CMAKE_CURRENT_SOURCE_DIR}/base/test_SFML_formatter.cpp)

    # 链接第三方库
    target_link_libraries(test_assert_msg
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_string_convert
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_floating_point_compare
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_SFML_formatter
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    # 添加安装支持
    install(TARGETS test_assert_msg test_string_convert test_floating_point_compare DESTINATION bin/tests)

    # 添加测试支持
    add_test(NAME RunTestBase1 COMMAND test_string_convert)
    add_test(NAME TestBase1StandardUse COMMAND test_string_convert)
    set_tests_properties(TestBase1StandardUse
        PROPERTIES PASS_REGULAR_EXPRESSION "这是一个类型为 const char8_t \\* 类型的字符串 这是一个类型为 std::u8string 类型的字符串\\s*"
    )
    add_test(NAME RunTestBase3 COMMAND test_floating_point_compare)
    add_test(NAME RunTestBase4 COMMAND test_SFML_formatter)
    add_test(NAME TestBase4StandardUse COMMAND test_SFML_formatter)
    set_tests_properties(TestBase4StandardUse
        PROPERTIES PASS_REGULAR_EXPRESSION "vec1 is \\(10.000, 10.000\\), vec2 is \\(11, 11\\), vec3 is \\(13, 10\\)\\s*"
    )

    # todo : 解决下面的测试失败的问题
    # add_test(NAME RunTestBase2 COMMAND test_assert_msg)
    # set_tests_properties(RunTestBase2 PROPERTIES WILL_FAIL TRUE)
endif()

if(TNRW_BUILD_ECS_TESTS)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    # 构建程序

    # 设置链接文件
    add_executable(test_game_state_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/Systems/testGameStateSystem.cpp)
    add_executable(test_scene_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/Systems/testSceneSystem.cpp)
    add_executable(test_render_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/Systems/testRenderSystem.cpp)
    add_executable(test_movement_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/Systems/testMovementSystem.cpp)

    # 链接第三方库
    target_link_libraries(test_game_state_system
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_scene_system
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_render_system
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_movement_system
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    # 添加安装支持
    install(TARGETS test_game_state_system test_scene_system test_render_system test_movement_system DESTINATION bin/tests)

    # 添加测试支持
    add_test(NAME RunTestEcs1 COMMAND test_game_state_system)
    add_test(NAME TestEcs1StandardUse COMMAND test_game_state_system)
    set_tests_properties(TestEcs1StandardUse
        PROPERTIES PASS_REGULAR_EXPRESSION "加入Mainpage状态（代码为1）\n当前顶状态代码为：1\n当前所有状态代码为：\n1 \n删除Mainpage状态\n加入Mainpage状态（代码为1）\n加入Game状态（代码为2）\n当前顶状态代码为：2\n当前所有状态代码为：\n1 2 \n删除Game状态\n删除Mainpage状态\\s*"
    )

    add_test(NAME RunTestEcs2 COMMAND test_scene_system)
    # todo : 修复此错误
    # add_test(NAME TestEcs2StandardUse COMMAND test_scene_system)
    # set_tests_properties(TestEcs2StandardUse
    # PROPERTIES PASS_REGULAR_EXPRESSION "创建场景1\n将孩子1添加到场景1中\n现在孩子1的父亲有：\n\\d+\n将孩子2添加到场景1中\n现在场景1的子实体有：\n\\d+ \\d+\n将孩子1从场景1中删除\n现在孩子1的父亲有：\n创建场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\nid: 2 entity: \\d+\n销毁场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\n删除所有场景\n现在registry拥有场景：\\s*")

    # todo : 添加test_render_system的测试支持
    # todo : 添加test_movement_system的测试支持
endif()

if(TNRW_BUILD_MATH_TESTS)
    option(TNRW_BUILD_OLD_MATH_TESTS "Build the old tests in the tests/math directory" OFF)

    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    # 构建程序

    # 设置链接文件
    add_executable(test_expressions ${CMAKE_CURRENT_SOURCE_DIR}/math/test_expressions.cpp)
    if(TNRW_BUILD_OLD_MATH_TESTS)
        add_executable(test_alge_expr ${CMAKE_CURRENT_SOURCE_DIR}/math/testAlgeExpr.cpp)
        add_executable(test_num_expr ${CMAKE_CURRENT_SOURCE_DIR}/math/testNumExpr.cpp)
    endif()

    # 链接第三方库
    target_link_libraries(test_expressions
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )
    if(TNRW_BUILD_OLD_MATH_TESTS)
        target_link_libraries(test_alge_expr
            PUBLIC
            tnrw_compiler_flags
            tnrw
        )

        target_link_libraries(test_num_expr
            PUBLIC
            tnrw_compiler_flags
            tnrw
        )
    endif()

    # 添加安装支持
    install(TARGETS test_expressions DESTINATION bin/tests)
    if(TNRW_BUILD_OLD_MATH_TESTS)
        install(TARGETS test_alge_expr test_num_expr DESTINATION bin/tests)
    endif()

    # todo : 添加测试支持

endif()
