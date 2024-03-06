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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H

#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
class RSRenderNode;

class RSPropertyDrawable : public RSDrawable {
public:
    RSPropertyDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    RSPropertyDrawable() = default;
    ~RSPropertyDrawable() override = default;

    void OnSync() override;
    // RSDrawable::Ptr CreateDrawable() const override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

protected:
    bool needSync_ = false;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList_;
    std::shared_ptr<Drawing::DrawCmdList> stagingDrawCmdList_;

    friend class RSPropertyDrawCmdListUpdater;
    friend class RSPropertyDrawableNG;
};

// ============================================================================
// Frame offset
class RSFrameOffsetDrawable : public RSPropertyDrawable {
public:
    RSFrameOffsetDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSFrameOffsetDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

// ============================================================================
// Clip To Bounds and Clip To Frame
class RSClipToBoundsDrawable : public RSPropertyDrawable {
public:
    RSClipToBoundsDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSClipToBoundsDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSClipToFrameDrawable : public RSPropertyDrawable {
public:
    RSClipToFrameDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSClipToFrameDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

// ============================================================================
// Background
class RSBackgroundColorDrawable : public RSPropertyDrawable {
public:
    RSBackgroundColorDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundColorDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundShaderDrawable : public RSPropertyDrawable {
public:
    RSBackgroundShaderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundShaderDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundImageDrawable : public RSPropertyDrawable {
public:
    RSBackgroundImageDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundImageDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundFilterDrawable : public RSDrawable {
public:
    RSBackgroundFilterDrawable() = default;
    ~RSBackgroundFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::shared_ptr<RSFilter> filter_;
    std::shared_ptr<RSFilter> stagingFilter_;
};

class RSBorderDrawable : public RSPropertyDrawable {
    friend class RSOutlineDrawable;

public:
    RSBorderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBorderDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
};

class RSOutlineDrawable : public RSPropertyDrawable {
public:
    RSOutlineDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSOutlineDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSShadowDrawable : public RSPropertyDrawable {
public:
    RSShadowDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSShadowDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawColorfulShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
};

class RSForegroundColorDrawable : public RSPropertyDrawable {
public:
    RSForegroundColorDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSForegroundColorDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSForegroundFilterDrawable : public RSDrawable {
public:
    RSForegroundFilterDrawable() = default;
    ~RSForegroundFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::shared_ptr<RSFilter> filter_;
    std::shared_ptr<RSFilter> stagingFilter_;
};

class RSPixelStretchDrawable : public RSDrawable {
public:
    RSPixelStretchDrawable() = default;
    ~RSPixelStretchDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::optional<Vector4f> pixelStretch_;
    std::optional<Vector4f> stagingPixelStretch_;
    bool boundsGeoValid_ = false;
    bool stagingBoundsGeoValid_ = false;
    RectF boundsRect_;
    RectF stagingBoundsRect_;
};

class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    RSDynamicLightUpDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSDynamicLightUpDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;
    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(
        float dynamicLightUpRate, float dynamicLightUpDeg);
};

class RSLightUpEffectDrawable : public RSDrawable {
public:
    RSLightUpEffectDrawable() = default;
    ~RSLightUpEffectDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    float lightUpEffectDegree_ = 1.0f;
    float stagingLightUpEffectDegree_ = 1.0f;
};

class RSColorFilterDrawable : public RSDrawable {
public:
    RSColorFilterDrawable() = default;
    ~RSColorFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::shared_ptr<Drawing::ColorFilter> filter_;
    std::shared_ptr<Drawing::ColorFilter> stagingFilter_;
};

class RSMaskDrawable : public RSPropertyDrawable {
public:
    RSMaskDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList)) {}
    RSMaskDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBinarizationDrawable : public RSDrawable {
public:
    RSBinarizationDrawable() = default;
    ~RSBinarizationDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::optional<Vector4f> aiInvert_;
    std::optional<Vector4f> stagingAiInvert_;
};

class RSParticleDrawable : public RSPropertyDrawable {
public:
    RSParticleDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) :
        RSPropertyDrawable(std::move(drawCmdList)) {}
    RSParticleDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBeginBlendModeDrawable : public RSPropertyDrawable {
public:
    RSBeginBlendModeDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBeginBlendModeDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSEndBlendModeDrawable : public RSPropertyDrawable {
public:
    RSEndBlendModeDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSEndBlendModeDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSPointLightDrawable : public RSPropertyDrawable {
public:
    RSPointLightDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSPointLightDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> phongShaderBuilder_;

    static const std::shared_ptr<Drawing::RuntimeShaderBuilder>& GetPhongShaderBuilder();
    static void DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Vector4f& lightIntensity);
    static void DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen, Vector4f& lightIntensity);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
