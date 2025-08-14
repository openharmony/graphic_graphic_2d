/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_GRAPHIC_TEST_EXT_H
#define RS_GRAPHIC_TEST_EXT_H

#include "gtest/gtest.h"
#include "rs_graphic_log.h"

namespace OHOS {
namespace Rosen {
enum RSGraphicTestType {
    FRAMEWORK_TEST,
    ANIMATION_TEST,
    CONTENT_DISPLAY_TEST,
    SCREEN_MANAGER_TEST,
    HARDWARE_PRESENT_TEST,
    DRAWING_TEST,
    LTPO_TEST,
    TEXT_TEST,
    PIXMAP_TEST,
    SYMBOL_TEST,
    EFFECT_TEST,
    HYBRID_RENDER_TEST,
};

enum class RSGraphicTestMode : uint8_t {
    AUTOMATIC = 0x01,
    MANUAL = 0x02,
    DYNAMIC = 0x04,
    ALL = 0x01 | 0x02 | 0x04,
};

struct TestDefInfo {
    std::string testCaseName;
    std::string testName;
    RSGraphicTestType testType;
    RSGraphicTestMode testMode;
    std::string filePath;
    bool isMultiple;
    uint8_t testId;
};

class TestDefManager {
private:
    TestDefManager() {};
    std::map<std::string, TestDefInfo> testInfos_;
    std::map<std::string, int> testCaseInfos_;

public:
    static TestDefManager& Instance();
    bool Regist(const char* testCaseName, const char* testName, RSGraphicTestType type, RSGraphicTestMode mode,
        const char* filePath, const bool isMultiple);
    const TestDefInfo* GetTestInfo(const char* testCaseName, const char* testName) const;
    std::vector<const TestDefInfo*> GetTestInfosByType(RSGraphicTestType type) const;
    std::vector<const TestDefInfo*> GetAllTestInfos() const;
    int GetTestCaseCnt(std::string testCaseName) const;
};
} // namespace Rosen
} // namespace OHOS

#define GRAPHIC_TEST_PARAMS(test_case_name, test_name, test_type, test_mode, multiple_test) \
    bool GTEST_TEST_UNIQUE_ID_(test_case_name, test_name, __LINE__) = \
        OHOS::Rosen::TestDefManager::Instance().Regist( \
        #test_case_name, #test_name, test_type, test_mode, __FILE__, multiple_test); \
    TEST_F(test_case_name, test_name)

#define GRAPHIC_TEST_2(test_type, test_name) \
    GRAPHIC_TEST_PARAMS(RSGraphicTest, test_name, test_type, RSGraphicTestMode::AUTOMATIC, false)

#define GRAPHIC_TEST_3(test_case_name, test_type, test_name) \
    GRAPHIC_TEST_PARAMS(test_case_name, test_name, test_type, RSGraphicTestMode::AUTOMATIC, false)

#define GRAPHIC_TESTS_2(test_type, test_name) \
    GRAPHIC_TEST_PARAMS(RSGraphicTest, test_name, test_type, RSGraphicTestMode::AUTOMATIC, true)

#define GRAPHIC_TESTS_3(test_case_name, test_type, test_name) \
    GRAPHIC_TEST_PARAMS(test_case_name, test_name, test_type, RSGraphicTestMode::AUTOMATIC, true)

#define GRAPHIC_N_TEST_2(test_type, test_name) \
    GRAPHIC_TEST_PARAMS(RSGraphicTest, test_name, test_type, RSGraphicTestMode::MANUAL, false)

#define GRAPHIC_N_TEST_3(test_case_name, test_type, test_name) \
    GRAPHIC_TEST_PARAMS(test_case_name, test_name, test_type, RSGraphicTestMode::MANUAL, false)

#define GRAPHIC_D_TEST_2(test_type, test_name) \
    GRAPHIC_TEST_PARAMS(RSGraphicTest, test_name, test_type, RSGraphicTestMode::DYNAMIC, false)

#define GRAPHIC_D_TEST_3(test_case_name, test_type, test_name) \
    GRAPHIC_TEST_PARAMS(test_case_name, test_name, test_type, RSGraphicTestMode::DYNAMIC, false)

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define GRAPHIC_TEST(...) GET_MACRO(__VA_ARGS__, GRAPHIC_TEST_3, GRAPHIC_TEST_2)(__VA_ARGS__)
#define GRAPHIC_N_TEST(...) GET_MACRO(__VA_ARGS__, GRAPHIC_N_TEST_3, GRAPHIC_N_TEST_2)(__VA_ARGS__)
#define GRAPHIC_TESTS(...) GET_MACRO(__VA_ARGS__, GRAPHIC_TESTS_3, GRAPHIC_TESTS_2)(__VA_ARGS__)
#define GRAPHIC_D_TEST(...) GET_MACRO(__VA_ARGS__, GRAPHIC_D_TEST_3, GRAPHIC_D_TEST_2)(__VA_ARGS__)

#endif // RS_GRAPHIC_TEST_EXT_H
