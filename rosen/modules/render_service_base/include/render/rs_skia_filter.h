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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H

#include "include/core/SkImageFilter.h"
#include "include/core/SkPaint.h"

#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSSkiaFilter : public RSFilter {
public:
    ~RSSkiaFilter() override;
    void ApplyTo(SkPaint& paint);

protected:
    RSSkiaFilter(sk_sp<SkImageFilter> imagefilter);

private:
    sk_sp<SkImageFilter> imageFilter_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H