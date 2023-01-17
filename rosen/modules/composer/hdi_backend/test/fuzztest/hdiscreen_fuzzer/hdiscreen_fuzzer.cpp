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
#include "display_type.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
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
        // get data
        uint32_t screenId = GetData<uint32_t>();
        uint32_t propId = GetData<uint32_t>();
        uint64_t value = GetData<uint64_t>();
        InterfaceType type = GetData<InterfaceType>();
        uint32_t phyWidth = GetData<uint32_t>();
        uint32_t phyHeight = GetData<uint32_t>();
        uint32_t supportLayers = GetData<uint32_t>();
        uint32_t virtualDispCount = GetData<uint32_t>();
        bool supportWriteBack = GetData<bool>();
        uint32_t propertyCount = GetData<uint32_t>();
        uint32_t formatCount = GetData<uint32_t>();
        HDRFormat formats = GetData<HDRFormat>();
        float maxLum = GetData<float>();
        float maxAverageLum = GetData<float>();
        float minLum = GetData<float>();

        // test
        std::unique_ptr<HdiScreen> hdiScreen = HdiScreen::CreateHdiScreen(screenId);
        PropertyObject props = {"propName", propId, value};
        DisplayCapability dcap = {
            .name = "dispName",
            .type = type,
            .phyWidth = phyWidth,
            .phyHeight = phyHeight,
            .supportLayers = supportLayers,
            .virtualDispCount = virtualDispCount,
            .supportWriteBack = supportWriteBack,
            .propertyCount = propertyCount,
            .props = &props,
        };
        hdiScreen->GetScreenCapability(dcap);

        HDRCapability info = {
            .formatCount = formatCount,
            .formats = &formats,
            .maxLum = maxLum,
            .maxAverageLum = maxAverageLum,
            .minLum = minLum,
        };
        hdiScreen->GetHDRCapabilityInfos(info);
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
        uint32_t screenId = GetData<uint32_t>();
        uint32_t modeId = GetData<uint32_t>();
        DispPowerStatus status = GetData<DispPowerStatus>();
        uint32_t level = GetData<uint32_t>();
        bool enabled = GetData<bool>();
        GraphicColorGamut gamut = GetData<GraphicColorGamut>();
        GamutMap gamutMap = GetData<GamutMap>();
        float matrix = GetData<float>();
        uint32_t sequence = GetData<uint32_t>();
        uint64_t ns = GetData<uint64_t>();
        void* dt = static_cast<void*>(GetStringFromData(STR_LEN).data());

        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        uint32_t freshRate = GetData<uint32_t>();
        int32_t id = GetData<int32_t>();

        // test
        std::unique_ptr<HdiScreen> hdiScreen = HdiScreen::CreateHdiScreen(screenId);
        hdiScreen->Init();
        hdiScreen->SetScreenMode(modeId);
        hdiScreen->SetScreenPowerStatus(status);
        hdiScreen->SetScreenBacklight(level);
        hdiScreen->SetScreenVsyncEnabled(enabled);
        hdiScreen->SetScreenColorGamut(gamut);
        hdiScreen->SetScreenGamutMap(gamutMap);
        hdiScreen->SetScreenColorTransform(&matrix);
        hdiScreen->OnVsync(sequence, ns, dt);

        DisplayModeInfo mode = {width, height, freshRate, id};
        std::vector<DisplayModeInfo> modes = {mode};
        hdiScreen->GetScreenSupportedModes(modes);
        hdiScreen->GetScreenMode(modeId);
        hdiScreen->GetScreenPowerStatus(status);
        hdiScreen->GetScreenBacklight(level);
        hdiScreen->GetScreenColorGamut(gamut);
        hdiScreen->GetScreenGamutMap(gamutMap);

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

