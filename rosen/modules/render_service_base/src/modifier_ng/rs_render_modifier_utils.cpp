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

#include "modifier_ng/rs_render_modifier_utils.h"

#include "pipeline/rs_recording_canvas.h"

namespace OHOS::Rosen::ModifierNG {
RSDisplayListModifierUpdater::RSDisplayListModifierUpdater(RSDisplayListRenderModifier* target) : target_(target)
{
    // PLANNING: use RSRenderNode to determine the correct recording canvas size
    recordingCanvas_ = ExtendRecordingCanvas::Obtain(10, 10, false); // width 10, height 10
}

RSDisplayListModifierUpdater::~RSDisplayListModifierUpdater()
{
    if (recordingCanvas_ && target_) {
        target_->stagingDrawCmd_ = recordingCanvas_->GetDrawCmdList();
        ExtendRecordingCanvas::Recycle(recordingCanvas_);
        recordingCanvas_.reset();
        target_ = nullptr;
    } else {
        // ROSEN_LOGE("Update failed, recording canvas is null!");
    }
}

const std::unique_ptr<ExtendRecordingCanvas>& RSDisplayListModifierUpdater::GetRecordingCanvas() const
{
    return recordingCanvas_;
}
} // namespace OHOS::Rosen::ModifierNG
