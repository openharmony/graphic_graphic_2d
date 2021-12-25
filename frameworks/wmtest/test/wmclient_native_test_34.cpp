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

#include "wmclient_native_test_34.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest34 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "virtual display destory";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 34;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto wm = WindowManager::GetInstance();
        if (wm == nullptr) {
            printf("WindowManager::GetInstance failed.\n");
            return;
        }

        auto wmRet = wm->Init();
        if (wmRet) {
            printf("init failed with %s\n", GSErrorStr(wmRet).c_str());
            ExitTest();
            return;
        }

        wm->GetDisplays(displays);
        for (display = displays.begin(); display != displays.end(); display++) {
        if (display->type == DISPLAY_TYPE_VIRTUAL)
            break;
        }

        if (display == displays.end()) {
        printf("virtual display non-existent.\n");
        } else {
            auto ret = wm->DestroyVirtualDisplay(display->id);
            if (ret != GSERROR_OK) {
                printf("destroy virtual display failed.\n");
                return;
            }
            printf("virtual display destroy succeed.\n");
        }
    }

private:
    std::vector<struct WMDisplayInfo> displays;
    std::vector<struct WMDisplayInfo>::iterator display;
    sptr<Window> window;
    sptr<NativeTestSync> windowSync;
} g_autoload;
}
