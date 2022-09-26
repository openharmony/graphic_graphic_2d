/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_EXTENDED_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_EXTENDED_MODIFIER_H

#include "command/rs_node_command.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "modifier/rs_modifier.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "transaction/rs_transaction_proxy.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSExtendedModifierHelper {
public:
    static RSDrawingContext CreateDrawingContext(NodeId nodeId);
    static std::shared_ptr<RSRenderModifier> CreateRenderModifier(
        RSDrawingContext& ctx, PropertyId id, RSModifierType type);
    static std::shared_ptr<DrawCmdList> FinishDrawing(RSDrawingContext& ctx);
};

class RS_EXPORT RSExtendedModifier : public RSModifier {
public:
    explicit RSExtendedModifier(const std::shared_ptr<RSPropertyBase> property)
        : RSModifier(property, RSModifierType::EXTENDED)
    {
        property_->SetIsCustom(true);
    }
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::EXTENDED;
    }
    virtual ~RSExtendedModifier() = default;
    virtual void Draw(RSDrawingContext& context) const = 0;

protected:
    RSExtendedModifier(const std::shared_ptr<RSPropertyBase> property, const RSModifierType type)
        : RSModifier(property, type)
    {
        property_->SetIsCustom(true);
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(node->GetId());
        Draw(ctx);
        return RSExtendedModifierHelper::CreateRenderModifier(ctx, property_->GetId(), GetModifierType());
    }

    void UpdateToRender() override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return;
        }
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(node->GetId());
        Draw(ctx);
        auto drawCmdList = RSExtendedModifierHelper::FinishDrawing(ctx);
        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawCmdList>(
            node->GetId(), drawCmdList, property_->id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, node->IsRenderServiceNode());
            if (node->NeedForcedSendToRemote()) {
                std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawCmdList>(
                    node->GetId(), drawCmdList, property_->id_, false);
                transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
            }
            if (node->NeedSendExtraCommand()) {
                std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSUpdatePropertyDrawCmdList>(
                    node->GetId(), drawCmdList, property_->id_, false);
                transactionProxy->AddCommand(extraCommand, !node->IsRenderServiceNode(), node->GetFollowType(),
                    node->GetId());
            }
        }
    }
};

class RS_EXPORT RSContentStyleModifier : public RSExtendedModifier {
public:
    explicit RSContentStyleModifier(const std::shared_ptr<RSPropertyBase> property)
        : RSExtendedModifier(property, RSModifierType::CONTENT_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::CONTENT_STYLE;
    }
};

class RS_EXPORT RSOverlayStyleModifier : public RSExtendedModifier {
public:
    explicit RSOverlayStyleModifier(const std::shared_ptr<RSPropertyBase> property)
        : RSExtendedModifier(property, RSModifierType::OVERLAY_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::OVERLAY_STYLE;
    }

    void SetOverlayBounds(std::shared_ptr<RectI> rect)
    {
        overlayRect_ = rect;
    }

    std::shared_ptr<RectI> GetOverlayBounds() const
    {
        return overlayRect_;
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override
    {
        auto renderModifier = RSExtendedModifier::CreateRenderModifier();
        auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(renderModifier);
        if (drawCmdModifier != nullptr && drawCmdModifier->GetType() == RSModifierType::OVERLAY_STYLE) {
            drawCmdModifier->SetOverlayBounds(overlayRect_);
        }
        return renderModifier;
    }

private:
    std::shared_ptr<RectI> overlayRect_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_EXTENDED_MODIFIER_H
