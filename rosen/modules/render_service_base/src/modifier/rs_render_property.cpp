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

#include "modifier/rs_render_property.h"

#include <iomanip>

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {

void RSRenderPropertyBase::OnChange() const
{
    if (auto node = node_.lock()) {
        ROSEN_LOGD("Node id %{public}" PRIu64 " set dirty, property changed", node->GetId());
        node->SetDirty();
        node->AddDirtyType(modifierType_);
        if (modifierType_ < RSModifierType::BOUNDS || modifierType_ > RSModifierType::TRANSLATE_Z ||
            modifierType_ == RSModifierType::POSITION_Z) {
            node->MarkNonGeometryChanged();
        }
        if (modifierType_ > RSModifierType::EXTENDED) {
            node->SetContentDirty();
        }
        if (modifierType_ == RSModifierType::POSITION_Z) {
            node->MarkParentNeedRegenerateChildren();
        }
    }
}

void RSRenderPropertyBase::UpdatePropertyUnit(RSModifierType type)
{
    switch (type) {
        case RSModifierType::FRAME:
        case RSModifierType::TRANSLATE:
            SetPropertyUnit(RSPropertyUnit::PIXEL_POSITION);
            break;
        case RSModifierType::SCALE:
            SetPropertyUnit(RSPropertyUnit::RATIO_SCALE);
            break;
        case RSModifierType::ROTATION_X:
        case RSModifierType::ROTATION_Y:
        case RSModifierType::ROTATION:
            SetPropertyUnit(RSPropertyUnit::ANGLE_ROTATION);
            break;
        default:
            SetPropertyUnit(RSPropertyUnit::UNKNOWN);
            break;
    }
}

bool RSRenderPropertyBase::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val)
{
    if (val == nullptr) {
        parcel.WriteUint16(static_cast<int16_t>(RSModifierType::INVALID));
        return true;
    }
    RSRenderPropertyType type = val->GetPropertyType();
    if (!(parcel.WriteInt16(static_cast<int16_t>(type)))) {
        return false;
    }
    RSPropertyUnit unit = val->GetPropertyUnit();
    if (!(parcel.WriteInt16(static_cast<int16_t>(unit)))) {
        return false;
    }
    switch (type) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_MATRIX3F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Matrix3f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_QUATERNION: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_FILTER: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_RRECT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<RRect>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        default: {
            return false;
        }
    }
    return true;
}

bool RSRenderPropertyBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val)
{
    int16_t typeId = 0;
    if (!parcel.ReadInt16(typeId)) {
        return false;
    }
    RSRenderPropertyType type = static_cast<RSRenderPropertyType>(typeId);
    if (type == RSRenderPropertyType::INVALID) {
        val.reset();
        return true;
    }
    int16_t unitId = 0;
    if (!parcel.ReadInt16(unitId)) {
        return false;
    }
    RSPropertyUnit unit = static_cast<RSPropertyUnit>(unitId);
    PropertyId id = 0;
    if (!parcel.ReadUint64(id)) {
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, id);
    switch (type) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            float value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<float>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            Color value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Color>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_MATRIX3F: {
            Matrix3f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Matrix3f>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_QUATERNION: {
            Quaternion value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Quaternion>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_FILTER: {
            std::shared_ptr<RSFilter> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            Vector2f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector2f>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            Vector4f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector4f>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            Vector4<Color> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector4<Color>>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_RRECT: {
            RRect value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<RRect>(value, id, type, unit));
            break;
        }
        default: {
            return false;
        }
    }
    return val != nullptr;
}

template<>
float RSRenderAnimatableProperty<float>::ToFloat() const
{
    return std::fabs(RSRenderProperty<float>::stagingValue_);
}

template<>
float RSRenderAnimatableProperty<Vector4f>::ToFloat() const
{
    return RSRenderProperty<Vector4f>::stagingValue_.GetLength();
}

template<>
float RSRenderAnimatableProperty<Quaternion>::ToFloat() const
{
    return RSRenderProperty<Quaternion>::stagingValue_.GetLength();
}

template<>
float RSRenderAnimatableProperty<Vector2f>::ToFloat() const
{
    return RSRenderProperty<Vector2f>::stagingValue_.GetLength();
}

std::shared_ptr<RSRenderPropertyBase> operator+=(
    const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Add(b);
}

std::shared_ptr<RSRenderPropertyBase> operator-=(
    const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Minus(b);
}

std::shared_ptr<RSRenderPropertyBase> operator*=(const std::shared_ptr<RSRenderPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Multiply(scale);
}

std::shared_ptr<RSRenderPropertyBase> operator+(
    const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Clone()->Add(b);
}

std::shared_ptr<RSRenderPropertyBase> operator-(
    const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Clone()->Minus(b);
}

std::shared_ptr<RSRenderPropertyBase> operator*(
    const std::shared_ptr<const RSRenderPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Clone()->Multiply(scale);
}

bool operator==(
    const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->IsEqual(b);
}

bool operator!=(
    const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return !a->IsEqual(b);
}

template<>
void RSRenderProperty<int>::Dump(std::string& out) const
{
    out += "[" + std::to_string(Get()) + "]";
}

template<>
void RSRenderProperty<float>::Dump(std::string& out) const
{
    std::stringstream ss;
    ss << "[" << std::fixed << std::setprecision(1) << Get() << "]";
    out += ss.str();
}

template<>
void RSRenderProperty<Vector4<uint32_t>>::Dump(std::string& out) const
{
    Vector4 v4 = Get();
    switch (modifierType_) {
        case RSModifierType::BORDER_STYLE:
        case RSModifierType::OUTLINE_STYLE: {
            out += "[left:" + std::to_string(v4.x_);
            out += " top:" + std::to_string(v4.y_);
            out += " right:" + std::to_string(v4.z_);
            out += " bottom:" + std::to_string(v4.w_) + "]";
            break;
        }
        default: {
            out += "[x:" + std::to_string(v4.x_) + " y:";
            out += std::to_string(v4.y_) + " z:";
            out += std::to_string(v4.z_) + " w:";
            out += std::to_string(v4.w_) + "]";
            break;
        }
    }
}

template<>
void RSRenderProperty<Vector4f>::Dump(std::string& out) const
{
    Vector4f v4f = Get();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    switch (modifierType_) {
        case RSModifierType::BORDER_WIDTH:
        case RSModifierType::BORDER_DASH_WIDTH:
        case RSModifierType::BORDER_DASH_GAP:
        case RSModifierType::OUTLINE_WIDTH:
        case RSModifierType::OUTLINE_DASH_WIDTH:
        case RSModifierType::OUTLINE_DASH_GAP: {
            ss << "[left:" << v4f.x_ << " top:" << v4f.y_ << " right:" << v4f.z_ << " bottom:" << v4f.w_ << + "]";
            break;
        }
        case RSModifierType::CORNER_RADIUS:
        case RSModifierType::OUTLINE_RADIUS: {
            ss << "[topLeft:" << v4f.x_ << " topRight:" << v4f.y_ \
               << " bottomRight:" << v4f.z_ << " bottomLeft:" << v4f.w_ << + "]";
            break;
        }
        case RSModifierType::BOUNDS: {
            ss << "[x:" << v4f.x_ << " y:" << v4f.y_ << " width:" << v4f.z_ << " height:" << v4f.w_ << + "]";
            break;
        }
        default: {
            ss << "[x:" << v4f.x_ << " y:" << v4f.y_ << " z:" << v4f.z_ << " w:" << v4f.w_ << + "]";
            break;
        }
    }
    out += ss.str();
}

template<>
void RSRenderProperty<Quaternion>::Dump(std::string& out) const
{
    Quaternion q = Get();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "[x:" << q.x_ << " y:" << q.y_ << " z:" << q.z_ << " w:" << q.w_ << + "]";
    out += ss.str();
}

template<>
void RSRenderProperty<Vector2f>::Dump(std::string& out) const
{
    Vector2f v2f = Get();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "[x:" << v2f.x_ << " y:" << v2f.y_ << "]";
    out += ss.str();
}

template<>
void RSRenderProperty<Matrix3f>::Dump(std::string& out) const
{
}

template<>
void RSRenderProperty<Color>::Dump(std::string& out) const
{
    out += "[";
    Get().Dump(out);
    out += "]";
}

template<>
void RSRenderProperty<std::shared_ptr<RSFilter>>::Dump(std::string& out) const
{
    auto filter = Get();
    out += "[";
    if (filter != nullptr && filter->IsValid()) {
        out += filter->GetDescription();
    }
    out += "]";
}

template<>
void RSRenderProperty<Vector4<Color>>::Dump(std::string& out) const
{
    Vector4<Color> v4Color = Get();
    out += "[left:";
    v4Color.x_.Dump(out);
    out += " top:";
    v4Color.y_.Dump(out);
    out += " right:";
    v4Color.z_.Dump(out);
    out += " bottom";
    v4Color.w_.Dump(out);
    out += "]";
}

template<>
void RSRenderProperty<RRect>::Dump(std::string& out) const
{
}

template<>
void RSRenderProperty<Drawing::DrawCmdListPtr>::Dump(std::string& out) const
{
    out += "drawCmdList[";
    Get()->Dump(out);
    out += "]";
}

template<>
void RSRenderProperty<ForegroundColorStrategyType>::Dump(std::string& out) const
{
    out += "ENV_FOREGROUND_COLOR_STRATEGY[";
    out += std::to_string(static_cast<int>(Get()));
    out += "]";
}

template<>
void RSRenderProperty<SkMatrix>::Dump(std::string& out) const
{
    out += "GEOMETRYTRANS[";
    Get().dump(out, 0);
    out += "]";
}

template<>
void RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>::Dump(std::string& out) const 
{
    auto property = Get();
    out += "[blurRadius:" + std::to_string(property->blurRadius_); 
    out += " direction:" + std::to_string(static_cast<int>(property->direction_)) + " fractionStops:[";
        for (auto& val : property->fractionStops_) {
            out += "[" + std::to_string(val.first) + ", " + std::to_string(val.second) + "] ";
        }
    if (property->fractionStops_.size() > 0) { out.pop_back(); }
    out += "]]";
}

template<>
void RSRenderProperty<std::shared_ptr<MotionBlurParam>>::Dump(std::string& out) const
{
    auto property = Get();
    out += "[radius:" + std::to_string(property->radius) + " scaleAnchor:["; 
    out += std::to_string(property->scaleAnchor.x_) + " " + std::to_string(property->scaleAnchor.y_) + "]]";
}

template<>
void RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::Dump(std::string& out) const
{
    auto property = Get();
    out += "[";
    for (auto& eu : property) { 
        out += "[emitterIndex:" + std::to_string(eu->emitterIndex_);
        if (eu->position_) {
            out += " position:[" + std::to_string(eu->position_->x_) + " ";
            out += std::to_string(eu->position_->y_) + "]";
        }
        if (eu->emitSize_) {
            out += " emitSize:[" + std::to_string(eu->emitSize_->x_) + " ";
            out += std::to_string(eu->emitSize_->y_) + "]";
        }
        if (eu->emitRate_) {
            out += " emitRate:" + std::to_string(eu->emitRate_.value());
        }
        out += "] ";
    }
    if (property.size() > 0) { out.pop_back(); }
    out += "]";
}

template<>
void RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::Dump(std::string& out) const
{
    auto property = Get();
    out += "[fields:[";
    for (auto& field : property->fields_) {
        out += "[fieldStrength:" + std::to_string(field->fieldStrength_);
        out += " fieldShape:"  + std::to_string(static_cast<int>(field->fieldShape_));
        out += " fieldSize:[" + std::to_string(field->fieldSize_.x_) + " " + std::to_string(field->fieldSize_.y_) + "]";
        out += " fieldCenter:[" + std::to_string(field->fieldCenter_.x_) + " ";
        out += std::to_string(field->fieldCenter_.y_) + "] fieldFeather:" + std::to_string(field->fieldFeather_);
        out += " noiseScale:" + std::to_string(field->noiseScale_);
        out += " noiseFrequency:" + std::to_string(field->noiseFrequency_);
        out += " noiseAmplitude:" + std::to_string(field->noiseAmplitude_) + "] ";
    }
    if (property->fields_.size() > 0) { out.pop_back(); }
    out += "]]";
}

inline void dumpBrush(std::string& out, Drawing::Brush brush) {
    auto color = brush.GetColor();
    out += " Brush:[Color:[A:" + std::to_string(color.GetAlpha()) + " R:" + std::to_string(color.GetRed());
    out += " G:" + std::to_string(color.GetGreen()) + " B:" + std::to_string(color.GetBlue()) + "]";
    out += " blendMode:" + std::to_string(static_cast<int>(brush.GetBlendMode()));
    out += " colorSpaceType:" + std::to_string(static_cast<int>(brush.GetColorSpace()->GetType()));
    out += " shaderEffectType:" + std::to_string(static_cast<int>(brush.GetShaderEffect()->GetType()));
    out += " isAntiAlias:" + std::to_string(brush.IsAntiAlias());
    out += " isForceBrightnessDisable:" + std::to_string(brush.IsForceBrightnessDisable());
    out += " isHDR:" + std::to_string(brush.IsHdr());
    out += "]";
}

inline void dumpPen(std::string& out, Drawing::Pen pen) {
    auto color = pen.GetColor();
    out += " Pen: [Color:[A:" + std::to_string(color.GetAlpha()) + " R:" + std::to_string(color.GetRed());
    out += " G:" + std::to_string(color.GetGreen()) + " B:" + std::to_string(color.GetBlue()) + "]";
    out += " width:" + std::to_string(pen.GetWidth()) + " blenderEnabled:" + std::to_string(pen.GetBlenderEnabled());
    out += " capStyle:" + std::to_string(static_cast<int>(pen.GetCapStyle()));
    out += " joinStyle:" + std::to_string(static_cast<int>(pen.GetJoinStyle()));
    out += " blendMode:" + std::to_string(static_cast<int>(pen.GetBlendMode()));
    out += " colorSpace:" + std::to_string(static_cast<int>(pen.GetColorSpace()->GetType()));
    out += "]";
}

template<>
void RSRenderProperty<std::shared_ptr<RSMask>>::Dump(std::string& out) const
{
    auto property = Get();
    out += "[MaskType:";
    if (property->IsSvgMask())
    {
        out += "SVG x:" + std::to_string(property->GetSvgX()) + " y:" + std::to_string(property->GetSvgY());
        out += " scaleX:" + std::to_string(property->GetScaleX()) + " scaleY:" + std::to_string(property->GetScaleY());
    }
    else if (property->IsGradientMask())
    {
        out += "GRADIENT";
        dumpBrush(out, property->GetMaskBrush());
    }
    else if (property->IsPathMask())
    {
        out += "PATH";
        auto path = property->GetMaskPath();
        out += " Path:[isValid:" + std::to_string(path->IsValid());
        auto bounds = path->GetBounds();
        out += " bounds:[top:" + std::to_string(bounds.GetTop()) + " bottom:" + std::to_string(bounds.GetBottom());
        out += " left:" + std::to_string(bounds.GetLeft()) + " right:" + std::to_string(bounds.GetRight()) + "]";
        out += " drawingType:" + std::to_string(static_cast<int>(path->GetDrawingType())) + "]";
        dumpBrush(out, property->GetMaskBrush());
        dumpPen(out, property->GetMaskPen());
    }
    else if (property->IsPixelMapMask())
    {
        out += "PIXEL_MAP";
        auto pixelMap = property->GetPixelMap();
        out += " width:" + std::to_string(pixelMap->GetWidth()) + " height:" + std::to_string(pixelMap->GetHeight());
        out += " byteCount:" + std::to_string(pixelMap->GetByteCount());
    }
    else
    {
        out += "NONE";
    }
}

template<>
bool RSRenderAnimatableProperty<float>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<float>>(value);
    if (animatableProperty != nullptr) {
        return fabs(RSRenderProperty<float>::stagingValue_ - animatableProperty->stagingValue_) <= zeroThreshold;
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<float>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<Vector2f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Vector2f>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Vector2f>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Vector2f>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<Quaternion>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Quaternion>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Quaternion>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Quaternion>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<Vector4f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Vector4f>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Vector4f>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Vector4f>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<Matrix3f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Matrix3f>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Matrix3f>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Matrix3f>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<Color>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Color>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Color>::stagingValue_.IsNearEqual(
            animatableProperty->Get(), static_cast<int16_t>(zeroThreshold));
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Color>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty =
        std::static_pointer_cast<const RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    if (animatableProperty == nullptr) {
        return true;
    }

    auto filter = RSRenderProperty<std::shared_ptr<RSFilter>>::stagingValue_;
    auto otherFilter = animatableProperty->Get();
    if ((filter != nullptr) && (otherFilter != nullptr)) {
        return filter->IsNearEqual(otherFilter, zeroThreshold);
    } else if ((filter == nullptr) && (otherFilter == nullptr)) {
        ROSEN_LOGE("RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsNearEqual: "
            "both values compared are null Pointers!");
        return true;
    } else if (filter == nullptr) {
        ROSEN_LOGE(
            "RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsNearEqual: the staging value is a null pointer!");
        return otherFilter->IsNearZero(zeroThreshold);
    } else {
        ROSEN_LOGE("RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsNearEqual: "
            "the value of the comparison is a null pointer!");
        return filter->IsNearZero(zeroThreshold);
    }
}

template<>
bool RSRenderAnimatableProperty<Vector4<Color>>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Vector4<Color>>>(value);
    if (animatableProperty != nullptr) {
        auto thisData = RSRenderProperty<Vector4<Color>>::stagingValue_.data_;
        auto otherValue = animatableProperty->Get();
        auto& otherData = otherValue.data_;
        int16_t threshold = static_cast<int16_t>(zeroThreshold);
        return thisData[0].IsNearEqual(otherData[0], threshold) && thisData[2].IsNearEqual(otherData[2], threshold) &&
               thisData[2].IsNearEqual(otherData[2], threshold) && thisData[3].IsNearEqual(otherData[3], threshold);
    }
    ROSEN_LOGE(
        "RSRenderAnimatableProperty<Vector4<Color>>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<RRect>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<RRect>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<RRect>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<RRect>::IsNearEqual: the value of the comparison is a null pointer!");
    return true;
}

template<>
bool RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsEqual(
    const std::shared_ptr<const RSRenderPropertyBase>& value) const
{
    auto animatableProperty =
        std::static_pointer_cast<const RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    if (animatableProperty == nullptr) {
        return true;
    }

    auto filter = RSRenderProperty<std::shared_ptr<RSFilter>>::stagingValue_;
    auto otherFilter = animatableProperty->Get();
    if ((filter != nullptr) && (otherFilter != nullptr)) {
        return filter->IsEqual(otherFilter);
    } else if ((filter == nullptr) && (otherFilter == nullptr)) {
        ROSEN_LOGE("RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsEqual: "
            "both values compared are null Pointers!");
        return true;
    } else if (filter == nullptr) {
        ROSEN_LOGE(
            "RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsEqual: the staging value is a null pointer!");
        return otherFilter->IsEqualZero();
    } else {
        ROSEN_LOGE("RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>::IsEqual: "
            "the value of the comparison is a null pointer!");
        return filter->IsEqualZero();
    }
}

template class RSRenderProperty<int>;
template class RSRenderProperty<float>;
template class RSRenderProperty<Vector4<uint32_t>>;
template class RSRenderProperty<Vector4f>;
template class RSRenderProperty<Quaternion>;
template class RSRenderProperty<Vector2f>;
template class RSRenderProperty<Matrix3f>;
template class RSRenderProperty<Color>;
template class RSRenderProperty<std::shared_ptr<RSFilter>>;
template class RSRenderProperty<Vector4<Color>>;
template class RSRenderProperty<RRect>;
template class RSRenderProperty<Drawing::DrawCmdListPtr>;
template class RSRenderProperty<ForegroundColorStrategyType>;
template class RSRenderProperty<SkMatrix>;
template class RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>;
template class RSRenderProperty<std::shared_ptr<MotionBlurParam>>;
template class RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>;
template class RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>;
template class RSRenderProperty<std::shared_ptr<RSMask>>;

template class RSRenderAnimatableProperty<float>;
template class RSRenderAnimatableProperty<Vector4f>;
template class RSRenderAnimatableProperty<Quaternion>;
template class RSRenderAnimatableProperty<Vector2f>;
template class RSRenderAnimatableProperty<Matrix3f>;
template class RSRenderAnimatableProperty<Color>;
template class RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>;
template class RSRenderAnimatableProperty<Vector4<Color>>;
template class RSRenderAnimatableProperty<RRect>;
} // namespace Rosen
} // namespace OHOS
