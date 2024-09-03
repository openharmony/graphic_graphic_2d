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

#include "drawing_shadow_layer.h"
#include "drawing_helper.h"

#include "drawing_canvas_utils.h"

#include "effect/blur_draw_looper.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_ShadowLayer* OH_Drawing_ShadowLayerCreate(float blurRadius, float x, float y, uint32_t color)
{
    if (blurRadius <= 0.f) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    NativeHandle<BlurDrawLooper>* blurDrawLooperHandle = new NativeHandle<BlurDrawLooper>;
    blurDrawLooperHandle->value = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, x, y, color);
    if (blurDrawLooperHandle->value == nullptr) {
        delete blurDrawLooperHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<BlurDrawLooper>*, OH_Drawing_ShadowLayer*>(blurDrawLooperHandle);
}

void OH_Drawing_ShadowLayerDestroy(OH_Drawing_ShadowLayer* cShadowLayer)
{
    if (!cShadowLayer) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_ShadowLayer*, NativeHandle<BlurDrawLooper>*>(cShadowLayer);
}
