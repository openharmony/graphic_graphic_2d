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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MOTION_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MOTION_BLUR_FILTER_H

#include "common/rs_vector2.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT MotionBlurParam {
public:
    float radius = 0.f;
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);

    explicit MotionBlurParam(float r, Vector2f& s) : radius(r), scaleAnchor(s) {}
    ~MotionBlurParam() = default;
};

class RSB_EXPORT RSMotionBlurFilter : public RSDrawingFilterOriginal {
public:
    RSMotionBlurFilter(const std::shared_ptr<MotionBlurParam>& para);
    RSMotionBlurFilter(const RSMotionBlurFilter&) = delete;
    RSMotionBlurFilter operator=(const RSMotionBlurFilter&) = delete;
    ~RSMotionBlurFilter() override;

    void PostProcess(Drawing::Canvas& canvas) override {};
    std::string GetDescription() override;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

    void SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight) override
    {
        Drawing::Matrix mat = canvas.GetTotalMatrix();
        Drawing::Rect rect = canvas.GetDeviceClipBounds();
        mat.MapRect(rect, rect);
        curRect_ = Drawing::Rect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    }

    void IsOffscreenCanvas(bool isOffscreenCanvas) override
    {
        isOffscreenCanvas_ = isOffscreenCanvas;
    }

private:
    static bool RectValid(const Drawing::Rect& rect1, const Drawing::Rect& rect2);
    static void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst);

    static std::shared_ptr<Drawing::ShaderEffect> MakeMotionBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, Vector2f& scaleAnchor, Vector2f& scaleSize,
        Vector2f& rectOffset, float radius);

    friend class RSMarshallingHelper;
    inline static Drawing::Rect lastRect_ = Drawing::Rect(0.f, 0.f, 0.f, 0.f);
    inline static Drawing::Rect curRect_ = Drawing::Rect(0.f, 0.f, 0.f, 0.f);
    inline static bool isOffscreenCanvas_ = false;
    std::shared_ptr<MotionBlurParam> motionBlurPara_ = nullptr;

    static std::shared_ptr<Drawing::RuntimeEffect> motionBlurShaderEffect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MOTION_BLUR_FILTER_H