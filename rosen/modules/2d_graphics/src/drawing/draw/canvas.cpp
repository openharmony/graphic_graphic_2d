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

namespace OHOS {
namespace Rosen {
namespace Drawing {
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