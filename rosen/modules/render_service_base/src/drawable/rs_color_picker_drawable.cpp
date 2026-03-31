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

#include "drawable/rs_color_picker_drawable.h"

#include "feature/color_picker/color_pick_alt_manager.h"
#include "feature/color_picker/rs_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_thread.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
RSColorPickerDrawable::RSColorPickerDrawable(bool useAlt, NodeId nodeId)
{
    if (useAlt) {
        colorPickerManager_ = std::make_shared<ColorPickAltManager>(nodeId);
    } else {
        colorPickerManager_ = std::make_shared<RSColorPickerManager>(nodeId);
    }
}

RSDrawable::Ptr RSColorPickerDrawable::OnGenerate(const RSRenderNode& node)
{
    auto colorPicker = node.GetRenderProperties().GetColorPicker();
    const bool useAlt = colorPicker ? colorPicker->strategy == ColorPickStrategyType::CLIENT_CALLBACK : false;
    if (auto ret = std::make_shared<RSColorPickerDrawable>(useAlt, node.GetId()); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSColorPickerDrawable::OnPrepare(bool darkMode)
{
    RS_OPTIONAL_TRACE_NAME_FMT(
        "RSColorPickerDrawable::OnPrepareFrame node %" PRIu64 " darkMode = %d", stagingNodeId_, darkMode);
    const bool darkModeChanged = std::exchange(stagingIsSystemDarkColorMode_, darkMode) != darkMode;

    // Derive stagingNeedColorPick_ directly from state
    const bool prevNeedColorPick =
        std::exchange(stagingNeedColorPick_, stagingState_ == DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME);

    // Return true if sync is needed
    return prevNeedColorPick != stagingNeedColorPick_ || darkModeChanged;
}

int64_t RSColorPickerDrawable::ScheduleColorPickIfReady(uint64_t vsyncTime)
{
    if (!stagingColorPicker_ || stagingColorPicker_->strategy == ColorPickStrategyType::NONE) {
        return -1;
    }

    // Only schedule if in PREPARING state
    if (stagingState_ != ColorPickerState::PREPARING) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSColorPickerDrawable::ScheduleColorPickIfReady: Invalid state %u (only PREPARING allowed)",
            static_cast<uint8_t>(stagingState_));
        return -1;
    }

    constexpr uint64_t NS_TO_MS = 1000000;
    const uint64_t interval = stagingColorPicker_->interval;
    const uint64_t vsyncTimeMs = vsyncTime / NS_TO_MS;

    // Always schedule color pick when in PREPARING state, adjust delay based on cooldown
    int64_t delayTime = 0;
    if (vsyncTimeMs < interval + lastUpdateTime_) {
        delayTime = interval + lastUpdateTime_ - vsyncTimeMs;
        lastUpdateTime_ += interval;
    } else {
        lastUpdateTime_ = vsyncTimeMs;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSColorPickerDrawable::ScheduleColorPickIfReady node %" PRIu64
                               " scheduling pick with delay %" PRId64 "ms",
        stagingNodeId_, delayTime);

    // Transition to SCHEDULED state, caller will post delayed task
    stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    return delayTime;
}

bool RSColorPickerDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingColorPicker_ = node.GetRenderProperties().GetColorPicker();
    needSync_ = true;
    return stagingColorPicker_ && stagingColorPicker_->strategy != ColorPickStrategyType::NONE;
}

void RSColorPickerDrawable::OnSync()
{
    needColorPick_ = stagingNeedColorPick_;
    if (colorPickerManager_) {
        colorPickerManager_->SetSystemDarkColorMode(stagingIsSystemDarkColorMode_);
    }
    if (!needSync_) {
        return;
    }
    nodeId_ = stagingNodeId_;
    params_ = stagingColorPicker_ ? *stagingColorPicker_ : ColorPickerParam();

    needSync_ = false;
}

void RSColorPickerDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (!colorPickerManager_) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (!paintFilterCanvas) {
        return;
    }

    auto maybeColor = colorPickerManager_->GetColorPick();
    if (maybeColor.has_value()) {
        paintFilterCanvas->SetColorPicked(maybeColor.value());
    }
    RS_OPTIONAL_TRACE_NAME_FMT("ColorPicker: onDraw nodeId=%" PRIu64 " rect=[%s], need execute = %d", nodeId_,
        rect ? rect->ToString().c_str() : "null", needColorPick_);

    if (needColorPick_) {
        colorPickerManager_->ScheduleColorPick(*paintFilterCanvas, rect, params_);
    }
}

void RSColorPickerDrawable::ResetColorMemory()
{
    if (colorPickerManager_) {
        colorPickerManager_->ResetColorMemory();
    }
    stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    stagingNeedColorPick_ = false;
}

void RSColorPickerDrawable::SetState(DrawableV2::ColorPickerState state)
{
    const DrawableV2::ColorPickerState currentState = stagingState_;

    // Validate state transitions
    switch (currentState) {
        case DrawableV2::ColorPickerState::PREPARING:
            // Can transition to SCHEDULED, or stay in PREPARING (no-op)
            if (state != DrawableV2::ColorPickerState::SCHEDULED && state != DrawableV2::ColorPickerState::PREPARING) {
                RS_LOGD("Invalid state transition from PREPARING to %u (only SCHEDULED or PREPARING allowed)",
                    static_cast<uint8_t>(state));
                return;
            }
            break;

        case DrawableV2::ColorPickerState::SCHEDULED:
            // Can only transition to COLOR_PICK_THIS_FRAME
            if (state != DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME) {
                RS_LOGD("Invalid state transition from SCHEDULED to %u (only COLOR_PICK_THIS_FRAME allowed)",
                    static_cast<uint8_t>(state));
                return;
            }
            break;

        case DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME:
            // Can only transition back to PREPARING
            if (state != DrawableV2::ColorPickerState::PREPARING) {
                RS_LOGD("Invalid state transition from COLOR_PICK_THIS_FRAME to %u (only PREPARING allowed)",
                    static_cast<uint8_t>(state));
                return;
            }
            break;

        default:
            RS_LOGD("Unknown current state %u during transition to %u", static_cast<uint8_t>(currentState),
                static_cast<uint8_t>(state));
            return;
    }

    stagingState_ = state;
}

ColorPickerState RSColorPickerDrawable::GetState() const
{
    return stagingState_;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen