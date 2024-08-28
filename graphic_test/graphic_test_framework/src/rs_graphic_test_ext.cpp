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

#include "rs_graphic_test_ext.h"

namespace OHOS {
namespace Rosen {
namespace {
std::string GetTestName(const char* testCaseName, const char* testName)
{
    return std::string(testCaseName) + "_" + testName;
}
}

TestDefManager& TestDefManager::Instance()
{
    static TestDefManager instance;
    return instance;
}

bool TestDefManager::Regist(const char* testCaseName, const char* testName,
    RSGraphicTestType type, RSGraphicTestMode mode, const char* filePath)
{
    const auto& name = GetTestName(testCaseName, testName);
    if (testInfos_.find(name) != testInfos_.end()) {
        LOGE("TestDefManager::Regist duplicate name %{public}s", name.c_str());
        return false;
    }

    std::string startPath = "graphic_test/";
    size_t startPos = std::string(filePath).find(startPath) + startPath.length();
    if (startPos == std::string::npos) {
        LOGE("TestDefManager::Regist fail filePath %{public}s", filePath);
        return false;
    }

    std::string savePath = std::string(filePath).substr(startPos);
    testInfos_.emplace(name, TestDefInfo {testCaseName, testName, type, mode, savePath});
    return true;
}

const TestDefInfo* TestDefManager::GetTestInfo(const char* testCaseName, const char* testName) const
{
    const auto& it = testInfos_.find(GetTestName(testCaseName, testName));
    if (it != testInfos_.end()) {
        return &it->second;
    }

    return nullptr;
}

std::vector<const TestDefInfo*> TestDefManager::GetTestInfosByType(RSGraphicTestType type) const
{
    std::vector<const TestDefInfo*> infos;
    for (const auto& it: testInfos_) {
        if (it.second.testType == type) {
            infos.push_back(&it.second);
        }
    }

    return infos;
}

static inline bool Cmp(const TestDefInfo* info1, const TestDefInfo* info2)
{
    std::string s1 = info1->filePath.substr(0, info1->filePath.rfind("/") + 1) + info1->testCaseName + info1->testName;
    std::string s2 = info2->filePath.substr(0, info2->filePath.rfind("/") + 1) + info2->testCaseName + info2->testName;
    return s1 < s2;
}

std::vector<const TestDefInfo*> TestDefManager::GetAllTestInfos() const
{
    std::vector<const TestDefInfo*> infos;
    for (const auto& it: testInfos_) {
        infos.push_back(&it.second);
    }

    sort(infos.begin(), infos.end(), Cmp);
    return infos;
}

} // namespace Rosen
} // namespace OHOS