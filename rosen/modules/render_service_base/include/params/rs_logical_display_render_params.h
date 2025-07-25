/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_LOGICAL_DISPLAY_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_LOGICAL_DISPLAY_RENDER_PARAMS_H

#include <memory>
#include <string>

#include "common/rs_macros.h"
#include "params/rs_render_params.h"
#include "params/rs_screen_render_params.h"
namespace OHOS::Rosen {
class RSB_EXPORT RSLogicalDisplayRenderParams : public RSRenderParams {
public:
    explicit RSLogicalDisplayRenderParams(NodeId nodeId);
    ~RSLogicalDisplayRenderParams() override = default;

    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    std::string ToString() const override;

    void SetScreenRotation(ScreenRotation screenRotation);
    ScreenRotation GetScreenRotation() const override;
    ScreenRotation GetNodeRotation() const;
    void SetRotationChanged(bool changed) override;
    bool IsRotationChanged() const override;

    void SetScreenId(uint64_t screenId);
    uint64_t GetScreenId() const;

    void SetNeedOffscreen(bool needOffscreen);
    bool GetNeedOffscreen() const;

    bool IsMirrorDisplay() const;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetMirrorSourceDrawable() override;
    bool GetVirtualScreenMuteStatus() const;
    bool IsSecurityDisplay() const;
    bool HasCaptureWindow() const;
    CompositeType GetCompositeType() const
    {
        return compositeType_;
    }
    bool GetSecurityExemption() const
    {
        return isSecurityExemption_;
    }

    void SetSecurityExemption(bool isSecurityExemption)
    {
        isSecurityExemption_ = isSecurityExemption;
    }

    const RSSpecialLayerManager& GetSpecialLayerMgr() const
    {
        return specialLayerManager_;
    }

    bool HasSecLayerInVisibleRect() const
    {
        return hasSecLayerInVisibleRect_;
    }

    bool IsSpecialLayerChanged() const
    {
        return displaySpecialSurfaceChanged_;
    }

    bool HasSecLayerInVisibleRectChanged() const
    {
        return hasSecLayerInVisibleRectChanged_;
    }

    void SetAncestorScreenDrawable(const DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr& ancestorScreenDrawable)
    {
        ancestorScreenDrawable_ = ancestorScreenDrawable;
    }

    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetAncestorScreenDrawable() const
    {
        return ancestorScreenDrawable_;
    }

    float GetOffsetX() const
    {
        return offsetX_;
    }

    float GetOffsetY() const
    {
        return offsetY_;
    }

private:
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr ancestorScreenDrawable_;
    bool hasSecLayerInVisibleRect_ = false;
    RSSpecialLayerManager specialLayerManager_;
    bool isSecurityExemption_ = false;
    uint64_t screenId_ = INVALID_SCREEN_ID;
    bool needOffscreen_ = false;
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;
    ScreenRotation nodeRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    bool isRotationChanged_ = false;

    bool isMirrorDisplay_ = false;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr mirrorSourceDrawable_;
    bool virtualScreenMuteStatus_ = false;
    bool isSecurityDisplay_ = false;
    bool hasCaptureWindow_ = false;
    CompositeType compositeType_ = CompositeType::HARDWARE_COMPOSITE;

    bool displaySpecialSurfaceChanged_ = false;
    bool hasSecLayerInVisibleRectChanged_ = false;

    float offsetX_ = 0.f;
    float offsetY_ = 0.f;

    friend class RSLogicalDisplayRenderNode;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_LOGICAL_DISPLAY_RENDER_PARAMS_H
