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

#include "params/rs_display_render_params.h"

#include "common/rs_occlusion_region.h"
#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
using RSRenderNodeDrawableAdapterSharedPtr = DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr;
RSDisplayRenderParams::RSDisplayRenderParams(NodeId id) : RSRenderParams(id) {}

std::vector<RSBaseRenderNode::SharedPtr>& RSDisplayRenderParams::GetAllMainAndLeashSurfaces()
{
    return allMainAndLeashSurfaces_;
}

std::vector<RSRenderNodeDrawableAdapterSharedPtr>& RSDisplayRenderParams::GetAllMainAndLeashSurfaceDrawables()
{
    return allMainAndLeashSurfaceDrawables_;
}

void RSDisplayRenderParams::SetAllMainAndLeashSurfaces(
    std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces)
{
    std::swap(allMainAndLeashSurfaces_, allMainAndLeashSurfaces);
}

void RSDisplayRenderParams::SetAllMainAndLeashSurfaceDrawables(
    std::vector<RSRenderNodeDrawableAdapterSharedPtr>& allMainAndLeashSurfaceDrawables)
{
    std::swap(allMainAndLeashSurfaceDrawables_, allMainAndLeashSurfaceDrawables);
}

void RSDisplayRenderParams::SetMainAndLeashSurfaceDirty(bool isDirty)
{
    if (isMainAndLeashSurfaceDirty_ == isDirty) {
        return;
    }
    isMainAndLeashSurfaceDirty_ = isDirty;
    needSync_ = true;
}

bool RSDisplayRenderParams::GetMainAndLeashSurfaceDirty() const
{
    return isMainAndLeashSurfaceDirty_;
}

void RSDisplayRenderParams::SetRotationChanged(bool changed)
{
    if (isRotationChanged_ == changed) {
        return;
    }
    isRotationChanged_ = changed;
    needSync_ = true;
}

void RSDisplayRenderParams::SetGlobalZOrder(float zOrder)
{
    zOrder_ = zOrder;
    needSync_ = true;
}

float RSDisplayRenderParams::GetGlobalZOrder() const
{
    return zOrder_;
}

bool RSDisplayRenderParams::IsRotationChanged() const
{
    return isRotationChanged_;
}

void RSDisplayRenderParams::SetHDRPresent(bool hasHdrPresent)
{
    if (hasHdrPresent_ == hasHdrPresent) {
        return;
    }
    hasHdrPresent_ = hasHdrPresent;
    needSync_ = true;
}

bool RSDisplayRenderParams::GetHDRPresent() const
{
    return hasHdrPresent_;
}

void RSDisplayRenderParams::SetBrightnessRatio(float brightnessRatio)
{
    if (ROSEN_EQ(brightnessRatio_, brightnessRatio)) {
        return;
    }
    brightnessRatio_ = brightnessRatio;
    needSync_ = true;
}

float RSDisplayRenderParams::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSDisplayRenderParams::SetNewColorSpace(const GraphicColorGamut& newColorSpace)
{
    if (newColorSpace_ == newColorSpace) {
        return;
    }
    needSync_ = true;
    newColorSpace_ = newColorSpace;
}

GraphicColorGamut RSDisplayRenderParams::GetNewColorSpace() const
{
    return newColorSpace_;
}

void RSDisplayRenderParams::SetNewPixelFormat(const GraphicPixelFormat& newPixelFormat)
{
    if (newPixelFormat_ == newPixelFormat) {
        return;
    }
    needSync_ = true;
    newPixelFormat_ = newPixelFormat;
}

GraphicPixelFormat RSDisplayRenderParams::GetNewPixelFormat() const
{
    return newPixelFormat_;
}

void RSDisplayRenderParams::SetZoomed(bool isZoomed)
{
    if (isZoomed_ == isZoomed) {
        return;
    }
    needSync_ = true;
    isZoomed_ = isZoomed;
}

bool RSDisplayRenderParams::GetZoomed() const
{
    return isZoomed_;
}

void RSDisplayRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetDisplayParams = static_cast<RSDisplayRenderParams*>(target.get());
    if (targetDisplayParams == nullptr) {
        RS_LOGE("RSDisplayRenderParams::OnSync targetDisplayParams is nullptr");
        return;
    }
    allMainAndLeashSurfaceDrawables_.clear();
    for (auto& surfaceNode : allMainAndLeashSurfaces_) {
        auto ptr = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(surfaceNode->GetId());
        if (ptr == nullptr || ptr->GetNodeType() != RSRenderNodeType::SURFACE_NODE) {
            continue;
        }
        allMainAndLeashSurfaceDrawables_.push_back(ptr);
    }
    targetDisplayParams->allMainAndLeashSurfaceDrawables_ = allMainAndLeashSurfaceDrawables_;
    targetDisplayParams->displayHasSecSurface_ = displayHasSecSurface_;
    targetDisplayParams->displayHasSkipSurface_ = displayHasSkipSurface_;
    targetDisplayParams->displayHasSnapshotSkipSurface_ = displayHasSnapshotSkipSurface_;
    targetDisplayParams->displayHasProtectedSurface_ = displayHasProtectedSurface_;
    targetDisplayParams->displaySpecailSurfaceChanged_ = displaySpecailSurfaceChanged_;
    targetDisplayParams->hasCaptureWindow_ = hasCaptureWindow_;
    targetDisplayParams->offsetX_ = offsetX_;
    targetDisplayParams->offsetY_ = offsetY_;
    targetDisplayParams->nodeRotation_ = nodeRotation_;
    targetDisplayParams->screenRotation_ = screenRotation_;
    targetDisplayParams->screenId_ = screenId_;
    targetDisplayParams->isSecurityDisplay_ = isSecurityDisplay_;
    targetDisplayParams->isSecurityExemption_ = isSecurityExemption_;
    targetDisplayParams->mirroredId_ = mirroredId_;
    targetDisplayParams->compositeType_ = compositeType_;
    targetDisplayParams->isMirrorScreen_ = isMirrorScreen_;
    targetDisplayParams->mirrorSourceDrawable_ = mirrorSourceDrawable_;
    targetDisplayParams->mirrorSourceId_ = mirrorSourceId_;
    targetDisplayParams->screenInfo_ = std::move(screenInfo_);
    targetDisplayParams->isMainAndLeashSurfaceDirty_ = isMainAndLeashSurfaceDirty_;
    targetDisplayParams->needOffscreen_ = needOffscreen_;
    targetDisplayParams->isRotationChanged_ = isRotationChanged_;
    targetDisplayParams->newColorSpace_ = newColorSpace_;
    targetDisplayParams->newPixelFormat_ = newPixelFormat_;
    targetDisplayParams->hasHdrPresent_ = hasHdrPresent_;
    targetDisplayParams->brightnessRatio_ = brightnessRatio_;
    targetDisplayParams->zOrder_ = zOrder_;
    targetDisplayParams->isZoomed_ = isZoomed_;
    RSRenderParams::OnSync(target);
}

std::string RSDisplayRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + ", RSDisplayRenderParams: {";
    ret += RENDER_BASIC_PARAM_TO_STRING(offsetX_);
    ret += RENDER_BASIC_PARAM_TO_STRING(offsetY_);
    ret += RENDER_BASIC_PARAM_TO_STRING(int(nodeRotation_));
    ret += RENDER_BASIC_PARAM_TO_STRING(int(screenRotation_));
    ret += RENDER_BASIC_PARAM_TO_STRING(screenId_);
    ret += RENDER_BASIC_PARAM_TO_STRING(mirroredId_);
    ret += RENDER_BASIC_PARAM_TO_STRING(compositeType_);
    ret += RENDER_BASIC_PARAM_TO_STRING(allMainAndLeashSurfaces_.size());
    ret += RENDER_BASIC_PARAM_TO_STRING(allMainAndLeashSurfaceDrawables_.size());
    ret += RENDER_PARAM_TO_STRING(screenInfo_);
    ret += "}";
    return ret;
}

DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSDisplayRenderParams::GetMirrorSourceDrawable()
{
    return mirrorSourceDrawable_;
}

bool RSDisplayRenderParams::HasSecurityLayer() const
{
    bool hasSecLayerFlag = false;
    auto iter = displayHasSecSurface_.find(screenId_);
    if (iter != displayHasSecSurface_.end()) {
        hasSecLayerFlag = iter->second;
    }
    return hasSecLayerFlag;
}

bool RSDisplayRenderParams::HasSkipLayer() const
{
    bool hasSkipLayerFlag = false;
    auto iter = displayHasSkipSurface_.find(screenId_);
    if (iter != displayHasSkipSurface_.end()) {
        hasSkipLayerFlag = iter->second;
    }
    return hasSkipLayerFlag;
}

bool RSDisplayRenderParams::HasSnapshotSkipLayer() const
{
    bool hasSnapshotSkipLayerFlag = false;
    auto iter = displayHasSnapshotSkipSurface_.find(screenId_);
    if (iter != displayHasSnapshotSkipSurface_.end()) {
        hasSnapshotSkipLayerFlag = iter->second;
    }
    return hasSnapshotSkipLayerFlag;
}

bool RSDisplayRenderParams::HasProtectedLayer() const
{
    bool hasProtectedLayerFlag = false;
    auto iter = displayHasProtectedSurface_.find(screenId_);
    if (iter != displayHasProtectedSurface_.end()) {
        hasProtectedLayerFlag = iter->second;
    }
    return hasProtectedLayerFlag;
}

bool RSDisplayRenderParams::HasCaptureWindow() const
{
    bool hasCaptureWindow = false;
    auto iter = hasCaptureWindow_.find(screenId_);
    if (iter != hasCaptureWindow_.end()) {
        hasCaptureWindow = iter->second;
    }
    return hasCaptureWindow;
}

void RSDisplayRenderParams::SetNeedOffscreen(bool needOffscreen)
{
    if (needOffscreen_ == needOffscreen) {
        return;
    }
    needOffscreen_ = needOffscreen;
    needSync_ = true;
}

bool RSDisplayRenderParams::GetNeedOffscreen() const
{
    return needOffscreen_;
}

void RSDisplayRenderParams::SetDrawnRegion(const Occlusion::Region& region)
{
    drawnRegion_ = region;
}

const Occlusion::Region& RSDisplayRenderParams::GetDrawnRegion() const
{
    return drawnRegion_;
}
} // namespace OHOS::Rosen
