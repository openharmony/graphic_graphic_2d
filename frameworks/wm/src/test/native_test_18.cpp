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

#include "native_test_18.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest18 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "listener touch down and SwitchTop";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 18;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1 << 30;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        csurface1 = Surface::CreateSurfaceAsConsumer();
        window1 = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, csurface1);
        if (window1 == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window1->SwitchTop();
        auto surface1 = window1->GetSurface();
        windowSync1 = NativeTestSync::CreateSync(NativeTestDraw::ColorDraw, surface1);

        csurface2 = Surface::CreateSurfaceAsConsumer();
        window2 = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, csurface2);
        if (window2 == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window2->SwitchTop();
        auto surface2 = window2->GetSurface();
        windowSync2 = NativeTestSync::CreateSync(NativeTestDraw::ColorDraw, surface2);

        auto down1 = [this](void *, uint32_t, uint32_t, int32_t, double, double) { window2->SwitchTop(); };
        auto down2 = [this](void *, uint32_t, uint32_t, int32_t, double, double) { window1->SwitchTop(); };
        window1->OnTouchDown(down1);
        window2->OnTouchDown(down2);

        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        auto maxWidth = displays[0].width;
        auto maxHeight = displays[0].height;

        constexpr double percent = 0.75;
        uint32_t width = percent * maxWidth;
        uint32_t height = percent * maxHeight;

        window1->Move(0, 0);
        csurface1->SetDefaultWidthAndHeight(width, height);
        window1->Resize(width, height);

        window2->Move(maxWidth - width, maxHeight - height);
        csurface2->SetDefaultWidthAndHeight(width, height);
        window2->Resize(width, height);
    }

private:
    sptr<Surface> csurface1 = nullptr;
    sptr<Window> window1 = nullptr;
    sptr<NativeTestSync> windowSync1 = nullptr;

    sptr<Surface> csurface2 = nullptr;
    sptr<Window> window2 = nullptr;
    sptr<NativeTestSync> windowSync2 = nullptr;
} g_autoload;
} // namespace
