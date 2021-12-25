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

#include "wmclient_native_test_3.h"

#include <cstdio>
#include <iostream>
#include <securec.h>
#include <sstream>

#include <display_type.h>
#include <gslogger.h>
#include <option_parser.h>
#include <raw_parser.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest3 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "raw parser (filename [--rate=1 --rotate])";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 3;
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
        oparser.AddOption("r", "rate", vsyncRate);
        oparser.AddOption("o", "rotate", needRotate);
        if (oparser.Parse(argc, argv)) {
            GSLOG2SE(ERROR) << oparser.GetErrorString();
            ExitTest();
            return;
        }

        SetVsyncRate(vsyncRate);
        auto ret = resource.Parse(filename);
        if (ret) {
            GSLOG2SE(ERROR) << "resource Parse " << filename << " failed with " << ret;
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_FULL_SCREEN);
        if (window == nullptr) {
            ExitTest();
            return;
        }

        winWidth = window->GetWidth();
        winHeight = window->GetHeight();
        window->Resize(resource.GetWidth(), resource.GetHeight());
        window->ScaleTo(winWidth, winHeight);

        window->SwitchTop();
        auto surf = window->GetSurface();
        auto func = std::bind(&WMClientNativeTest3::Draw, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowSync = NativeTestSync::CreateSync(func, surf, &config);
        config.width = surf->GetDefaultWidth();
        config.height = surf->GetDefaultHeight();
        config.strideAlignment = 0x8;
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = surf->GetDefaultUsage();
        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            config.width = w;
            config.height = h;
        };
        window->OnSizeChange(onSizeChange);
    }

    void Draw(uint32_t *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        if (!needRotate) {
            resource.GetNextData(vaddr);
            return;
        }

        uint32_t rotationNumber = vsyncRate * 8;
        if (count % rotationNumber == (rotationNumber - 0x2)) {
            constexpr uint32_t degree = -90;
            windowManagerService->StartRotationAnimation(0, degree);
            rotationTime = GetNowTime();
        } else if (count % rotationNumber == (rotationNumber - 1)) {
            window->ScaleTo(winWidth, winHeight);
            constexpr int32_t rotateTypeMax = 4;
            int32_t next = (static_cast<int32_t>(rotateType) + 1) % rotateTypeMax;
            rotateType = static_cast<enum WindowRotateType>(next);
            window->Rotate(rotateType);
        }

        constexpr int64_t lastingTime = 500 * 1000 * 1000; // 500ms
        if (GetNowTime() - rotationTime > lastingTime) {
            resource.GetNextData(vaddr);
        } else {
            resource.GetNowData(vaddr);
        }
    }

private:
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
    RawParser resource;
    int32_t winWidth = 0;
    int32_t winHeight = 0;
    WindowRotateType rotateType;
    int32_t vsyncRate = 1;
    bool needRotate = false;
    int64_t rotationTime = 0;
    BufferRequestConfig config = {};
} g_autoload;
} // namespace
