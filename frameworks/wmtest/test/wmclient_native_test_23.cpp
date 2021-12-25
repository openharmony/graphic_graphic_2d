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

#include "wmclient_native_test_23.h"

#include <cstdio>
#include <fstream>
#include <sstream>

#include <display_type.h>
#include <gslogger.h>
#include <option_parser.h>
#include <raw_maker.h>
#include <securec.h>
#include <window_manager.h>
#include <zlib.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest23 : public INativeTest, public IScreenShotCallback {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "screen capture (--total-time=3000 (ms))";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 23;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    void Run(int32_t argc, const char **argv) override
    {
        OptionParser parser;
        parser.AddOption("t", "total-time", totalTime);
        if (parser.Parse(argc, argv)) {
            GSLOG2SE(ERROR) << parser.GetErrorString();
            ExitTest();
            return;
        }

        constexpr int64_t msecToNsec = 1000 * 1000;
        endTime = GetNowTime() + totalTime * msecToNsec;
        PostTask(std::bind(&WindowManager::ListenNextScreenShot, windowManager, 0, this));
    }

    void OnScreenShot(const struct WMImageInfo &info) override
    {
        GSLOG2SO(INFO) << "OnScreenShot";
        maker.SetFilename(captureFilepath);
        maker.SetWidth(info.width);
        maker.SetHeight(info.height);
        maker.SetHeaderType(RAW_HEADER_TYPE_COMPRESSED);
        auto ret = maker.WriteNextData(reinterpret_cast<const uint8_t *>(info.data));
        if (ret) {
            GSLOG2SE(ERROR) << "RawMaker WriteNextData failed with " << ret;
        }

        GSLOG2SO(INFO) << "OnScreenShot wrote";
        auto now = GetNowTime();
        GSLOG2SO(INFO) << now;
        if (now >= endTime) {
            GSLOG2SO(INFO) << "ScreenCapture Complete";
            ExitTest();
        } else {
            PostTask(std::bind(&WindowManager::ListenNextScreenShot, windowManager, 0, this));
        }
    }

private:
    int32_t totalTime = 3000;
    int64_t endTime = 0;
    static constexpr const char *captureFilepath = "/data/screen_capture.raw";
    RawMaker maker;
} g_autoload;
} // namespace
