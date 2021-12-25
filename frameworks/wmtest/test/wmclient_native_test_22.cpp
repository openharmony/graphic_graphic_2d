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

#include "wmclient_native_test_22.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest22 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "parallel horizon";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 22;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    bool OnTouch(const TouchEvent &event) override
    {
        int32_t x = event.GetPointerPosition(event.GetIndex()).GetX();
        int32_t y = event.GetPointerPosition(event.GetIndex()).GetY();
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            if (event.GetWindowID() == window2->GetID()) {
                OnWin2TouchDown(x, y);
            } else if (event.GetWindowID() == window3->GetID()) {
                OnWin3TouchDown(x, y);
            }
        }

        if (event.GetAction() == TouchEnum::POINT_MOVE) {
            if (event.GetWindowID() == window1->GetID()) {
                OnWin1TouchMotion(x, y);
            }
        }

        if (event.GetAction() == TouchEnum::PRIMARY_POINT_UP) {
            if (event.GetWindowID() == window1->GetID()) {
                OnWin1TouchUp();
            }
        }

        return false;
    }

    void OnWin1TouchMotion(double x, double y)
    {
        percent = x / maxWidth;

        csurf2->SetDefaultWidthAndHeight(maxWidth * percent - lineWidth, maxHeight);
        window2->Resize(maxWidth * percent - lineWidth, maxHeight);

        csurf3->SetDefaultWidthAndHeight(maxWidth * (1 - percent) - lineWidth, maxHeight);
        window3->Move(maxWidth * percent + lineWidth * 0x2, 0);
        window3->Resize(maxWidth * (1 - percent) - lineWidth, maxHeight);
    }

    void OnWin1TouchUp()
    {
        constexpr double boundary = 0.2;
        constexpr double factor = 0.25;
        int32_t level = static_cast<int32_t>(percent / boundary);
        percent = level * factor;
        if (level == 0) {
            window2->Hide();
        }

        if (level == 0x4) {
            window3->Hide();
        }

        if (percent == 0 || percent == 1) {
            csurf2->SetDefaultWidthAndHeight(maxWidth * percent, maxHeight);
            window2->Resize(maxWidth * percent, maxHeight);

            csurf3->SetDefaultWidthAndHeight(maxWidth * (1 - percent), maxHeight);
            window3->Move(maxWidth * percent, 0);
            window3->Resize(maxWidth * (1 - percent), maxHeight);

            constexpr double half = 0.5;
            percent = half;
        } else {
            csurf2->SetDefaultWidthAndHeight(maxWidth * percent - lineWidth, maxHeight);
            window2->Resize(maxWidth * percent - lineWidth, maxHeight);

            csurf3->SetDefaultWidthAndHeight(maxWidth * (1 - percent) - lineWidth, maxHeight);
            window3->Move(maxWidth * percent + lineWidth * 0x2, 0);
            window3->Resize(maxWidth * (1 - percent) - lineWidth, maxHeight);
        }
    }

    void OnWin2TouchDown(double x, double y)
    {
        csurf2->SetDefaultWidthAndHeight(maxWidth * percent - lineWidth, maxHeight);
        window2->Show();
        window2->Resize(maxWidth * percent - lineWidth, maxHeight);

        csurf3->SetDefaultWidthAndHeight(maxWidth * (1 - percent) - lineWidth, maxHeight);
        window3->Show();
        window3->Move(maxWidth * percent + lineWidth * 0x2, 0);
        window3->Resize(maxWidth * (1 - percent) - lineWidth, maxHeight);
    }

    void OnWin3TouchDown(double x, double y)
    {
        csurf3->SetDefaultWidthAndHeight(0, 0);
        window3->Resize(0, 0);
        window3->Hide();

        csurf2->SetDefaultWidthAndHeight(maxWidth, maxHeight);
        window2->Show();
        window2->Resize(maxWidth, maxHeight);
        window2->SwitchTop();
    }

    void Run(int32_t argc, const char **argv) override
    {
        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        maxWidth = displays[0].width;
        maxHeight = displays[0].height;

        // window1
        csurf1 = Surface::CreateSurfaceAsConsumer();
        window1 = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, csurf1);
        if (window1 == nullptr) {
            ExitTest();
            return;
        }

        window1->SwitchTop();
        auto surf1 = window1->GetSurface();
        windowSync1 = NativeTestSync::CreateSync(NativeTestDraw::BlackDraw, surf1);

        window1->Move(0, 0);
        csurf1->SetDefaultWidthAndHeight(maxWidth, maxWidth);
        window1->Resize(maxWidth, maxHeight);

        // window2
        csurf2 = Surface::CreateSurfaceAsConsumer();
        window2 = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, csurf2);
        if (window2 == nullptr) {
            ExitTest();
            return;
        }

        window2->SwitchTop();
        auto surf2 = window2->GetSurface();
        windowSync2 = NativeTestSync::CreateSync(NativeTestDraw::ColorDraw, surf2);

        window2->Move(0, 0);
        csurf2->SetDefaultWidthAndHeight(maxWidth, maxWidth);
        window2->Resize(maxWidth, maxHeight);

        // window3
        csurf3 = Surface::CreateSurfaceAsConsumer();
        window3 = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL, csurf3);
        if (window3 == nullptr) {
            ExitTest();
            return;
        }

        window3->Hide();
        auto surf3 = window3->GetSurface();
        windowSync3 = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, surf3);
        ListenWindowTouchEvent(window1->GetID());
        ListenWindowTouchEvent(window2->GetID());
        ListenWindowTouchEvent(window3->GetID());
    }

private:
    sptr<Surface> csurf1 = nullptr;
    sptr<Window> window1 = nullptr;
    sptr<NativeTestSync> windowSync1 = nullptr;

    sptr<Surface> csurf2 = nullptr;
    sptr<Window> window2 = nullptr;
    sptr<NativeTestSync> windowSync2 = nullptr;

    sptr<Surface> csurf3 = nullptr;
    sptr<Window> window3 = nullptr;
    sptr<NativeTestSync> windowSync3 = nullptr;

    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    uint32_t lineWidth = 15;
    double percent = 0.5;
} g_autoload;
} // namespace
