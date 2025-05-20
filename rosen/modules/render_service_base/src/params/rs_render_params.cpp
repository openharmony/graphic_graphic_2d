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

#include "params/rs_render_params.h"

#include <string>

#include "params/rs_surface_render_params.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/rs_screen_info.h"
namespace OHOS::Rosen {
namespace {
thread_local Drawing::Matrix parentSurfaceMatrix_;
}
void RSRenderParams::SetDirtyType(RSRenderParamsDirtyType dirtyType)
{
    dirtyType_.set(dirtyType);
}

void RSRenderParams::SetAlpha(float alpha)
{
    if (alpha_ == alpha) {
        return;
    }
    alpha_ = alpha;
    needSync_ = true;
}
float RSRenderParams::GetAlpha() const
{
    return alpha_;
}

void RSRenderParams::SetAlphaOffScreen(bool alphaOffScreen)
{
    if (alphaOffScreen_ == alphaOffScreen) {
        return;
    }
    alphaOffScreen_ = alphaOffScreen;
    needSync_ = true;
}

bool RSRenderParams::GetAlphaOffScreen() const
{
    return alphaOffScreen_;
}

void RSRenderParams::SetMatrix(const Drawing::Matrix& matrix)
{
    matrix_ = matrix;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::MATRIX_DIRTY);
}

const Drawing::Matrix& RSRenderParams::GetMatrix() const
{
    return matrix_;
}

bool RSRenderParams::HasUnobscuredUEC() const
{
    return hasUnobscuredUEC_;
}

void RSRenderParams::SetHasUnobscuredUEC(bool flag)
{
    hasUnobscuredUEC_ = flag;
}

void RSRenderParams::ApplyAlphaAndMatrixToCanvas(RSPaintFilterCanvas& canvas, bool applyMatrix) const
{
    if (UNLIKELY(HasSandBox())) {
        if (applyMatrix) {
            canvas.SetMatrix(parentSurfaceMatrix_);
            canvas.ConcatMatrix(matrix_);
        }
        canvas.SetAlpha(alpha_);
    } else {
        if (applyMatrix) {
            canvas.ConcatMatrix(matrix_);
        }
        if (alpha_ < 1.0f && (drawingCacheType_ == RSDrawingCacheType::FORCED_CACHE || alphaOffScreen_)) {
            auto rect = RSPropertiesPainter::Rect2DrawingRect(GetLocalDrawRect());
            Drawing::Brush brush;
            brush.SetAlpha(static_cast<uint32_t>(std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX));
            Drawing::SaveLayerOps slr(&rect, &brush);
            canvas.SaveLayer(slr);
        } else {
            canvas.MultiplyAlpha(alpha_);
        }
    }
}

void RSRenderParams::SetBoundsRect(const Drawing::RectF& boundsRect)
{
    if (boundsRect_ == boundsRect) {
        return;
    }
    boundsRect_ = boundsRect;
    needSync_ = true;
}

const Drawing::Rect& RSRenderParams::GetBounds() const
{
    return boundsRect_;
}

void RSRenderParams::SetFrameRect(const Drawing::RectF& frameRect)
{
    if (frameRect_ == frameRect) {
        return;
    }

    frameRect_ = frameRect;
    needSync_ = true;
}

const Drawing::Rect& RSRenderParams::GetFrameRect() const
{
    return frameRect_;
}

bool RSRenderParams::SetLocalDrawRect(const RectF& localDrawRect)
{
    if (localDrawRect_.IsNearEqual(localDrawRect)) {
        return false;
    }
    localDrawRect_ = localDrawRect;
    needSync_ = true;
    return true;
}

const RectF& RSRenderParams::GetLocalDrawRect() const
{
    return localDrawRect_;
}

void RSRenderParams::SetHasSandBox(bool hasSandbox)
{
    if (hasSandBox_ == hasSandbox) {
        return;
    }
    hasSandBox_ = hasSandbox;
    needSync_ = true;
}

bool RSRenderParams::HasSandBox() const
{
    return hasSandBox_;
}

void RSRenderParams::SetShouldPaint(bool shouldPaint)
{
    if (shouldPaint_ == shouldPaint) {
        return;
    }
    shouldPaint_ = shouldPaint;
    needSync_ = true;
}

void RSRenderParams::SetContentEmpty(bool empty)
{
    if (contentEmpty_ == empty) {
        return;
    }
    contentEmpty_ = empty;
    needSync_ = true;
}

bool RSRenderParams::GetShouldPaint() const
{
    return shouldPaint_ && !contentEmpty_;
}

void RSRenderParams::SetChildHasVisibleFilter(bool val)
{
    if (childHasVisibleFilter_ == val) {
        return;
    }
    childHasVisibleFilter_ = val;
    needSync_ = true;
}

bool RSRenderParams::ChildHasVisibleFilter() const
{
    return childHasVisibleFilter_;
}

void RSRenderParams::SetChildHasVisibleEffect(bool val)
{
    if (childHasVisibleEffect_ == val) {
        return;
    }
    childHasVisibleEffect_ = val;
    needSync_ = true;
}

bool RSRenderParams::ChildHasVisibleEffect() const
{
    return childHasVisibleEffect_;
}

void RSRenderParams::SetCacheSize(Vector2f size)
{
    if (cacheSize_ == size) {
        return;
    }
    cacheSize_ = size;
    needSync_ = true;
}

void RSRenderParams::SetDrawingCacheChanged(bool isChanged, bool lastFrameSynced)
{
    if (lastFrameSynced) {
        if (isDrawingCacheChanged_) { // force sync if cache changed
            needSync_ = true;
        }
        if (isDrawingCacheChanged_ == isChanged) {
            return;
        }
        isDrawingCacheChanged_ = isChanged;
        needSync_ = true;
    } else {
        needSync_ = needSync_ || (isDrawingCacheChanged_ || (isDrawingCacheChanged_ != isChanged));
        isDrawingCacheChanged_ = isDrawingCacheChanged_ || isChanged;
    }
}

bool RSRenderParams::GetDrawingCacheChanged() const
{
    return isDrawingCacheChanged_;
}

void RSRenderParams::SetNeedUpdateCache(bool needUpdateCache)
{
    isNeedUpdateCache_ = needUpdateCache;
}

bool RSRenderParams::GetNeedUpdateCache() const
{
    return isNeedUpdateCache_;
}

void RSRenderParams::SetDrawingCacheType(RSDrawingCacheType cacheType)
{
    if (drawingCacheType_ == cacheType) {
        return;
    }
    dirtyType_.set(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);
    drawingCacheType_ = cacheType;
    needSync_ = true;
}

RSDrawingCacheType RSRenderParams::GetDrawingCacheType() const
{
    return drawingCacheType_;
}

void RSRenderParams::SetDrawingCacheIncludeProperty(bool includeProperty)
{
    if (drawingCacheIncludeProperty_ == includeProperty) {
        return;
    }
    drawingCacheIncludeProperty_ = includeProperty;
    needSync_ = true;
}

bool RSRenderParams::GetDrawingCacheIncludeProperty() const
{
    return drawingCacheIncludeProperty_;
}

void RSRenderParams::SetRSFreezeFlag(bool freezeFlag)
{
    if (freezeFlag_ == freezeFlag) {
        return;
    }
    freezeFlag_ = freezeFlag;
    needSync_ = true;
}

bool RSRenderParams::GetRSFreezeFlag() const
{
    return freezeFlag_;
}

void RSRenderParams::OpincUpdateRootFlag(bool suggestFlag)
{
    if (isOpincRootFlag_ == suggestFlag) {
        return;
    }
    isOpincRootFlag_ = suggestFlag;
    needSync_ = true;
}

bool RSRenderParams::OpincGetRootFlag() const
{
    return isOpincRootFlag_;
}

void RSRenderParams::OpincSetCacheChangeFlag(bool state, bool lastFrameSynced)
{
    if (lastFrameSynced) {
        isOpincStateChanged_ = state;
        needSync_ = true;
    } else {
        needSync_ = needSync_ || (isOpincStateChanged_ || (isOpincStateChanged_ != state));
        isOpincStateChanged_ = isOpincStateChanged_ || state;
    }
}

bool RSRenderParams::OpincGetCacheChangeState()
{
    bool state = isOpincStateChanged_;
    isOpincStateChanged_ = false;
    return state;
}

void RSRenderParams::SetShadowRect(Drawing::Rect rect)
{
    if (shadowRect_ == rect) {
        return;
    }
    shadowRect_ = rect;
    needSync_ = true;
}

Drawing::Rect RSRenderParams::GetShadowRect() const
{
    return shadowRect_;
}

void RSRenderParams::SetNeedSync(bool needSync)
{
    needSync_ = needSync;
}

void RSRenderParams::SetFrameGravity(Gravity gravity)
{
    if (frameGravity_ == gravity) {
        return;
    }
    frameGravity_ = gravity;
    needSync_ = true;
}

void RSRenderParams::SetHDRBrightness(float hdrBrightness)
{
    if (ROSEN_EQ(hdrBrightness_, hdrBrightness)) {
        return;
    }
    hdrBrightness_ = hdrBrightness;
    needSync_ = true;
}

void RSRenderParams::SetNeedFilter(bool needFilter)
{
    if (needFilter_ == needFilter) {
        return;
    }
    needFilter_ = needFilter;
    needSync_ = true;
}

void RSRenderParams::SetNodeType(RSRenderNodeType type)
{
    if (renderNodeType_ == type) {
        return;
    }
    renderNodeType_ = type;
    needSync_ = true;
}

void RSRenderParams::SetEffectNodeShouldPaint(bool effectNodeShouldPaint)
{
    if (effectNodeShouldPaint_ == effectNodeShouldPaint) {
        return;
    }
    effectNodeShouldPaint_ = effectNodeShouldPaint;
    needSync_ = true;
}

void RSRenderParams::SetHasGlobalCorner(bool hasGlobalCorner)
{
    if (hasGlobalCorner_ == hasGlobalCorner) {
        return;
    }
    hasGlobalCorner_ = hasGlobalCorner;
    needSync_ = true;
}

void RSRenderParams::SetHasBlurFilter(bool hasBlurFilter)
{
    if (hasBlurFilter_ == hasBlurFilter) {
        return;
    }
    hasBlurFilter_ = hasBlurFilter;
    needSync_ = true;
}

void RSRenderParams::SetGlobalAlpha(float alpha)
{
    if (ROSEN_EQ(globalAlpha_, alpha)) {
        return;
    }
    globalAlpha_ = alpha;
    needSync_ = true;
}

bool RSRenderParams::NeedSync() const
{
    return needSync_;
}

void RSRenderParams::OnCanvasDrawingSurfaceChange(const std::unique_ptr<RSRenderParams>& target)
{
    if (!canvasDrawingNodeSurfaceChanged_) {
        return;
    }
    target->canvasDrawingNodeSurfaceChanged_ = true;
    target->surfaceParams_.width = surfaceParams_.width;
    target->surfaceParams_.height = surfaceParams_.height;
    if (GetParamsType() == RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE) {
        return;
    }
    canvasDrawingNodeSurfaceChanged_ = false;
}

bool RSRenderParams::GetCanvasDrawingSurfaceChanged() const
{
    return canvasDrawingNodeSurfaceChanged_;
}

void RSRenderParams::SetCanvasDrawingSurfaceChanged(bool changeFlag)
{
    if (changeFlag) {
        needSync_ = true;
    }
    canvasDrawingNodeSurfaceChanged_ = changeFlag;
}

const std::shared_ptr<RSFilter>& RSRenderParams::GetForegroundFilterCache() const
{
    return foregroundFilterCache_;
}

void RSRenderParams::SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache)
{
    if (foregroundFilterCache_ == foregroundFilterCache) {
        return;
    }
    foregroundFilterCache_ = foregroundFilterCache;
    needSync_ = true;
}

RSRenderParams::SurfaceParam RSRenderParams::GetCanvasDrawingSurfaceParams()
{
    return surfaceParams_;
}

void RSRenderParams::SetCanvasDrawingSurfaceParams(int width, int height)
{
    surfaceParams_.width = width;
    surfaceParams_.height = height;
}

void RSRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    if (dirtyType_.test(RSRenderParamsDirtyType::MATRIX_DIRTY)) {
        target->matrix_.Swap(matrix_);
        dirtyType_.reset(RSRenderParamsDirtyType::MATRIX_DIRTY);
    }
    if (dirtyType_.test(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY)) {
        target->drawingCacheType_ = drawingCacheType_;
        dirtyType_.reset(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);
    }
    target->alpha_ = alpha_;
    target->boundsRect_ = boundsRect_;
    target->frameRect_ = frameRect_;
    target->shouldPaint_ = shouldPaint_;
    target->contentEmpty_ = contentEmpty_;
    target->hasSandBox_ = hasSandBox_;
    target->localDrawRect_ = localDrawRect_;
    target->id_ = id_;
    target->cacheSize_ = cacheSize_;
    target->frameGravity_ = frameGravity_;
    target->childHasVisibleFilter_ = childHasVisibleFilter_;
    target->childHasVisibleEffect_ = childHasVisibleEffect_;
    // use flag in render param and staging render param to determine if cache should be updated
    // (flag in render param may be not used because of occlusion skip, so we need to update cache in next frame)
    target->isDrawingCacheChanged_ = target->isDrawingCacheChanged_ || isDrawingCacheChanged_;
    target->shadowRect_ = shadowRect_;
    target->drawingCacheIncludeProperty_ = drawingCacheIncludeProperty_;
    target->isInBlackList_ = isInBlackList_;
    target->dirtyRegionInfoForDFX_ = dirtyRegionInfoForDFX_;
    target->alphaOffScreen_ = alphaOffScreen_;
    target->hdrBrightness_ = hdrBrightness_;
    target->needFilter_ = needFilter_;
    target->renderNodeType_ = renderNodeType_;
    target->globalAlpha_ = globalAlpha_;
    target->effectNodeShouldPaint_ = effectNodeShouldPaint_;
    target->hasGlobalCorner_ = hasGlobalCorner_;
    target->hasBlurFilter_ = hasBlurFilter_;
    target->foregroundFilterCache_ = foregroundFilterCache_;
    OnCanvasDrawingSurfaceChange(target);
    target->isOpincRootFlag_ = isOpincRootFlag_;
    target->isOpincStateChanged_ = target->isOpincStateChanged_ || isOpincStateChanged_;
    target->startingWindowFlag_ = startingWindowFlag_;
    target->freezeFlag_ = freezeFlag_;
    target->absDrawRect_ = absDrawRect_;
    target->firstLevelNodeId_ = firstLevelNodeId_;
    target->uifirstRootNodeId_ = uifirstRootNodeId_;
    target->isFirstLevelCrossNode_ = isFirstLevelCrossNode_;
    target->cloneSourceDrawable_ = cloneSourceDrawable_;
    target->isCrossNodeOffscreenOn_ = isCrossNodeOffscreenOn_;
    target->absRotation_ = absRotation_;
    target->hasUnobscuredUEC_ = hasUnobscuredUEC_;

    // [Attention] Only used in PC window resize scene now
    target->windowKeyframeEnabled_ = windowKeyframeEnabled_;
    target->linkedRootNodeDrawable_ = linkedRootNodeDrawable_;
    target->needSwapBuffer_ = needSwapBuffer_;
    target->cacheNodeFrameRect_ = cacheNodeFrameRect_;

    needSync_ = false;
}

std::string RSRenderParams::ToString() const
{
    std::string ret = "RSRenderParams:";
    ret += RENDER_BASIC_PARAM_TO_STRING(id_);
    if (alpha_ != 1.0f) {
        ret += RENDER_BASIC_PARAM_TO_STRING(alpha_);
    }
    if (hasSandBox_) {
        ret += RENDER_BASIC_PARAM_TO_STRING(hasSandBox_);
    }
    ret += RENDER_RECT_PARAM_TO_STRING(localDrawRect_);
    ret += RENDER_BASIC_PARAM_TO_STRING(shouldPaint_);
    ret += RENDER_BASIC_PARAM_TO_STRING(int(frameGravity_));
    if (foregroundFilterCache_ != nullptr) {
        ret += foregroundFilterCache_->GetDescription();
    }
    return ret;
}

void RSRenderParams::SetParentSurfaceMatrix(const Drawing::Matrix& parentSurfaceMatrix)
{
    parentSurfaceMatrix_ = parentSurfaceMatrix;
}
const Drawing::Matrix& RSRenderParams::GetParentSurfaceMatrix()
{
    return parentSurfaceMatrix_;
}

bool RSRenderParams::SetFirstLevelNode(NodeId firstLevelNodeId)
{
    if (firstLevelNodeId_ == firstLevelNodeId) {
        return false;
    }
    firstLevelNodeId_ = firstLevelNodeId;
    needSync_ = true;
    return true;
}

NodeId RSRenderParams::GetFirstLevelNodeId() const
{
    return firstLevelNodeId_;
}

bool RSRenderParams::SetUiFirstRootNode(NodeId uifirstRootNodeId)
{
    if (uifirstRootNodeId_ == uifirstRootNodeId) {
        return false;
    }
    uifirstRootNodeId_ = uifirstRootNodeId;
    needSync_ = true;
    return true;
}

NodeId RSRenderParams::GetUifirstRootNodeId() const
{
    return uifirstRootNodeId_;
}

// overrided surface params
const RSLayerInfo& RSRenderParams::GetLayerInfo() const
{
    static const RSLayerInfo defaultLayerInfo = {};
    return defaultLayerInfo;
}

// overrided by displayNode
const ScreenInfo& RSRenderParams::GetScreenInfo() const
{
    static ScreenInfo defalutScreenInfo;
    return defalutScreenInfo;
}

const Drawing::Matrix& RSRenderParams::GetTotalMatrix()
{
    static const Drawing::Matrix defaultMatrix;
    return defaultMatrix;
}
// virtual display params
DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSRenderParams::GetMirrorSourceDrawable()
{
    static DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr defaultPtr;
    return defaultPtr;
}

DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSRenderParams::GetCloneSourceDrawable() const
{
    return cloneSourceDrawable_;
}

void RSRenderParams::SetCloneSourceDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable)
{
    cloneSourceDrawable_ = drawable;
}

// [Attention] Only used in PC window resize scene now
void RSRenderParams::EnableWindowKeyFrame(bool enable)
{
    if (windowKeyframeEnabled_ == enable) {
        return;
    }

    windowKeyframeEnabled_ = enable;
    needSync_ = true;
}

// [Attention] Only used in PC window resize scene now
bool RSRenderParams::IsWindowKeyFrameEnabled() const
{
    return windowKeyframeEnabled_;
}

// [Attention] Only used in PC window resize scene now
void RSRenderParams::SetLinkedRootNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable)
{
    linkedRootNodeDrawable_ = drawable;
    needSync_ = true;
}

// [Attention] Only used in PC window resize scene now
DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr RSRenderParams::GetLinkedRootNodeDrawable()
{
    return linkedRootNodeDrawable_;
}

// [Attention] Only used in PC window resize scene now
void RSRenderParams::SetNeedSwapBuffer(bool needSwapBuffer)
{
    if (needSwapBuffer_ == needSwapBuffer) {
        return;
    }
    needSwapBuffer_ = needSwapBuffer;
    needSync_ = true;
}

// [Attention] Only used in PC window resize scene now
bool RSRenderParams::GetNeedSwapBuffer() const
{
    return needSwapBuffer_;
}

// [Attention] Only used in PC window resize scene now
void RSRenderParams::SetCacheNodeFrameRect(const Drawing::RectF& cacheNodeFrameRect)
{
    if (cacheNodeFrameRect_ == cacheNodeFrameRect) {
        return;
    }
    cacheNodeFrameRect_ = cacheNodeFrameRect;
    needSync_ = true;
}

// [Attention] Only used in PC window resize scene now
const Drawing::RectF& RSRenderParams::GetCacheNodeFrameRect() const
{
    return cacheNodeFrameRect_;
}
} // namespace OHOS::Rosen
