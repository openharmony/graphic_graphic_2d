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

#include "wmclient_native_test_7.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
WMClientNativeTest7 g_autoload;
} // namespace

std::string WMClientNativeTest7::GetDescription() const
{
    constexpr const char *desc = "video subwindow";
    return desc;
}

int32_t WMClientNativeTest7::GetID() const
{
    constexpr int32_t id = 7;
    return id;
}

void WMClientNativeTest7::Run(int32_t argc, const char **argv)
{
    auto initRet = WindowManager::GetInstance()->Init();
    if (initRet) {
        printf("init failed with %s\n", WMErrorStr(initRet).c_str());
        ExitTest();
        return;
    }

    WMClientNativeTest1::Run(argc, argv);
    constexpr uint32_t nextRunTime = 1000;
    PostTask(std::bind(&WMClientNativeTest7::AfterRun, this), nextRunTime);
}

void WMClientNativeTest7::AfterRun()
{
    auto option = SubwindowOption::Get();
    if (option == nullptr) {
        printf("option is nullptr\n");
        ExitTest();
        return;
    }

    constexpr uint32_t x = 100;
    constexpr uint32_t y = 200;
    constexpr uint32_t w = 300;
    constexpr uint32_t h = 200;
    option->SetWindowType(SUBWINDOW_TYPE_VIDEO);
    option->SetX(x);
    option->SetY(y);
    option->SetWidth(w);
    option->SetHeight(h);

    auto wret = WindowManager::GetInstance()->CreateSubwindow(subwindow, window, option);
    if (subwindow == nullptr) {
        printf("create subwindow failed %s\n", WMErrorStr(wret).c_str());
        ExitTest();
        return;
    }

    printf("subwindow default width = %d\n", subwindow->GetSurface()->GetDefaultWidth());
    printf("subwindow default height = %d\n", subwindow->GetSurface()->GetDefaultHeight());
}
