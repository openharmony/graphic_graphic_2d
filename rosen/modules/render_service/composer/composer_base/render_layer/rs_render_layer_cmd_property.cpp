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

#include "rs_render_layer_cmd_property.h"
#include "rs_layer_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderLayerPropertyBase> operator+=(
    const std::shared_ptr<RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }
    return a->Add(b);
}

std::shared_ptr<RSRenderLayerPropertyBase> operator-=(
    const std::shared_ptr<RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }
    return a->Minus(b);
}

std::shared_ptr<RSRenderLayerPropertyBase> operator*=(
    const std::shared_ptr<RSRenderLayerPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }
    return value->Multiply(scale);
}

std::shared_ptr<RSRenderLayerPropertyBase> operator+(
    const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
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

std::shared_ptr<RSRenderLayerPropertyBase> operator-(
    const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
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

std::shared_ptr<RSRenderLayerPropertyBase> operator*(
    const std::shared_ptr<const RSRenderLayerPropertyBase>& a, const float scale)
{
    if (a == nullptr) {
        return {};
    }
    auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Multiply(scale);
}

bool operator==(
    const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }
    return a->IsEqual(b);
}

bool operator!=(
    const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
    const std::shared_ptr<const RSRenderLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }
    return !(a->IsEqual(b));
}
} // namespace Rosen
} // namespace OHOS
