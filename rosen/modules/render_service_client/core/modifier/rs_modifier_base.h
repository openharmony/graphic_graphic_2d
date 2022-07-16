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

class RS_EXPORT RSModifier {
public:
    RSModifier() = default;
    virtual ~RSModifier() = default;

    virtual PropertyId GetPropertyId() = 0;

    virtual RSModifierType GetModifierType() const = 0;
protected:
    virtual void MarkAddToNode(const NodeId& id) = 0;
    virtual void MarkRemoveFromNode() = 0;

    virtual void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) = 0;
    virtual void SetIsAdditive(bool isAdditive) = 0;

    virtual void UpdateToRender() = 0;
    virtual std::shared_ptr<RSRenderModifier> CreateRenderModifier() const = 0;

    friend class RSNode;
    template<typename T>
    friend class RSProperty;
    friend class RSModifierManager;
};

template<typename T>
class RS_EXPORT RSAnimatableModifier : public RSModifier {
public:
    explicit RSAnimatableModifier(const std::shared_ptr<RSProperty<T>> property)
        : property_(property ? property : std::make_shared<RSProperty<T>>())
    {}

    virtual ~RSAnimatableModifier() = default;

    PropertyId GetPropertyId() override
    {
        return property_->id_;
    }

    std::shared_ptr<RSProperty<T>> GetProperty()
    {
        return property_;
    }

    RSModifierType GetModifierType() const override
    {
        return RSModifierType::INVALID;
    }

protected:
    RSAnimatableModifier(const std::shared_ptr<RSProperty<T>> property, const RSModifierType type)
        : property_(property ? property : std::make_shared<RSProperty<T>>())
    {
        property_->type_ = type;
    }

    void Update(const T& value) {
        property_->Set(value);
    }

    void MarkAddToNode(const NodeId& id) override
    {
        property_->hasAddToNode_ = true;
        property_->nodeId_ = id;
    }

    void MarkRemoveFromNode() override
    {
        property_->hasAddToNode_ = false;
    }

    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) override
    {
        property_->motionPathOption_ = motionPathOption;
    }

    void SetIsAdditive(bool isAdditive) override
    {
        isAdditive_ = isAdditive;
    }

    void UpdateToRender() override {}

    std::shared_ptr<RSProperty<T>> property_;
    bool isAdditive_ { false };

    friend class RSModifierExtractor;
    friend class RSNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_BASE_H
