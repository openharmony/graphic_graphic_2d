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

#include "wmclient_native_test_32.h"

#include <cstdio>
#include <functional>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest32Ability : public INativeTest {
public:
    virtual void Draw(uint32_t *vaddr, uint32_t width, uint32_t height, uint32_t count) = 0;

    void Run(int32_t argc, const char **argv) override;

protected:
    sptr<Window> window = nullptr;

private:
    sptr<NativeTestSync> windowSync = nullptr;
    BufferRequestConfig rconfig = {};
};

class WMClientNativeTest32 : public WMClientNativeTest32Ability {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "PIP mode";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 32;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    void Run(int32_t argc, const char **argv) override
    {
        WMClientNativeTest32Ability::Run(argc, argv);
        auto func = std::bind(&WMClientNativeTest32::OnPIPModeChange, this, std::placeholders::_1);
        window->OnPIPModeChange(func);
    }

    void Draw(uint32_t *vaddr, uint32_t width, uint32_t height, uint32_t count) override
    {
        drawptr(vaddr, width, height, count);
    }

    void OnPIPModeChange(bool isOnPIPMode)
    {
        if (isOnPIPMode) {
            drawptr = NativeTestDraw::ColorDraw;
        } else {
            drawptr = NativeTestDraw::FlushDraw;
        }
    }

private:
    DrawFunc drawptr = NativeTestDraw::FlushDraw;
} g_autoload;

void WMClientNativeTest32Ability::Run(int32_t argc, const char **argv)
{
    window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
    if (window == nullptr) {
        ExitTest();
        return;
    }

    auto surf = window->GetSurface();
    auto draw = std::bind(&WMClientNativeTest32Ability::Draw, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    windowSync = NativeTestSync::CreateSync(draw, surf, &rconfig);
    rconfig.width = surf->GetDefaultWidth();
    rconfig.height = surf->GetDefaultHeight();
    rconfig.strideAlignment = 0x8;
    rconfig.format = PIXEL_FMT_RGBA_8888;
    rconfig.usage = surf->GetDefaultUsage();
    window->SwitchTop();

    auto onSizeChange = [this](uint32_t w, uint32_t h) {
        rconfig.width = w;
        rconfig.height = h;
    };
    window->OnSizeChange(onSizeChange);
    ListenWindowInputEvent(window->GetID());
}
} // namespace
