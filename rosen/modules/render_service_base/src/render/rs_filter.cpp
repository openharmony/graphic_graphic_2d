/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "render/rs_filter.h"

#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_light_up_effect_filter.h"

namespace OHOS {
namespace Rosen {
RSFilter::RSFilter()
    : type_(FilterType::NONE)
{}

RSFilter::~RSFilter() {}

std::function<void(std::weak_ptr<RSFilter::RSFilterTask>)> RSFilter::postTask = nullptr;
std::function<void()> RSFilter::clearGpuContext = nullptr;

std::string RSFilter::GetDescription()
{
    return "RSFilter " + std::to_string(type_);
}

std::shared_ptr<RSFilter> RSFilter::CreateBlurFilter(float blurRadiusX, float blurRadiusY)
{
    return std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
}

std::shared_ptr<RSFilter> RSFilter::CreateMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio)
{
    return std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
}

std::shared_ptr<RSFilter> RSFilter::CreateMaterialFilter(float radius, float saturation,
    float brightness, uint32_t colorValue, BLUR_COLOR_MODE mode)
{
    MaterialParam materialParam = {radius, saturation, brightness, Color::FromArgbInt(colorValue)};
    return std::make_shared<RSMaterialFilter>(materialParam, mode);
}

std::shared_ptr<RSFilter> RSFilter::CreateLightUpEffectFilter(float lightUpDegree)
{
    return std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
}

std::shared_ptr<RSFilter> operator+(const std::shared_ptr<RSFilter>& lhs, const std::shared_ptr<RSFilter>& rhs)
{
    if (lhs == nullptr) {
        return rhs;
    }
    if (rhs == nullptr) {
        return lhs;
    }
    return lhs->Add(rhs);
}

std::shared_ptr<RSFilter> operator-(const std::shared_ptr<RSFilter>& lhs, const std::shared_ptr<RSFilter>& rhs)
{
    if (rhs == nullptr) {
        return lhs;
    }
    if (lhs == nullptr) {
        return rhs->Negate();
    }
    return lhs->Sub(rhs);
}

std::shared_ptr<RSFilter> operator*(const std::shared_ptr<RSFilter>& lhs, float rhs)
{
    if (lhs == nullptr) {
        return nullptr;
    }
    return lhs->Multiply(rhs);
}
} // namespace Rosen
} // namespace OHOS
