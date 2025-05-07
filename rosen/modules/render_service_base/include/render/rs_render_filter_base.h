/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_BASE_H
#define ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_BASE_H
#include "modifier/rs_render_property.h"
#include "transaction/rs_marshalling_helper.h"
namespace OHOS {
namespace Rosen {

enum class RSUIFilterType : int16_t {
    NONE = 0,
    // filter type
    BLUR,
    DISPLACEMENT_DISTORT,
    EDGE_LIGHT,
    SOUND_WAVE,
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
    DISPLACEMENT_DISTORT_FACTOR, // Vector2f

    // edge light value type
    EDGE_LIGHT_DETECT_COLOR, // uint32_t
    EDGE_LIGHT_COLOR, // uint32_t
    EDGE_LIGHT_EDGE_THRESHOLD, // float
    EDGE_LIGHT_EDGE_INTENSITY, // float
    EDGE_LIGHT_EDGE_SOFT_THRESHOLD, // float
    EDGE_LIGHT_BLOOM_LEVEL, // int
    EDGE_LIGHT_USE_RAW_COLOR, // bool
    EDGE_LIGHT_GRADIENT, // bool
    EDGE_LIGHT_ALPHA_PROGRESS, // float
    EDGE_LIGHT_ADD_IMAGE, // bool

    //value type
    SOUND_WAVE_COLOR_ONE, //RSCOLOR
    SOUND_WAVE_COLOR_TWO, //RSCOLOR
    SOUND_WAVE_COLOR_THREE, //RSCOLOR
    SOUND_WAVE_COLOR_PROGRESS, //float
    SOUND_WAVE_CENTER_BRIGHTNESS, //float
    SOUND_INTENSITY, //float
    SHOCK_WAVE_ALPHA_ONE, //float
    SHOCK_WAVE_ALPHA_TWO, //float
    SHOCK_WAVE_PROGRESS_ONE, //float
    SHOCK_WAVE_PROGRESS_TWO, //float
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

#endif // ROSEN_ENGINE_CORE_RENDER_FILTER_RENDER_PROPERTY_BASE_H
