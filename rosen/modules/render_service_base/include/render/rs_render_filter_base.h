/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_RENDER_FILTER_BASE_H
#define RENDER_SERVICE_BASE_RENDER_FILTER_BASE_H

#include "modifier/rs_render_property.h"
#include "transaction/rs_marshalling_helper.h"
namespace OHOS {
namespace Rosen {

enum class RSUIFilterType : int16_t {
    NONE = 0,
    // filter type
    BLUR,
    DISPLACEMENT_DISTORT,
    COLOR_GRADIENT,

    // mask type
    RIPPLE_MASK,
    RADIAL_GRADIENT_MASK,
    PIXEL_MAP_MASK,

    // value type
    BLUR_RADIUS_X,  // float
    BLUR_RADIUS_Y,  // float

    // value type
    RIPPLE_MASK_CENTER, // Vector2f
    RIPPLE_MASK_RADIUS, // float
    RIPPLE_MASK_WIDTH, // float
    RIPPLE_MASK_WIDTH_CENTER_OFFSET, // float
    DISPLACEMENT_DISTORT_FACTOR, // Vector2f

    COLOR_GRADIENT_COLOR, // std::vector<float>
    COLOR_GRADIENT_POSITION, // std::vector<float>
    COLOR_GRADIENT_STRENGTH, // std::vector<float>
};

class RSB_EXPORT RSRenderFilterParaBase : public RSRenderPropertyBase,
    public std::enable_shared_from_this<RSRenderFilterParaBase> {
public:

    RSRenderFilterParaBase(RSUIFilterType type) : type_(type) {}
    virtual ~RSRenderFilterParaBase() = default;

    RSUIFilterType GetType() const;

    virtual bool IsValid() const;

    inline void Setter(RSUIFilterType type, const std::shared_ptr<RSRenderPropertyBase>& property)
    {
        properties_[type] = property;
        if (properties_[type]) {
            properties_[type]->SetModifierType(RSModifierType::BACKGROUND_UI_FILTER);
        }
    }

    virtual void GetDescription(std::string& out) const {}

    void Dump(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel);

    virtual bool ReadFromParcel(Parcel& parcel);

    std::shared_ptr<RSRenderPropertyBase> GetRenderPropert(RSUIFilterType type) const;

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties();

protected:
    RSUIFilterType type_;
    std::map<RSUIFilterType, std::shared_ptr<RSRenderPropertyBase>> properties_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_FILTER_BASE_H
