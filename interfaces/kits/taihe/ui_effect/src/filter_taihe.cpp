/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "filter_taihe.h"

#include "filter/include/filter.h"
#include "filter/include/filter_blur_para.h"
#include "filter/include/filter_pixel_stretch_para.h"
#include "ohos.graphics.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.VisualEffect.proj.1.hpp"
#include "ui_effect_taihe_utils.h"

using namespace ANI::UIEffect;

namespace ANI::UIEffect {
FilterImpl::FilterImpl()
{
    nativeFilter_ = std::make_shared<OHOS::Rosen::Filter>();
}

FilterImpl::FilterImpl(std::shared_ptr<OHOS::Rosen::Filter> filter)
{
    nativeFilter_ = filter;
}

FilterImpl::~FilterImpl()
{
    nativeFilter_ = nullptr;
}

Filter FilterImpl::blur(double blurRadius)
{
    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::blur failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    auto filterBlurPara = std::make_shared<OHOS::Rosen::FilterBlurPara>();
    filterBlurPara->SetRadius(static_cast<float>(blurRadius));

    nativeFilter_->AddPara(filterBlurPara);

    return make_holder<FilterImpl, Filter>(nativeFilter_);
}

Filter FilterImpl::pixelStretch(taihe::array_view<double> stretchSizes, TileMode tileMode)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call pixelStretch failed, is not system app");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    if (!IsFilterValid()) {
        UIEFFECT_LOG_E("FilterImpl::pixelStretch failed, filter is invalid");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }

    auto pixelStretchPara = std::make_shared<OHOS::Rosen::PixelStretchPara>();
    pixelStretchPara->SetTileMode(ConvertTileModeFromTaiheTileMode(tileMode));
    OHOS::Rosen::Vector4f stretchPercent;
    if (!ConvertVector4fFromTaiheArray(stretchPercent, stretchSizes)) {
        UIEFFECT_LOG_E("FilterImpl::pixelStretch ConvertVector4fFromTaiheArray failed");
        return make_holder<FilterImpl, Filter>(nativeFilter_);
    }
    pixelStretchPara->SetStretchPercent(stretchPercent);

    nativeFilter_->AddPara(pixelStretchPara);

    return make_holder<FilterImpl, Filter>(nativeFilter_);
}

bool FilterImpl::IsFilterValid() const
{
    return nativeFilter_ != nullptr;
}

Filter createFilter()
{
    return make_holder<FilterImpl, Filter>();
}
} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_createFilter(createFilter);
// NOLINTEND