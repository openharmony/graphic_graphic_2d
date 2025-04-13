/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_filter.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "effect/filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Filter* CastToFilter(OH_Drawing_Filter* cFilter)
{
    return reinterpret_cast<Filter*>(cFilter);
}

OH_Drawing_Filter* OH_Drawing_FilterCreate()
{
    return (OH_Drawing_Filter*)new Filter();
}

void OH_Drawing_FilterSetImageFilter(OH_Drawing_Filter* cFliter, OH_Drawing_ImageFilter* cImageFilter)
{
    Filter* filter = CastToFilter(cFliter);
    if (filter == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cImageFilter == nullptr) {
        filter->SetImageFilter(nullptr);
        return;
    }
    auto imageFilterHandle = Helper::CastTo<OH_Drawing_ImageFilter*, NativeHandle<ImageFilter>*>(cImageFilter);
    filter->SetImageFilter(imageFilterHandle->value);
}

void OH_Drawing_FilterSetMaskFilter(OH_Drawing_Filter* cFliter, OH_Drawing_MaskFilter* cMaskFilter)
{
    Filter* filter = CastToFilter(cFliter);
    if (filter == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cMaskFilter == nullptr) {
        filter->SetMaskFilter(nullptr);
        return;
    }
    auto maskFilterHandle = Helper::CastTo<OH_Drawing_MaskFilter*, NativeHandle<MaskFilter>*>(cMaskFilter);
    filter->SetMaskFilter(maskFilterHandle->value);
}

void OH_Drawing_FilterSetColorFilter(OH_Drawing_Filter* cFliter, OH_Drawing_ColorFilter* cColorFilter)
{
    Filter* filter = CastToFilter(cFliter);
    if (filter == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cColorFilter == nullptr) {
        filter->SetColorFilter(nullptr);
        return;
    }
    auto colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*, NativeHandle<ColorFilter>*>(cColorFilter);
    filter->SetColorFilter(colorFilterHandle->value);
}

void OH_Drawing_FilterGetColorFilter(OH_Drawing_Filter* cFliter, OH_Drawing_ColorFilter* cColorFilter)
{
    Filter* filter = CastToFilter(cFliter);
    if (filter == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(cColorFilter);
    if (colorFilterHandle == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    std::shared_ptr<ColorFilter> colorFilterPtr = filter->GetColorFilter();
    if (colorFilterPtr == nullptr) {
        colorFilterHandle->value = std::make_shared<ColorFilter>(ColorFilter::FilterType::NO_TYPE);
        return;
    }
    colorFilterHandle->value = colorFilterPtr;
}

void OH_Drawing_FilterDestroy(OH_Drawing_Filter* cFilter)
{
    if (!cFilter) {
        return;
    }
    delete CastToFilter(cFilter);
}
