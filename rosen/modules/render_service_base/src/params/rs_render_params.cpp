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
const Drawing::Matrix RSRenderParams::GetMatrix() const
{
    return matrix_;
}

const Drawing::Rect RSRenderParams::GetBounds() const
{
    return boundsRect_;
}

void RSRenderParams::SetMatrix(Drawing::Matrix matrix)
{
    matrix_ = matrix;
}

void RSRenderParams::SetBoundsRect(Drawing::RectF boundsRect)
{
    boundsRect_ = boundsRect;
}

void RSRenderParams::SetShouldPaint(bool shouldPaint)
{
    shouldPaint_ = shouldPaint;
}

void RSRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    target->SetMatrix(matrix_);
    target->SetBoundsRect(boundsRect_);
    target->shouldPaint_ = shouldPaint_;
}

} // namespace OHOS::Rosen
