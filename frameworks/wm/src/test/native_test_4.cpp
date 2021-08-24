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

#include "native_test_4.h"

#include <cstdio>
#include <fstream>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest4 : public INativeTest, public IScreenShotCallback {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "shot screen";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 4;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1000;
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

        auto wm = WindowManager::GetInstance();
        wm->ListenNextScreenShot(0, this);
    }

    void OnScreenShot(const struct WMImageInfo &info) override
    {
        printf("width: %u, height: %u\n", info.width, info.height);
        printf("format: %u, size: %u, data: %p\n", info.format, info.size, info.data);

        printf("writing to /data/wmtest4.raw\n");
        std::ofstream rawDataFile("/data/wmtest4.raw", std::ofstream::binary);
        rawDataFile.write(static_cast<const char *>(info.data), info.size);
        rawDataFile.close();
        printf("write completed\n");

        ExitTest();
    }
} g_autoload;
} // namespace
