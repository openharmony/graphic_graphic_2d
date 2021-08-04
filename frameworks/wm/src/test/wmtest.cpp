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

#include <cstdio>
#include <securec.h>
#include <vector>

#include <vsync_helper.h>
#include <window_manager.h>

#include "inative_test.h"

using namespace OHOS;

namespace {
void Usage(const char *argv0)
{
    printf("Usage: %s test_id\n", argv0);
    auto visitFunc = [](INativeTest *test) {
        printf("test %d: %s (last %u millseconds)\n",
            test->GetID(), test->GetDescription().c_str(), test->GetLastTime());
    };
    INativeTest::VisitTests(visitFunc);
}
} // namespace

int32_t main(int32_t argc, const char **argv)
{
    if (argc <= 1) {
        Usage(argv[0]);
        return 0;
    }

    int32_t testcase = -1;
    int ret = sscanf_s(argv[1], "%d", &testcase);
    if (ret == 0) {
        Usage(argv[0]);
        return 1;
    }

    auto initRet = WindowManager::GetInstance()->Init();
    if (initRet) {
        printf("init failed with %s\n", WMErrorStr(initRet).c_str());
        return 1;
    }

    INativeTest *found = nullptr;
    auto visitFunc = [testcase, &found](INativeTest *test) {
        if (test->GetID() == testcase) {
            found = test;
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
    handler->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner), found->GetLastTime());

    printf("%d %s run!\n", found->GetID(), found->GetDescription().c_str());
    runner->Run();
    return 0;
}
