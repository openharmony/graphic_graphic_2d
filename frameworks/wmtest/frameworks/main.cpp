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

#include <gslogger.h>
#include <vsync_helper.h>
#include <window_manager.h>

#include "inative_test.h"
#include "main_option.h"
#include "native_test_class.h"

using namespace OHOS;

namespace {
void Usage(const char *argv0)
{
    GSLOG0SE(INFO) << "Usage: " << argv0 << " [option] type id";
    GSLOG0SE(INFO) << "  Option:";
    GSLOG0SE(INFO) << "    -d, --display[=0]  Created Window's Display ID";
    GSLOG0SE(INFO) << "";
    GSLOG0SE(INFO) << "  Available Tests: type, id, description (time) [process]";
    auto visitFunc = [](const INativeTest *test) {
        std::stringstream ss;
        ss << "    ";
        ss << test->GetDomain() << " id=";
        ss << test->GetID() << ": ";
        ss << test->GetDescription();
        if (test->GetLastTime() != INativeTest::LAST_TIME_FOREVER) {
            constexpr double msecToSec = 1000.0;
            ss << " (last " << std::setprecision(1) << test->GetLastTime() / msecToSec << "s)";
        }

        if (test->GetProcessNumber() <= 1) {
            GSLOG0SE(INFO) << ss.str();
            return false;
        }

        if (test->GetProcessSequence() == -1) {
            ss << " [process number:" << test->GetProcessNumber() << "]";
            GSLOG0SE(INFO) << ss.str();
            return false;
        }

        ss << " [" << test->GetProcessSequence() << "/" << test->GetProcessNumber() << "]";
        auto sss = ss.str();
        auto size = sss.find_first_of(':') + 1;
        sss.erase(0, size);
        sss.insert(sss.begin(), size, ' ');
        GSLOG0SE(INFO) << sss;
        return false;
    };
    INativeTest::VisitTests(visitFunc);
}

bool GetTestFunc(INativeTest *test, MainOption &option)
{
    if (test->GetDomain() != option.domain) {
        return false;
    }

    if (test->GetID() != option.testcase) {
        return false;
    }

    if (test->GetProcessSequence() != option.processSequence) {
        return false;
    }

    return true;
};

int32_t LoadService(INativeTest *test)
{
    if (test->GetAutoLoadService() & AutoLoadService::WindowManager) {
        test->windowManager = WindowManager::GetInstance();
        auto wret = test->windowManager->Init();
        if (wret) {
            GSLOG7SO(ERROR) << "WindowManager Init failed with " << GSErrorStr(wret);
            return wret;
        }
    }

    if (test->GetAutoLoadService() & AutoLoadService::WindowManagerService) {
        auto wmsc = WindowManagerServiceClient::GetInstance();
        auto wret = wmsc->Init();
        if (wret) {
            GSLOG7SO(ERROR) << "WindowManagerServiceClient Init failed with " << GSErrorStr(wret);
            return wret;
        }
        test->windowManagerService = wmsc->GetService();
    }

    return 0;
}
} // namespace

int32_t main(int32_t argc, const char **argv)
{
    // parse option
    MainOption option;
    if (option.Parse(argc, argv)) {
        GSLOG0SE(ERROR) << option.GetErrorString() << std::endl;
        Usage(argv[0]);
        GSLOG7SE(ERROR) << "exiting, return 1";
        return 1;
    }

    // find test
    auto visitFunc = std::bind(GetTestFunc, std::placeholders::_1, std::ref(option));
    INativeTest *found = INativeTest::VisitTests(visitFunc);
    if (found == nullptr) {
        GSLOG7SE(ERROR) << "not found test " << option.testcase << ", exiting, return 1";
        return 1;
    }

    // default value assign
    NativeTestFactory::defaultDisplayID = option.displayID;
    found->processArgv = argv;
    if (LoadService(found)) {
        GSLOG7SE(ERROR) << "exiting, return 1";
        return 1;
    }

    if (option.systemAbilityID) {
        auto ret = found->IPCClientConnectServer(option.systemAbilityID);
        if (ret) {
            GSLOG7SE(ERROR) << "exiting, return 1";
            return 1;
        }
    }

    // run test
    auto runner = AppExecFwk::EventRunner::Create(false);
    found->SetEventHandler(std::move(std::make_shared<AppExecFwk::EventHandler>(runner)));
    found->PostTask(std::bind(&INativeTest::Run, found, option.GetSkippedArgc(), option.GetSkippedArgv()));
    if (found->GetLastTime() != INativeTest::LAST_TIME_FOREVER) {
        found->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner), found->GetLastTime());
    }

    GSLOG7SO(INFO) << found->GetID() << " "
        << found->GetDescription() << " run! pid=" << getpid();
    runner->Run();
    GSLOG7SO(INFO) << "exiting, return 0";
    return 0;
}
