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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_FLY_OUT_SHADER_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_FLY_OUT_SHADER_FILTER_H

#include <memory>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
constexpr size_t FLY_POINT_NUM = 12; // 12 anchor points of a patch

class RSB_EXPORT RSFlyOutShaderFilter : public RSDrawingFilterOriginal {
public:
    RSFlyOutShaderFilter(float degree_, uint32_t flyMode_);
    RSFlyOutShaderFilter(const RSFlyOutShaderFilter&) = delete;
    RSFlyOutShaderFilter operator=(const RSFlyOutShaderFilter&) = delete;
    ~RSFlyOutShaderFilter() override;

    std::string GetDescription() override;
    Drawing::Brush GetBrush(const std::shared_ptr<Drawing::Image>& image) const;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    void PostProcess(Drawing::Canvas& canvas) override {};
    float GetDegree() const;
    uint32_t GetFlyMode() const;
    void SetPathTo(Drawing::Path &path, const float width, const std::array<Drawing::Point, FLY_POINT_NUM>& flyUp,
        const std::array<Drawing::Point, FLY_POINT_NUM>& flyDown) const;
    Drawing::Point CloserToHalf(const Drawing::Point &pointOfPatch, const float nodeBounds, const float width) const;
    void CalculateDeformation(std::array<Drawing::Point, FLY_POINT_NUM>& flyUp,
        std::array<Drawing::Point, FLY_POINT_NUM>& flyDown, const float deformWidth, const float deformHeight) const;

    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

private:
    float degree_ = 0.0f;
    uint32_t flyMode_ = 0;

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_FLY_OUT_SHADER_FILTER_H