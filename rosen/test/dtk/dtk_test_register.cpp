/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "dtk_test_register.h"

namespace OHOS {
namespace Rosen {
DtkTestRegister* DtkTestRegister::Instance()
{
    static DtkTestRegister* instance = nullptr;
    if (instance == nullptr) {
        instance = new DtkTestRegister();
    }
    return instance;
}

bool DtkTestRegister::regist(const std::string testName,
                             const std::function<std::shared_ptr<TestBase>()> testCase,
                             TestLevel testLevel, int testCaseIndex)
{
    int iTestLevel = testLevel;
    std::string key = testName + "_L" + std::to_string(iTestLevel) + "_" + std::to_string(testCaseIndex);
    testCaseMap.emplace(key, testCase);
    return true;
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> DtkTestRegister::getTestCaseMap()
{
    return testCaseMap;
}
} // namespace Rosen
} // namespace OHOS