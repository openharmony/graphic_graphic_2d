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

#include "drawable/rs_property_drawable.h"

#include "rs_trace.h"

#include "drawable/rs_property_drawable_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (const auto& drawCmdList = ptr->drawCmdList_) {
            drawCmdList->Playback(*canvas);
        }
    };
}

// ============================================================================
// Updater
RSPropertyDrawCmdListUpdater::RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target)
{
    // PLANNING: use RSRenderNode to determine the correct recording canvas size
    recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(10, 10, false);
}

RSPropertyDrawCmdListUpdater::~RSPropertyDrawCmdListUpdater()
{
    if (recordingCanvas_ && target_) {
        target_->stagingDrawCmdList_ = recordingCanvas_->GetDrawCmdList();
        target_->needSync_ = true;
        recordingCanvas_.reset();
        target_ = nullptr;
    } else {
        ROSEN_LOGE("Update failed, recording canvas is null!");
    }
}

const std::unique_ptr<ExtendRecordingCanvas>& RSPropertyDrawCmdListUpdater::GetRecordingCanvas() const
{
    return recordingCanvas_;
}

// ============================================================================
RSDrawable::Ptr RSFrameOffsetDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSFrameOffsetDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSFrameOffsetDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->Translate(frameOffsetX, frameOffsetY);
    return true;
}

// ============================================================================
RSDrawable::Ptr RSClipToBoundsDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToBoundsDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSClipToBoundsDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    auto& canvas = *updater.GetRecordingCanvas();
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, true);
    }
    return true;
}

RSDrawable::Ptr RSClipToFrameDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSClipToFrameDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.GetClipToFrame()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->ClipRect(
        RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetFrameRect()), Drawing::ClipOp::INTERSECT, false);
    return true;
}

RSDrawable::Ptr RSBeginBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBeginBlendModeDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBeginBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    if (blendMode == 0) {
        // no blend
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // fast blend mode
    if (properties.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
        // TODO canvas.SaveBlendMode, canvas.SetBlendMode
        // canvas.SaveBlendMode();
        // canvas.SetBlendMode({ blendMode - 1 }); // map blendMode to SkBlendMode
        return true;
    }

    // save layer mode
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush blendBrush_;
    blendBrush_.SetAlphaF(canvas.GetAlpha());
    blendBrush_.SetBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1)); // map blendMode to Drawing::BlendMode
    Drawing::SaveLayerOps maskLayerRec(nullptr, &blendBrush_, 0);
    canvas.SaveLayer(maskLayerRec);
    // TODO canvas.SaveBlendMode, canvas.SetBlendMode, canvas.SaveAlpha, canvas.SetAlpha
    // canvas.SaveBlendMode();
    // canvas.SetBlendMode(std::nullopt);
    // canvas.SaveAlpha();
    // canvas.SetAlpha(1.0f);
    return true;
}

RSDrawable::Ptr RSEndBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSEndBlendModeDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSEndBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetColorBlendMode() == 0) {
        // no blend
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // TODO canvas.RestoreBlendMode
    // canvas.RestoreBlendMode();
    if (properties.GetColorBlendApplyType() != static_cast<int>(RSColorBlendApplyType::FAST)) {
        // TODO canvas.RestoreAlpha
        // canvas.RestoreAlpha();
    }
    canvas.Restore();
    return true;
}

} // namespace OHOS::Rosen
