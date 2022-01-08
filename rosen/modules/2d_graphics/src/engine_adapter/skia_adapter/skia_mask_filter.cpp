/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "skia_mask_filter.h"

#include "src/core/SkBlurMask.h"

#include "effect/mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMaskFilter::SkiaMaskFilter() noexcept : filter_(nullptr) {}

void SkiaMaskFilter::InitWithBlur(BlurType t, scalar radius)
{
    SkScalar sima = SkBlurMask::ConvertRadiusToSigma(radius);
    filter_ = SkMaskFilter::MakeBlur(static_cast<SkBlurStyle>(t), sima);
}

sk_sp<SkMaskFilter> SkiaMaskFilter::GetMaskFilter() const
{
    return filter_;
}
}
}
}