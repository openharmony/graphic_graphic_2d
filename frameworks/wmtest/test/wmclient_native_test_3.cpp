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
        constexpr const char *desc = "raw parser";
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

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        OptionParser oparser;
        std::string filename = "";
        oparser.AddArguments(filename);
        oparser.AddArguments(vsyncRate);
        if (oparser.Parse(argc, argv)) {
            ExitTest();
            return;
        }

        SetVsyncRate(vsyncRate);
        auto ret = resource.Parse(filename);
        if (ret) {
            ExitTest();
            return;
        }

        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        wms = wmsc->GetService();

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_FULL_SCREEN);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            ExitTest();
            return;
        }

        winWidth = window->GetWidth();
        winHeight = window->GetHeight();
        window->Resize(resource.GetWidth(), resource.GetHeight());
        window->ScaleTo(winWidth, winHeight);

        window->SwitchTop();
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(std::bind(&WMClientNativeTest3::Draw, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), surface);
    }

    void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        uint32_t rotationNumber = vsyncRate * 8;
        if (count % rotationNumber == (rotationNumber - 0x2)) {
            constexpr uint32_t degree = -90;
            wms->StartRotationAnimation(0, degree);
            rotationTime = GetNowTime();
        } else if (count % rotationNumber == (rotationNumber - 1)) {
            window->ScaleTo(winWidth, winHeight);
            constexpr int32_t rotateTypeMax = 4;
            int32_t next = (static_cast<int32_t>(rotateType) + 1) % rotateTypeMax;
            rotateType = static_cast<enum WindowRotateType>(next);
            window->Rotate(rotateType);
        }

        if (GetNowTime() - rotationTime > 500 * 1000 * 1000) {
            resource.GetNextData(vaddr);
        } else {
            resource.GetNowData(vaddr);
        }
    }

private:
    sptr<IWindowManagerService> wms = nullptr;
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
    RawParser resource;
    int32_t winWidth;
    int32_t winHeight;
    WindowRotateType rotateType;
    int32_t vsyncRate = 0;
    int64_t rotationTime = 0;
} g_autoload;
} // namespace
