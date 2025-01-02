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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COLORFUL_SHADOW_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COLORFUL_SHADOW_FILTER_H

#include <memory>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_foreground_effect_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSColorfulShadowFilter : public RSForegroundEffectFilter {
public:
    RSColorfulShadowFilter(float blurRadius, float offsetX, float offsetY);
    RSColorfulShadowFilter(const RSColorfulShadowFilter &) = delete;
    RSColorfulShadowFilter operator=(const RSColorfulShadowFilter &) = delete;
    ~RSColorfulShadowFilter() override = default;

    bool IsValid() const override;
    std::string GetDescription() override;
    void DrawImageRect(Drawing::Canvas &canvas, const std::shared_ptr<Drawing::Image> &image, const Drawing::Rect &src,
        const Drawing::Rect &dst) const override;

private:
    float blurRadius_{};
    float offsetX_{};
    float offsetY_{};
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COLORFUL_SHADOW_FILTER_H
