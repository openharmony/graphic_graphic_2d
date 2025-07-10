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

#include "rs_trace.h"

#include "params/rs_logical_display_render_params.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSLogicalDisplayRenderParams::RSLogicalDisplayRenderParams(NodeId nodeId)
    : RSRenderParams(nodeId) {}

void RSLogicalDisplayRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetLogicalDisplayRenderParam = static_cast<RSLogicalDisplayRenderParams*>(target.get());
    if (targetLogicalDisplayRenderParam == nullptr) {
        RS_LOGE("targetLogicalDisplayRenderParam::OnSync targetLogicalDisplayRenderParam is null");
        return;
    }
    targetLogicalDisplayRenderParam->screenRotation_ = screenRotation_;
    targetLogicalDisplayRenderParam->screenId_ = screenId_;
    targetLogicalDisplayRenderParam->needOffscreen_ = needOffscreen_;
    targetLogicalDisplayRenderParam->nodeRotation_ = nodeRotation_;
    targetLogicalDisplayRenderParam->compositeType_ = compositeType_;
    targetLogicalDisplayRenderParam->isRotationChanged_ = isRotationChanged_;
    targetLogicalDisplayRenderParam->isMirrorDisplay_ = isMirrorDisplay_;
    targetLogicalDisplayRenderParam->mirrorSourceDrawable_ = mirrorSourceDrawable_;
    targetLogicalDisplayRenderParam->virtualScreenMuteStatus_ = virtualScreenMuteStatus_;
    targetLogicalDisplayRenderParam->isSecurityDisplay_ = isSecurityDisplay_;
    targetLogicalDisplayRenderParam->specialLayerManager_ = specialLayerManager_;
    targetLogicalDisplayRenderParam->displaySpecialSurfaceChanged_ = displaySpecialSurfaceChanged_;
    targetLogicalDisplayRenderParam->isSecurityExemption_ = isSecurityExemption_;
    targetLogicalDisplayRenderParam->hasSecLayerInVisibleRect_ = hasSecLayerInVisibleRect_;
    targetLogicalDisplayRenderParam->hasSecLayerInVisibleRectChanged_ = hasSecLayerInVisibleRectChanged_;
    targetLogicalDisplayRenderParam->ancestorScreenDrawable_ = ancestorScreenDrawable_;
    targetLogicalDisplayRenderParam->hasCaptureWindow_ = hasCaptureWindow_;

    RSRenderParams::OnSync(target);
}

std::string RSLogicalDisplayRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + ", RSLogicalDisplayRenderParams: {";
    ret += RENDER_BASIC_PARAM_TO_STRING(static_cast<int>(screenId_));
    ret += RENDER_BASIC_PARAM_TO_STRING(static_cast<int>(screenRotation_));
    ret += RENDER_BASIC_PARAM_TO_STRING(int(nodeRotation_));
    ret += "}";
    return ret;
}

ScreenRotation RSLogicalDisplayRenderParams::GetScreenRotation() const
{
    return screenRotation_;
}

ScreenRotation RSLogicalDisplayRenderParams::GetNodeRotation() const
{
    return nodeRotation_;
}

void RSLogicalDisplayRenderParams::SetScreenRotation(ScreenRotation screenRotation)
{
    if (screenRotation_ == screenRotation) {
        return;
    }
    screenRotation_ = screenRotation;
    needSync_ = true;
}

uint64_t RSLogicalDisplayRenderParams::GetScreenId() const
{
    return screenId_;
}

void RSLogicalDisplayRenderParams::SetScreenId(uint64_t screenId)
{
    if (screenId_ == screenId) {
        return;
    }
    screenId_ = screenId;
    needSync_ = true;
}

void RSLogicalDisplayRenderParams::SetNeedOffscreen(bool needOffscreen)
{
    if (needOffscreen_ == needOffscreen) {
        return;
    }
    needOffscreen_ = needOffscreen;
    needSync_ = true;
}

bool RSLogicalDisplayRenderParams::GetNeedOffscreen() const
{
    return needOffscreen_;
}

void RSLogicalDisplayRenderParams::SetRotationChanged(bool changed)
{
    if (isRotationChanged_ == changed) {
        return;
    }
    isRotationChanged_ = changed;
    needSync_ = true;
}

bool RSLogicalDisplayRenderParams::IsRotationChanged() const
{
    return isRotationChanged_;
}

bool RSLogicalDisplayRenderParams::IsMirrorDisplay() const
{
    return isMirrorDisplay_;
}

DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSLogicalDisplayRenderParams::GetMirrorSourceDrawable()
{
    return mirrorSourceDrawable_;
}

bool RSLogicalDisplayRenderParams::GetVirtualScreenMuteStatus() const
{
    return virtualScreenMuteStatus_;
}

bool RSLogicalDisplayRenderParams::IsSecurityDisplay() const
{
    return isSecurityDisplay_;
}

bool RSLogicalDisplayRenderParams::HasCaptureWindow() const
{
    return hasCaptureWindow_;
}
} // namespace OHOS::Rosen
