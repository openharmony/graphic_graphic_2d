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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_EXTENDED_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_EXTENDED_MODIFIER_H

#include "command/rs_node_command.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"

namespace OHOS {
namespace Rosen {
namespace ModifierNG {
struct RSDrawingContext {
    Drawing::Canvas* canvas;
    float width;
    float height;
};

class RSC_EXPORT RSExtendedModifierHelper {
public:
    static RSDrawingContext CreateDrawingContext(NodeId nodeId);
    static std::shared_ptr<Drawing::DrawCmdList> FinishDrawing(RSDrawingContext& ctx);
};

class RSC_EXPORT RSExtendedModifier : public RSModifier {
public:
    RSExtendedModifier() = default;
    virtual ~RSExtendedModifier() = default;

    virtual void Draw(RSDrawingContext& context) const = 0;

    void SetNoNeedUICaptured(bool noNeedUICaptured)
    {
        noNeedUICaptured_ = noNeedUICaptured;
    }

protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() override
    {
        auto node = node_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(node->GetId());
        Draw(ctx);
        auto drawCmdList = RSExtendedModifierHelper::FinishDrawing(ctx);
        Setter<RSProperty, std::shared_ptr<Drawing::DrawCmdList>>(GetInnerPropertyType(), drawCmdList);
        return RSModifier::CreateRenderModifier();
    }

    virtual RSPropertyType GetInnerPropertyType() const
    {
        return RSPropertyType::CUSTOM;
    }

    void UpdateToRender() override
    {
        auto node = node_.lock();
        if (node == nullptr) {
            return;
        }
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(node->GetId());
        Draw(ctx);
        auto drawCmdList = RSExtendedModifierHelper::FinishDrawing(ctx);
        bool isEmpty = drawCmdList == nullptr;
        if (lastDrawCmdListEmpty_ && isEmpty) {
            return;
        }
        if (drawCmdList) {
            drawCmdList->SetNoNeedUICaptured(noNeedUICaptured_);
            drawCmdList->SetIsNeedUnmarshalOnDestruct(!node->IsRenderServiceNode());
        }
        lastDrawCmdListEmpty_ = isEmpty;
        Setter<RSProperty, std::shared_ptr<Drawing::DrawCmdList>>(GetInnerPropertyType(), drawCmdList);
    }

private:
    bool lastDrawCmdListEmpty_ = false;
    bool noNeedUICaptured_ = false;
};
} // namespace ModifierNG
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_EXTENDED_MODIFIER_H
