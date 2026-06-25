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

#include "modifier_ng/custom/rs_content_style_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSContentStyleModifier::FlushContentModifierImmediately()
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        return;
    }
    auto node = node_.lock();
    if (node == nullptr || node->GetType() != RSUINodeType::CANVAS_DRAWING_NODE) {
        return;
    }
    UpdateToRender();
    if (auto uiContext = node->GetRSUIContext()) {
        uiContext->OnCanvasDrawingNodeUpdate();
    }
#endif
}

bool RSContentStyleModifier::RenderInClient(Drawing::DrawCmdListPtr drawCmdList, std::shared_ptr<RSNode> node)
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    return node != nullptr && node->RenderInClient(drawCmdList);
#endif
    return false;
}
} // namespace OHOS::Rosen::ModifierNG
