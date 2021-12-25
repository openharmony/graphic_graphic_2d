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

#include "wmclient_native_test_35.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#include <display_type.h>
#include <option_parser.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
constexpr int32_t DISPLAY_MODE_SINGLE = 1;
constexpr int32_t DISPLAY_MODE_CLONE = 2;
constexpr int32_t DISPLAY_MODE_EXTEND = 3;
constexpr int32_t DISPLAY_MODE_EXPAND = 4;
class WMClientNativeTest35 : public INativeTest {
public:
    std::string GetDescription() const
    {
        constexpr const char *desc = "set display mode";
        return desc;
    }

    std::string GetDomain() const
    {
        constexpr const char *desc = "wmclient";
        return desc;
    }

    int32_t GetID() const
    {
        constexpr int32_t id = 35;
        return id;
    }

    uint32_t GetLastTime() const
    {
        constexpr uint32_t lastTime = 1000;
        return lastTime;
    }

    void inputExplain()
    {
        printf("please input set display mode: wmtest wmclient 35 [parameter] \n");
        printf("[parameter] : 1 , 2 , 3 , 4 \n");
        printf("1 : set display mode to SINGLE \n");
        printf("2 : set display mode to CLONE\n");
        printf("3 : set display mode to EXTEND\n");
        printf("4 : set display mode to EXPAND\n");
    }

    void Run(int32_t argc, const char **argv)
    {
        OptionParser parser;
        int32_t displayMode = 0;
        parser.AddArguments(displayMode);
        if (parser.Parse(argc, argv)) {
            std::cerr << parser.GetErrorString() << std::endl;
            inputExplain();
            ExitTest();
            return;
        }

        auto wm = WindowManager::GetInstance();
        auto wmRet = wm->Init();
        if (wmRet) {
            printf("init failed with %s\n", GSErrorStr(wmRet).c_str());
            ExitTest();
            return;
        }

        switch (displayMode) {
            case DISPLAY_MODE_SINGLE:
                wmRet = wm->SetDisplayMode(WM_DISPLAY_MODE_SINGLE);
                printf("1 : set display mode to SINGLE \n");
                break;
            case DISPLAY_MODE_CLONE:
                wmRet = wm->SetDisplayMode(WM_DISPLAY_MODE_CLONE);
                printf("2 : set display mode to CLONE\n");
                break;
            case DISPLAY_MODE_EXTEND:
                wmRet = wm->SetDisplayMode(WM_DISPLAY_MODE_EXTEND);
                printf("3 : set display mode to EXTEND\n");
                break;
            case DISPLAY_MODE_EXPAND:
                wmRet = wm->SetDisplayMode(WM_DISPLAY_MODE_EXPAND);
                printf("4 : set display mode to EXPAND\n");
                break;
            default:
                inputExplain();
                return;
                break;
        }

        if (wmRet != GSERROR_OK) {
            printf("set display mode failed with %s.\n", GSErrorStr(wmRet).c_str());
            ExitTest();
            return;
        }

        printf("set display mode succeed.\n");
    }
private:
    sptr<Window> window;
    sptr<NativeTestSync> windowSync;
} g_autoload;
} // namespace
