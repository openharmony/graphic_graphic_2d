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

#include "drawing_mask_filter.h"
#include "drawing_helper.h"
#include "effect/mask_filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_MaskFilter* OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType blurType, float sigma, bool respectCTM)
{
    NativeHandle<MaskFilter>* maskFilterHandle = new NativeHandle<MaskFilter>;
    maskFilterHandle->value = MaskFilter::CreateBlurMaskFilter(static_cast<BlurType>(blurType), sigma, respectCTM);
    if (maskFilterHandle->value == nullptr) {
        delete maskFilterHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<MaskFilter>*, OH_Drawing_MaskFilter*>(maskFilterHandle);
}

void OH_Drawing_MaskFilterDestroy(OH_Drawing_MaskFilter* cMaskFilter)
{
    if (!cMaskFilter) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_MaskFilter*, NativeHandle<MaskFilter>*>(cMaskFilter);
}
