/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rsuieffect_fuzzer.h"

#include <securec.h>

#include "background_color_effect_para.h"
#include "blender.h"
#include "brightness_blender.h"
#include "filter_blur_para.h"
#include "filter_fly_out_para.h"
#include "filter_distort_para.h"
#include "filter_para.h"
#include "filter_pixel_stretch_para.h"
#include "filter_water_ripple_para.h"
#include "filter.h"
#include "visual_effect_para.h"
#include "visual_effect.h"


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

    void BlenderFuzzTest()
    {
        auto blender = std::make_shared<Blender>();
        blender->GetBlenderType();
    }

    void BackgroundColorEffectParaFuzzTest()
    {
        auto backgroundColorEffectPara = std::make_shared<BackgroundColorEffectPara>();
        auto blender = std::make_shared<Blender>();
        backgroundColorEffectPara->SetBlender(blender);
        backgroundColorEffectPara->GetBlender();
    }

    void BrightnessBlenderFuzzTest()
    {
        float cubicRate = GetData<float>();
        float quadRate = GetData<float>();
        float linearRate = GetData<float>();
        float degree = GetData<float>();
        float saturation = GetData<float>();
        float fraction = GetData<float>();
        Vector3f positiveCoeff(GetData<float>(), GetData<float>(), GetData<float>());
        Vector3f negativeCoeff(GetData<float>(), GetData<float>(), GetData<float>());

        auto brightnessBlender = std::make_shared<BrightnessBlender>();
        brightnessBlender->SetCubicRate(cubicRate);
        brightnessBlender->GetCubicRate();
        brightnessBlender->SetQuadRate(quadRate);
        brightnessBlender->GetQuadRate();
        brightnessBlender->SetLinearRate(linearRate);
        brightnessBlender->GetLinearRate();
        brightnessBlender->SetDegree(degree);
        brightnessBlender->GetDegree();
        brightnessBlender->SetSaturation(saturation);
        brightnessBlender->GetSaturation();

        brightnessBlender->SetPositiveCoeff(positiveCoeff);
        brightnessBlender->GetPositiveCoeff();
        brightnessBlender->SetNegativeCoeff(negativeCoeff);
        brightnessBlender->GetNegativeCoeff();

        brightnessBlender->SetFraction(fraction);
        brightnessBlender->GetFraction();
    }

    void VisualEffectParaFuzzTest()
    {
        auto visualEffectPara = std::make_shared<VisualEffectPara>();
        visualEffectPara->GetParaType();
    }

    void VisualEffectFuzzTest()
    {
        auto visualEffect = std::make_shared<VisualEffect>();
        auto visualEffectPara = std::make_shared<VisualEffectPara>();
        visualEffect->AddPara(visualEffectPara);
    }

    void FilterBlurParaFuzzTest()
    {
        auto filterBlurPara = std::make_shared<FilterBlurPara>();
        float radius = GetData<float>();
        filterBlurPara->SetRadius(radius);
        filterBlurPara->GetRadius();
    }

    void FlyOutParaFuzzTest()
    {
        auto flyOutPara = std::make_shared<FlyOutPara>();
        float degree = GetData<float>();
        uint32_t flyMode = GetData<uint32_t>();
        flyOutPara->SetDegree(degree);
        flyOutPara->GetDegree();
        flyOutPara->SetFlyMode(flyMode);
        flyOutPara->GetFlyMode();
    }

    void DistortParaFuzzTest()
    {
        auto distortPara = std::make_shared<DistortPara>();
        float distortionK = GetData<float>();
        distortPara->SetDistortionK(distortionK);
        distortPara->GetDistortionK();
    }

    void FilterParaFuzzTest()
    {
        auto filterPara = std::make_shared<FilterPara>();
        filterPara->GetParaType();
    }

    void PixelStretchParaFuzzTest()
    {
        Vector4f stretchPercent;
        stretchPercent[0] = GetData<float>(); // 0 represents left
        stretchPercent[1] = GetData<float>(); // 1 represents top
        stretchPercent[2] = GetData<float>(); // 2 represents right
        stretchPercent[3] = GetData<float>(); // 3 represents bottom
        Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP;

        auto pixelStretchPara = std::make_shared<PixelStretchPara>();
        pixelStretchPara->SetStretchPercent(stretchPercent);
        pixelStretchPara->GetStretchPercent();
        pixelStretchPara->SetTileMode(stretchTileMode);
        pixelStretchPara->GetTileMode();
    }

    void WaterRippleParaFuzzTest()
    {
        float progress = GetData<float>();
        uint32_t waveCount = GetData<uint32_t>();
        float rippleCenterX = GetData<float>();
        float rippleCenterY = GetData<float>();
        uint32_t rippleMode = GetData<uint32_t>();

        auto waterRipplePara = std::make_shared<WaterRipplePara>();
        waterRipplePara->SetProgress(progress);
        waterRipplePara->GetProgress();
        waterRipplePara->SetWaveCount(waveCount);
        waterRipplePara->GetWaveCount();
        waterRipplePara->SetRippleCenterX(rippleCenterX);
        waterRipplePara->GetRippleCenterX();
        waterRipplePara->SetRippleCenterY(rippleCenterY);
        waterRipplePara->GetRippleCenterY();
        waterRipplePara->SetRippleMode(rippleMode);
        waterRipplePara->GetRippleMode();
    }

    void FilterFuzzTest()
    {
        auto filter = std::make_shared<Filter>();
        auto filterPara = std::make_shared<FilterPara>();
        filter->AddPara(filterPara);
        filter->GetAllPara();
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

        BlenderFuzzTest();
        BackgroundColorEffectParaFuzzTest();
        BrightnessBlenderFuzzTest();
        VisualEffectParaFuzzTest();
        VisualEffectFuzzTest();
        FilterBlurParaFuzzTest();
        FlyOutParaFuzzTest();
        DistortParaFuzzTest();
        FilterParaFuzzTest();
        PixelStretchParaFuzzTest();
        WaterRippleParaFuzzTest();
        FilterFuzzTest();

        return true;
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

