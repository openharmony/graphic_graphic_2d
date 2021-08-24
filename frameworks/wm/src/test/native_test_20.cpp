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

#include "native_test_20.h"

#include <cstdio>

#include <display_type.h>
#include <window_manager.h>

#include "native_test_7.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest20 : public NativeTest7 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "subwindow(video) move, resize";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 20;
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

        NativeTest7::Run(argc, argv);
        constexpr uint32_t nextRunTime = 1500;
        PostTask(std::bind(&NativeTest20::AfterRun1, this), nextRunTime);
    }

    void AfterRun1()
    {
        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            config.width = w;
            config.height = h;
        };
        subwindow->OnSizeChange(onSizeChange);
        auto subpsurface = subwindow->GetSurface();
        config.width = subpsurface->GetDefaultWidth();
        config.height = subpsurface->GetDefaultHeight();
        config.strideAlignment = 0x8;
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = Surface::USAGE_CPU_READ | Surface::USAGE_CPU_WRITE | Surface::USAGE_MEM_DMA;
        subwindowSync = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, subpsurface, &config);

        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        maxWidth = displays[0].width;
        maxHeight = displays[0].height;

        constexpr double half = 0.5;
        leftHeight = half * half * maxHeight;
        rightHeight = half * maxHeight;

        leftX = 0;
        rightX = half * half * maxWidth;

        leftWidth = half * maxWidth;
        rightWidth = maxWidth;

        height = leftHeight;
        x = rightX;
        width = leftWidth;
        AfterRun2();
    }

    void AfterRun2()
    {
        constexpr double half = 0.5;
        auto wret = subwindow->Move(x, maxHeight * half);
        if (wret != WM_OK) {
            printf("Move: %d(%s)\n", wret, WMErrorStr(wret).c_str());
        }
        wret = subwindow->Resize(width, height);
        if (wret != WM_OK) {
            printf("Resize: %d(%s)\n", wret, WMErrorStr(wret).c_str());
        }

        height += diffHeight;
        width += diffWidth;
        x += diffX;
        if (height <= leftHeight || height >= rightHeight) {
            diffHeight = -diffHeight;
        }
        if (width <= leftWidth || width >= rightWidth) {
            diffWidth = -diffWidth;
        }
        if (x <= leftX || x >= rightX) {
            diffX = -diffX;
        }

        constexpr uint32_t delayTime = 100;
        PostTask(std::bind(&NativeTest20::AfterRun2, this), delayTime);
    }

private:
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;

    int32_t leftHeight = 0;
    int32_t rightHeight = 0;
    int32_t height = 0;
    int32_t diffHeight = 4;

    int32_t leftX = 0;
    int32_t rightX = 0;
    int32_t x = 0;
    int32_t diffX = -1;

    int32_t leftWidth = 0;
    int32_t rightWidth = 0;
    int32_t width = 0;
    int32_t diffWidth = 2;

    BufferRequestConfig config = {};
    sptr<NativeTestSync> subwindowSync = nullptr;
} g_autoload;
} // namespace
