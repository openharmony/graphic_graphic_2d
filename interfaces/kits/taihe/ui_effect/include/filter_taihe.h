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

#ifndef TAIHE_INCLUDE_UIEFFECT_FILTER_TAIHE_H
#define TAIHE_INCLUDE_UIEFFECT_FILTER_TAIHE_H

#include "filter/include/filter.h"
#include "ohos.graphics.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.VisualEffect.proj.1.hpp"
#include "ohos.graphics.uiEffect.TileMode.proj.1.hpp"
#include "ohos.graphics.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.impl.hpp"
#include "stdexcept"
#include "taihe/array.hpp"

namespace ANI::UIEffect {
using namespace taihe;
using namespace ohos::graphics::uiEffect;

class FilterImpl {
public:
    FilterImpl();
    explicit FilterImpl(std::shared_ptr<OHOS::Rosen::Filter> filter);
    ~FilterImpl();

    Filter blur(double blurRadius);
    Filter pixelStretch(taihe::array_view<double> stretchSizes, TileMode tileMode);

private:
    bool IsFilterValid() const;

    std::shared_ptr<OHOS::Rosen::Filter> nativeFilter_;
};
} // namespace ANI::UIEffect

#endif