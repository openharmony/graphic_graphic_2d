/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_EXTENDED_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_EXTENDED_MODIFIER_H

#include "command/rs_node_command.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "modifier/rs_modifier.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"


namespace OHOS {
namespace Rosen {
struct RSDrawingContext {
    Drawing::Canvas* canvas;
    float width;
    float height;
};

class RSC_EXPORT RSExtendedModifierHelper {
public:
    static RSDrawingContext CreateDrawingContext(NodeId nodeId);
    static std::shared_ptr<RSRenderModifier> CreateRenderModifier(
        RSDrawingContext& ctx, PropertyId id, RSModifierType type);
    static std::shared_ptr<Drawing::DrawCmdList> FinishDrawing(RSDrawingContext& ctx);
#if defined(RS_ENABLE_VK)
    static void DeleteVkImage(void *context);
#endif
};

class RSC_EXPORT RSExtendedModifier : public RSModifier {
public:
    RSExtendedModifier(const std::shared_ptr<RSPropertyBase>& property = {})
        : RSModifier(property, RSModifierType::EXTENDED)
    {
        property_->SetIsCustom(true);
    }
    virtual ~RSExtendedModifier() = default;

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::EXTENDED;
    }
    virtual void Draw(RSDrawingContext& context) const = 0;

protected:
    explicit RSExtendedModifier(const RSModifierType type, const std::shared_ptr<RSPropertyBase>& property = {})
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
        bool isEmpty = drawCmdList == nullptr;
        if (lastDrawCmdListEmpty_ && isEmpty) {
            return;
        }
        lastDrawCmdListEmpty_ = isEmpty;

        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawCmdList>(
            node->GetId(), drawCmdList, property_->id_, UPDATE_TYPE_OVERWRITE);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, node->IsRenderServiceNode());
            if (node->NeedForcedSendToRemote()) {
                std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawCmdList>(
                    node->GetId(), drawCmdList, property_->id_, UPDATE_TYPE_OVERWRITE);
                transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
            }
        }
    }
private:
    bool lastDrawCmdListEmpty_ = false;
};

class RS_EXPORT RSGeometryTransModifier : public RSExtendedModifier {
public:
    RSGeometryTransModifier() : RSExtendedModifier(RSModifierType::GEOMETRYTRANS)
    {}
    virtual ~RSGeometryTransModifier() = default;

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::GEOMETRYTRANS;
    }

    void Draw(RSDrawingContext& context) const override {};

    virtual Drawing::Matrix GeometryEffect(float width, float height) const = 0;

protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        auto canvasnode = RSNodeMap::Instance().GetNode<RSCanvasNode>(node->GetId());
        if (canvasnode == nullptr) {
            return nullptr;
        }
        auto renderProperty = std::make_shared<RSRenderProperty<Drawing::Matrix>>(
            GeometryEffect(canvasnode->GetPaintWidth(), canvasnode->GetPaintHeight()), property_->GetId());
        auto renderModifier = std::make_shared<RSGeometryTransRenderModifier>(renderProperty);
        return renderModifier;
    }

    void UpdateToRender() override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return;
        }
        auto canvasnode = RSNodeMap::Instance().GetNode<RSCanvasNode>(node->GetId());
        if (canvasnode == nullptr) {
            return;
        }
        auto matrix = GeometryEffect(canvasnode->GetPaintWidth(), canvasnode->GetPaintHeight());
        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawingMatrix>(
            node->GetId(), matrix, property_->id_, UPDATE_TYPE_OVERWRITE);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, node->IsRenderServiceNode());
            if (node->NeedForcedSendToRemote()) {
                std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawingMatrix>(
                    node->GetId(), matrix, property_->id_, UPDATE_TYPE_OVERWRITE);
                transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
            }
        }
    }
};

class RSC_EXPORT RSTransitionModifier : public RSExtendedModifier {
public:
    RSTransitionModifier() : RSExtendedModifier(RSModifierType::TRANSITION)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::TRANSITION;
    }

    virtual void Active() = 0;

    virtual void Identity() = 0;
};

class RSC_EXPORT RSBackgroundStyleModifier : public RSExtendedModifier {
public:
    RSBackgroundStyleModifier() : RSExtendedModifier(RSModifierType::BACKGROUND_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::BACKGROUND_STYLE;
    }
};

class RSC_EXPORT RSContentStyleModifier : public RSExtendedModifier {
public:
    RSContentStyleModifier() : RSExtendedModifier(RSModifierType::CONTENT_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::CONTENT_STYLE;
    }
};

class RSC_EXPORT RSForegroundStyleModifier : public RSExtendedModifier {
public:
    RSForegroundStyleModifier() : RSExtendedModifier(RSModifierType::FOREGROUND_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::FOREGROUND_STYLE;
    }
};

class RSC_EXPORT RSOverlayStyleModifier : public RSExtendedModifier {
public:
    RSOverlayStyleModifier() : RSExtendedModifier(RSModifierType::OVERLAY_STYLE)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::OVERLAY_STYLE;
    }
};

class RSC_EXPORT RSNodeModifier : public RSExtendedModifier {
public:
    RSNodeModifier() : RSExtendedModifier(RSModifierType::NODE_MODIFIER)
    {}

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::NODE_MODIFIER;
    }

    virtual void Modify(RSNode& target) const = 0;

private:
    void OnAttachToNode(const std::weak_ptr<RSNode>& target) override
    {
        target_ = target;
    }

    void UpdateToRender() override
    {
        auto node = target_.lock();
        if (node == nullptr) {
            return;
        }
        Modify(*node);
    }

    void Draw(RSDrawingContext& context) const override final {}

    std::weak_ptr<RSNode> target_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_EXTENDED_MODIFIER_H
