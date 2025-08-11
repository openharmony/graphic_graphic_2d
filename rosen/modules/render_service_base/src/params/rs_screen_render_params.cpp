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

#include "params/rs_screen_render_params.h"

#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
using RSRenderNodeDrawableAdapterSharedPtr = DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr;
RSScreenRenderParams::RSScreenRenderParams(NodeId id) : RSRenderParams(id) {}

std::vector<RSBaseRenderNode::SharedPtr>& RSScreenRenderParams::GetAllMainAndLeashSurfaces()
{
    return allMainAndLeashSurfaces_;
}

std::vector<RSRenderNodeDrawableAdapterSharedPtr>& RSScreenRenderParams::GetAllMainAndLeashSurfaceDrawables()
{
    return allMainAndLeashSurfaceDrawables_;
}

void RSScreenRenderParams::SetAllMainAndLeashSurfaces(
    std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces)
{
    std::swap(allMainAndLeashSurfaces_, allMainAndLeashSurfaces);
}

void RSScreenRenderParams::SetAllMainAndLeashSurfaceDrawables(
    std::vector<RSRenderNodeDrawableAdapterSharedPtr>& allMainAndLeashSurfaceDrawables)
{
    std::swap(allMainAndLeashSurfaceDrawables_, allMainAndLeashSurfaceDrawables);
}

void RSScreenRenderParams::SetMainAndLeashSurfaceDirty(bool isDirty)
{
    if (isMainAndLeashSurfaceDirty_ == isDirty) {
        return;
    }
    isMainAndLeashSurfaceDirty_ = isDirty;
    needSync_ = true;
}

bool RSScreenRenderParams::GetMainAndLeashSurfaceDirty() const
{
    return isMainAndLeashSurfaceDirty_;
}

void RSScreenRenderParams::SetNeedForceUpdateHwcNodes(bool needForceUpdateHwcNodes)
{
    if (needForceUpdateHwcNodes_ == needForceUpdateHwcNodes) {
        return;
    }
    needForceUpdateHwcNodes_ = needForceUpdateHwcNodes;
    needSync_ = true;
}

bool RSScreenRenderParams::GetNeedForceUpdateHwcNodes() const
{
    return needForceUpdateHwcNodes_;
}

void RSScreenRenderParams::SetGlobalZOrder(float zOrder)
{
    zOrder_ = zOrder;
    needSync_ = true;
}

float RSScreenRenderParams::GetGlobalZOrder() const
{
    return zOrder_;
}

void RSScreenRenderParams::SetFingerprint(bool hasFingerprint)
{
    if (hasFingerprint_ == hasFingerprint) {
        return;
    }
    hasFingerprint_ = hasFingerprint;
    needSync_ = true;
}

bool RSScreenRenderParams::GetFingerprint()
{
    return hasFingerprint_;
}

void RSScreenRenderParams::SetFixVirtualBuffer10Bit(bool isFixVirtualBuffer10Bit)
{
    if (isFixVirtualBuffer10Bit_ == isFixVirtualBuffer10Bit) {
        return;
    }
    isFixVirtualBuffer10Bit_ = isFixVirtualBuffer10Bit;
    needSync_ = true;
}

bool RSScreenRenderParams::GetFixVirtualBuffer10Bit() const
{
    return isFixVirtualBuffer10Bit_;
}

void RSScreenRenderParams::SetExistHWCNode(bool existHWCNode)
{
    if (existHWCNode_ == existHWCNode) {
        return;
    }
    existHWCNode_ = existHWCNode;
    needSync_ = true;
}

bool RSScreenRenderParams::GetExistHWCNode() const
{
    return existHWCNode_;
}

void RSScreenRenderParams::SetHDRPresent(bool hasHdrPresent)
{
    if (hasHdrPresent_ == hasHdrPresent) {
        return;
    }
    hasHdrPresent_ = hasHdrPresent;
    needSync_ = true;
}

bool RSScreenRenderParams::GetHDRPresent() const
{
    return hasHdrPresent_;
}

void RSScreenRenderParams::SetHDRStatusChanged(bool isHDRStatusChanged)
{
    if (isHDRStatusChanged_ == isHDRStatusChanged) {
        return;
    }
    isHDRStatusChanged_ = isHDRStatusChanged;
    needSync_ = true;
}

bool RSScreenRenderParams::IsHDRStatusChanged() const
{
    return isHDRStatusChanged_;
}

void RSScreenRenderParams::SetBrightnessRatio(float brightnessRatio)
{
    if (ROSEN_EQ(brightnessRatio_, brightnessRatio)) {
        return;
    }
    brightnessRatio_ = brightnessRatio;
    needSync_ = true;
}

float RSScreenRenderParams::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSScreenRenderParams::SetNewColorSpace(const GraphicColorGamut& newColorSpace)
{
    if (newColorSpace_ == newColorSpace) {
        return;
    }
    needSync_ = true;
    newColorSpace_ = newColorSpace;
}

GraphicColorGamut RSScreenRenderParams::GetNewColorSpace() const
{
    return newColorSpace_;
}

void RSScreenRenderParams::SetNewPixelFormat(const GraphicPixelFormat& newPixelFormat)
{
    if (newPixelFormat_ == newPixelFormat) {
        return;
    }
    needSync_ = true;
    newPixelFormat_ = newPixelFormat;
}

GraphicPixelFormat RSScreenRenderParams::GetNewPixelFormat() const
{
    return newPixelFormat_;
}

void RSScreenRenderParams::SetZoomed(bool isZoomed)
{
    if (isZoomed_ == isZoomed) {
        return;
    }
    needSync_ = true;
    isZoomed_ = isZoomed;
}

bool RSScreenRenderParams::GetZoomed() const
{
    return isZoomed_;
}

void RSScreenRenderParams::SetHasMirrorScreen(bool hasMirrorScreen)
{
    if (hasMirrorScreen) {
        mirrorDstCount_++;
    } else {
        mirrorDstCount_--;
    }
    bool ret = (mirrorDstCount_ != 0);
    if (hasMirrorScreen_ == ret) {
        return;
    }
    needSync_ = true;
    hasMirrorScreen_ = hasMirrorScreen;
}

bool RSScreenRenderParams::HasMirrorScreen() const
{
    return hasMirrorScreen_;
}

void RSScreenRenderParams::SetTargetSurfaceRenderNodeDrawable(
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable)
{
    if (ROSEN_EQ(targetSurfaceRenderNodeDrawable_, drawable)) {
        return;
    }
    targetSurfaceRenderNodeDrawable_ = drawable;
    needSync_ = true;
}

DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSScreenRenderParams::GetTargetSurfaceRenderNodeDrawable() const
{
    return targetSurfaceRenderNodeDrawable_;
}

void RSScreenRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetScreenParams = static_cast<RSScreenRenderParams*>(target.get());
    if (targetScreenParams == nullptr) {
        RS_LOGE("RSScreenRenderParams::OnSync targetScreenParams is nullptr");
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
    targetScreenParams->allMainAndLeashSurfaceDrawables_ = allMainAndLeashSurfaceDrawables_;
    targetScreenParams->hasChildCrossNode_ = hasChildCrossNode_;
    targetScreenParams->isFirstVisitCrossNodeDisplay_ = isFirstVisitCrossNodeDisplay_;
    targetScreenParams->compositeType_ = compositeType_;
    targetScreenParams->mirrorSourceDrawable_ = mirrorSourceDrawable_;
    targetScreenParams->isMirrorScreen_ = isMirrorScreen_;
    targetScreenParams->screenInfo_ = std::move(screenInfo_);
    targetScreenParams->isMainAndLeashSurfaceDirty_ = isMainAndLeashSurfaceDirty_;
    targetScreenParams->hasFingerprint_ = hasFingerprint_;
    targetScreenParams->newColorSpace_ = newColorSpace_;
    targetScreenParams->newPixelFormat_ = newPixelFormat_;
    targetScreenParams->hasHdrPresent_ = hasHdrPresent_;
    targetScreenParams->isHDRStatusChanged_ = isHDRStatusChanged_;
    targetScreenParams->brightnessRatio_ = brightnessRatio_;
    targetScreenParams->isFixVirtualBuffer10Bit_ = isFixVirtualBuffer10Bit_;
    targetScreenParams->existHWCNode_ = existHWCNode_;
    targetScreenParams->zOrder_ = zOrder_;
    targetScreenParams->isZoomed_ = isZoomed_;
    targetScreenParams->hasMirrorScreen_ = hasMirrorScreen_;
    targetScreenParams->targetSurfaceRenderNodeDrawable_ = targetSurfaceRenderNodeDrawable_;
    targetScreenParams->roundCornerSurfaceDrawables_ = roundCornerSurfaceDrawables_;
    targetScreenParams->needForceUpdateHwcNodes_ = needForceUpdateHwcNodes_;
    targetScreenParams->childDisplayCount_ =  childDisplayCount_;
    targetScreenParams->logicalDisplayNodeDrawables_ =  std::move(logicalDisplayNodeDrawables_);
    targetScreenParams->forceFreeze_ = forceFreeze_;

    RSRenderParams::OnSync(target);
}

std::string RSScreenRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + ", RSScreenRenderParams: {";
    ret += RENDER_BASIC_PARAM_TO_STRING(screenInfo_.id);
    ret += RENDER_BASIC_PARAM_TO_STRING(static_cast<uint32_t>(compositeType_));
    ret += RENDER_BASIC_PARAM_TO_STRING(allMainAndLeashSurfaces_.size());
    ret += RENDER_BASIC_PARAM_TO_STRING(allMainAndLeashSurfaceDrawables_.size());
    ret += RENDER_PARAM_TO_STRING(screenInfo_);
    ret += "}";
    return ret;
}

DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSScreenRenderParams::GetMirrorSourceDrawable()
{
    return mirrorSourceDrawable_;
}

void RSScreenRenderParams::SetDrawnRegion(const Occlusion::Region& region)
{
    drawnRegion_ = region;
}

const Occlusion::Region& RSScreenRenderParams::GetDrawnRegion() const
{
    return drawnRegion_;
}

void RSScreenRenderParams::SetForceFreeze(bool forceFreeze)
{
    if (forceFreeze_ == forceFreeze) {
        return;
    }
    forceFreeze_ = forceFreeze;
    needSync_ = true;
}

bool RSScreenRenderParams::GetForceFreeze() const
{
    return forceFreeze_ && RSSystemProperties::GetSupportScreenFreezeEnabled();
}

} // namespace OHOS::Rosen
