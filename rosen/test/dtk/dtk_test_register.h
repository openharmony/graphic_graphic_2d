/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
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