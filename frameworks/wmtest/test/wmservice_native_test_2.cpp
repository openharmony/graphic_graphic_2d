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

#include "wmservice_native_test_2.h"

#include <cstdio>
#include <iostream>

#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMServiceNativeTest2 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "get/set display_power";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmservice";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 2;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 5000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto wmsc = WindowManagerServiceClient::GetInstance();
        auto wret = wmsc->Init();
        if (wret != WM_OK) {
            std::cerr << "WindowManagerServiceClient::Init failed with " << WMErrorStr(wret) << std::endl;
            ExitTest();
            return;
        }

        wms = wmsc->GetService();
        wret = wms->GetDisplays(displays);
        if (wret != WM_OK) {
            std::cerr << "WindowManagerService::GetDisplays failed with " << WMErrorStr(wret) << std::endl;
            ExitTest();
            return;
        }

        for (const auto &display : displays) {
            auto ret = wms->GetDisplayPower(display.id)->Await();
            if (ret.wret != WM_OK) {
                std::cerr << "WindowManagerService::GetDisplayPower failed with "
                    << WMErrorStr(ret.wret) << std::endl;
            }
            std::cout << "WindowManagerService::GetDisplayPower " << display.id << ": " << ret.status;
        }

        for (const auto &display : displays) {
            auto wret = wms->SetDisplayPower(display.id, POWER_STATUS_OFF)->Await();
            if (wret != WM_OK) {
                std::cerr << "WindowManagerService::SetDisplayPower ON failed with "
                    << WMErrorStr(wret) << std::endl;
            }
            std::cout << "WindowManagerService::SetDisplayPower OFF Success" << std::endl;
        }

        constexpr uint32_t nextRunTime = 2500;
        PostTask(std::bind(&WMServiceNativeTest2::AfterRun, this), nextRunTime);
    }

    void AfterRun()
    {
        for (const auto &display : displays) {
            auto ret = wms->GetDisplayPower(display.id)->Await();
            if (ret.wret != WM_OK) {
                std::cerr << "WindowManagerService::GetDisplayPower failed with "
                    << WMErrorStr(ret.wret) << std::endl;
            }
            std::cout << "WindowManagerService::GetDisplayPower " << display.id << ": " << ret.status;
        }

        for (const auto &display : displays) {
            auto wret = wms->SetDisplayPower(display.id, POWER_STATUS_ON)->Await();
            if (wret != WM_OK) {
                std::cerr << "WindowManagerService::SetDisplayPower ON failed with "
                    << WMErrorStr(wret) << std::endl;
            }
            std::cout << "WindowManagerService::SetDisplayPower ON Success" << std::endl;
        }
    }

private:
    std::vector<struct WMDisplayInfo> displays;
    sptr<IWindowManagerService> wms = nullptr;
} g_autoload;
} // namespace
