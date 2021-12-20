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

#include "wmservice_native_test_7.h"

#include <gslogger.h>
#include <option_parser.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;
using namespace std::chrono_literals;

namespace {
class WMServiceNativeTest7 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "launch page";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmservice";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 7;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager | AutoLoadService::WindowManagerService;
    }

    void Run(int32_t argc, const char **argv) override
    {
        OptionParser oparser;
        std::string filename = "";
        oparser.AddArguments(filename);
        if (oparser.Parse(argc, argv)) {
            GSLOG2SE(ERROR) << "Need a filename as raw";
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, surface);

        auto ret = windowManagerService->CreateLaunchPage(filename);
        GSLOG2SO(INFO) << "CreateLaunchPage: " << ret;

        constexpr uint32_t delayTime = 2000;
        PostTask(std::bind(&WMServiceNativeTest7::Run2, this), delayTime);
    }

    void Run2()
    {
        auto ret = windowManagerService->CancelLaunchPage();
        GSLOG2SO(INFO) << "CancelLaunchPage: " << ret;
    }

private:
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload;
} // namespace
