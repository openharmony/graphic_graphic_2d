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

namespace OHOS {
namespace Rosen {
RSRenderContent::RSRenderContent()
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        propertyDrawablesVec_.resize(Slot::RSPropertyDrawableSlot::MAX);
    }
}

RSProperties& RSRenderContent::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderContent::GetRenderProperties() const
{
    return renderProperties_;
}

const RSPropertyDrawable::DrawableVec& RSRenderContent::GetPropertyDrawableVec() const
{
    return propertyDrawablesVec_;
}

void RSRenderContent::IterateOnDrawableRange(Slot::RSPropertyDrawableSlot begin, Slot::RSPropertyDrawableSlot end,
    RSPaintFilterCanvas& canvas, RSRenderNode& node) 
{
    for (uint16_t index = begin; index <= end; index++) {
        auto& drawablePtr = propertyDrawablesVec_[index];
        if (drawablePtr) {
            drawablePtr->Draw(node, canvas);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
