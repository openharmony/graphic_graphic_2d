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

#include "wmclient_native_test_33.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
constexpr int32_t VIRTUAL_DISPLAY_X = 100;
constexpr int32_t VIRTUAL_DISPLAY_Y = 10;
constexpr int32_t VIRTUAL_DISPLAY_W = 500;
constexpr int32_t VIRTUAL_DISPLAY_H = 500;

class WMClientNativeTest33 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "virtual display create";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 33;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 5000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto wm = WindowManager::GetInstance();
        if (wm == nullptr) {
            printf("WindowManager::GetInstance failed.\n");
            return;
        }

        auto wmRet = wm->Init();
        if (wmRet) {
            printf("init failed with %s\n", WMErrorStr(wmRet).c_str());
            ExitTest();
            return;
        }

        wm->GetDisplays(displays);
        for (display = displays.begin(); display != displays.end(); display++) {
        if (display->type == DISPLAY_TYPE_VIRTUAL)
            break;
        }

        if (display != displays.end()) {
        printf("virtual display already extsis.\n");
        } else {
            auto virtualDisplayOption = VirtualDisplayOption::Get();
            if (virtualDisplayOption == nullptr) {
                printf("VirtualDisplayOption::Get failed.\n");
                return;
            }
            virtualDisplayOption->SetX(VIRTUAL_DISPLAY_X);
            virtualDisplayOption->SetY(VIRTUAL_DISPLAY_Y);
            virtualDisplayOption->SetWidth(VIRTUAL_DISPLAY_W);
            virtualDisplayOption->SetHeight(VIRTUAL_DISPLAY_H);

            auto ret = wm->CreateVirtualDisplay(virtualDisplayOption);
            if (ret != WM_OK) {
                printf("create virtual display failed.\n");
                return;
            }
            printf("create virtual display succeed.\n");
        }

        wm->GetDisplays(displays);
        for (display = displays.begin(); display != displays.end(); display++) {
        if (display->type == DISPLAY_TYPE_VIRTUAL)
            break;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, nullptr, display->id);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surface);
    }

private:
    std::vector<struct WMDisplayInfo> displays;
    std::vector<struct WMDisplayInfo>::iterator display;
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload;
}
