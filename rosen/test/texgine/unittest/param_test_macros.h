/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_PARAM_TEST_MACROS_H
#define ROSEN_TEST_TEXGINE_UNITTEST_PARAM_TEST_MACROS_H

#include <vector>

#include "texgine_exception.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define PARAM_BINDING_ARGS0
#define PARAM_BINDING_ARGS1 arg1,
#define PARAM_BINDING_ARGS2 PARAM_BINDING_ARGS1 arg2,
#define PARAM_BINDING_ARGS3 PARAM_BINDING_ARGS2 arg3,
#define PARAM_BINDING_ARGS4 PARAM_BINDING_ARGS3 arg4,

#define PARAM_CALLING_ARGS0
#define PARAM_CALLING_ARGS1 arg1
#define PARAM_CALLING_ARGS2 PARAM_CALLING_ARGS1, arg2
#define PARAM_CALLING_ARGS3 PARAM_CALLING_ARGS2, arg3
#define PARAM_CALLING_ARGS4 PARAM_CALLING_ARGS3, arg4

#define PARAM_DEFINE_ARG0(...)
#define PARAM_DEFINE_ARG1(Arg1T, ...) \
    [[maybe_unused]] Arg1T arg1;
#define PARAM_DEFINE_ARG2(Arg1T, Arg2T, ...) \
    PARAM_DEFINE_ARG1(Arg1T) [[maybe_unused]] Arg2T arg2;
#define PARAM_DEFINE_ARG3(Arg1T, Arg2T, Arg3T, ...) \
    PARAM_DEFINE_ARG2(Arg1T, Arg2T) [[maybe_unused]] Arg3T arg3;
#define PARAM_DEFINE_ARG4(Arg1T, Arg2T, Arg3T, Arg4T, ...) \
    PARAM_DEFINE_ARG3(Arg1T, Arg2T, Arg3T) [[maybe_unused]] Arg4T arg4;

#define PARAM_REF_ARG0(...)
#define PARAM_REF_ARG1(T, ...) T&,
#define PARAM_REF_ARG2(T, ...) T&, PARAM_REF_ARG1(__VA_ARGS__)
#define PARAM_REF_ARG3(T, ...) T&, PARAM_REF_ARG2(__VA_ARGS__)
#define PARAM_REF_ARG4(T, ...) T&, PARAM_REF_ARG3(__VA_ARGS__)

#define PARAM_DATA_ARGS0(...) __VA_ARGS__
#define PARAM_DATA_ARGS1(arg1, ...) PARAM_DATA_ARGS0(__VA_ARGS__)
#define PARAM_DATA_ARGS2(arg2, ...) PARAM_DATA_ARGS1(__VA_ARGS__)
#define PARAM_DATA_ARGS3(arg3, ...) PARAM_DATA_ARGS2(__VA_ARGS__)
#define PARAM_DATA_ARGS4(arg4, ...) PARAM_DATA_ARGS3(__VA_ARGS__)

#define DEFINE_PARAM_TEST0(...) DEFINE_PARAM_TEST(RETVAL, 0, __VA_ARGS__)
#define DEFINE_PARAM_TEST1(...) DEFINE_PARAM_TEST(RETVAL, 1, __VA_ARGS__)
#define DEFINE_PARAM_TEST2(...) DEFINE_PARAM_TEST(RETVAL, 2, __VA_ARGS__)
#define DEFINE_PARAM_TEST3(...) DEFINE_PARAM_TEST(RETVAL, 3, __VA_ARGS__)
#define DEFINE_PARAM_TEST4(...) DEFINE_PARAM_TEST(RETVAL, 4, __VA_ARGS__)

#define DEFINE_VOID_PARAM_TEST0(...) DEFINE_PARAM_TEST(OBJECT, 0, __VA_ARGS__)
#define DEFINE_VOID_PARAM_TEST1(...) DEFINE_PARAM_TEST(OBJECT, 1, __VA_ARGS__)
#define DEFINE_VOID_PARAM_TEST2(...) DEFINE_PARAM_TEST(OBJECT, 2, __VA_ARGS__)
#define DEFINE_VOID_PARAM_TEST3(...) DEFINE_PARAM_TEST(OBJECT, 3, __VA_ARGS__)
#define DEFINE_VOID_PARAM_TEST4(...) DEFINE_PARAM_TEST(OBJECT, 4, __VA_ARGS__)

#define DEFINE_ALL_PARAM_TEST0(...) DEFINE_PARAM_TEST(ALL, 0, __VA_ARGS__)
#define DEFINE_ALL_PARAM_TEST1(...) DEFINE_PARAM_TEST(ALL, 1, __VA_ARGS__)
#define DEFINE_ALL_PARAM_TEST2(...) DEFINE_PARAM_TEST(ALL, 2, __VA_ARGS__)
#define DEFINE_ALL_PARAM_TEST3(...) DEFINE_PARAM_TEST(ALL, 3, __VA_ARGS__)
#define DEFINE_ALL_PARAM_TEST4(...) DEFINE_PARAM_TEST(ALL, 4, __VA_ARGS__)

#define DEFINE_CHECK_RETVAL_FN(N, CLASS, FUNC, ...) \
    std::function<void(decltype(((CLASS *)nullptr)->FUNC(PARAM_CALLING_ARGS##N)) &&)>
#define DEFINE_CHECK_OBJECT_FN(N, CLASS, FUNC, ...) std::function<void(PARAM_REF_ARG##N(__VA_ARGS__) CLASS &)>
#define DEFINE_CHECK_ALL_FN(N, CLASS, FUNC, ...) std::function<void( \
    decltype(((CLASS *)nullptr)->FUNC(PARAM_CALLING_ARGS##N)) &&, PARAM_REF_ARG##N(__VA_ARGS__) CLASS &)>

#define CHECK_RETVAL_CALL(N, FUNC) checkFunc(object.FUNC(PARAM_CALLING_ARGS##N))
#define CHECK_OBJECT_CALL(N, FUNC) object.FUNC(PARAM_CALLING_ARGS##N); checkFunc(PARAM_BINDING_ARGS##N object)
#define CHECK_ALL_CALL(N, FUNC) checkFunc(object.FUNC(PARAM_CALLING_ARGS##N), PARAM_BINDING_ARGS##N object)

#define ASSERT_EXCEPTION(ERRCODE, ...)                                       \
    try {                                                                    \
        __VA_ARGS__;                                                         \
        FAIL() << "Do not trigger Exception: " << static_cast<int>(ERRCODE); \
    } catch (const TexgineException &err) {                                  \
        ASSERT_EQ(ERRCODE, err.code);                                        \
    }                                                                        \

#define DEFINE_PARAM_TEST(CHECKING, N, CLASS, FUNC, ...)                               \
HWTEST_F(CLASS##Test, FUNC, testing::ext::TestSize.Level1)                             \
{                                                                                      \
    PARAM_DEFINE_ARG##N(__VA_ARGS__);                                                  \
    struct TestInfo {                                                                  \
        std::function<void(void)> init = nullptr;                                      \
        CLASS object;                                                                  \
        PARAM_DEFINE_ARG##N(__VA_ARGS__)                                               \
        ExceptionType exception = ExceptionType::SUCCESS;                              \
        DEFINE_CHECK_##CHECKING##_FN(N, CLASS, FUNC, __VA_ARGS__) checkFunc = nullptr; \
    };                                                                                 \
    std::vector<struct TestInfo> data = PARAM_DATA_ARGS##N(__VA_ARGS__);               \
    for (auto &[init, object, PARAM_BINDING_ARGS##N exception, checkFunc] : data) {    \
        if (init != nullptr) {                                                         \
            init();                                                                    \
        }                                                                              \
        if (exception != ExceptionType::SUCCESS) {                                     \
            ASSERT_EXCEPTION(exception, object.FUNC(PARAM_CALLING_ARGS##N));           \
        } else {                                                                       \
            ASSERT_NE(checkFunc, nullptr);                                             \
            CHECK_##CHECKING##_CALL(N, FUNC);                                          \
        }                                                                              \
    }                                                                                  \
}                                                                                      \

#define DEFINE_CHECK_RETVAL_FN2(N, CLASS, FUNC, ...) \
    std::function<bool(decltype(((CLASS *)nullptr)->FUNC(PARAM_CALLING_ARGS##N)) &&)>
#define DEFINE_CHECK_OBJECT_FN2(N, CLASS, FUNC, ...) \
    std::function<bool(PARAM_REF_ARG##N(__VA_ARGS__) CLASS &)>
#define DEFINE_CHECK_ALL_FN2(N, CLASS, FUNC, ...) \
    std::function<bool( \
    decltype(((CLASS *)nullptr)->FUNC(PARAM_CALLING_ARGS##N)) &&, PARAM_REF_ARG##N(__VA_ARGS__) CLASS &)>

#define CHECK_RETVAL_CALL2(N, FUNC, object) \
    EXPECT_TRUE(checkFunc((object).FUNC(PARAM_CALLING_ARGS##N)))
#define CHECK_OBJECT_CALL2(N, FUNC, object)                \
    (object).FUNC(PARAM_CALLING_ARGS##N);                  \
    EXPECT_TRUE(checkFunc(PARAM_BINDING_ARGS##N(object)))
#define CHECK_ALL_CALL2(N, FUNC, object) \
    EXPECT_TRUE(checkFunc((object).FUNC(PARAM_CALLING_ARGS##N), PARAM_BINDING_ARGS##N object))

#define DEFINE_TESTINFO(CHECKING, N, ...)                                                        \
    PARAM_DEFINE_ARG##N(__VA_ARGS__);                                                            \
    struct TestInfo {                                                                            \
        std::function<void(void)> init = nullptr;                                                \
        PARAM_DEFINE_ARG##N(__VA_ARGS__)                                                         \
        ExceptionType exception = ExceptionType::SUCCESS;                                        \
        DEFINE_CHECK_##CHECKING##_FN2(N, PARAMCLASS, PARAMFUNC, __VA_ARGS__) checkFunc = nullptr; \
    };                                                                                           \

#define RUN_TESTINFO(CHECKING, N, object, ...)                                            \
    {                                                                                     \
        auto &&[init, PARAM_BINDING_ARGS##N exception, checkFunc] = TestInfo __VA_ARGS__; \
        if (init != nullptr) {                                                            \
            init();                                                                       \
        }                                                                                 \
        if (exception != ExceptionType::SUCCESS) {                                        \
            ASSERT_EXCEPTION(exception, (object).PARAMFUNC(PARAM_CALLING_ARGS##N));       \
        } else {                                                                          \
            ASSERT_NE(checkFunc, nullptr);                                                \
            CHECK_##CHECKING##_CALL2(N, PARAMFUNC, object);                                \
        }                                                                                 \
    }                                                                                     \

#define DEFINE_TESTINFO0(...) DEFINE_TESTINFO(RETVAL, 0, __VA_ARGS__)
#define DEFINE_TESTINFO1(...) DEFINE_TESTINFO(RETVAL, 1, __VA_ARGS__)
#define DEFINE_TESTINFO2(...) DEFINE_TESTINFO(RETVAL, 2, __VA_ARGS__)
#define DEFINE_TESTINFO3(...) DEFINE_TESTINFO(RETVAL, 3, __VA_ARGS__)
#define DEFINE_TESTINFO4(...) DEFINE_TESTINFO(RETVAL, 4, __VA_ARGS__)

#define DEFINE_VOID_TESTINFO0(...) DEFINE_TESTINFO(OBJECT, 0, __VA_ARGS__)
#define DEFINE_VOID_TESTINFO1(...) DEFINE_TESTINFO(OBJECT, 1, __VA_ARGS__)
#define DEFINE_VOID_TESTINFO2(...) DEFINE_TESTINFO(OBJECT, 2, __VA_ARGS__)
#define DEFINE_VOID_TESTINFO3(...) DEFINE_TESTINFO(OBJECT, 3, __VA_ARGS__)
#define DEFINE_VOID_TESTINFO4(...) DEFINE_TESTINFO(OBJECT, 4, __VA_ARGS__)

#define DEFINE_ALL_TESTINFO0(...) DEFINE_TESTINFO(ALL, 0, __VA_ARGS__)
#define DEFINE_ALL_TESTINFO1(...) DEFINE_TESTINFO(ALL, 1, __VA_ARGS__)
#define DEFINE_ALL_TESTINFO2(...) DEFINE_TESTINFO(ALL, 2, __VA_ARGS__)
#define DEFINE_ALL_TESTINFO3(...) DEFINE_TESTINFO(ALL, 3, __VA_ARGS__)
#define DEFINE_ALL_TESTINFO4(...) DEFINE_TESTINFO(ALL, 4, __VA_ARGS__)

#define RUN_TESTINFO0(...) RUN_TESTINFO(RETVAL, 0, __VA_ARGS__)
#define RUN_TESTINFO1(...) RUN_TESTINFO(RETVAL, 1, __VA_ARGS__)
#define RUN_TESTINFO2(...) RUN_TESTINFO(RETVAL, 2, __VA_ARGS__)
#define RUN_TESTINFO3(...) RUN_TESTINFO(RETVAL, 3, __VA_ARGS__)
#define RUN_TESTINFO4(...) RUN_TESTINFO(RETVAL, 4, __VA_ARGS__)

#define RUN_VOID_TESTINFO0(...) RUN_TESTINFO(OBJECT, 0, __VA_ARGS__)
#define RUN_VOID_TESTINFO1(...) RUN_TESTINFO(OBJECT, 1, __VA_ARGS__)
#define RUN_VOID_TESTINFO2(...) RUN_TESTINFO(OBJECT, 2, __VA_ARGS__)
#define RUN_VOID_TESTINFO3(...) RUN_TESTINFO(OBJECT, 3, __VA_ARGS__)
#define RUN_VOID_TESTINFO4(...) RUN_TESTINFO(OBJECT, 4, __VA_ARGS__)

#define RUN_ALL_TESTINFO0(...) RUN_TESTINFO(ALL, 0, __VA_ARGS__)
#define RUN_ALL_TESTINFO1(...) RUN_TESTINFO(ALL, 1, __VA_ARGS__)
#define RUN_ALL_TESTINFO2(...) RUN_TESTINFO(ALL, 2, __VA_ARGS__)
#define RUN_ALL_TESTINFO3(...) RUN_TESTINFO(ALL, 3, __VA_ARGS__)
#define RUN_ALL_TESTINFO4(...) RUN_TESTINFO(ALL, 4, __VA_ARGS__)

template<class T>
auto GetResultChecker(T result)
{
    return [result](T &&actual) { ASSERT_EQ(result, actual); };
}

template<class T>
auto GetVecSizeChecker(std::size_t size)
{
    return [size](std::vector<T> &&ret) {ASSERT_EQ(size, ret.size());};
}

template<class T>
auto CreateChecker(T result)
{
    return [result](T &&actual) { return actual == result; };
}

template<class T>
auto CreateVecSizeChecker(std::size_t size)
{
    return [size](std::vector<T> &&ret) { return size == ret.size(); };
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_TEST_TEXGINE_UNITTEST_PARAM_TEST_MACROS_H
