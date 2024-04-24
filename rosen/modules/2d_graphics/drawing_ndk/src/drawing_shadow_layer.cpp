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

#include "effect/blur_draw_looper.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static BlurDrawLooper* CastToBlurDrawLooper(OH_Drawing_ShadowLayer* cShadowLayer)
{
    return reinterpret_cast<BlurDrawLooper*>(cShadowLayer);
}

OH_Drawing_ShadowLayer* OH_Drawing_ShadowLayerCreate(float blurRadius, float x, float y, uint32_t color)
{
    return (OH_Drawing_ShadowLayer*)new BlurDrawLooper(blurRadius, x, y, color);
}

void OH_Drawing_ShadowLayerDestroy(OH_Drawing_ShadowLayer* cShadowLayer)
{
    delete CastToBlurDrawLooper(cShadowLayer);
}
