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

#include "native_test_0.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest0 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "screensaver";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 0;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1 << 30;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        csurface = Surface::CreateSurfaceAsConsumer();
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_ALARM_SCREEN, csurface);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window->SwitchTop();
        window->OnSizeChange([this](uint32_t w, uint32_t h) { csurface->SetDefaultWidthAndHeight(w, h); });
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::BlackDraw, surface);

        constexpr uint32_t nextTime = 1000;
        PostTask(std::bind(&NativeTest0::AfterRun, this), nextTime);
    }

    void AfterRun() const
    {
        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        auto maxWidth = displays[0].width;
        auto maxHeight = displays[0].height;
        window->Move(0, 0);
        window->Resize(maxWidth, maxHeight);
    }

private:
    sptr<Surface> csurface = nullptr;
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload;
} // namespace
