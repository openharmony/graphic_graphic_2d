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
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"

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
class RSModifierBase;

template<typename T>
class RS_EXPORT RSProperty {
public:
    RSProperty();
    explicit RSProperty(const T& value);
    virtual ~RSProperty() = default;

    RSProperty& operator=(const T& value)
    {
        Set(value);
        return *this;
    }

    virtual void Set(const T& value);
    virtual T Get() const
    {
        return stagingValue_;
    }

    PropertyId GetId() const
    {
        return id_;
    }

protected:
    void UpdateToRender(const T& value, bool isDelta, bool forceUpdate = false) const;

    void UpdateOnAllAnimationFinish()
    {
        UpdateToRender(stagingValue_, false, true);
    }

    T stagingValue_ {};
    NodeId nodeId_ { 0 };
    PropertyId id_;
    bool hasAddToNode_ { false };
    RSModifierType type_ { RSModifierType::INVALID };
    std::shared_ptr<RSMotionPathOption> motionPathOption_ {};

    template<typename T1>
    friend class RSPathAnimation;
    friend class RSImplicitAnimator;
    template<typename T2>
    friend class RSModifier;
};

template<typename T>
class RS_EXPORT RSAnimatableProperty : public RSProperty<T> {
    // static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> ||
    //     std::is_base_of_v<RSAnimatableArithmetic<T>, T>);
public:
    RSAnimatableProperty() {}
    RSAnimatableProperty(const T& value) : RSProperty<T>(value)
    {
        showingValue_ = value;
    }

    virtual ~RSAnimatableProperty() = default;

    void Set(const T& value) override;

    T Get() const override
    {
        if (isCustom_) {
            return showingValue_;
        }
        return RSProperty<T>::stagingValue_;
    }

protected:
    void SetIsCustom(bool isCustom)
    {
        isCustom_ = isCustom;
    }

    void SetShowingValue(const T& value)
    {
        showingValue_ = value;
    }

    void AttachModifier(const std::shared_ptr<RSModifierBase>& modifier)
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

    T showingValue_ {};
    int runningPathNum_ { 0 };
    bool isCustom_ { false };
    std::weak_ptr<RSModifierBase> modifier_ {};

    template<typename T1>
    friend class RSPropertyAnimation;
    template<typename T1>
    friend class RSPathAnimation;
    friend class RSUIAnimationManager;
    template<typename T1>
    friend class RSExtendedModifier;
    template<typename T1>
    friend class RSModifier;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROP_H
