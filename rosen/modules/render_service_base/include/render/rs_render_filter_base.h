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

#include "draw/canvas.h"
#include "effect/rs_render_property_tag.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
class GEVisualEffectContainer;
class GEVisualEffect;
struct CanvasInfo;
} // namespace Drawing

enum class RSUIFilterType : int16_t {
    INVALID = -1,
    NONE = 0,
    // filter type
    BLUR,
    DISPLACEMENT_DISTORT,
    COLOR_GRADIENT,
    SOUND_WAVE,

    EDGE_LIGHT,
    BEZIER_WARP,
    DISPERSION,

    AIBAR,
    GREY,
    MATERIAL,
    MAGNIFIER,
    MESA,
    MASK_COLOR,
    KAWASE,
    LIGHT_BLUR,
    PIXEL_STRETCH,
    WATER_RIPPLE,

    LINEAR_GRADIENT_BLUR,
    FLY_OUT,
    DISTORTION,
    ALWAYS_SNAPSHOT,
    // mask type
    RIPPLE_MASK,
    DOUBLE_RIPPLE_MASK,
    RADIAL_GRADIENT_MASK,
    PIXEL_MAP_MASK,
    WAVE_GRADIENT_MASK,

    // value type
    BLUR_RADIUS_X,  // float
    BLUR_RADIUS_Y,  // float

    // value type
    RIPPLE_MASK_CENTER, // Vector2f
    RIPPLE_MASK_RADIUS, // float
    RIPPLE_MASK_WIDTH, // float
    RIPPLE_MASK_WIDTH_CENTER_OFFSET, // float
    DISPLACEMENT_DISTORT_FACTOR, // Vector2f

    // value type
    DOUBLE_RIPPLE_MASK_CENTER1, // Vector2f
    DOUBLE_RIPPLE_MASK_CENTER2, // Vector2f
    DOUBLE_RIPPLE_MASK_RADIUS, // float
    DOUBLE_RIPPLE_MASK_WIDTH, // float
    DOUBLE_RIPPLE_MASK_TURBULENCE, // float

    // value type
    COLOR_GRADIENT_COLOR, // vector<float>
    COLOR_GRADIENT_POSITION, // vector<float>
    COLOR_GRADIENT_STRENGTH, // vector<float>

    //value type
    SOUND_WAVE_COLOR_A, //RSCOLOR
    SOUND_WAVE_COLOR_B, //RSCOLOR
    SOUND_WAVE_COLOR_C, //RSCOLOR
    SOUND_WAVE_COLOR_PROGRESS, //float
    SOUND_INTENSITY, //float
    SHOCK_WAVE_ALPHA_A, //float
    SHOCK_WAVE_ALPHA_B, //float
    SHOCK_WAVE_PROGRESS_A, //float
    SHOCK_WAVE_PROGRESS_B, //float
    SHOCK_WAVE_TOTAL_ALPHA, //float

    // edge light value type
    EDGE_LIGHT_ALPHA, // float
    EDGE_LIGHT_COLOR, // Vector4f

    // bezier warp value type
    BEZIER_CONTROL_POINT0, // Vector2f
    BEZIER_CONTROL_POINT1, // Vector2f
    BEZIER_CONTROL_POINT2, // Vector2f
    BEZIER_CONTROL_POINT3, // Vector2f
    BEZIER_CONTROL_POINT4, // Vector2f
    BEZIER_CONTROL_POINT5, // Vector2f
    BEZIER_CONTROL_POINT6, // Vector2f
    BEZIER_CONTROL_POINT7, // Vector2f
    BEZIER_CONTROL_POINT8, // Vector2f
    BEZIER_CONTROL_POINT9, // Vector2f
    BEZIER_CONTROL_POINT10, // Vector2f
    BEZIER_CONTROL_POINT11, // Vector2f

    // pixel map mask value type
    PIXEL_MAP_MASK_PIXEL_MAP, // Media::PixelMap
    PIXEL_MAP_MASK_SRC, // Vector4f
    PIXEL_MAP_MASK_DST, // Vector4f
    PIXEL_MAP_MASK_FILL_COLOR, // Vector4f

    // dispersion value type
    DISPERSION_OPACITY, // float
    DISPERSION_RED_OFFSET, // Vector2f
    DISPERSION_GREEN_OFFSET, // Vector2f
    DISPERSION_BLUE_OFFSET, // Vector2f

    // edge light bloom value type
    EDGE_LIGHT_BLOOM, // bool

    // radial gradient mask value type
    RADIAL_GRADIENT_MASK_CENTER, // Vector2f
    RADIAL_GRADIENT_MASK_RADIUSX, // float
    RADIAL_GRADIENT_MASK_RADIUSY, // float
    RADIAL_GRADIENT_MASK_COLORS, // vector<float>
    RADIAL_GRADIENT_MASK_POSITIONS, // vector<float>

    // wave gradient mask value type
    WAVE_GRADIENT_MASK_WAVE_CENTER, // Vector2f
    WAVE_GRADIENT_MASK_WAVE_WIDTH, // float
    WAVE_GRADIENT_MASK_TURBULENCE_STRENGTH, // float
    WAVE_GRADIENT_MASK_BLUR_RADIUS, // float
    WAVE_GRADIENT_MASK_PROPAGATION_RADIUS, // float

    //content light filter value type
    CONTENT_LIGHT,
    LIGHT_POSITION,
    LIGHT_COLOR,
    LIGHT_INTENSITY,

    // edge light if use raw color value type
    EDGE_LIGHT_USE_RAW_COLOR, // bool
};

using RSUIFilterTypeUnderlying = std::underlying_type<RSUIFilterType>::type;

class RSB_EXPORT RSRenderFilterParaBase : public RSRenderPropertyBase,
    public std::enable_shared_from_this<RSRenderFilterParaBase> {
public:
    RSRenderFilterParaBase() = default;
    RSRenderFilterParaBase(RSUIFilterType type) : type_(type) {}
    virtual ~RSRenderFilterParaBase() = default;

    virtual std::shared_ptr<RSRenderFilterParaBase> DeepCopy() const
    {
        return nullptr;
    }

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

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty(RSUIFilterType type) const;

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties();

    virtual bool ParseFilterValues()
    {
        return false;
    }

    virtual void PreProcess(std::shared_ptr<Drawing::Image>& image) {};
    virtual std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
    {
        return image;
    }

    virtual void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) {};
    virtual void PostProcess(Drawing::Canvas& canvas) {};

    virtual bool NeedForceSubmit() const { return false; }

    uint32_t Hash() const
    {
        return hash_;
    }
    RSPropertyType GetPropertyType() const override {return RSPropertyType::INVALID;}
    size_t GetSize() const override {return sizeof(*this);}
    bool Marshalling(Parcel& parcel) override {return false;}

    virtual void SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight);
    Drawing::CanvasInfo GetFilterCanvasInfo() const;

protected:
    RSUIFilterType type_;
    std::map<RSUIFilterType, std::shared_ptr<RSRenderPropertyBase>> properties_;
    float geoWidth_ = 0.f;
    float geoHeight_ = 0.f;
    float tranX_ = 0.f;
    float tranY_ = 0.f;
    Drawing::Matrix mat_;
    uint32_t hash_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_FILTER_BASE_H
