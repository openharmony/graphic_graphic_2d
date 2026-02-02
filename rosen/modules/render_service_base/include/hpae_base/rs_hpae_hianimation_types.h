/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H
#define RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H

#include <cstdint>
#include <vector>

const uint32_t HAE_COLOR_MATRIX_COEF_COUNT = 20;
namespace OHOS::Rosen {
struct HaeRect {
    int32_t l;
    int32_t t;
    int32_t r;
    int32_t b;

    HaeRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        l = left;
        t = top;
        r = right;
        b = bottom;
    };

    HaeRect()
    {
        l = 0;
        t = 0;
        r = 0;
        b = 0;
    };
} __attribute__ ((aligned(8)));

struct HaeImage {
    void* handle;
    struct HaeRect rect;
} __attribute__ ((aligned(8)));

struct HaePixel {
    uint16_t a;
    uint16_t r;
    uint16_t g;
    uint16_t b;
} __attribute__ ((aligned(8)));

struct HaeNoiseValue {
    float noiseRatio;
    float noiseValue;
    float noiseUpClip;
    float noiseDnClip;
} __attribute__ ((aligned(8)));

struct BlurImgParam {
    uint32_t width;
    uint32_t height;
    float sigmaNum;
} __attribute__ ((aligned(8)));

struct HaeBlurBasicAttr {
    struct HaeImage srcLayer;
    struct HaeImage dstLayer;
    uint32_t perfLevel;
    uint32_t timeoutMs = 0;
    int32_t expectRunTime = -1;
    float sigmaNum;
    bool enablePremult;
} __attribute__ ((aligned(8)));

enum BlppAbility : uint32_t {
    BLPP_CANCEL_PREMULT_EN = 1 << 0,
    BLPP_NOISE_EN = 1 << 1,
    BLPP_ALPHA_REPLACE_EN = 1 << 2,
    BLPP_COLOR_MATRIX_EN = 1 << 3,
    BLPP_PREMULT_EN = 1 << 4,
    BLPP_COLOR_MASK_EN = 1 << 5,
    BLPP_CLMASK_PREMULT_EN = 1 << 6,
    BLPP_CLMASK_CXL_PREMULT_EN = 1 << 7,
};

struct HaeBlurEffectAttr {
    uint16_t effectCaps;
    uint32_t alphaReplaceVal;
    struct HaeNoiseValue noisePara;
    std::vector<float> colorMatrixCoef;
    struct HaePixel colorMaskPara;
};

// define function pointer types
using HianimationOpenDeviceFunc = void* (*)();
using HianimationCloseDeviceFunc = void (*)();
using HianimationInputCheckFunc = int32_t (*)(const BlurImgParam* imgInfo, const HaeNoiseValue* noisePara);
using HianimationAlgoInitFunc = int32_t (*)(uint32_t imgWidth, uint32_t imgHeight, float maxSigma, uint32_t format);
using HianimationAlgoDeInitFunc = int32_t (*)();
using HianimationBuildTaskFunc = int32_t (*)(const HaeBlurBasicAttr* basicInfo, const HaeBlurEffectAttr* effectInfo,
    uint32_t* outTaskId, void** taskPtr);
using HianimationDestroyTaskFunc = int32_t (*)(uint32_t taskId);
using HianimationDumpDebugInfoFunc = int32_t (*)(uint32_t taskId);

// define device structure
struct hianimation_algo_device_t {
    void* device;
    HianimationOpenDeviceFunc openDevice;
    HianimationCloseDeviceFunc closeDevice;
    HianimationInputCheckFunc hianimationInputCheck;
    HianimationAlgoInitFunc hianimationAlgoInit;
    HianimationAlgoDeInitFunc hianimationAlgoDeInit;
    HianimationBuildTaskFunc hianimationBuildTask;
    HianimationDestroyTaskFunc hianimationDestroyTask;
    HianimationDumpDebugInfoFunc hianimationDumpDebugInfo;
    
    hianimation_algo_device_t() = default;
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H