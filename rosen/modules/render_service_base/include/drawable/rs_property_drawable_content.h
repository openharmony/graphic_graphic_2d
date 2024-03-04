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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H

#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable_content.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
class RSRenderNode;

class RSPropertyDrawableContent : public RSDrawableContent {
public:
    RSPropertyDrawableContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    RSPropertyDrawableContent() = default;
    ~RSPropertyDrawableContent() override = default;

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

class RSBackgroundColorContent : public RSPropertyDrawableContent {
public:
    RSBackgroundColorContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBackgroundColorContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundShaderContent : public RSPropertyDrawableContent {
public:
    RSBackgroundShaderContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBackgroundShaderContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundImageContent : public RSPropertyDrawableContent {
public:
    RSBackgroundImageContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBackgroundImageContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundFilterContent : public RSPropertyDrawableContent {
public:
    RSBackgroundFilterContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBackgroundFilterContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBorderContent : public RSPropertyDrawableContent {
friend class RSOutlineContent;
public:
    RSBorderContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBorderContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
};

class RSOutlineContent : public RSPropertyDrawableContent {
public:
    RSOutlineContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSOutlineContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSShadowContent : public RSPropertyDrawableContent {
public:
    RSShadowContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSShadowContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawColorfulShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
};

class RSForegroundColorContent : public RSPropertyDrawableContent {
public:
    RSForegroundColorContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSForegroundColorContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSPixelStretchContent : public RSPropertyDrawableContent {
public:
    RSPixelStretchContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSPixelStretchContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSDynamicLightUpContent : public RSPropertyDrawableContent {
public:
    RSDynamicLightUpContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSDynamicLightUpContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;

    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(float dynamicLightUpRate,
        float dynamicLightUpDeg);
};

class RSLightUpEffectContent : public RSPropertyDrawableContent {
public:
    RSLightUpEffectContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSLightUpEffectContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSColorFilterContent : public RSPropertyDrawableContent {
public:
    RSColorFilterContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSColorFilterContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSMaskContent : public RSPropertyDrawableContent {
public:
    RSMaskContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSMaskContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBinarizationShaderContent : public RSPropertyDrawableContent {
public:
    RSBinarizationShaderContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBinarizationShaderContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;

    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
};

class RSParticleContent : public RSPropertyDrawableContent {
public:
    RSParticleContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSParticleContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H
