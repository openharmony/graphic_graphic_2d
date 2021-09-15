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

#include "native_test_17.h"

#include <cstdio>
#include <securec.h>

#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest17 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "attr listener";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 17;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1 << 30;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surface);
        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        maxWidth = displays[0].width;
        maxHeight = displays[0].height;

        auto option = SubwindowOption::Get();
        if (option == nullptr) {
            printf("option is nullptr\n");
            ExitTest();
            return;
        }

        constexpr uint32_t x = 100;
        constexpr uint32_t y = 200;
        constexpr uint32_t w = 300;
        constexpr uint32_t h = 200;
        option->SetWindowType(SUBWINDOW_TYPE_NORMAL);
        option->SetX(x);
        option->SetY(y);
        option->SetWidth(w);
        option->SetHeight(h);
        auto wret = WindowManager::GetInstance()->CreateSubwindow(subwindow, window, option);
        if (subwindow == nullptr) {
            printf("create subwindow failed %s\n", WMErrorStr(wret).c_str());
            ExitTest();
            return;
        }

        surface = subwindow->GetSurface();
        subwindowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surface);

        AfterRunWindow();
        AfterRunSubwindow();
    }

    void AfterRunWindow() const
    {
        {
            constexpr int32_t step = 37;
            constexpr int32_t minWidthHeight = 100;
            static int32_t width = maxWidth;
            static int32_t height = maxHeight;
            width -= step;
            height -= step;
            if (width < minWidthHeight) {
                width += maxWidth;
            }
            if (height < minWidthHeight) {
                height += maxHeight;
            }
            window->Move(width, height);
        }

        {
            constexpr int32_t step = 10;
            constexpr int32_t minWidthHeight = 40;
            constexpr int32_t maxWidthHeight = 100;
            static int32_t width = minWidthHeight;
            static int32_t height = minWidthHeight;
            width -= step;
            height -= step;
            if (width < minWidthHeight) {
                width += maxWidthHeight;
            }
            if (height < minWidthHeight) {
                height += maxWidthHeight;
            }
            window->Resize(width, height);
        }

        auto positionChangeListener = [](int32_t x, int32_t y) {
            printf("window position: %d, %d\n", x, y);
        };
        auto sizeChangeListener = [](uint32_t w, uint32_t h) {
            printf("window size: %u, %u\n", w, h);
        };
        window->OnPositionChange(positionChangeListener);
        window->OnSizeChange(sizeChangeListener);

        constexpr uint32_t delayTime = 2000;
        PostTask(std::bind(&NativeTest17::AfterRunWindow, this), delayTime);
    }

    void AfterRunSubwindow() const
    {
        constexpr uint32_t half = 2;
        {
            constexpr int32_t step = 37;
            constexpr int32_t minWidthHeight = 100;
            static int32_t width = maxWidth;
            static int32_t height = maxHeight;
            width -= step;
            height -= step;
            if (width < minWidthHeight) {
                width += maxWidth;
            }
            if (height < minWidthHeight) {
                height += maxHeight;
            }
            subwindow->Move(width / half, height / half);
        }

        {
            constexpr int32_t step = 10;
            constexpr int32_t minWidthHeight = 10;
            constexpr int32_t maxWidthHeight = 100;
            static int32_t width = minWidthHeight;
            static int32_t height = minWidthHeight;
            width -= step;
            height -= step;
            if (width < minWidthHeight) {
                width += maxWidthHeight;
            }
            if (height < minWidthHeight) {
                height += maxWidthHeight;
            }
            subwindow->Resize(width / half, height / half);
        }

        auto positionChangeListener = [](int32_t x, int32_t y) {
            printf("subwindow position: %d, %d\n", x, y);
        };
        auto sizeChangeListener = [](uint32_t w, uint32_t h) {
            printf("subwindow size: %u, %u\n", w, h);
        };
        subwindow->OnPositionChange(positionChangeListener);
        subwindow->OnSizeChange(sizeChangeListener);

        constexpr uint32_t delayTime = 2000;
        PostTask(std::bind(&NativeTest17::AfterRunSubwindow, this), delayTime);
    }

protected:
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
    sptr<Subwindow> subwindow = nullptr;
    sptr<NativeTestSync> subwindowSync = nullptr;
} g_autoload;
} // namespace
