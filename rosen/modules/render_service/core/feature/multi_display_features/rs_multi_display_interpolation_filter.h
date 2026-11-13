/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef RS_MULTI_DISPLAY_INTERPOLATION_FILTER_H
#define RS_MULTI_DISPLAY_INTERPOLATION_FILTER_H

#include <memory>
#include <string>

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class RSMultiDisplayInterpolationFilter {
public:
    RSMultiDisplayInterpolationFilter(float paramA, float paramN, float realWidth, float realHeight);
    ~RSMultiDisplayInterpolationFilter() = default;
    RSMultiDisplayInterpolationFilter(const RSMultiDisplayInterpolationFilter&) = delete;
    RSMultiDisplayInterpolationFilter& operator=(const RSMultiDisplayInterpolationFilter&) = delete;
    RSMultiDisplayInterpolationFilter(RSMultiDisplayInterpolationFilter&&) = delete;
    RSMultiDisplayInterpolationFilter& operator=(RSMultiDisplayInterpolationFilter&&) = delete;

    std::string GetDescription() const;
    void DrawInterpolation(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& dstRect) const;
private:
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeInterpolationShaderBuilder();

    float paramA_ = 0.0f;
    float paramN_ = 0.0f;
    float realWidth_ = 0.0f;
    float realHeight_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_MULTI_DISPLAY_INTERPOLATION_FILTER_H
