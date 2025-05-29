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
#include "render/rs_render_filter.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {

void RSRenderPropertyBase::OnChange() const
{
    if (auto node = node_.lock()) {
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
        if (!parcel.WriteUint16(static_cast<int16_t>(RSModifierType::INVALID))) {
            ROSEN_LOGE("RSRenderPropertyBase::Marshalling WriteUint16 1 failed");
            return false;
        }
        return true;
    }
    RSRenderPropertyType type = val->GetPropertyType();
    if (!(parcel.WriteInt16(static_cast<int16_t>(type)))) {
        ROSEN_LOGE("RSRenderPropertyBase::Marshalling WriteUint16 2 failed");
        return false;
    }
    RSPropertyUnit unit = val->GetPropertyUnit();
    if (!(parcel.WriteInt16(static_cast<int16_t>(unit)))) {
        ROSEN_LOGE("RSRenderPropertyBase::Marshalling WriteUint16 3 failed");
        return false;
    }
    switch (type) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_FLOAT failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_COLOR failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_MATRIX3F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Matrix3f>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_MATRIX3F failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_QUATERNION: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_QUATERNION failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_FILTER: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_FILTER failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_VECTOR2F failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR3F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector3f>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_VECTOR3F failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_VECTOR4F failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_VECTOR4_COLOR failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_RRECT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<RRect>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(property->GetId()) &&
                        RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_RRECT failed");
            }
            return flag;
        }
        case RSRenderPropertyType::PROPERTY_SHADER_PARAM: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::vector<float>>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_UI_FILTER: {
            auto property = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(val);
            if (property == nullptr) {
                return false;
            }
            bool flag = parcel.WriteUint64(
                property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
            if (!flag) {
                ROSEN_LOGE("RSRenderPropertyBase::Marshalling PROPERTY_UI_FILTER failed");
            }
            return flag;
        }
        default: {
            ROSEN_LOGE("RSRenderPropertyBase::Marshalling unknown type failed");
            return false;
        }
    }
    return true;
}

bool RSRenderPropertyBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val)
{
    int16_t typeId = 0;
    if (!parcel.ReadInt16(typeId)) {
        ROSEN_LOGE("RSRenderPropertyBase::Unmarshalling ReadInt16 1 failed");
        return false;
    }
    RSRenderPropertyType type = static_cast<RSRenderPropertyType>(typeId);
    if (type == RSRenderPropertyType::INVALID) {
        val.reset();
        return true;
    }
    int16_t unitId = 0;
    if (!parcel.ReadInt16(unitId)) {
        ROSEN_LOGE("RSRenderPropertyBase::Unmarshalling ReadInt16 2 failed");
        return false;
    }
    RSPropertyUnit unit = static_cast<RSPropertyUnit>(unitId);
    PropertyId id = 0;
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("RSRenderPropertyBase::Unmarshalling ReadInt16 3 failed");
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
        case RSRenderPropertyType::PROPERTY_VECTOR3F: {
            Vector3f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector3f>(value, id, type, unit));
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
        case RSRenderPropertyType::PROPERTY_SHADER_PARAM: {
            std::vector<float> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<std::vector<float>>(value, id, type, unit));
            break;
        }
        case RSRenderPropertyType::PROPERTY_UI_FILTER: {
            std::shared_ptr<RSRenderFilter> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                ROSEN_LOGE("RSRenderPropertyBase::Unmarshalling PROPERTY_UI_FILTER failed");
                return false;
            }
            val = std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(value, id, type);
            break;
        }
        default: {
            ROSEN_LOGE("RSRenderPropertyBase::Unmarshalling unknown type failed");
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

template<>
float RSRenderAnimatableProperty<Vector3f>::ToFloat() const
{
    return RSRenderProperty<Vector3f>::stagingValue_.GetLength();
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

    auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Add(b);
}

std::shared_ptr<RSRenderPropertyBase> operator-(
    const std::shared_ptr<const RSRenderPropertyBase>& a, const std::shared_ptr<const RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Minus(b);
}

std::shared_ptr<RSRenderPropertyBase> operator*(
    const std::shared_ptr<const RSRenderPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    auto clone = value->Clone();
    if (clone == nullptr) {
        return {};
    }

    return clone->Multiply(scale);
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
void RSRenderProperty<bool>::Dump(std::string& out) const
{
    out += "[" + std::string(Get() ? "true" : "false") + "]";
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
void RSRenderProperty<Vector3f>::Dump(std::string& out) const
{
    Vector3f v3f = Get();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "[x:" << v3f.x_ << " y:" << v3f.y_ << " z:" << v3f.z_ << "]";
    out += ss.str();
}

template<>
void RSRenderProperty<Matrix3f>::Dump(std::string& out) const
{
}

template<>
void RSRenderProperty<Color>::Dump(std::string& out) const
{
    Get().Dump(out);
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
    out += "[left";
    v4Color.x_.Dump(out);
    out += " top";
    v4Color.y_.Dump(out);
    out += " right";
    v4Color.z_.Dump(out);
    out += " bottom";
    v4Color.w_.Dump(out);
    out += ']';
}

template<>
void RSRenderProperty<RRect>::Dump(std::string& out) const
{
    const auto& rrect = stagingValue_;
    out += "[rect:[x:";
    out += std::to_string(rrect.rect_.left_) + " y:";
    out += std::to_string(rrect.rect_.top_) + " width:";
    out += std::to_string(rrect.rect_.width_) + " height:";
    out += std::to_string(rrect.rect_.height_) + "]";
    out += " radius:[[";
    out += std::to_string(rrect.radius_[0][0]) + " " + std::to_string(rrect.radius_[0][1]) + "] [";
    out += std::to_string(rrect.radius_[1][0]) + " " + std::to_string(rrect.radius_[1][1]) + "] [";
    out += std::to_string(rrect.radius_[2][0]) + " " + std::to_string(rrect.radius_[2][1]) + "] ["; // 2 is vector index
    out += std::to_string(rrect.radius_[3][0]) + " " + std::to_string(rrect.radius_[3][1]) + "]"; // 3 is vector index
    out += "]";
}

template<>
void RSRenderProperty<Drawing::DrawCmdListPtr>::Dump(std::string& out) const
{
    auto propertyData = Get();
    if (propertyData != nullptr) {
        out += "drawCmdList[";
        propertyData->Dump(out);
        out += ']';
    }
}

template<>
size_t RSRenderProperty<Drawing::DrawCmdListPtr>::GetSize() const
{
    auto propertyData = Get();
    size_t size = sizeof(*this);
    if (propertyData != nullptr) {
        size += propertyData->GetSize();
    }
    return size;
}

template<>
void RSRenderProperty<ForegroundColorStrategyType>::Dump(std::string& out) const
{
    out += std::to_string(static_cast<int>(Get()));
}

template<>
void RSRenderProperty<SkMatrix>::Dump(std::string& out) const
{
#ifdef USE_M133_SKIA
    (void)out; // todo : Intrusive modification of the waiting turn
#else
    Get().dump(out, 0);
#endif
}

template<>
void RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>::Dump(std::string& out) const
{
    auto property = Get();
    if (property != nullptr) {
        property->Dump(out);
    }
}

template<>
void RSRenderProperty<std::shared_ptr<MotionBlurParam>>::Dump(std::string& out) const
{
    auto property = Get();
    if (property != nullptr) {
        property->Dump(out);
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSMagnifierParams>>::Dump(std::string& out) const
{
    auto property = Get();
    if (property != nullptr) {
        property->Dump(out);
    }
}

template<>
void RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>::Dump(std::string& out) const
{
    auto property = Get();
    out += '[';
    bool found = false;
    for (auto& eu : property) {
        if (eu != nullptr) {
            found = true;
            out += "emitterUpdater";
            eu->Dump(out);
            out += ' ';
        }
    }
    if (found) {
        out.pop_back();
    }
    out += ']';
}

template<>
void RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>::Dump(std::string& out) const
{
    auto property = Get();
    if (property != nullptr) {
        property->Dump(out);
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSMask>>::Dump(std::string& out) const
{
    auto property = Get();
    if (property != nullptr) {
        property->Dump(out);
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSShader>>::Dump(std::string& out) const
{
    if (!Get() || !Get()->GetDrawingShader()) {
        out += "[null]";
        return;
    }
    out += "[";
    out += std::to_string(static_cast<int>(Get()->GetDrawingShader()->GetType()));
    out += "]";
}

template<>
void RSRenderProperty<std::shared_ptr<RSImage>>::Dump(std::string& out) const
{
    if (!Get()) {
        out += "[null]";
        return;
    }
    std::string info;
    Get()->Dump(info, 0);
    info.erase(std::remove_if(info.begin(), info.end(), [](auto c) { return c == '\t' || c == '\n'; }),
        info.end());
    out += "[\"" + info + "\"]";
}

template<>
void RSRenderProperty<std::shared_ptr<RSPath>>::Dump(std::string& out) const
{
    if (!Get()) {
        out += "[null]";
        return;
    }
    const auto& path = Get()->GetDrawingPath();
    const auto bounds = path.GetBounds();
    out += "[length:";
    out += std::to_string(path.GetLength(false)) + " bounds[x:";
    out += std::to_string(bounds.GetLeft()) + " y:";
    out += std::to_string(bounds.GetTop()) + " width:";
    out += std::to_string(bounds.GetWidth()) + " height:";
    out += std::to_string(bounds.GetHeight()) + "] valid:";
    out += std::string(path.IsValid() ? "true" : "false");
    out += "]";
}

template<>
void RSRenderProperty<Gravity>::Dump(std::string& out) const
{
    out += "[";
    out += std::to_string(static_cast<int>(Get()));
    out += "]";
}

template<>
void RSRenderProperty<Drawing::Matrix>::Dump(std::string& out) const
{
    out += "[";
    Drawing::Matrix::Buffer buffer;
    Get().GetAll(buffer);
    for (auto v: buffer) {
        out += std::to_string(v) + " ";
    }
    out.pop_back();
    out += "]";
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
bool RSRenderAnimatableProperty<Vector3f>::IsNearEqual(
    const std::shared_ptr<RSRenderPropertyBase>& value, float zeroThreshold) const
{
    auto animatableProperty = std::static_pointer_cast<const RSRenderAnimatableProperty<Vector3f>>(value);
    if (animatableProperty != nullptr) {
        return RSRenderProperty<Vector3f>::stagingValue_.IsNearEqual(animatableProperty->Get(), zeroThreshold);
    }
    ROSEN_LOGE("RSRenderAnimatableProperty<Vector3f>::IsNearEqual: the value of the comparison is a null pointer!");
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
        return thisData[0].IsNearEqual(otherData[0], threshold) && thisData[1].IsNearEqual(otherData[1], threshold) &&
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

template class RSRenderProperty<bool>;
template class RSRenderProperty<int>;
template class RSRenderProperty<float>;
template class RSRenderProperty<Vector4<uint32_t>>;
template class RSRenderProperty<Vector4f>;
template class RSRenderProperty<Quaternion>;
template class RSRenderProperty<Vector2f>;
template class RSRenderProperty<Vector3f>;
template class RSRenderProperty<Matrix3f>;
template class RSRenderProperty<Color>;
template class RSRenderProperty<std::shared_ptr<RSFilter>>;
template class RSRenderProperty<Vector4<Color>>;
template class RSRenderProperty<RRect>;
template class RSRenderProperty<Drawing::DrawCmdListPtr>;
template class RSRenderProperty<ForegroundColorStrategyType>;
template class RSRenderProperty<SkMatrix>;
template class RSRenderProperty<std::shared_ptr<RSShader>>;
template class RSRenderProperty<std::shared_ptr<RSImage>>;
template class RSRenderProperty<std::shared_ptr<RSPath>>;
template class RSRenderProperty<Gravity>;
template class RSRenderProperty<Drawing::Matrix>;
template class RSRenderProperty<std::shared_ptr<RSLinearGradientBlurPara>>;
template class RSRenderProperty<std::shared_ptr<MotionBlurParam>>;
template class RSRenderProperty<std::shared_ptr<RSMagnifierParams>>;
template class RSRenderProperty<std::vector<std::shared_ptr<EmitterUpdater>>>;
template class RSRenderProperty<std::shared_ptr<ParticleNoiseFields>>;
template class RSRenderProperty<std::shared_ptr<RSMask>>;
template class RSRenderProperty<std::shared_ptr<RSRenderFilter>>;
template class RSRenderProperty<std::shared_ptr<RSRenderMaskPara>>;

template class RSRenderAnimatableProperty<float>;
template class RSRenderAnimatableProperty<Vector4f>;
template class RSRenderAnimatableProperty<Quaternion>;
template class RSRenderAnimatableProperty<Vector2f>;
template class RSRenderAnimatableProperty<Vector3f>;
template class RSRenderAnimatableProperty<Matrix3f>;
template class RSRenderAnimatableProperty<Color>;
template class RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>;
template class RSRenderAnimatableProperty<Vector4<Color>>;
template class RSRenderAnimatableProperty<RRect>;
template class RSRenderAnimatableProperty<std::vector<float>>;
} // namespace Rosen
} // namespace OHOS
