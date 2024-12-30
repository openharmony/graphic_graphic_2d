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

#include "modifier_ng/rs_content_render_modifier.h"

#include "pipeline/rs_canvas_drawing_render_node.h"

namespace OHOS::Rosen::ModifierNG {
void RSChildrenDrawable::OnSync()
{
    std::swap(stagingChildrenDrawableVec_, childrenDrawableVec_);
    stagingChildrenDrawableVec_.clear();
}

void RSChildrenDrawable::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    for (size_t i = 0; i < childrenDrawableVec_.size(); i++) {
#ifdef RS_ENABLE_PREFETCH
        size_t prefetchIndex = i + 2;
        if (prefetchIndex < childrenDrawableVec_.size()) {
            __builtin_prefetch(&(childrenDrawableVec_[prefetchIndex]), 0, 1);
        }
#endif
        const auto& drawable = childrenDrawableVec_[i];
        drawable->Draw(canvas);
    }
}

void RSChildrenDrawable::Purge()
{
    stagingChildrenDrawableVec_.clear();
    OnSync();
}
} // namespace OHOS::Rosen::ModifierNG
