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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H

#include <utility>

#include "common/rs_color.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_render_node_drawable_adapter.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "external_window.h"
#include "surface_buffer.h"
#endif

namespace OHOS::Rosen {
class RSProperties;
class RSFilter;
namespace Drawing {
class RuntimeEffect;
}
#ifdef RS_ENABLE_VK
namespace NativeBufferUtils {
class VulkanCleanupHelper;
}
#endif

namespace DrawableV2 {
class RSShadowDrawable : public RSDrawable {
public:
    RSShadowDrawable() = default;
    ~RSShadowDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    Drawing::Path path_;
    Drawing::Path stagingPath_;
    Color color_;
    Color stagingColor_;
    float offsetX_;
    float stagingOffsetX_;
    float offsetY_;
    float stagingOffsetY_;
    float elevation_;
    float stagingElevation_;
    bool isFilled_;
    bool stagingIsFilled_;
    float radius_;
    float stagingRadius_;
    int colorStrategy_;
    int stagingColorStrategy_;
};

class RSMaskDrawable : public RSPropertyDrawable {
public:
    RSMaskDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList)) {}
    RSMaskDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
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

private:
};

class RSBackgroundShaderDrawable : public RSPropertyDrawable {
public:
    RSBackgroundShaderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundShaderDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSBackgroundImageDrawable : public RSPropertyDrawable {
public:
    RSBackgroundImageDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    ~RSBackgroundImageDrawable() override;
    RSBackgroundImageDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;
private:
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    static Drawing::ColorType GetColorTypeFromVKFormat(VkFormat vkFormat);
    std::shared_ptr<Drawing::Image> MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer);
    void ReleaseNativeWindowBuffer();
    void SetCompressedDataForASTC();
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    pid_t tid_ = 0;
    uint32_t pixelMapId_ = 0;
    Drawing::BackendTexture backendTexture_ = {};
    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper_ = nullptr;
#endif
    std::shared_ptr<RSImage> stagingBgImage_ = nullptr;
    Drawing::Rect stagingBoundsRect_;
    std::shared_ptr<RSImage> bgImage_ = nullptr;
    Drawing::Rect boundsRect_;
};

class RSBackgroundFilterDrawable : public RSFilterDrawable {
public:
    RSBackgroundFilterDrawable() = default;
    ~RSBackgroundFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void RemovePixelStretch();
    bool FuzePixelStretch(const RSRenderNode& node);
};

class RSBackgroundEffectDrawable : public RSFilterDrawable {
public:
    RSBackgroundEffectDrawable() = default;
    ~RSBackgroundEffectDrawable() override = default;

    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;
};

class RSUseEffectDrawable : public RSDrawable {
public:
    RSUseEffectDrawable() = default;
    RSUseEffectDrawable(DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr drawable)
        : effectRenderNodeDrawableWeakRef_(drawable)
    {}
    ~RSUseEffectDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override {};
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr effectRenderNodeDrawableWeakRef_;
};

class RSDynamicLightUpDrawable : public RSDrawable {
public:
    explicit RSDynamicLightUpDrawable(float dynamicLightUpRate, float dynamicLightUpDeg)
        : dynamicLightUpRate_(dynamicLightUpRate), dynamicLightUpDeg_(dynamicLightUpDeg)
    {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(float rate, float degree, float alpha);

    bool needSync_ = false;
    float dynamicLightUpRate_ = 0.0f;
    float dynamicLightUpDeg_ = 0.0f;
    float stagingDynamicLightUpRate_ = 0.0f;
    float stagingDynamicLightUpDeg_ = 0.0f;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H
