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

#include "wmclient_native_test_29.h"

#include <cstdio>
#include <functional>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest29 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "rotate";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 29;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", GSErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto surf = window->GetSurface();
        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            config.width = w;
            config.height = h;
        };
        config.width = surf->GetDefaultWidth();
        config.height = surf->GetDefaultHeight();
        config.strideAlignment = 0x8,
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = surf->GetDefaultUsage();
        window->OnSizeChange(onSizeChange);
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surf, &config);

        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        maxWidth = displays[0].width;
        maxHeight = displays[0].height;

        window->Move(0, 0);
        window->Resize(maxWidth, maxHeight);

        constexpr uint32_t nextRunTime = 3000;
        PostTask(std::bind(&WMClientNativeTest29::AfterRun, this), nextRunTime);
    }

    void AfterRun()
    {
        constexpr int32_t rotateTypeMax = 4;
        int32_t next = (static_cast<int32_t>(rotateType) + 1) % rotateTypeMax;
        rotateType = static_cast<enum WindowRotateType>(next);
        window->Rotate(rotateType);
        if (rotateType % 0x2 == 0) {
            window->Resize(maxWidth, maxHeight);
        } else {
            window->Resize(maxHeight, maxWidth);
        }

        constexpr uint32_t nextRunTime = 3000;
        PostTask(std::bind(&WMClientNativeTest29::AfterRun, this), nextRunTime);
    }

private:
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
    BufferRequestConfig config = {};
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    WindowRotateType rotateType = WINDOW_ROTATE_TYPE_NORMAL;
} g_autoload;
} // namespace
