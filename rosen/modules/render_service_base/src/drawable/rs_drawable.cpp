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

#include "drawable/rs_drawable.h"

#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable_content.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {

// =================== RSChildrenDrawable =====================
RSChildrenDrawable::RSChildrenDrawable(std::shared_ptr<const RSChildrenDrawableContent> content)
    : content_(std::move(content))
{}

void RSChildrenDrawable::OnDraw(RSPaintFilterCanvas* canvas) const
{
    for (auto& drawable : content_->childrenDrawables_) {
        drawable->OnDraw(canvas);
    }
}

// =================== RSCustomModifierDrawable =====================
RSCustomModifierDrawable::RSCustomModifierDrawable(std::shared_ptr<const RSCustomModifierDrawableContent> content)
    : content_(std::move(content))
{}

void RSCustomModifierDrawable::OnDraw(RSPaintFilterCanvas* canvas) const
{
    for (auto& drawCmdList : content_->drawCmdList_) {
        drawCmdList->Playback(*canvas);
    }
}

} // namespace OHOS::Rosen
