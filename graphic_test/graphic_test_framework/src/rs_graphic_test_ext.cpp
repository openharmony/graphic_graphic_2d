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

using namespace std;
namespace OHOS {
namespace Rosen {
namespace {
string GetTestName(const char* testCaseName, const char* testName)
{
    return string(testCaseName) + "_" + testName;
}
}

TestDefManager& TestDefManager::Instance()
{
    static TestDefManager instance;
    return instance;
}

bool TestDefManager::Regist(const char* testCaseName, const char* testName,
    RSGraphicTestType type, RSGraphicTestMode mode, const char* filePath, const bool isMultiple)
{
    const auto& name = GetTestName(testCaseName, testName);
    if (testInfos_.find(name) != testInfos_.end()) {
        LOGE("TestDefManager::Regist duplicate name %{public}s", name.c_str());
        return false;
    }

    string startPath = "graphic_test/";
    size_t startPos = string(filePath).find(startPath) + startPath.length();
    if (startPos == string::npos) {
        LOGE("TestDefManager::Regist fail filePath %{public}s", filePath);
        return false;
    }
    string strName = string(testCaseName);
    string savePath = string(filePath).substr(startPos);
    testInfos_.emplace(name,
        TestDefInfo {testCaseName, testName, type, mode, savePath, isMultiple, GetTestCaseCnt(strName)});

    if (testCaseInfos_.find(strName) != testCaseInfos_.end()) {
        testCaseInfos_[strName]++;
    } else {
        testCaseInfos_.emplace(strName, 1);
    }
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

vector<const TestDefInfo*> TestDefManager::GetTestInfosByType(RSGraphicTestType type) const
{
    vector<const TestDefInfo*> infos;
    for (const auto& it: testInfos_) {
        if (it.second.testType == type) {
            infos.push_back(&it.second);
        }
    }

    return infos;
}

static inline bool Cmp(const TestDefInfo* info1, const TestDefInfo* info2)
{
    string s1 = info1->filePath.substr(0, info1->filePath.rfind("/") + 1) + info1->testCaseName + info1->testName;
    string s2 = info2->filePath.substr(0, info2->filePath.rfind("/") + 1) + info2->testCaseName + info2->testName;
    return s1 < s2;
}

vector<const TestDefInfo*> TestDefManager::GetAllTestInfos() const
{
    vector<const TestDefInfo*> infos;
    for (const auto& it: testInfos_) {
        infos.push_back(&it.second);
    }

    sort(infos.begin(), infos.end(), Cmp);
    return infos;
}

int TestDefManager::GetTestCaseCnt(string testCaseName) const
{
    int cnt = 0;
    for (const auto& it: testInfos_) {
        if ((it.second.testCaseName == testCaseName) && it.second.isMultiple) {
            cnt++;
        }
    }
    return cnt;
}

} // namespace Rosen
} // namespace OHOS