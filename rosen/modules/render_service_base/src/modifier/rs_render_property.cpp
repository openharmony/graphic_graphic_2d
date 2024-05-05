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

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
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
