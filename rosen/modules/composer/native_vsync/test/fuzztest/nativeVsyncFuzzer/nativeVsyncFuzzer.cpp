/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "nativeVsyncFuzzer.h"

#include <cstdint>
#include <memory>
#include <securec.h>
#include <string>

#include <native_vsync.h>

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        constexpr int32_t RANGE_MAX_REFRESHRATE = 144;
        size_t g_size = 0;
        size_t g_pos = 0;
        const uint8_t* data_ = nullptr;

        void OnVSyncFrame(long long timestamp, void* data) {}
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        if (strlen <= 0) {
            return "";
        }
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    /*
    * Fuzz test for OH_NativeVSync_Create_ForAssociatedWindow with fuzzed inputs
    */
    void NativeVSyncCreateForAssociatedWindowFuzzTest()
    {
        uint64_t windowID = GetData<uint64_t>();
        string name = GetStringFromData(STR_LEN);
        unsigned int length = GetData<unsigned int>();

        OH_NativeVSync* nativeVSync = nullptr;

        nativeVSync = OH_NativeVSync_Create_ForAssociatedWindow(windowID, nullptr, length);
        OH_NativeVSync_Destroy(nativeVSync);
        nativeVSync = OH_NativeVSync_Create_ForAssociatedWindow(windowID, name.c_str(), 0);
        OH_NativeVSync_Destroy(nativeVSync);
        nativeVSync = OH_NativeVSync_Create_ForAssociatedWindow(windowID, name.c_str(), GetData<unsigned int>());
        OH_NativeVSync_Destroy(nativeVSync);
    }

    /*
    * Fuzz test for OH_NativeVSync_Destroy with nullptr
    */
    void NativeVSyncDestroyFuzzTest(OH_NativeVSync* nativeVSync)
    {
        OH_NativeVSync_Destroy(nullptr);
        OH_NativeVSync_Destroy(nativeVSync);
    }

    /*
    * Fuzz test for OH_NativeVSync_RequestFrame with various inputs
    */
    void NativeVSyncRequestFrameFuzzTest(OH_NativeVSync* nativeVSync)
    {
        void* data = reinterpret_cast<void*>(GetData<intptr_t>());

        OH_NativeVSync_RequestFrame(nullptr, OnVSyncFrame, data);
        OH_NativeVSync_RequestFrame(nativeVSync, nullptr, data);
        OH_NativeVSync_RequestFrame(nativeVSync, OnVSyncFrame, data);
    }

    /*
    * Fuzz test for OH_NativeVSync_RequestFrameWithMultiCallback with various inputs
    */
    void NativeVSyncRequestFrameWithMultiCallbackFuzzTest(OH_NativeVSync* nativeVSync)
    {
        void* data = reinterpret_cast<void*>(GetData<intptr_t>());

        OH_NativeVSync_RequestFrameWithMultiCallback(nullptr, OnVSyncFrame, data);
        OH_NativeVSync_RequestFrameWithMultiCallback(nativeVSync, nullptr, data);
        OH_NativeVSync_RequestFrameWithMultiCallback(nativeVSync, OnVSyncFrame, data);
    }

    /*
    * Fuzz test for OH_NativeVSync_GetPeriod with various inputs
    */
    void NativeVSyncGetPeriodFuzzTest(OH_NativeVSync* nativeVSync)
    {
        long long period = 0;

        OH_NativeVSync_GetPeriod(nullptr, &period);
        OH_NativeVSync_GetPeriod(nativeVSync, nullptr);
        OH_NativeVSync_GetPeriod(nativeVSync, &period);
    }

    /*
    * Fuzz test for OH_NativeVSync_DVSyncSwitch with various inputs
    */
    void NativeVSyncDVSyncSwitchFuzzTest(OH_NativeVSync* nativeVSync)
    {
        bool enable = GetData<bool>();

        OH_NativeVSync_DVSyncSwitch(nullptr, enable);
        OH_NativeVSync_DVSyncSwitch(nativeVSync, enable);
    }

    /*
    * Fuzz test for OH_NativeVSync_SetExpectedFrameRateRange with various inputs
    */
    void NativeVSyncSetExpectedFrameRateRangeFuzzTest(OH_NativeVSync* nativeVSync)
    {
        int32_t min = GetData<int32_t>();
        int32_t max = GetData<int32_t>();
        int32_t expected = GetData<int32_t>();

        OH_NativeVSync_ExpectedRateRange range = {min, max, expected};

        OH_NativeVSync_SetExpectedFrameRateRange(nullptr, &range);
        OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, nullptr);
        OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &range);
    }

    /*
    * Fuzz test for edge cases with boundary values
    */
    void NativeVSyncEdgeCaseFuzzTest()
    {
        const char* validName = "FuzzTest";
        OH_NativeVSync* nativeVSync = OH_NativeVSync_Create(validName, 8);
        if (nativeVSync != nullptr) {
            long long period = 0;
            OH_NativeVSync_GetPeriod(nativeVSync, &period);

            OH_NativeVSync_DVSyncSwitch(nativeVSync, true);
            OH_NativeVSync_DVSyncSwitch(nativeVSync, false);

            OH_NativeVSync_ExpectedRateRange validRange = {0, 60, 30};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &validRange);

            OH_NativeVSync_ExpectedRateRange cancelRange = {0, 0, 0};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &cancelRange);

            OH_NativeVSync_ExpectedRateRange maxRange = {0, RANGE_MAX_REFRESHRATE, RANGE_MAX_REFRESHRATE};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &maxRange);

            OH_NativeVSync_Destroy(nativeVSync);
        }
    }

    /*
    * Fuzz test for invalid frame rate ranges
    */
    void NativeVSyncInvalidRangeFuzzTest()
    {
        const char* validName = "FuzzTest";
        OH_NativeVSync* nativeVSync = OH_NativeVSync_Create(validName, 8);
        if (nativeVSync != nullptr) {
            OH_NativeVSync_ExpectedRateRange invalidRange1 = {-1, 60, 30};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &invalidRange1);

            OH_NativeVSync_ExpectedRateRange invalidRange2 = {0, RANGE_MAX_REFRESHRATE + 1, 60};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &invalidRange2);

            OH_NativeVSync_ExpectedRateRange invalidRange3 = {60, 30, 45};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &invalidRange3);

            OH_NativeVSync_ExpectedRateRange invalidRange4 = {30, 60, 15};
            OH_NativeVSync_SetExpectedFrameRateRange(nativeVSync, &invalidRange4);

            OH_NativeVSync_Destroy(nativeVSync);
        }
    }

    /*
    * Fuzz test for associated window creation
    */
    void NativeVSyncAssociatedWindowFuzzTest()
    {
        uint64_t windowID = GetData<uint64_t>();
        string name = GetStringFromData(STR_LEN);
        unsigned int length = GetData<unsigned int>();

        OH_NativeVSync* nativeVSync = OH_NativeVSync_Create_ForAssociatedWindow(windowID, name.c_str(), length);
        if (nativeVSync != nullptr) {
            long long period = 0;
            OH_NativeVSync_GetPeriod(nativeVSync, &period);
            OH_NativeVSync_Destroy(nativeVSync);
        }
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        data_ = data;
        g_size = size;
        g_pos = 0;

        string name = GetStringFromData(STR_LEN);
        unsigned int length = GetData<unsigned int>();

        OH_NativeVSync* nativeVSync = OH_NativeVSync_Create(name.c_str(), length);

        NativeVSyncCreateForAssociatedWindowFuzzTest();

        if (nativeVSync != nullptr) {
            NativeVSyncRequestFrameFuzzTest(nativeVSync);
            NativeVSyncRequestFrameWithMultiCallbackFuzzTest(nativeVSync);
            NativeVSyncGetPeriodFuzzTest(nativeVSync);
            NativeVSyncDVSyncSwitchFuzzTest(nativeVSync);
            NativeVSyncSetExpectedFrameRateRangeFuzzTest(nativeVSync);
            NativeVSyncDestroyFuzzTest(nativeVSync);
        }

        NativeVSyncEdgeCaseFuzzTest();
        NativeVSyncInvalidRangeFuzzTest();
        NativeVSyncAssociatedWindowFuzzTest();

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
