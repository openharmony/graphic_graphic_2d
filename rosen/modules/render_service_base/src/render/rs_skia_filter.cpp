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

void RSSkiaFilter::ApplyTo(SkPaint& paint)
{
    paint.setImageFilter(imageFilter_);
}
} // namespace Rosen
} // namespace OHOS
