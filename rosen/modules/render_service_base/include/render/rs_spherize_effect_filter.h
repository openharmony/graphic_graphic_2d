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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SPHERIZE_EFFECT_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SPHERIZE_EFFECT_FILTER_H

#include <memory>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSpherizeEffectFilter : public RSDrawingFilterOriginal {
public:
    RSSpherizeEffectFilter(float spherizeDegree);
    RSSpherizeEffectFilter(const RSSpherizeEffectFilter&) = delete;
    RSSpherizeEffectFilter operator=(const RSSpherizeEffectFilter&) = delete;
    ~RSSpherizeEffectFilter() override;

    bool IsValid() const override;
    std::string GetDescription() override;
    Drawing::Brush GetBrush(const std::shared_ptr<Drawing::Image>& image) const;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    void PostProcess(Drawing::Canvas& canvas) override {};
    float GetSpherizeDegree() const;

    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

private:
    float spherizeDegree_;

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SPHERIZE_EFFECT_FILTER_H