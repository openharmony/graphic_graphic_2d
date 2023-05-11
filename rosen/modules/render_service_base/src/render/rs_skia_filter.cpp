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
RSSkiaFilter::RSSkiaFilter(sk_sp<SkImageFilter> imageFilter) : RSFilter(), imageFilter_(imageFilter), otherFilter_(nullptr) {}

RSSkiaFilter::~RSSkiaFilter() {}

SkPaint RSSkiaFilter::GetPaint()
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setBlendMode(SkBlendMode::kSrcOver);
    sk_sp<SkImageFilter> composedFilter = SkImageFilters::Compose(imageFilter_, otherFilter_);
    paint.setImageFilter(composedFilter);
    otherFilter_ = nullptr;
    return paint;
}

sk_sp<SkImageFilter> RSSkiaFilter::GetImageFilter() const
{
    return imageFilter_;
}

void RSSkiaFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    if (inner == nullptr) {
        return;
    }
    otherFilter_ = inner->GetImageFilter();
}
} // namespace Rosen
} // namespace OHOS
