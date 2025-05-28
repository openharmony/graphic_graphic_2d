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

#include <memory>
#include "common/rs_vector4.h"
#include "platform/common/rs_log.h"
#include "render/rs_render_filter_base.h"
#include "render/rs_render_pixel_map_mask.h"
#include "ui_effect/property/include/rs_ui_pixel_map_mask.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"

namespace OHOS {
namespace Rosen {
bool RSUIPixelMapMaskPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::PIXEL_MAP_MASK) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUIPixelMapMaskPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::PIXEL_MAP_MASK) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto maskProperty = std::static_pointer_cast<RSUIPixelMapMaskPara>(other);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::SetProperty maskProperty is nullptr!");
        return;
    }
    auto pixelMapProp = maskProperty->GetRSProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    auto srcProp = maskProperty->GetRSProperty(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    auto dstProp = maskProperty->GetRSProperty(RSUIFilterType::PIXEL_MAP_MASK_DST);
    auto fillColorProp = maskProperty->GetRSProperty(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    if (pixelMapProp == nullptr || srcProp == nullptr || dstProp == nullptr || fillColorProp == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::SetProperty pixelMap, src, dst or fillColor is nullptr!");
        return;
    }

    auto pixelMap = std::static_pointer_cast<RSProperty<std::shared_ptr<Media::PixelMap>>>(pixelMapProp)->Get();
    auto src = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(srcProp)->Get();
    auto dst = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(dstProp)->Get();
    auto fillColor = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(fillColorProp)->Get();
    SetPixelMap(pixelMap);
    SetSrc(src);
    SetDst(dst);
    SetFillColor(fillColor);
}

void RSUIPixelMapMaskPara::SetPara(const std::shared_ptr<MaskPara>& para)
{
    if (para == nullptr || para->GetMaskParaType() != MaskPara::Type::PIXEL_MAP_MASK) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::SetPara type NG %{public}d!",
            para == nullptr ? -1 : static_cast<int>(para->GetMaskParaType()));
        return;
    }

    auto maskPara = std::static_pointer_cast<PixelMapMaskPara>(para);
    if (!maskPara) {
        return;
    }
    SetPixelMap(maskPara->GetPixelMap());
    SetSrc(maskPara->GetSrc());
    SetDst(maskPara->GetDst());
    SetFillColor(maskPara->GetFillColor());
}

void RSUIPixelMapMaskPara::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    Setter<RSProperty<std::shared_ptr<Media::PixelMap>>>(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP, pixelMap);
}

void RSUIPixelMapMaskPara::SetSrc(const Vector4f& src)
{
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::PIXEL_MAP_MASK_SRC, src);
}

void RSUIPixelMapMaskPara::SetDst(const Vector4f& dst)
{
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::PIXEL_MAP_MASK_DST, dst);
}

void RSUIPixelMapMaskPara::SetFillColor(const Vector4f& fillColor)
{
    Setter<RSAnimatableProperty<Vector4f>>(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR, fillColor);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIPixelMapMaskPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderPixelMapMaskPara>(id_);
    // pixel map
    auto prop = GetRSProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    if (prop == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::CreateRSRenderFilter pixel map property not found");
        return nullptr;
    }
    auto pixelMap = std::static_pointer_cast<RSProperty<std::shared_ptr<Media::PixelMap>>>(prop);
    auto pixelMapRenderProperty = std::make_shared<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(
        pixelMap->Get(), pixelMap->GetId());
    frProperty->Setter(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP, pixelMapRenderProperty);
    // src
    auto srcRenderProperty = GetAnimatableRenderProperty<Vector4f>(
        RSUIFilterType::PIXEL_MAP_MASK_SRC, RSRenderPropertyType::PROPERTY_VECTOR4F);
    if (srcRenderProperty == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::CreateRSRenderFilter src property not found");
        return nullptr;
    }
    frProperty->Setter(RSUIFilterType::PIXEL_MAP_MASK_SRC, srcRenderProperty);
    // dst
    auto dstRenderProperty = GetAnimatableRenderProperty<Vector4f>(
        RSUIFilterType::PIXEL_MAP_MASK_DST, RSRenderPropertyType::PROPERTY_VECTOR4F);
    if (dstRenderProperty == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::CreateRSRenderFilter dst property not found");
        return nullptr;
    }
    frProperty->Setter(RSUIFilterType::PIXEL_MAP_MASK_DST, dstRenderProperty);
    // fill color
    auto fillColorRenderProperty = GetAnimatableRenderProperty<Vector4f>(
        RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR, RSRenderPropertyType::PROPERTY_VECTOR4F);
    if (fillColorRenderProperty == nullptr) {
        ROSEN_LOGW("RSUIPixelMapMaskPara::CreateRSRenderFilter fill color property not found");
        return nullptr;
    }
    frProperty->Setter(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR, fillColorRenderProperty);
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIPixelMapMaskPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS
