/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rswindowanimation_fuzzer.h"

#include <securec.h>

#include "rs_window_animation_finished_callback_proxy.h"
#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_stub.h"
#include "rs_iwindow_animation_finished_callback.h"
#include "rs_window_animation_target.h"
#include "rs_iwindow_animation_controller.h"
#include "rs_window_animation_proxy.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
    }
    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    void FinishedCallbackFuzzTest()
    {
        // test
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        finishedCallback->OnAnimationFinished();
    }

    void FinishedCallbackProxyFuzzTest()
    {
        // test
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        sptr<RSWindowAnimationFinishedCallbackProxy> finishedCallbackProxy =
            new RSWindowAnimationFinishedCallbackProxy(finishedCallback);
        finishedCallbackProxy->OnAnimationFinished();
    }

    void RSWindowAnimationProxyFuzzTest()
    {
        // get data
        StartingAppType type = GetData<StartingAppType>();
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        // test
        sptr<RSWindowAnimationProxy> rSWindowAnimationProxy = new RSWindowAnimationProxy(nullptr);
        rSWindowAnimationProxy->OnStartApp(type, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnAppTransition(nullptr, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnAppBackTransition(nullptr, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnMinimizeWindow(nullptr, finishedCallback);
        std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget;
        rSWindowAnimationProxy->OnMinimizeAllWindow(minimizingWindowsTarget, finishedCallback);
        rSWindowAnimationProxy->OnCloseWindow(nullptr, finishedCallback);
        rSWindowAnimationProxy->OnScreenUnlock(finishedCallback);
        std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets;
        rSWindowAnimationProxy->OnWindowAnimationTargetsUpdate(nullptr, floatingWindowTargets);
        rSWindowAnimationProxy->OnWallpaperUpdate(nullptr);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        FinishedCallbackFuzzTest();
        FinishedCallbackProxyFuzzTest();
        RSWindowAnimationProxyFuzzTest();

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

