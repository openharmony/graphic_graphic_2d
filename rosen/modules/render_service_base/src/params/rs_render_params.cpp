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

#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
namespace {
thread_local Drawing::Matrix parentSurfaceMatrix_;
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

void RSRenderParams::ApplyAlphaAndMatrixToCanvas(RSPaintFilterCanvas& canvas) const
{
    if (UNLIKELY(HasSandBox())) {
        canvas.SetMatrix(parentSurfaceMatrix_);
        canvas.ConcatMatrix(matrix_);
        canvas.SetAlpha(alpha_);
    } else {
        canvas.ConcatMatrix(matrix_);
        if (alpha_ < 1.0f && (drawingCacheType_ == RSDrawingCacheType::FORCED_CACHE || alphaOffScreen_)) {
            auto rect = GetBounds();
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

Vector2f RSRenderParams::GetCacheSize() const
{
    return cacheSize_;
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

void RSRenderParams::SetDrawingCacheType(RSDrawingCacheType cacheType)
{
    if (drawingCacheType_ == cacheType) {
        return;
    }
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

void RSRenderParams::OpincSetCacheChangeFlag(bool state)
{
    isOpincStateChanged_ = state;
    needSync_ = true;
}

bool RSRenderParams::OpincGetCacheChangeState()
{
    bool state = isOpincStateChanged_;
    isOpincStateChanged_ = false;
    return state;
}

bool RSRenderParams::OpincGetCachedMark()
{
    return isOpincMarkCached_;
}

void RSRenderParams::OpincSetCachedMark(bool mark)
{
    isOpincMarkCached_ = mark;
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

void RSRenderParams::SetDirtyRegionInfoForDFX(DirtyRegionInfoForDFX dirtyRegionInfo)
{
    if (dirtyRegionInfoForDFX_ == dirtyRegionInfo) {
        return;
    }
    dirtyRegionInfoForDFX_ = dirtyRegionInfo;
    needSync_ = true;
}

DirtyRegionInfoForDFX RSRenderParams::GetDirtyRegionInfoForDFX() const
{
    return dirtyRegionInfoForDFX_;
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

void RSRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    if (dirtyType_.test(RSRenderParamsDirtyType::MATRIX_DIRTY)) {
        target->matrix_.Swap(matrix_);
        dirtyType_.reset(RSRenderParamsDirtyType::MATRIX_DIRTY);
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
    target->isDrawingCacheChanged_ = isDrawingCacheChanged_;
    target->shadowRect_ = shadowRect_;
    target->drawingCacheType_ = drawingCacheType_;
    target->drawingCacheIncludeProperty_ = drawingCacheIncludeProperty_;
    target->dirtyRegionInfoForDFX_ = dirtyRegionInfoForDFX_;
    target->alphaOffScreen_ = alphaOffScreen_;
    OnCanvasDrawingSurfaceChange(target);
    target->isOpincRootFlag_ = isOpincRootFlag_;
    target->isOpincStateChanged_ = OpincGetCacheChangeState();
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

} // namespace OHOS::Rosen
