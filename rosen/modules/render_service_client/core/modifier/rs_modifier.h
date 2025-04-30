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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H

#include "common/rs_macros.h"
#include "common/rs_obj_abs_geometry.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSModifier : public std::enable_shared_from_this<RSModifier> {
public:
    explicit RSModifier(const std::shared_ptr<RSPropertyBase>& property)
        : property_(property ? property : std::make_shared<RSPropertyBase>())
    {}

    virtual ~RSModifier() = default;

    std::shared_ptr<RSPropertyBase> GetProperty()
    {
        return property_;
    }

    PropertyId GetPropertyId()
    {
        return property_->id_;
    }

protected:
    RSModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : property_(property ? property : std::make_shared<RSPropertyBase>())
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
        property_->target_ = std::weak_ptr<RSNode>(target);
        property_->AttachModifier(shared_from_this());
        MarkNodeDirty();
        OnAttachToNode(target);
    }

    void DetachFromNode()
    {
        OnDetachFromNode();
        MarkNodeDirty();
        property_->target_.reset();
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

class RSC_EXPORT RSGeometryModifier : public RSModifier {
public:
    RSGeometryModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

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

class RSC_EXPORT RSBackgroundModifier : public RSModifier {
public:
    RSBackgroundModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    virtual ~RSBackgroundModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::BACKGROUND;
    }
};

class RSC_EXPORT RSContentModifier : public RSModifier {
public:
    RSContentModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    virtual ~RSContentModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::CONTENT;
    }
};

class RSC_EXPORT RSForegroundModifier : public RSModifier {
public:
    RSForegroundModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    virtual ~RSForegroundModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::FOREGROUND;
    }
};

class RSC_EXPORT RSOverlayModifier : public RSModifier {
public:
    RSOverlayModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

    virtual ~RSOverlayModifier() = default;

protected:
    RSPropertyModifierType GetPropertyModifierType() const override
    {
        return RSPropertyModifierType::OVERLAY;
    }
};

class RSC_EXPORT RSAppearanceModifier : public RSModifier {
public:
    RSAppearanceModifier(const std::shared_ptr<RSPropertyBase>& property, const RSModifierType type)
        : RSModifier(property, type)
    {}

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

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H
