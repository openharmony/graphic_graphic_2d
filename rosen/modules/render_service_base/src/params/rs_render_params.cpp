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

#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
void RSRenderParams::SetMatrix(Drawing::Matrix matrix)
{
    if (matrix_ == matrix) {
        return;
    }
    matrix_ = matrix;
    needSync_ = true;
}
const Drawing::Matrix RSRenderParams::GetMatrix() const
{
    return matrix_;
}

void RSRenderParams::SetBoundsRect(Drawing::RectF boundsRect)
{
    if (boundsRect_ == boundsRect) {
        return;
    }
    boundsRect_ = boundsRect;
    needSync_ = true;
}
const Drawing::Rect RSRenderParams::GetBounds() const
{
    return boundsRect_;
}

void RSRenderParams::SetShouldPaint(bool shouldPaint)
{
    if (shouldPaint_ == shouldPaint) {
        return;
    }
    shouldPaint_ = shouldPaint;
    needSync_ = true;
}

bool RSRenderParams::GetShouldPaint() const
{
    return shouldPaint_;
}

void RSRenderParams::SetNeedSync(bool needSync)
{
    needSync_ = needSync;
}

bool RSRenderParams::NeedSync() const
{
    return needSync_;
}
void RSRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    target->SetMatrix(matrix_);
    target->SetBoundsRect(boundsRect_);
    target->shouldPaint_ = shouldPaint_;
    needSync_ = false;
}

} // namespace OHOS::Rosen
