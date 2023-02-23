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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H

#include <memory>
#include "parcel.h"

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"


namespace OHOS {
namespace Rosen {
class RSProperties;
class RSPaintFilterCanvas;

struct RSModifierContext {
    RSProperties& property_;
    RSPaintFilterCanvas* canvas_ = nullptr;
};

class RSB_EXPORT RSRenderModifier {
public:
    RSRenderModifier() = default;
    virtual ~RSRenderModifier() = default;

    virtual void Apply(RSModifierContext& context) = 0;

    virtual PropertyId GetPropertyId() = 0;
    virtual std::shared_ptr<RSRenderPropertyBase> GetProperty() = 0;
    virtual RSModifierType GetType() = 0;
    virtual void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) = 0;

    virtual bool Marshalling(Parcel& parcel) = 0;
    static RSRenderModifier* Unmarshalling(Parcel& parcel);
};

class RSB_EXPORT RSDrawCmdListRenderModifier : public RSRenderModifier {
public:
    RSDrawCmdListRenderModifier(const std::shared_ptr<RSRenderProperty<DrawCmdListPtr>>& property)
        : property_(property ? property : std::make_shared<RSRenderProperty<DrawCmdListPtr>>())
    {}
    virtual ~RSDrawCmdListRenderModifier() = default;
    void Apply(RSModifierContext& context) override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;

    virtual PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

    RSModifierType GetType() override
    {
        return drawStyle_;
    }
    void SetType(RSModifierType type)
    {
        drawStyle_ = type;
    }

    void SetOverlayBounds(std::shared_ptr<RectI> rect)
    {
        overlayRect_ = rect;
    }

    std::shared_ptr<RectI> GetOverlayBounds() const
    {
        return overlayRect_;
    }

protected:
    std::shared_ptr<RectI> overlayRect_ = nullptr;
    RSModifierType drawStyle_ = RSModifierType::EXTENDED;
    std::shared_ptr<RSRenderProperty<DrawCmdListPtr>> property_;
};

class RSAnimatableRenderModifier : public RSRenderModifier {
public:
    RSAnimatableRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : property_(property ? property : std::make_shared<RSRenderPropertyBase>())
    {}

    virtual ~RSAnimatableRenderModifier() = default;

    virtual PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

protected:
    std::shared_ptr<RSRenderPropertyBase> property_;

    friend class RSRenderPropertyAnimation;
};

// declare RenderModifiers like RSBoundsRenderModifier
#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP)                        \
    class RSB_EXPORT RS##MODIFIER_NAME##RenderModifier : public RSAnimatableRenderModifier {             \
    public:                                                                                              \
        RS##MODIFIER_NAME##RenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)         \
            : RSAnimatableRenderModifier(property)                                                       \
        {}                                                                                               \
        virtual ~RS##MODIFIER_NAME##RenderModifier() = default;                                          \
        void Apply(RSModifierContext& context) override;                                                 \
        void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;           \
        bool Marshalling(Parcel& parcel) override;                                                       \
        RSModifierType GetType() override                                                                \
        {                                                                                                \
            return RSModifierType::MODIFIER_TYPE;                                                        \
        }                                                                                                \
    };

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
    DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, Add)

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H
