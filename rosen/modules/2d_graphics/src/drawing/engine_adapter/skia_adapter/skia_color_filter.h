/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_COLOR_FILTER_H
#define SKIA_COLOR_FILTER_H

#include "include/core/SkColorFilter.h"

#include "impl_interface/color_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorFilter : public ColorFilterImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaColorFilter() noexcept;
    ~SkiaColorFilter() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void InitWithBlendMode(ColorQuad c, BlendMode mode) override;
    void InitWithColorMatrix(const ColorMatrix& m) override;
    void InitWithColorFloat(const float f[20]) override;
    void InitWithLinearToSrgbGamma() override;
    void InitWithSrgbGammaToLinear() override;
    void InitWithCompose(const ColorFilter& f1, const ColorFilter& f2) override;
    void InitWithCompose(const float f1[MATRIX_SIZE], const float f2[MATRIX_SIZE]) override;
    void Compose(const ColorFilter& f) override;
    void InitWithLuma() override;
    void InitWithOverDrawColor(const ColorQuad colors[OVER_DRAW_COLOR_NUM]) override;
    sk_sp<SkColorFilter> GetColorFilter() const;

    /*
     * @brief  Update the member variable to filter, adaptation layer calls.
     */
    void SetColorFilter(const sk_sp<SkColorFilter>& filter);

    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;
    bool AsAColorMatrix(scalar matrix[MATRIX_SIZE]) const override;
private:
    sk_sp<SkColorFilter> filter_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
