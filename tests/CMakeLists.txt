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
    add_executable(test_overload ${CMAKE_CURRENT_SOURCE_DIR}/base/test_overload.cpp)
    add_executable(test_observable ${CMAKE_CURRENT_SOURCE_DIR}/base/test_observable.cpp)

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

    target_link_libraries(test_overload
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_observable
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    # 添加安装支持
    install(
        TARGETS test_assert_msg test_string_convert test_floating_point_compare
        test_SFML_formatter test_overload test_observable
        DESTINATION bin/tests)

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
    add_test(NAME RunTestBase5 COMMAND test_overload)
    add_test(NAME TestBase5StandardUse COMMAND test_overload)
    set_tests_properties(TestBase5StandardUse
        PROPERTIES PASS_REGULAR_EXPRESSION "value: 2\ncall from lambda\nvalue: 1\ncall from lambda3\\s*"
    )
    add_test(NAME RunTestBase6 COMMAND test_observable)
    add_test(NAME TestBase6StandardUse COMMAND test_observable)
    set_tests_properties(TestBase6StandardUse
        PROPERTIES PASS_REGULAR_EXPRESSION "value changed, now is: 11\nvalue changed, now is: 22\\s*"
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
    add_executable(test_scene_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/test_scene_system.cpp)
    add_executable(test_render_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/test_render_system.cpp)
    add_executable(test_movement_system ${CMAKE_CURRENT_SOURCE_DIR}/ecs/systems/test_movement_system.cpp)

    # 链接第三方库
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
    install(TARGETS test_scene_system test_render_system test_movement_system DESTINATION bin/tests)

    # 添加测试支持
    add_test(NAME RunTestEcs2 COMMAND test_scene_system)
    # todo : 修复此错误
    # add_test(NAME TestEcs2StandardUse COMMAND test_scene_system)
    # set_tests_properties(TestEcs2StandardUse
    # PROPERTIES PASS_REGULAR_EXPRESSION "创建场景1\n将孩子1添加到场景1中\n现在孩子1的父亲有：\n\\d+\n将孩子2添加到场景1中\n现在场景1的子实体有：\n\\d+ \\d+\n将孩子1从场景1中删除\n现在孩子1的父亲有：\n创建场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\nid: 2 entity: \\d+\n销毁场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\n删除所有场景\n现在registry拥有场景：\\s*")

    # todo : 添加test_render_system的测试支持
    # todo : 添加test_movement_system的测试支持
endif()

if(TNRW_BUILD_MATH_TESTS)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    # 构建程序

    # 设置链接文件
    add_executable(test_expressions ${CMAKE_CURRENT_SOURCE_DIR}/math/test_expressions.cpp)
    add_executable(test_complex ${CMAKE_CURRENT_SOURCE_DIR}/math/test_complex.cpp)

    # 链接第三方库
    target_link_libraries(test_expressions
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    target_link_libraries(test_complex
        PUBLIC
        tnrw_compiler_flags
        tnrw
    )

    # 添加安装支持
    install(TARGETS test_expressions DESTINATION bin/tests)

    # 添加测试支持
    add_test(NAME RunTestMath1 COMMAND test_complex)
    # todo : 添加 test_expression` 测试支持
endif()
