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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_BASE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_BASE_H

#include "common/rs_macros.h"
#include "modifier/rs_property.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
struct RSDrawingContext {
    SkCanvas* canvas;
    float width;
    float height;
};
class RSRenderModifier;

class RS_EXPORT RSModifierBase {
public:
    RSModifierBase() = default;
    virtual ~RSModifierBase() = default;

    virtual PropertyId GetPropertyId() = 0;

    virtual std::shared_ptr<RSPropertyBase> GetProperty()
    {
        return nullptr;
    }

protected:
    virtual RSModifierType GetModifierType() const = 0;

    virtual void AttachToNode(const NodeId& id) = 0;
    virtual void DetachFromNode() = 0;

    virtual void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) = 0;

    virtual void UpdateToRender() = 0;
    virtual std::shared_ptr<RSRenderModifier> CreateRenderModifier() const = 0;

    friend class RSNode;
    template<typename T>
    friend class RSProperty;
    friend class RSModifierManager;
    friend class RSPathAnimation;
    friend class RSModifierExtractor;
};

template<typename T>
class RS_EXPORT RSModifier : public RSModifierBase {
public:
    explicit RSModifier(const std::shared_ptr<T>& property)
        : property_(property ? property : std::make_shared<T>())
    {}

    virtual ~RSModifier() = default;

    PropertyId GetPropertyId() override
    {
        return property_->id_;
    }

    std::shared_ptr<RSPropertyBase> GetProperty() override
    {
        return property_;
    }

protected:
    RSModifier(const std::shared_ptr<T>& property, const RSModifierType type)
        : property_(property ? property : std::make_shared<T>())
    {
        property_->type_ = type;
    }

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::INVALID;
    }

    void AttachToNode(const NodeId& id) override
    {
        property_->hasAddToNode_ = true;
        property_->nodeId_ = id;
    }

    void DetachFromNode() override
    {
        property_->hasAddToNode_ = false;
    }

    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) override
    {
        property_->motionPathOption_ = motionPathOption;
    }

    void UpdateToRender() override {}

    std::shared_ptr<T> property_;

    friend class RSModifierExtractor;
    friend class RSNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_BASE_H
