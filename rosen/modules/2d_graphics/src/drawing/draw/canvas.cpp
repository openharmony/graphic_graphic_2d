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

#include "draw/canvas.h"
#include "recording/record_cmd.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
AutoCanvasMatrixBrush::AutoCanvasMatrixBrush(Canvas* canvas,
    const Matrix* matrix, const Brush* brush, const Rect& bounds)
    : canvas_(canvas), saveCount_(canvas->Save()),
    paintPen_(canvas->GetMutablePen()), paintBrush_(canvas->GetMutableBrush())
{
    if (brush) {
        Rect newBounds = bounds;
        if (matrix) {
            matrix->MapRect(newBounds, bounds);
        }
        SaveLayerOps slr(&newBounds, brush);
        canvas->SaveLayer(slr);
    } else if (matrix) {
        canvas->Save();
    }

    if (matrix) {
        canvas->ConcatMatrix(*matrix);
    }
}

AutoCanvasMatrixBrush::~AutoCanvasMatrixBrush()
{
    canvas_->GetMutablePen() = paintPen_;
    canvas_->GetMutableBrush() = paintBrush_;
    canvas_->RestoreToCount(saveCount_);
}

Canvas* Canvas::GetRecordingCanvas() const
{
    return nullptr;
}

void Canvas::AddCanvas(Canvas* canvas)
{
    if (canvas != nullptr) {
        pCanvasList_.push_back(canvas);
    }
}

void Canvas::RemoveAll()
{
    pCanvasList_.clear();
}

Canvas::~Canvas()
{
    RemoveAll();
}

void Canvas::RestoreToCount(uint32_t count)
{
    for (uint32_t i = this->GetSaveCount(); i > count; i--) {
        this->Restore();
    }
}

void Canvas::DrawRecordCmd(const std::shared_ptr<RecordCmd> recordCmd,
    const Matrix* matrix, const Brush* brush)
{
    if (recordCmd == nullptr) {
        LOGE("Canvas::DrawRecordCmd, recordCmd is nullptr!");
        return;
    }
    if (matrix && matrix->IsIdentity()) {
        matrix = nullptr;
    }

    AutoCanvasMatrixBrush autoCanvasMatrixBrush(this, matrix, brush, recordCmd->GetCullRect());
    recordCmd->Playback(this);
}

bool Canvas::GetRecordingState() const
{
    return recordingState_;
}

void Canvas::SetRecordingState(bool flag)
{
    recordingState_ = flag;
}

void Canvas::SetOffscreen(bool isOffscreen)
{
    isOffscreen_ = isOffscreen;
}

bool Canvas::GetOffscreen() const
{
    return isOffscreen_;
}

void Canvas::SetUICapture(bool isUICapture)
{
    isUICapture_ = isUICapture;
}

bool Canvas::GetUICapture() const
{
    return isUICapture_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS