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

const uint32_t HAE_COLOR_MATRIX_COEF_COUNT = 20;
namespace OHOS::Rosen {
struct HaeRect {
    int l;
    int t;
    int r;
    int b;

HaeRect(int left, int top, int right, int bottom)
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
};

struct HaeImage {
    void *handle;
    struct HaeRect rect;
};

struct HaePixel {
    uint16_t a;
    uint16_t r;
    uint16_t g;
    uint16_t b;
};

struct HaeNoiseValue {
    float noiseRatio;
    float noiseValue;
    float noiseUpClip;
    float noiseDnClip;
};

struct BlurImgParam {
    uint32_t width;
    uint32_t height;
    float sigmaNum;
};

struct HaeBlurBasicAttr {
    struct HaeImage *srcLayer;
    struct HaeImage *dstLayer;
    uint32_t perfLevel;
    uint32_t timeoutMs = 0;
    int32_t expectRunTime = -1;
    float sigmaNum;
    bool enablePremult;
};

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
    struct HaeNoiseValue noisePara;
    uint32_t alphaReplaceVal;
    float colorMatrixCoef[HAE_COLOR_MATRIX_COEF_COUNT];
    struct HaePixel colorMaskPara;
};

struct hianimation_algo_device_t {
    bool (* hianimationInputCheck)(const struct BlurImgParam *imgInfo, const struct HaeNoiseValue *noisePara);
    int32_t (* hianimationAlgoInit)(uint32_t imgWeight, uint32_t imgHeight, float maxSigma, uint32_t format);
    int32_t (* hianimationAlgoDeInit)();
    int32_t (* hianimationBuildTask)(const struct HaeBlurBasicAttr *basicInfo,
        const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr);
    int32_t (* hianimationSyncProcess) (const struct HaeBlurBasicAttr *basicInfo,
        const struct HaeBlurEffectAttr *effectInfo);
    int32_t (* hianimationDestroyTask)(uint32_t taskId);
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H