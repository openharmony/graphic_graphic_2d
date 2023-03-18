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

class RSC_EXPORT RSModifier : public std::enable_shared_from_this<RSModifier> {
public:
    explicit RSModifier(const std::shared_ptr<RSPropertyBase>& property)
        : property_(property ? property : std::make_shared<RSPropertyBase>())
    {}

    virtual ~RSModifier() = default;

    virtual std::shared_ptr<RSPropertyBase> GetProperty()
    {
        return property_;
    }

    virtual PropertyId GetPropertyId()
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

    void AttachProperty(const std::shared_ptr<RSPropertyBase>& property)
    {
        if (property != nullptr) {
            property->target_ = property_->target_;
            property->SetIsCustom(true);
            property->AttachModifier(shared_from_this());
            property->UpdateExtendedProperty();
        }
    }

    void AttachToNode(const std::shared_ptr<RSNode> target)
    {
        property_->target_ = std::weak_ptr<RSNode>(target);
    }

    virtual void DetachFromNode()
    {
        property_->target_.reset();
    }

    virtual void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
    {
        property_->SetMotionPathOption(motionPathOption);
    }

    virtual void UpdateToRender() {}

    virtual std::shared_ptr<RSRenderModifier> CreateRenderModifier() const = 0;

    std::shared_ptr<RSPropertyBase> property_;

    friend class RSModifierExtractor;
    friend class RSModifierManager;
    friend class RSNode;
    friend class RSPathAnimation;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_H
