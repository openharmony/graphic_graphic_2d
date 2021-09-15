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

#include "wmclient_native_test_5.h"

#include <cstdio>
#include <fstream>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "native_test_class.h"
#include "util.h"
#include "wmclient_native_test_1.h"

using namespace OHOS;

namespace {
class WMClientNativeTest5 : public WMClientNativeTest1, public IWindowShotCallback {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "shot window";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 5;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 2000;
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

        WMClientNativeTest1::Run(argc, argv);

        constexpr uint32_t delayTime = 1000;
        PostTask(std::bind(&WMClientNativeTest5::AfterRun, this), delayTime);
    }

    void AfterRun()
    {
        auto wm = WindowManager::GetInstance();
        wm->ListenNextWindowShot(window, this);
    }

    void OnWindowShot(const struct WMImageInfo &info) override
    {
        printf("width: %u, height: %u\n", info.width, info.height);
        printf("format: %u, size: %u, data: %p\n", info.format, info.size, info.data);

        printf("writing to /data/wmtest5.raw\n");
        std::ofstream rawDataFile("/data/wmtest5.raw", std::ofstream::binary);
        rawDataFile.write(static_cast<const char *>(info.data), info.size);
        rawDataFile.close();
        printf("write completed\n");

        ExitTest();
    }
} g_autoload;
} // namespace
