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

class RSBackgroundColorDrawable : public RSPropertyDrawable {
public:
    RSBackgroundColorDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundColorDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundShaderDrawable : public RSPropertyDrawable {
public:
    RSBackgroundShaderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundShaderDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundImageDrawable : public RSPropertyDrawable {
public:
    RSBackgroundImageDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundImageDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBackgroundFilterDrawable : public RSPropertyDrawable {
public:
    RSBackgroundFilterDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundFilterDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBorderDrawable : public RSPropertyDrawable {
friend class RSOutlineDrawable;
public:
    RSBorderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBorderDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
};

class RSOutlineDrawable : public RSPropertyDrawable {
public:
    RSOutlineDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSOutlineDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSShadowDrawable : public RSPropertyDrawable {
public:
    RSShadowDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSShadowDrawable() : RSPropertyDrawable()
    {}
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
    RSForegroundColorDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSPixelStretchDrawable : public RSPropertyDrawable {
public:
    RSPixelStretchDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSPixelStretchDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    RSDynamicLightUpDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSDynamicLightUpDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;

    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(float dynamicLightUpRate,
        float dynamicLightUpDeg);
};

class RSLightUpEffectDrawable : public RSPropertyDrawable {
public:
    RSLightUpEffectDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSLightUpEffectDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSColorFilterDrawable : public RSPropertyDrawable {
public:
    RSColorFilterDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSColorFilterDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSMaskDrawable : public RSPropertyDrawable {
public:
    RSMaskDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSMaskDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

class RSBinarizationDrawable : public RSPropertyDrawable {
public:
    RSBinarizationDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBinarizationDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;

    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
};

class RSParticleDrawable : public RSPropertyDrawable {
public:
    RSParticleDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSParticleDrawable() : RSPropertyDrawable()
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
