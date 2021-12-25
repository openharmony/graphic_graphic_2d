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

#include "wmclient_native_test_1.h"

#include <cstdio>

#include <display_type.h>
#include <window_manager.h>

#include "util.h"

using namespace OHOS;

namespace {
WMClientNativeTest1 g_autoload;
} // namespace

std::string WMClientNativeTest1::GetDescription() const
{
    constexpr const char *desc = "normal window";
    return desc;
}

std::string WMClientNativeTest1::GetDomain() const
{
    constexpr const char *desc = "wmclient";
    return desc;
}

int32_t WMClientNativeTest1::GetID() const
{
    constexpr int32_t id = 1;
    return id;
}

uint32_t WMClientNativeTest1::GetLastTime() const
{
    constexpr uint32_t lastTime = LAST_TIME_FOREVER;
    return lastTime;
}

void WMClientNativeTest1::Run(int32_t argc, const char **argv)
{
    auto initRet = WindowManager::GetInstance()->Init();
    if (initRet) {
        printf("init failed with %s\n", GSErrorStr(initRet).c_str());
        ExitTest();
        return;
    }

    window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
    if (window == nullptr) {
        printf("NativeTestFactory::CreateWindow return nullptr\n");
        return;
    }

    window->SwitchTop();
    auto surf = window->GetSurface();
    windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surf);
}
