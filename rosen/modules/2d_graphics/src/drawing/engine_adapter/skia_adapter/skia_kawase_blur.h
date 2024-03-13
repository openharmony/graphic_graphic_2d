/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef SKIA_KAWASE_BLUR_H
#define SKIA_KAWASE_BLUR_H

#include "impl_interface/kawase_blur_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaKawaseBlur : public KawaseBlurImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaKawaseBlur() noexcept {}
    ~SkiaKawaseBlur() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool DrawWithKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
        const std::shared_ptr<Drawing::Image>& image, const KawaseParameters& blurParam,
        const KawaseProperties& properties) override;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_KAWASE_BLUR_H