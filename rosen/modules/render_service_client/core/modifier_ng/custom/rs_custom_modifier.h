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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_CUSTOM_RS_CUSTOM_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_CUSTOM_RS_CUSTOM_MODIFIER_H

#include "command/rs_node_command.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_draw_cmd_list.h"

namespace OHOS::Rosen {
class RSNode;
namespace ModifierNG {
struct RSDrawingContext {
    Drawing::Canvas* canvas;
    float width;
    float height;
};

class RSC_EXPORT RSCustomModifierHelper {
public:
    static RSDrawingContext CreateDrawingContext(std::shared_ptr<RSNode> node);
    static std::shared_ptr<Drawing::DrawCmdList> FinishDrawing(RSDrawingContext& ctx);
};

class RSC_EXPORT RSCustomModifier : public RSModifier {
public:
    RSCustomModifier() = default;
    virtual ~RSCustomModifier() = default;

    virtual void Draw(RSDrawingContext& context) const = 0;

    void SetNoNeedUICaptured(bool noNeedUICaptured)
    {
        noNeedUICaptured_ = noNeedUICaptured;
    }

    bool IsCustom() const override
    {
        return true;
    }

    int16_t GetIndex() const
    {
        return Getter(RSPropertyType::CUSTOM_INDEX, 0);
    }

    void SetIndex(int16_t index)
    {
        Setter<RSProperty>(RSPropertyType::CUSTOM_INDEX, index);
    }

protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() override
    {
        auto node = node_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        RSDrawingContext ctx = RSCustomModifierHelper::CreateDrawingContext(node);
        Draw(ctx);
        auto drawCmdList = RSCustomModifierHelper::FinishDrawing(ctx);
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
        RSDrawingContext ctx = RSCustomModifierHelper::CreateDrawingContext(node);
        Draw(ctx);
        auto drawCmdList = RSCustomModifierHelper::FinishDrawing(ctx);
        bool isEmpty = drawCmdList == nullptr;
        if (lastDrawCmdListEmpty_ && isEmpty) {
            return;
        }
        if (drawCmdList) {
            drawCmdList->SetNoNeedUICaptured(noNeedUICaptured_);
            drawCmdList->SetIsNeedUnmarshalOnDestruct(!node->IsRenderServiceNode());
        }
        lastDrawCmdListEmpty_ = isEmpty;
        auto it = properties_.find(GetInnerPropertyType());
        if (it == properties_.end()) {
            return;
        }
        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawCmdListNG>(
            node->GetId(), id_, GetType(), GetInnerPropertyType(), drawCmdList);
        node->AddCommand(command, node->IsRenderServiceNode());
        if (node->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawCmdListNG>(
                node->GetId(), id_, GetType(), GetInnerPropertyType(), drawCmdList);
            node->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
        }
    }

private:
    bool lastDrawCmdListEmpty_ = false;
    bool noNeedUICaptured_ = false;

    friend class OHOS::Rosen::RSNode;
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_CUSTOM_RS_CUSTOM_MODIFIER_H
