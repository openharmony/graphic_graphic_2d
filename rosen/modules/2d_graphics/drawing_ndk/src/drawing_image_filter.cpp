/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "drawing_image_filter.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "effect/image_filter.h"
#include "effect/image_filter_lazy.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Rect& CatsToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Rect&>(cRect);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateBlur(float sigmaX, float sigmaY, OH_Drawing_TileMode cTileMode,
    OH_Drawing_ImageFilter* input)
{
    NativeHandle<ImageFilter>* inputHandle = nullptr;
    if (input) {
        inputHandle = Helper::CastTo<OH_Drawing_ImageFilter*, NativeHandle<ImageFilter>*>(input);
    }
    NativeHandle<ImageFilter>* imageFilterHandle = new NativeHandle<ImageFilter>;
    imageFilterHandle->value = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, static_cast<TileMode>(cTileMode),
        inputHandle ? inputHandle->value : nullptr, ImageBlurType::GAUSS);
    if (imageFilterHandle->value == nullptr) {
        delete imageFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ImageFilter>*, OH_Drawing_ImageFilter*>(imageFilterHandle);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateBlurWithCrop(float sigmaX, float sigmaY,
    OH_Drawing_TileMode cTileMode, OH_Drawing_ImageFilter* input, const OH_Drawing_Rect* rect)
{
    NativeHandle<ImageFilter>* inputHandle = nullptr;
    if (input) {
        inputHandle = Helper::CastTo<OH_Drawing_ImageFilter*, NativeHandle<ImageFilter>*>(input);
    }
    NativeHandle<ImageFilter>* imageFilterHandle = new NativeHandle<ImageFilter>;
    if (rect == nullptr) {
        imageFilterHandle->value = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, static_cast<TileMode>(cTileMode),
            inputHandle ? inputHandle->value : nullptr, ImageBlurType::GAUSS);
    } else {
        imageFilterHandle->value = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, static_cast<TileMode>(cTileMode),
            inputHandle ? inputHandle->value : nullptr, ImageBlurType::GAUSS, CatsToRect(*rect));
    }
    
    if (imageFilterHandle->value == nullptr) {
        delete imageFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ImageFilter>*, OH_Drawing_ImageFilter*>(imageFilterHandle);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateFromColorFilter(
    OH_Drawing_ColorFilter* cf, OH_Drawing_ImageFilter* input)
{
    if (cf == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(cf);
    NativeHandle<ImageFilter>* inputHandle = nullptr;
    if (input) {
        inputHandle = Helper::CastTo<OH_Drawing_ImageFilter*, NativeHandle<ImageFilter>*>(input);
    }
    NativeHandle<ImageFilter>* imageFilterHandle = new NativeHandle<ImageFilter>;
    imageFilterHandle->value = ImageFilterLazy::CreateColorFilter(colorFilterHandle->value,
        inputHandle ? inputHandle->value : nullptr);
    if (imageFilterHandle->value == nullptr) {
        delete imageFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ImageFilter>*, OH_Drawing_ImageFilter*>(imageFilterHandle);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateOffset(float x, float y, OH_Drawing_ImageFilter* imageFilter)
{
    NativeHandle<ImageFilter>* inputHandle = imageFilter ? Helper::CastTo<OH_Drawing_ImageFilter*,
        NativeHandle<ImageFilter>*>(imageFilter) : nullptr;
    NativeHandle<ImageFilter>* imageFilterHandle = new NativeHandle<ImageFilter>;
    imageFilterHandle->value =
        ImageFilterLazy::CreateOffset(x, y, inputHandle ? inputHandle->value : nullptr);
    if (imageFilterHandle->value == nullptr) {
        delete imageFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ImageFilter>*, OH_Drawing_ImageFilter*>(imageFilterHandle);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateFromShaderEffect(OH_Drawing_ShaderEffect* shaderEffect)
{
    if (shaderEffect == nullptr) {
        return nullptr;
    }
    NativeHandle<ShaderEffect>* shaderFilterHandle =
        Helper::CastTo<OH_Drawing_ShaderEffect*, NativeHandle<ShaderEffect>*>(shaderEffect);
    if (shaderFilterHandle->value == nullptr) {
        return nullptr;
    }
    NativeHandle<ImageFilter>* imageFilterHandle = new NativeHandle<ImageFilter>;
    imageFilterHandle->value = ImageFilterLazy::CreateShader(shaderFilterHandle->value);
    if (imageFilterHandle->value == nullptr) {
        delete imageFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<ImageFilter>*, OH_Drawing_ImageFilter*>(imageFilterHandle);
}

void OH_Drawing_ImageFilterDestroy(OH_Drawing_ImageFilter* cImageFilter)
{
    if (!cImageFilter) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_ImageFilter*, NativeHandle<ImageFilter>*>(cImageFilter);
}
