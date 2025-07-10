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
 * @file rs_modifier.h
 *
 * @brief Defines the properties and methods of the Modifier class and the main Modifier class.
 *
 * Other modifiers inherit from these six modifiers.
 * RSGeometryModifier: Describes the size, position, and affine transformation of rendering nodes.
 * RSBackgroundModifier: Describes the background color, background image of the rendered node.
 * RSContentModifier: Describes the text, images of the rendered node.
 * RSForegroundModifier: Describes the foreground color of the rendered node.
 * RSOverlayModifier: The content displayed at the top layer of the rendering node
 * RSAppearanceModifier: Describes the overall appearance of the rendered node, such as transparency, Fliter, clip.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H

#include "common/rs_macros.h"
#include "common/rs_obj_abs_geometry.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSModifier
 *
 * @brief The base class for all modifiers.
 */
class RSC_EXPORT RSModifier : public std::enable_shared_from_this<RSModifier> {
public:
    /**
     * @brief Construct an RSModifier object with the specified property.
     * 
     * If the provided property is a valid shared pointer, it is assigned to the RSModifier.
     * Otherwise, a new default instance of RSPropertyBase is created and assigned.
     * 
     * @param property A shared pointer to an RSPropertyBase object.
     */
    explicit RSModifier(const std::shared_ptr<RSPropertyBase>& property)
        : property_(property ? property : std::make_shared<RSProperty<bool>>())
    {}

    /**
     * @brief Destructor for RSModifier.
     */
    virtual ~RSModifier() = default;

    /**
     * @brief Get the property associated with this modifier.
     *
     * @return A shared pointer to the RSPropertyBase object representing the property.
     */
    std::shared_ptr<RSPropertyBase> GetProperty()
    {
        return property_;
    }

    /**
     * @brief Get the unique identifier of the property associated with this modifier.
     * 
     * @return PropertyId The unique identifier of the property.
     */
    PropertyId GetPropertyId()
    {
        return property_->id_;
    }

protected:
    RSModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : property_(property ? property : std::make_shared<RSProperty<bool>>())
    {
        property_->type_ = type;
    }

    virtual RSModifierType GetModifierType() const
    {
        return RSModifierType::INVALID;
    }

    std::string GetModifierTypeString() const
    {
        auto modifierTypeString = std::make_shared<RSModifierTypeString>();
        return modifierTypeString->GetModifierTypeString(GetModifierType());
    }

    virtual RSPropertyModifierType GetPropertyModifierType() const = 0;

    std::weak_ptr<RSNode> GetTarget() const
    {
        return property_->target_;
    }

    virtual void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) {}

    void AttachProperty(const std::shared_ptr<RSPropertyBase>& property);

    void AttachToNode(const std::shared_ptr<RSNode> target)
    {
        if (!target) {
            property_->target_.reset();
        } else {
            property_->Attach(*target);
        }
        property_->AttachModifier(shared_from_this());
        MarkNodeDirty();
        OnAttachToNode(target);
    }

    void DetachFromNode()
    {
        OnDetachFromNode();
        MarkNodeDirty();
        property_->Detach();
    }

    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
    {
        property_->SetMotionPathOption(motionPathOption);
    }

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() const
    {
        return property_->GetRenderProperty();
    }

    virtual std::shared_ptr<RSRenderModifier> CreateRenderModifier() const = 0;

    virtual void UpdateToRender() {}

    virtual void OnAttachToNode(const std::weak_ptr<RSNode>& target) {}

    virtual void OnDetachFromNode() {}

    void SetDirty(const bool isDirty, const std::shared_ptr<RSModifierManager>& modifierManager = nullptr);

    virtual void MarkNodeDirty() {}

    void ResetRSNodeExtendModifierDirty();

    bool isDirty_ { false };
    std::shared_ptr<RSPropertyBase> property_;

    friend class RSModifierExtractor;
    friend class RSModifierManager;
    friend class RSNode;
    friend class RSPathAnimation;
    friend class RSPropertyBase;
};

/**
 * @class RSGeometryModifier
 *
 * @brief The class for geometry modifiers.
 */
class RSC_EXPORT RSGeometryModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSGeometryModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSGeometryModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSGeometryModifier.
     */
    virtual ~RSGeometryModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::GEOMETRY;
    }

    void MarkNodeDirty() override
    {
        if (auto node = GetTarget().lock()) {
            node->MarkDirty(NodeDirtyType::GEOMETRY, true);
        }
    }
};

/**
 * @class RSBackgroundModifier
 *
 * @brief The class for background modifiers.
 */
class RSC_EXPORT RSBackgroundModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSBackgroundModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSBackgroundModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSBackgroundModifier.
     */
    virtual ~RSBackgroundModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::BACKGROUND;
    }
};

/**
 * @class RSContentModifier
 *
 * @brief The class for content modifiers.
 */
class RSC_EXPORT RSContentModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSContentModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSContentModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSContentModifier.
     */
    virtual ~RSContentModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::CONTENT;
    }
};

/**
 * @class RSForegroundModifier
 *
 * @brief The class for foreground modifiers.
 */
class RSC_EXPORT RSForegroundModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSForegroundModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSForegroundModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSForegroundModifier.
     */
    virtual ~RSForegroundModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::FOREGROUND;
    }
};

/**
 * @class RSOverlayModifier
 *
 * @brief The class for overlay modifiers.
 */
class RSC_EXPORT RSOverlayModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSOverlayModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSOverlayModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSOverlayModifier.
     */
    virtual ~RSOverlayModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::OVERLAY;
    }
};

/**
 * @class RSAppearanceModifier
 *
 * @brief The class for appearance modifiers.
 */
class RSC_EXPORT RSAppearanceModifier : public RSModifier {
public:
    /**
     * @brief Construct an RSAppearanceModifier object with the specified property and type.
     *
     * @param property A shared pointer to an RSPropertyBase object.
     * @param type The type of the modifier.
     */
    RSAppearanceModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    /**
     * @brief Destructor for RSAppearanceModifier.
     */
    virtual ~RSAppearanceModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::APPEARANCE;
    }

    void MarkNodeDirty() override
    {
        if (auto node = GetTarget().lock()) {
            node->MarkDirty(NodeDirtyType::APPEARANCE, true);
        }
    }
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H
