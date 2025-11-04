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
    virtual RectF CalcRect(const RectF& bound, EffectRectType type) const { return bound; }

protected:
    RSUIFilterType type_;
    std::map<RSUIFilterType, std::shared_ptr<RSRenderPropertyBase>> properties_;
    float geoWidth_ = 0.f;
    float geoHeight_ = 0.f;
    float tranX_ = 0.f;
    float tranY_ = 0.f;
    Drawing::Matrix mat_;
    uint32_t basicHash_ = 0;
    uint32_t hash_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_FILTER_BASE_H
