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

#include "hdiscreen_fuzzer.h"

#include <securec.h>

#include "hdi_screen.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
        std::unique_ptr<HdiScreen> g_hdiScreen = nullptr;
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
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    void HdiScreenFuzzTest2()
    {
        // test
        if (g_hdiScreen == nullptr) {
            return;
        }
        GraphicGamutMap gamutMap = GetData<GraphicGamutMap>();
        g_hdiScreen->SetScreenGamutMap(gamutMap);
        g_hdiScreen->GetScreenGamutMap(gamutMap);
        GraphicDisplayCapability dcap = {};
        g_hdiScreen->GetScreenCapability(dcap);

        GraphicHDRCapability info = {};
        g_hdiScreen->GetHDRCapabilityInfos(info);
        g_hdiScreen = nullptr;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        uint32_t modeId = GetData<uint32_t>();
        GraphicDispPowerStatus status = GetData<GraphicDispPowerStatus>();
        uint32_t level = GetData<uint32_t>();
        bool enabled = GetData<bool>();
        GraphicColorGamut gamut = GetData<GraphicColorGamut>();
        float matrixElement = GetData<float>();
        std::vector<float> matrix = { matrixElement };
        uint32_t sequence = GetData<uint32_t>();
        uint64_t ns = GetData<uint64_t>();
        void* dt = static_cast<void*>(GetStringFromData(STR_LEN).data());

        // test
        if (g_hdiScreen == nullptr) {
            uint32_t screenId = GetData<uint32_t>();
            g_hdiScreen = HdiScreen::CreateHdiScreen(screenId);
            if (g_hdiScreen == nullptr) {
                return false;
            }
            HdiDevice *device = HdiDevice::GetInstance();
            bool ret = g_hdiScreen->SetHdiDevice(device);
            if (!ret) {
                return false;
            }
        }
        
        g_hdiScreen->SetScreenMode(modeId);
        uint32_t width = 1080;
        uint32_t height = 1920;
        g_hdiScreen->SetScreenOverlayResolution(width, height);
        g_hdiScreen->SetScreenPowerStatus(status);
        g_hdiScreen->SetScreenBacklight(level);
        g_hdiScreen->SetScreenVsyncEnabled(enabled);
        g_hdiScreen->SetScreenColorGamut(gamut);
        g_hdiScreen->SetScreenColorTransform(matrix);
        g_hdiScreen->OnVsync(sequence, ns, dt);

        std::vector<GraphicDisplayModeInfo> modes = {};
        g_hdiScreen->GetScreenSupportedModes(modes);
        g_hdiScreen->GetScreenMode(modeId);
        g_hdiScreen->GetScreenPowerStatus(status);
        g_hdiScreen->GetScreenBacklight(level);
        g_hdiScreen->GetScreenColorGamut(gamut);
        HdiScreenFuzzTest2();
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

