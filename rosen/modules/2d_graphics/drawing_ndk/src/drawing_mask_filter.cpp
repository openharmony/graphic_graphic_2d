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

#include "effect/mask_filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static MaskFilter* CastToMaskFilter(OH_Drawing_MaskFilter* cMaskFilter)
{
    return reinterpret_cast<MaskFilter*>(cMaskFilter);
}

OH_Drawing_MaskFilter* OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType blurType, float sigma, bool respectCTM)
{
    return (OH_Drawing_MaskFilter*)new MaskFilter(MaskFilter::FilterType::BLUR,
        static_cast<BlurType>(blurType), sigma, respectCTM);
}

void OH_Drawing_MaskFilterDestroy(OH_Drawing_MaskFilter* cMaskFilter)
{
    delete CastToMaskFilter(cMaskFilter);
}
