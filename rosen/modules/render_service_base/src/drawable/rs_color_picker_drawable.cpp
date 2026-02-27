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

std::pair<bool, bool> RSColorPickerDrawable::PrepareForExecution(uint64_t vsyncTime, bool darkMode)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSColorPickerDrawable::Preparing node %" PRIu64 " darkMode=%d", nodeId_, darkMode);
    const bool darkModeChanged = std::exchange(stagingIsSystemDarkColorMode_, darkMode) != darkMode;
    const bool prev = std::exchange(stagingNeedColorPick_, false);
    if (!stagingColorPicker_ || stagingColorPicker_->strategy == ColorPickStrategyType::NONE) {
        return { false, prev != stagingNeedColorPick_ || darkModeChanged };
    }

    constexpr uint64_t NS_TO_MS = 1000000; // Convert nanoseconds to milliseconds
    uint64_t interval = stagingColorPicker_->interval;
    uint64_t vsyncTimeMs = vsyncTime / NS_TO_MS;
    if (vsyncTimeMs >= interval + lastUpdateTime_) {
        stagingNeedColorPick_ = true;
        lastUpdateTime_ = vsyncTimeMs;
        isTaskScheduled_ = false; // Reset flag when processing a new frame
    } else if (!isTaskScheduled_) {
        // Schedule a postponed task to catch frames rendered during cooldown
        uint64_t remainingDelay = (interval + lastUpdateTime_) - vsyncTimeMs;
        isTaskScheduled_ = true;
        RSColorPickerThread::Instance().PostTask(
            [nodeId = stagingNodeId_]() { RSColorPickerThread::Instance().NotifyNodeDirty(nodeId); }, false,
            static_cast<int64_t>(remainingDelay));
    }
    return { stagingNeedColorPick_, prev != stagingNeedColorPick_ || darkModeChanged };
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

void RSColorPickerDrawable::SetIsSystemDarkColorMode(bool isSystemDarkColorMode)
{
    if (stagingIsSystemDarkColorMode_ != isSystemDarkColorMode) {
        stagingIsSystemDarkColorMode_ = isSystemDarkColorMode;
        needSync_ = true;
    }
}

void RSColorPickerDrawable::ResetColorMemory()
{
    if (colorPickerManager_) {
        colorPickerManager_->ResetColorMemory();
    }
}
} // namespace DrawableV2
} // namespace OHOS::Rosen