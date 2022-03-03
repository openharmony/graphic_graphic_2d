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

#include "wmclient_native_test_27.h"

#include <cstdio>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest27 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "free window can move by finger";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 27;
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

        staticWindow = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (staticWindow == nullptr) {
            ExitTest();
            return;
        }

        staticWindow->SwitchTop();
        auto surf = staticWindow->GetSurface();
        staticWindowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surf);

        freeWindow = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (freeWindow == nullptr) {
            ExitTest();
            return;
        }

        freeWindow->SwitchTop();
        freeWindow->SetWindowMode(WINDOW_MODE_FREE);
        surf = freeWindow->GetSurface();
        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            config.width = w;
            config.height = h;
        };
        config.width = surf->GetDefaultWidth();
        config.height = surf->GetDefaultHeight();
        config.strideAlignment = 0x8,
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = surf->GetDefaultUsage();
        freeWindowSync = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, surf, &config);

        freeWindow->OnSizeChange(onSizeChange);
        constexpr int32_t width = 300;
        constexpr int32_t height = 300;
        freeWindow->Resize(width, height);
        ListenWindowTouchEvent(freeWindow->GetID());
    }

    void OnMoveReturn(const GSError &err)
    {
        if (err != GSERROR_OK) {
            printf("Move failed %d, means %s\n", err, GSErrorStr(err).c_str());
        }
    }

private:
    sptr<Window> staticWindow = nullptr;
    sptr<NativeTestSync> staticWindowSync = nullptr;
    sptr<Window> freeWindow = nullptr;
    sptr<NativeTestSync> freeWindowSync = nullptr;
    BufferRequestConfig config = {};
} g_autoload;
} // namespace
