/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <iomanip>
#include <iostream>
#include <securec.h>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <vsync_helper.h>
#include <window_manager.h>

#include "inative_test.h"

using namespace OHOS;

namespace {
void Usage(const char *argv0)
{
    printf("Usage: %s type id\n", argv0);
    auto visitFunc = [](const INativeTest *test) {
        std::stringstream ss;
        ss << test->GetDomain() << ", id=";
        ss << test->GetID() << ": ";
        ss << test->GetDescription();
        if (test->GetLastTime() != INativeTest::LAST_TIME_FOREVER) {
            constexpr double msecToSec = 1000.0;
            ss << " (last " << std::setprecision(1) << test->GetLastTime() / msecToSec << "s)";
        }
        std::cout << ss.str() << std::endl;
    };
    INativeTest::VisitTests(visitFunc);
}
} // namespace

int32_t main(int32_t argc, const char **argv)
{
    constexpr int32_t argNumber = 2;
    if (argc <= argNumber) {
        Usage(argv[0]);
        return 0;
    }

    int32_t testcase = -1;
    constexpr int32_t domainIndex = 1;
    constexpr int32_t idIndex = 2;
    std::stringstream ss(argv[idIndex]);
    ss >> testcase;
    if (ss.fail() == true || testcase == -1) {
        Usage(argv[0]);
        return 1;
    }

    INativeTest *found = nullptr;
    auto visitFunc = [argv, testcase, &found](const INativeTest *test) {
        if (test->GetDomain() == argv[domainIndex] && test->GetID() == testcase) {
            found = const_cast<INativeTest *>(test);
        }
    };
    INativeTest::VisitTests(visitFunc);
    if (found == nullptr) {
        printf("not found test %d\n", testcase);
        return 1;
    }

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&INativeTest::Run, found, argc - 1, argv + 1));
    if (found->GetLastTime() != INativeTest::LAST_TIME_FOREVER) {
        handler->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner), found->GetLastTime());
    }

    printf("%d %s run! pid=%d\n", found->GetID(), found->GetDescription().c_str(), getpid());
    runner->Run();
    return 0;
}
