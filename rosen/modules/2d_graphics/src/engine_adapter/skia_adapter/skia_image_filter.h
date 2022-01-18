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

#ifndef SKIA_IMAGE_FILTER_H
#define SKIA_IMAGE_FILTER_H

#include "include/core/SkColorFilter.h"
#include "include/core/SkImageFilter.h"

#include "impl_interface/image_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageFilter : public ImageFilterImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaImageFilter() noexcept;
    ~SkiaImageFilter() {};
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void InitWithBlur(scalar sigmaX, scalar sigmaY, const ImageFilter& f) override;
    void InitWithColor(const ColorFilter& colorFilter, const ImageFilter& f) override;
    void InitWithOffset(scalar dx, scalar dy, const ImageFilter& f) override;
    void InitWithArithmetic(scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor, const ImageFilter& f1,
        const ImageFilter& f2) override;
    void InitWithCompose(const ImageFilter& f1, const ImageFilter& f2) override;
    sk_sp<SkImageFilter> GetImageFilter() const;

private:
    sk_sp<SkImageFilter> filter_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif