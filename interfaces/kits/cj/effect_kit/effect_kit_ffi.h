/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CJ_EFFECT_KIT_FFI_H
#define CJ_EFFECT_KIT_FFI_H

#include <vector>

#include "cj_common_ffi.h"

extern "C" {
struct CArrFloat {
    float* head;
    int64_t size;
};

struct CJColor {
    int r;
    int g;
    int b;
    int a;
};

struct CArrCJColor {
    CJColor* head;
    int64_t size;
};

struct CArrDouble {
    double* head;
    int64_t size;
};

FFI_EXPORT int64_t FfiEffectKitCreateEffect(int64_t sourceId, uint32_t* errorCode);
FFI_EXPORT void FfiEffectKitBlur(int64_t id, float radius);
FFI_EXPORT void FfiEffectKitInvert(int64_t id);
FFI_EXPORT void FfiEffectKitBrightness(int64_t id, float bright);
FFI_EXPORT void FfiEffectKitGrayscale(int64_t id);
FFI_EXPORT void FfiEffectKitSetColorMatrix(int64_t id, CArrFloat cjColorMatrix, uint32_t* errorCode);
FFI_EXPORT int64_t FfiEffectKitGetEffectPixelMap();
FFI_EXPORT int64_t FfiEffectKitCreateColorPicker(int64_t sourceId, uint32_t* errorCode);
FFI_EXPORT int64_t FfiEffectKitCreateColorPickerRegion(int64_t sourceId, CArrDouble region, uint32_t* errorCode);
FFI_EXPORT CJColor FfiEffectKitGetMainColor(uint32_t* errorCode);
FFI_EXPORT CJColor FfiEffectKitGetLargestProportionColor(uint32_t* errorCode);
FFI_EXPORT CJColor FfiEffectKitGetHighestSaturationColor(uint32_t* errorCode);
FFI_EXPORT CJColor FfiEffectKitGetAverageColor(uint32_t* errorCode);
FFI_EXPORT bool FfiEffectKitIsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t* errorCode);
FFI_EXPORT CArrCJColor FfiEffectKitGetTopProportionColors(double colorCount, uint32_t* errorCode);
}

#endif