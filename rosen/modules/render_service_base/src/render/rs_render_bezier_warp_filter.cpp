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
#include "render/rs_render_bezier_warp_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "platform/common/rs_log.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {

void RSRenderBezierWarpFilterPara::CalculateHash()
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&destinationPatch_, sizeof(destinationPatch_), hash_);
}

std::shared_ptr<RSRenderFilterParaBase> RSRenderBezierWarpFilterPara::DeepCopy() const
{
    auto copyFilter = std::make_shared<RSRenderBezierWarpFilterPara>(id_);
    copyFilter->type_ = type_;
    copyFilter->destinationPatch_ = destinationPatch_;
    copyFilter->CalculateHash();
    return copyFilter;
}

void RSRenderBezierWarpFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderBezierWarpFilterPara";
}

bool RSRenderBezierWarpFilterPara::WriteToParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(type_)) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(modifierType_))) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::WriteToParcel type Error");
        return false;
    }
    if (properties_.size() > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size large Error");
        return false;
    }
    if (!parcel.WriteUint32(properties_.size())) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::WriteToParcel size Error");
        return false;
    }
    for (const auto& [key, value] : properties_) {
        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSMarshallingHelper::Marshalling(parcel, value)) {
            return false;
        }
        ROSEN_LOGD("RSRenderBezierWarpFilterPara::WriteToParcel type %{public}d", static_cast<int>(key));
    }
    return true;
}

bool RSRenderBezierWarpFilterPara::ReadFromParcel(Parcel& parcel)
{
    int16_t type = 0;
    int16_t modifierType = 0;
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel type Error");
        return false;
    }
    type_ = static_cast<RSUIFilterType>(type);
    modifierType_ = static_cast<RSModifierType>(modifierType);

    uint32_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size Error");
        return false;
    }
    if (size > static_cast<uint32_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel size large Error");
        return false;
    }
    properties_.clear();
    for (uint32_t i = 0; i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel type %{public}d", static_cast<int>(key));
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderBezierWarpFilterPara::ReadFromParcel value %{public}d", static_cast<int>(key));
            return false;
        }
        Setter(key, value);
    }
    return true;
}

std::shared_ptr<RSRenderPropertyBase> RSRenderBezierWarpFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::BEZIER_CONTROL_POINT0 :
        case RSUIFilterType::BEZIER_CONTROL_POINT1 :
        case RSUIFilterType::BEZIER_CONTROL_POINT2 :
        case RSUIFilterType::BEZIER_CONTROL_POINT3 :
        case RSUIFilterType::BEZIER_CONTROL_POINT4 :
        case RSUIFilterType::BEZIER_CONTROL_POINT5 :
        case RSUIFilterType::BEZIER_CONTROL_POINT6 :
        case RSUIFilterType::BEZIER_CONTROL_POINT7 :
        case RSUIFilterType::BEZIER_CONTROL_POINT8 :
        case RSUIFilterType::BEZIER_CONTROL_POINT9 :
        case RSUIFilterType::BEZIER_CONTROL_POINT10 :
        case RSUIFilterType::BEZIER_CONTROL_POINT11 : {
            return std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
                Vector2f(0.f, 0.f), 0);
        }
        default: {
            ROSEN_LOGD("RSRenderBezierWarpFilterPara::CreateRenderProperty is nullptr");
            return std::shared_ptr<RSRenderPropertyBase>();
        }
    }
    return std::shared_ptr<RSRenderPropertyBase>();
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderBezierWarpFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}

bool RSRenderBezierWarpFilterPara::ParseFilterValues()
{
    std::array<RSUIFilterType, BEZIER_WARP_POINT_NUM> ctrlPointsType = {
        RSUIFilterType::BEZIER_CONTROL_POINT0,
        RSUIFilterType::BEZIER_CONTROL_POINT1,
        RSUIFilterType::BEZIER_CONTROL_POINT2,
        RSUIFilterType::BEZIER_CONTROL_POINT3,
        RSUIFilterType::BEZIER_CONTROL_POINT4,
        RSUIFilterType::BEZIER_CONTROL_POINT5,
        RSUIFilterType::BEZIER_CONTROL_POINT6,
        RSUIFilterType::BEZIER_CONTROL_POINT7,
        RSUIFilterType::BEZIER_CONTROL_POINT8,
        RSUIFilterType::BEZIER_CONTROL_POINT9,
        RSUIFilterType::BEZIER_CONTROL_POINT10,
        RSUIFilterType::BEZIER_CONTROL_POINT11,
    };
    Vector2f tmpBezierCtrlPoint;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> ctrlPointProperty;
    for (size_t i = 0; i < BEZIER_WARP_POINT_NUM; ++i) {
        ctrlPointProperty =
            std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(GetRenderProperty(ctrlPointsType[i]));
        if (ctrlPointProperty == nullptr) {
            ROSEN_LOGE("RSRenderBezierWarpFilterPara::ParseFilterValues GetRenderProperty nullptr, index:%zu", i);
            return false;
        }
        tmpBezierCtrlPoint = ctrlPointProperty->Get();
        destinationPatch_[i].Set(tmpBezierCtrlPoint.x_, tmpBezierCtrlPoint.y_);
    }
    return true;
}

const std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& RSRenderBezierWarpFilterPara::GetBezierWarpPoints() const
{
    return destinationPatch_;
}

void RSRenderBezierWarpFilterPara::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto bezierWarpFilter = std::make_shared<Drawing::GEVisualEffect>("BEZIER_WARP",
        Drawing::DrawingPaintType::BRUSH);

    bezierWarpFilter->SetParam("BEZIER_WARP_DESTINATION_PATCH", destinationPatch_);
    visualEffectContainer->AddToChainedFilter(bezierWarpFilter);
}

} // namespace Rosen
} // namespace OHOS