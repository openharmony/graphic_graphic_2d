/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef GRADIENT_BLUR_PARAM_H
#define GRADIENT_BLUR_PARAM_H

#include <array>
#include <iostream>

#include "effect/image_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct DRAWING_API GradientBlurParam {
    float blurRadius_ = 0.0f;
    std::vector<std::pair<float, float>> fractionStops_;
    GradientDir direction_;
    bool useMaskAlgorithm_ = true;
    float originalBase_ = 1000.0f; // 1000.0f represents original radius_base
    std::shared_ptr<Drawing::ImageFilter> blurImageFilter_ = nullptr;
    float imageFilterBlurRadius_ = 0.0f;
    Drawing::Rect clipIPadding_;
    uint8_t directionBias_ = 0;
    float imageScale_ = 1.0f;
    // Fast Path Judgment
    bool maskLinearBlurEnable_ = false;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GRADIENT_BLUR_PARAM_H