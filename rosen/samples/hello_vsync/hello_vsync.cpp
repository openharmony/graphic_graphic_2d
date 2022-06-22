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

#include "external_vsync.h"

#include <iostream>
#include <thread>
#include <unistd.h>

static bool flag = false;
static void OnVSync(long long timestamp, void* data)
{
    flag = true;
    std::cout << "OnVSync: " << timestamp << std::endl;
}

void ThreadMain(OHNativeVSync* nativeVSync)
{
    OHNativeVSyncFrameCallback callback = OnVSync;
    OH_NativeVSync_OHNativeVSyncRequestFrame(nativeVSync, callback, nullptr);
    while (!flag) {
        std::cout << "wait for vsync!\n";
        sleep(1);
    }
    std::cout << "vsync come, end this thread\n";
}
int32_t main(int32_t argc, const char *argv[])
{
    char name[] = "hello_vsync";
    OHNativeVSync* nativeVSync = OH_NativeVSync_CreateOHNativeVSync(name, strlen(name));
    std::thread th(ThreadMain, nativeVSync);
    th.join();
    OH_NativeVSync_DestroyOHNativeVSync(nativeVSync);
    return 0;
}