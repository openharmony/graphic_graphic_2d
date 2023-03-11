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

#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
RSSkiaFilter::RSSkiaFilter(sk_sp<SkImageFilter> imageFilter) : RSFilter(), imageFilter_(imageFilter) {}

RSSkiaFilter::~RSSkiaFilter() {}

SkPaint RSSkiaFilter::GetPaint() const
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setBlendMode(SkBlendMode::kSrcOver);
    paint.setImageFilter(imageFilter_);
    return paint;
}

sk_sp<SkImageFilter> RSSkiaFilter::GetImageFilter() const
{
    return imageFilter_;
}

std::shared_ptr<RSSkiaFilter> RSSkiaFilter::Compose(const std::shared_ptr<RSSkiaFilter>& outer,
    const std::shared_ptr<RSSkiaFilter>& inner)
{
    if (outer == nullptr && inner == nullptr) {
        return nullptr;
    }
    sk_sp<SkImageFilter> outerFilter = nullptr;
    sk_sp<SkImageFilter> innerFilter = nullptr;
    if (outer != nullptr) {
        outerFilter = outer->GetImageFilter();
    }
    if (inner != nullptr) {
        innerFilter = inner->GetImageFilter();
    }
    sk_sp<SkImageFilter> composedFilter = SkImageFilters::Compose(outerFilter, innerFilter);
    return std::make_shared<RSSkiaFilter>(composedFilter);
}
} // namespace Rosen
} // namespace OHOS
