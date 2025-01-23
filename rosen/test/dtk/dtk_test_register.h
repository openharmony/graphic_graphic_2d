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
#ifndef DDGR_TEST_DTK_DTK_TEST_REGISTER_H
#define DDGR_TEST_DTK_DTK_TEST_REGISTER_H

#include "dtk_test_base.h"

namespace OHOS {
namespace Rosen {
enum TestLevel {
    L0 = 0, L1, L2, L3, L4, L5, L6, L7, L8, L9, PERF = 100
};

class DtkTestRegister {
private:
    DtkTestRegister() {};
    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> testCaseMap;
public:
    static DtkTestRegister* Instance();
    bool regist(const std::string testName, const std::function<std::shared_ptr<TestBase>()> testCase,
                TestLevel testLevel, int testCaseIndex);
    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> getTestCaseMap();
};

} // namespace Rosen
} // namespace OHOS
#endif // DDGR_TEST_DTK_DTK_TEST_REGISTER_H