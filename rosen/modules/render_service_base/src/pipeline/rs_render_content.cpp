/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_content.h"

#include "pipeline/rs_recording_canvas.h"

namespace OHOS {
namespace Rosen {
RSRenderContent::RSRenderContent() = default;

RSProperties& RSRenderContent::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderContent::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderContent::DrawPropertyDrawable(RSPropertyDrawableSlot slot, RSPaintFilterCanvas& canvas) const
{
    auto& drawablePtr = propertyDrawablesVec_[static_cast<size_t>(slot)];
    if (!drawablePtr) {
        return;
    }

    auto recordingCanvas = canvas.GetRecordingCanvas();
    if (recordingCanvas == nullptr || !canvas.GetRecordDrawable()) {
        // non-recording canvas, draw directly
        drawablePtr->Draw(*this, canvas);
        return;
    }

    auto castRecordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas.GetRecordingCanvas());
    auto drawFunc = [sharedPtr = shared_from_this(), slot](Drawing::Canvas* canvas, const Drawing::Rect* rect) -> void {
        if (auto canvasPtr = static_cast<RSPaintFilterCanvas*>(canvas)) {
            sharedPtr->DrawPropertyDrawable(slot, *canvasPtr);
        }
    };
    // recording canvas, record lambda that draws the drawable
    castRecordingCanvas->DrawDrawFunc(std::move(drawFunc));
}

void RSRenderContent::DrawPropertyDrawableRange(
    RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end, RSPaintFilterCanvas& canvas) const
{
    auto recordingCanvas = canvas.GetRecordingCanvas();
    if (recordingCanvas == nullptr || !canvas.GetRecordDrawable()) {
        // non-recording canvas, draw directly
        std::for_each(propertyDrawablesVec_.begin() + static_cast<size_t>(begin),
            propertyDrawablesVec_.begin() + static_cast<size_t>(end) + 1, // +1 since we need to include end
            [&](auto& drawablePtr) {
                if (drawablePtr) {
                    drawablePtr->Draw(*this, canvas);
                }
            });
        return;
    }

    auto castRecordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas.GetRecordingCanvas());
    auto drawFunc =
        [sharedPtr = shared_from_this(), begin, end](Drawing::Canvas* canvas, const Drawing::Rect* rect) -> void {
        if (auto canvasPtr = static_cast<RSPaintFilterCanvas*>(canvas)) {
            sharedPtr->DrawPropertyDrawableRange(begin, end, *canvasPtr);
        }
    };
    // recording canvas, record lambda that draws the drawable
    castRecordingCanvas->DrawDrawFunc(std::move(drawFunc));
}

RSRenderNodeType RSRenderContent::GetType() const
{
    return type_;
}
} // namespace Rosen
} // namespace OHOS
