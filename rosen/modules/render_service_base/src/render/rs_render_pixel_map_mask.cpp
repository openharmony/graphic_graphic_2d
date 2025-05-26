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

#include "render/rs_render_pixel_map_mask.h"
#include <memory>
#include "common/rs_vector4.h"
#include "image/image.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_render_filter_base.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderPropertyBase> RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP: {
            return std::make_shared<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(
                nullptr, 0);
        }
        case RSUIFilterType::PIXEL_MAP_MASK_SRC:
        case RSUIFilterType::PIXEL_MAP_MASK_DST:
        case RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR: {
            return std::make_shared<RSRenderAnimatableProperty<Vector4f>>(
                Vector4f(), 0, RSRenderPropertyType::PROPERTY_VECTOR4F);
        }
        default:
            ROSEN_LOGE("RSRenderPixelMapMaskPara::CreateRenderProperty mask nullptr");
            return nullptr;
    }
    return nullptr;
}

void RSRenderPixelMapMaskPara::GetDescription(std::string& out) const
{
    out += "RSRenderPixelMapMaskPara";
}

bool RSRenderPixelMapMaskPara::WriteToParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderPixelMapMaskPara::WriteToParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::WriteToParcel type Error");
        return false;
    }

    size_t size = properties_.size();
    if (!RSMarshallingHelper::Marshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::WriteToParcel properties size Error");
        return false;
    }

    auto property = GetRenderPropert(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    if (property == nullptr) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::WriteToParcel pixel map not found");
        return false;
    }
    auto pixelMapProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(property);
    auto pixelMap = pixelMapProperty->Get();
    if (!RSMarshallingHelper::Marshalling(parcel, pixelMap)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::WriteToParcel marshall pixel map failed");
        return false;
    }

    for (const auto& [key, value] : properties_) {
        if (key == RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP) {
            continue;
        }

        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSMarshallingHelper::Marshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderPixelMapMaskPara::WriteToParcel marshall property failed, key: %{public}d",
                static_cast<int>(key));
            return false;
        }
    }
    return true;
}

bool RSRenderPixelMapMaskPara::ReadFromParcel(Parcel& parcel)
{
    cacheImage_ = nullptr;
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel type Error");
        return false;
    }
    ROSEN_LOGD("RSRenderPixelMapMaskPara::ReadFromParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));

    size_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel properties size Error");
        return false;
    }

    if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel properties size exceed");
        return false;
    }

    properties_.clear();
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    if (!RSMarshallingHelper::Unmarshalling(parcel, pixelMap)) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel unmarshall pixel map failed");
        return false;
    }
    if (pixelMap == nullptr) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel unmarshall null pixel map");
        return false;
    }

    auto pixelMapProperty = std::make_shared<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(pixelMap, 0);
    cacheImage_ = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (cacheImage_ == nullptr) {
        ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel extract image from pixel map failed");
        return false;
    }
    Setter(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP, pixelMapProperty);

    for (size_t i = 1; i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel unmarshal key failed, i: %{public}zu", i);
            return false;
        }

        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderPixelMapMaskPara::ReadFromParcel unmarshall value failed, key: %{public}d",
                static_cast<int>(key));
            return false;
        }
        Setter(key, value);
    }
    return true;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderPixelMapMaskPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}

const std::shared_ptr<Drawing::Image> RSRenderPixelMapMaskPara::GetImage() const
{
    return cacheImage_;
}
} // namespace Rosen
} // namespace OHOS
