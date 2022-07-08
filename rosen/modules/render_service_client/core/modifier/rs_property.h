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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROP_H
 
#include <type_traits>
#include <unistd.h>

#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_render_property.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_type.h"
#include "animation/rs_motion_path_option.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
class RSModifier;

template<typename T>
class RS_EXPORT RSProperty {
    // static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> ||
    //     std::is_base_of_v<RSAnimatableArithmetic<T>, T>);
public:
    RSProperty();
    RSProperty(const T& value);
    virtual ~RSProperty() = default;

    RSProperty& operator=(const T& value)
    {
        Set(value);
        return *this;
    }

    RSProperty& operator+=(const T& value);

    virtual void Set(const T& value);
    T Get() const
    {
        if (isCustom_) {
            return animatingValue_;
        }
        return stagingValue_;
    }

    PropertyId GetId() const
    {
        return id_;
    }

protected:
    void UpdateToRender(const T& value, bool isDelta) const;
    void AttachModifier(const std::shared_ptr<RSModifier>& modifier)
    {
        modifier_ = modifier;
    }

    void MarkModifierDirty(const std::shared_ptr<RSModifierManager>& modifierManager)
    {
        auto modifier = modifier_.lock();
        if (modifier != nullptr && modifierManager != nullptr) {
            modifierManager->AddModifier(modifier);
        }
    }

    void SetAnimatingValue(const T& value)
    {
        animatingValue_ = value;
    }

    void SetIsCustom(bool isCustom)
    {
        isCustom_ = isCustom;
    }

    T animatingValue_;
    T stagingValue_;
    NodeId nodeId_;
    PropertyId id_;
    bool hasAddToNode_ { false };
    bool isCustom_ { false };
    int runningPathNum_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;
    std::weak_ptr<RSModifier> modifier_;
    RSModifierType type_ = RSModifierType::INVALID;

    template<typename T1>
    friend class RSPropertyAnimation;
    template<typename T1>
    friend class RSPathAnimation;
    friend class RSImplicitAnimator;
    template<typename T2>
    friend class RSAnimatableModifier;
    template<typename T2>
    friend class RSExtendedModifier;
    friend class RSUIAnimationManager;
};

template<typename T>
class RS_EXPORT RSAnimatableProperty : public RSProperty<T> {
    // static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> ||
    //     std::is_base_of_v<RSAnimatableArithmetic<T>, T>);
public:
    RSAnimatableProperty() {}
    RSAnimatableProperty(const T& value) : RSProperty<T>(value) {}
    virtual ~RSAnimatableProperty() = default;

    void Set(const T& value) override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROP_H
