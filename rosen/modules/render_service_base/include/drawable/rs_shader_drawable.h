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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_SHADER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_SHADER_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "drawable/rs_drawable.h"

namespace OHOS::Rosen {
class RSNGRenderShaderBase;
class RSProperties;
class RSRenderNode;
namespace Drawing {
class GEVisualEffectContainer;
}

namespace DrawableV2 {
// Intermediate base class of NG shader drawables (background/foreground/material/overlay/coverage
// NG shader slots). It materializes an RSNGRenderShaderBase property into a
// Drawing::GEVisualEffectContainer during OnSync, and draws the container via GERender on RT.
// Thread contract (see RSDrawable): OnUpdate only writes staging* members on UI thread, OnSync
// publishes staging* members to RT members, OnDraw only reads RT members on render thread.
class RSShaderDrawable : public RSDrawable {
public:
    ~RSShaderDrawable() override = default;

    // Template method: takes the slot shader via GetShader and stages the common state (shader,
    // corner radius, node id, EDR pair, SDF shape binding and the effect draw rect). Returns
    // false to erase the drawable when the slot shader is absent.
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

    bool GetEnableEDR() const override
    {
        return stagingEnableEDREffect_;
    }

protected:
    RSShaderDrawable() = default;

    // Slot specific shader source, return nullptr to erase the drawable.
    virtual std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const = 0;
    // Dfx hook called at the head of the base OnDraw, each derived slot prints its own trace
    // (drawable name, node id, draw rect and corner radius).
    virtual void DfxOnDraw() const = 0;
    // Update frosted glass blur cache and handle DRM bypass, returns false when the DRM path has
    // finished drawing and the caller should return immediately.
    bool PrepareFrostedGlassEffect(Drawing::Canvas* canvas, const Drawing::Rect& drawRect) const;

    // UI (staging) members, written by derived OnUpdate on UI thread
    bool needSync_ = false;
    std::shared_ptr<RSNGRenderShaderBase> stagingShader_;
    float stagingCornerRadius_ = 0.f;
    RectF stagingDrawRect_;
    NodeId stagingNodeId_ = INVALID_NODEID;
    bool stagingEnableEDREffect_ = false;
    NodeId stagingScreenNodeId_ = INVALID_NODEID;

    // RT members, published by OnSync and only read by OnDraw on render thread
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer_;
    float cornerRadius_ = 0.f;
    RectF drawRect_;
    NodeId nodeId_ = INVALID_NODEID;
    bool enableEDREffect_ = false;
    NodeId screenNodeId_ = INVALID_NODEID;
};

// MATERIAL_SHADER slot.
class RSMaterialShaderDrawable : public RSShaderDrawable {
public:
    RSMaterialShaderDrawable() = default;
    ~RSMaterialShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;
};

// BACKGROUND_NG_SHADER slot.
class RSBackgroundNGShaderDrawable : public RSShaderDrawable {
public:
    RSBackgroundNGShaderDrawable() = default;
    ~RSBackgroundNGShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;
};

// FOREGROUND_SHADER slot.
class RSForegroundShaderDrawable : public RSShaderDrawable {
public:
    RSForegroundShaderDrawable() = default;
    ~RSForegroundShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;
};

// OVERLAY_NG_SHADER slot.
class RSOverlayNGShaderDrawable : public RSShaderDrawable {
public:
    RSOverlayNGShaderDrawable() = default;
    ~RSOverlayNGShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_SHADER_DRAWABLE_H
