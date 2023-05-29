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

#include "modifier/rs_render_modifier.h"
#ifndef USE_ROSEN_DRAWING
#else
#include "draw/color.h"
#include "image/bitmap.h"
#endif
#include "modifier/rs_modifier_type.h"
#include <memory>
#include <unordered_map>
#include "pixel_map.h"

#include "common/rs_obj_abs_geometry.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER)        \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                          \
            std::shared_ptr<RSRenderAnimatableProperty<TYPE>> prop;                                     \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                    \
                return nullptr;                                                                         \
            }                                                                                           \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                \
            if (!modifier) {                                                                            \
                return nullptr;                                                                         \
            }                                                                                           \
            return modifier;                                                                            \
        },                                                                                              \
    },

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                          \
            std::shared_ptr<RSRenderProperty<TYPE>> prop;                                               \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                    \
                return nullptr;                                                                         \
            }                                                                                           \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                \
            if (!modifier) {                                                                            \
                return nullptr;                                                                         \
            }                                                                                           \
            return modifier;                                                                            \
        },                                                                                              \
    },

static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> funcLUT = {
#include "modifier/rs_modifiers_def.in"
    { RSModifierType::EXTENDED, [](Parcel& parcel) -> RSRenderModifier* {
#ifndef USE_ROSEN_DRAWING
            std::shared_ptr<RSRenderProperty<std::shared_ptr<DrawCmdList>>> prop;
#else
            std::shared_ptr<RSRenderProperty<std::shared_ptr<Drawing::DrawCmdList>>> prop;
#endif
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                return nullptr;
            }
            RSDrawCmdListRenderModifier* modifier = new RSDrawCmdListRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<Color>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<ForegroundColorStrategyType>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorStrategyRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },

    { RSModifierType::GEOMETRYTRANS, [](Parcel& parcel) -> RSRenderModifier* {
#ifndef USE_ROSEN_DRAWING
            std::shared_ptr<RSRenderProperty<SkMatrix>> prop;
#else
            std::shared_ptr<RSRenderProperty<Drawing::Matrix>> prop;
#endif
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                return nullptr;
            }
            auto modifier = new RSGeometryTransRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            return modifier;
        },
    },
};

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
}

void RSDrawCmdListRenderModifier::Apply(RSModifierContext& context) const
{
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrame(context.property_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
#ifndef USE_ROSEN_DRAWING
    if (auto property = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)) {
        property_->Set(property->Get());
    }
#else
    if (auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)) {
        property_->Set(property->Get());
    }
#endif
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
        RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
}

RectF RSDrawCmdListRenderModifier::GetCmdsClipRect() const
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    auto cmds = property_->Get();
    return RSPropertiesPainter::GetCmdsClipRect(cmds);
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}

void RSDrawCmdListRenderModifier::ApplyForDrivenContent(RSModifierContext& context) const
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrameForDriven(context.property_, *context.canvas_, cmds);
    }
#endif
}

bool RSEnvForegroundColorRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
}

void RSEnvForegroundColorRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    context.canvas_->SetEnvForegroundColor(renderProperty->Get());
}

void RSEnvForegroundColorRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSEnvForegroundColorStrategyRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    return parcel.WriteInt16(static_cast<short>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
}


void RSEnvForegroundColorStrategyRenderModifier ::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    switch (renderProperty->Get()) {
        case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
            // calculate the color by screebshot
            Color color = GetInvertBackgroundColor(context);
            context.canvas_->SetEnvForegroundColor(color);
            break;
        }
        default: {
            break;
        }
    }
}

Color RSEnvForegroundColorStrategyRenderModifier::CalculateInvertColor(Color backgroundColor) const
{
    uint32_t a = backgroundColor.GetAlpha();
    uint32_t r = 255 - backgroundColor.GetRed();
    uint32_t g = 255 - backgroundColor.GetGreen();
    uint32_t b = 255 - backgroundColor.GetBlue();
    return Color(r, g, b, a);
}

Color RSEnvForegroundColorStrategyRenderModifier::GetInvertBackgroundColor(RSModifierContext& context) const
{
#ifdef ROSEN_OHOS
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(context.canvas_, true);
#else
    Drawing::AutoCanvasRestore acr(*context.canvas_, true);
#endif
    if (!context.property_.GetClipToBounds()) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor not GetClipToBounds");
        Vector4f clipRegion = context.property_.GetBounds();
#ifndef USE_ROSEN_DRAWING
        SkRect rect = SkRect::MakeXYWH(0, 0, clipRegion.z_, clipRegion.w_);
        context.canvas_->clipRect(rect);
#else
        Drawing::Rect rect = Drawing::Rect(0, 0, clipRegion.z_, clipRegion.w_);
        context.canvas_->ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
#endif
    }
    Color backgroundColor = context.property_.GetBackgroundColor();
    if (backgroundColor.GetAlpha() == 0xff) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor not alpha");
        return CalculateInvertColor(backgroundColor);
    }
#ifndef USE_ROSEN_DRAWING
    auto imageSnapshot = context.canvas_->GetSurface()->makeImageSnapshot(context.canvas_->getDeviceClipBounds());
#else
    auto imageSnapshot = context.canvas_->GetSurface()->GetImageSnapshot(context.canvas_->GetDeviceClipBounds());
#endif
    if (imageSnapshot == nullptr) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor imageSnapshot null");
        return Color(0);
    }
    Media::InitializationOptions opts;
    opts.size.width = context.property_.GetBoundsWidth();
    opts.size.height = context.property_.GetBoundsHeight();
    if (opts.size.width == 0 || opts.size.height == 0) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor opts.size.width/height == 0");
        return Color(0);
    }
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    uint8_t* data = static_cast<uint8_t*>(malloc(pixelmap->GetRowBytes() * pixelmap->GetHeight()));
    if (data == nullptr) {
        RS_LOGE("RSRenderModifier::GetInvertBackgroundColor: data is nullptr");
        return Color(0);
    }
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!imageSnapshot->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
#else
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(data);
    if (!imageSnapshot->ReadPixels(bitmap, 0, 0)) {
#endif
        RS_LOGE("RSRenderModifier::Run: readPixels failed");
        free(data);
        data = nullptr;
        return Color(0);
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
    OHOS::Media::InitializationOptions options;
    options.alphaType = pixelmap->GetAlphaType();
    options.pixelFormat = pixelmap->GetPixelFormat();
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap.get(), options);
    uint32_t colorVal = 0;
    newPixelMap->GetARGB32Color(0, 0, colorVal);
    return CalculateInvertColor(Color(colorVal));
#else
    return Color(0);
#endif
}

void RSEnvForegroundColorStrategyRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(property_);
        renderProperty->Set(property->Get());
    }
}

void RSGeometryTransRenderModifier::Apply(RSModifierContext& context) const
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(context.property_.GetBoundsGeometry());
    auto property = property_->Get();
    geoPtr->ConcatMatrix(property);
}

void RSGeometryTransRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
#ifndef USE_ROSEN_DRAWING
    if (auto property = std::static_pointer_cast<RSRenderProperty<SkMatrix>>(prop)) {
#else
    if (auto property = std::static_pointer_cast<RSRenderProperty<Drawing::Matrix>>(prop)) {
#endif
        property_->Set(property->Get());
    }
}

bool RSGeometryTransRenderModifier::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::GEOMETRYTRANS)) &&
           RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        return nullptr;
    }
    auto it = funcLUT.find(static_cast<RSModifierType>(type));
    if (it == funcLUT.end()) {
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %d", type);
        return nullptr;
    }
    return it->second(parcel);
}

namespace {
template<typename T>
T Add(T a, T b)
{
    return a + b;
}
template<typename T>
T Multiply(T a, T b)
{
    return a * b;
}
template<typename T>
T Replace(T a, T b)
{
    return b;
}
} // namespace

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER)                       \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                                \
    {                                                                                                                  \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                   \
        return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                               \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                               \
    }                                                                                                                  \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                    \
    {                                                                                                                  \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                   \
        context.property_.Set##MODIFIER_NAME(DELTA_OP(context.property_.Get##MODIFIER_NAME(), renderProperty->Get())); \
    }                                                                                                                  \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)    \
    {                                                                                                                  \
        if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(prop)) {                        \
            auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);               \
            renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());                \
        }                                                                                                              \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                            \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        return parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                              \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                 \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        context.property_.Set##MODIFIER_NAME(renderProperty->Get());                                                \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) \
    {                                                                                                               \
        if (auto property = std::static_pointer_cast<RSRenderProperty<TYPE>>(prop)) {                               \
            auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                      \
            renderProperty->Set(property->Get());                                                                   \
        }                                                                                                           \
    }

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
