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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_extended_modifier.h 
 *
 * @brief Defines the properties and methods of the RSExtendedModifier and its derived classes.
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
/**
 * @struct RSDrawingContext
 * @brief Represents the drawing context used for rendering operations.
 */
struct RSDrawingContext {
    /* Pointer to the canvas object used for drawing operations. */
    Drawing::Canvas* canvas;
    /* The width of the drawing area. */
    float width;
    /* The height of the drawing area. */
    float height;
};

class RSC_EXPORT RSExtendedModifierHelper {
public:
    /**
     * @brief Creates a drawing context for the specified canvas node.
     *
     * @param canvasnode A weak pointer to the RSCanvasNode for which the drawing context is to be created.
     * @return An RSDrawingContext object associated with the given canvas node.
     */
    static RSDrawingContext CreateDrawingContext(std::weak_ptr<RSCanvasNode> canvasnode);

    /**
     * @brief Creates a render modifier.
     *
     * @param ctx The drawing context in which the render modifier will be created.
     * @param id The property ID associated with the render modifier.
     * @param type The type of the render modifier to be created.
     * @return A shared pointer to the created RSRenderModifier object.
     */
    static std::shared_ptr<RSRenderModifier> CreateRenderModifier(
        RSDrawingContext& ctx, PropertyId id, RSModifierType type);
    
    
    /**
     * @brief Completes the drawing process for the given drawing context.
     *
     * @param ctx The drawing context containing the state and operations to be finalized.
     * @return A shared pointer to a DrawCmdList object representing the completed drawing commands.
     */
    static std::shared_ptr<Drawing::DrawCmdList> FinishDrawing(RSDrawingContext& ctx);
};

class RSC_EXPORT RSExtendedModifier : public RSModifier {
public:
    /**
     * @brief Constructs an RSExtendedModifier object with the specified property.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     */
    RSExtendedModifier(const std::shared_ptr<RSPropertyBase>& property = {})
        : RSModifier(property, RSModifierType::EXTENDED)
    {
        property_->SetIsCustom(true);
    }

    /**
     * @brief Destructor for RSExtendedModifier.
     */
    virtual ~RSExtendedModifier() = default;

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::EXTENDED;
    }

    /**
     * @brief Draws the content using the specified drawing context.
     *
     * @param context The drawing context used for rendering operations.
     */
    virtual void Draw(RSDrawingContext& context) const = 0;

    /**
     * @brief Sets whether the UI captured is not needed.
     *
     * @param noNeedUICaptured A boolean value indicating whether the UI captured is not needed.
     */
    void SetNoNeedUICaptured(bool noNeedUICaptured)
    {
        noNeedUICaptured_ = noNeedUICaptured;
    }

protected:
    explicit RSExtendedModifier(const RSModifierType type, const std::shared_ptr<RSPropertyBase>& property = {})
        : RSModifier(property, type)
    {
        property_->SetIsCustom(true);
    }

    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::CUSTOM;
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        std::weak_ptr<RSCanvasNode> canvasnode = RSBaseNode::ReinterpretCast<RSCanvasNode>(node);
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(canvasnode);
        Draw(ctx);
        return RSExtendedModifierHelper::CreateRenderModifier(ctx, property_->GetId(), GetModifierType());
    }

    void UpdateToRender() override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return;
        }
        std::weak_ptr<RSCanvasNode> canvasnode = RSBaseNode::ReinterpretCast<RSCanvasNode>(node);
        RSDrawingContext ctx = RSExtendedModifierHelper::CreateDrawingContext(canvasnode);
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

        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawCmdList>(
            node->GetId(), drawCmdList, property_->id_, UPDATE_TYPE_OVERWRITE);
        node->AddCommand(command, node->IsRenderServiceNode());
        if (node->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawCmdList>(
                node->GetId(), drawCmdList, property_->id_, UPDATE_TYPE_OVERWRITE);
            node->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
        }
    }
private:
    bool lastDrawCmdListEmpty_ = false;
    bool noNeedUICaptured_ = false;
};

class RS_EXPORT RSGeometryTransModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSGeometryTransModifier instance.
     */
    RSGeometryTransModifier() : RSExtendedModifier(RSModifierType::GEOMETRYTRANS)
    {}

    /**
     * @brief Destructor for RSGeometryTransModifier.
     */
    virtual ~RSGeometryTransModifier() = default;

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::GEOMETRYTRANS;
    }

    /**
     * @brief Draws the content using the specified drawing context.
     *
     * @param context Indicates drawing context.
     */
    void Draw(RSDrawingContext& context) const override {};

    /**
     * @brief Gets the geometry effect for the specified width and height.
     *
     * @param width Indicates the width of the geometry effect.
     * @param height Indicates the height of the geometry effect.
     * @return The geometry effect information.
     */
    virtual Drawing::Matrix GeometryEffect(float width, float height) const = 0;

protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override
    {
        auto node = property_->target_.lock();
        if (node == nullptr) {
            return nullptr;
        }
        std::shared_ptr<RSCanvasNode> canvasnode;
        auto uiContext = node->GetRSUIContext();
        if (uiContext == nullptr) {
            canvasnode = RSNodeMap::Instance().GetNode<RSCanvasNode>(node->GetId());
        } else {
            canvasnode = uiContext->GetNodeMap().GetNode<RSCanvasNode>(node->GetId());
        }
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
        std::shared_ptr<RSCanvasNode> canvasnode;
        auto uiContext = node->GetRSUIContext();
        if (uiContext == nullptr) {
            canvasnode = RSNodeMap::Instance().GetNode<RSCanvasNode>(node->GetId());
        } else {
            canvasnode = uiContext->GetNodeMap().GetNode<RSCanvasNode>(node->GetId());
        }
        if (canvasnode == nullptr) {
            return;
        }
        auto matrix = GeometryEffect(canvasnode->GetPaintWidth(), canvasnode->GetPaintHeight());
        std::unique_ptr<RSCommand> command = std::make_unique<RSUpdatePropertyDrawingMatrix>(
            node->GetId(), matrix, property_->id_, UPDATE_TYPE_OVERWRITE);
        node->AddCommand(command, node->IsRenderServiceNode());
        if (node->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSUpdatePropertyDrawingMatrix>(
                node->GetId(), matrix, property_->id_, UPDATE_TYPE_OVERWRITE);
            node->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
        }
    }
};

class RSC_EXPORT RSTransitionModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSTransitionModifier instance.
     */
    RSTransitionModifier() : RSExtendedModifier(RSModifierType::TRANSITION)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::TRANSITION;
    }

    /**
     * @brief Gets whether the modifier is active.
     */
    virtual void Active() = 0;

    /**
     * @brief Gets the identity of the modifier.
     */
    virtual void Identity() = 0;
};

class RSC_EXPORT RSBackgroundStyleModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSBackgroundStyleModifier instance.
     */
    RSBackgroundStyleModifier() : RSExtendedModifier(RSModifierType::BACKGROUND_STYLE)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::BACKGROUND_STYLE;
    }
};

class RSC_EXPORT RSContentStyleModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSContentStyleModifier instance.
     */
    RSContentStyleModifier() : RSExtendedModifier(RSModifierType::CONTENT_STYLE)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::CONTENT_STYLE;
    }
};

class RSC_EXPORT RSForegroundStyleModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSForegroundStyleModifier instance.
     */
    RSForegroundStyleModifier() : RSExtendedModifier(RSModifierType::FOREGROUND_STYLE)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::FOREGROUND_STYLE;
    }
};

class RSC_EXPORT RSOverlayStyleModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSOverlayStyleModifier instance.
     */
    RSOverlayStyleModifier() : RSExtendedModifier(RSModifierType::OVERLAY_STYLE)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::OVERLAY_STYLE;
    }
};

class RSC_EXPORT RSNodeModifier : public RSExtendedModifier {
public:
    /**
     * @brief Constructs an RSNodeModifier instance.
     */
    RSNodeModifier() : RSExtendedModifier(RSModifierType::NODE_MODIFIER)
    {}

    /**
     * @brief Gets the type of the modifier.
     *
     * @return The type of the modifier as an RSModifierType enum value.
     */
    RSModifierType GetModifierType() const override
    {
        return RSModifierType::NODE_MODIFIER;
    }

    /**
     * @brief Modifies the target node.
     *
     * @param target The target node to be modified.
     */
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

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_EXTENDED_MODIFIER_H
