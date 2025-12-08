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

#include "rs_layer_cmd_property.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int PID_SHIFT = 48;
constexpr uint64_t NEXTID_MASK_48bit = 0xFFFFFFFFFFFF;

RSLayerPropertyId GeneratePropertyId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint64_t> currentId_ = 0;
    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    return ((RSLayerPropertyId)(pid_ & 0xFFFF) << PID_SHIFT) | (currentId & NEXTID_MASK_48bit);
}
} // namespace

RSLayerPropertyBase::RSLayerPropertyBase() : id_(GeneratePropertyId())
{}

std::shared_ptr<RSLayerPropertyBase> operator+=(const std::shared_ptr<RSLayerPropertyBase>& a,
    const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Add(b);
}

std::shared_ptr<RSLayerPropertyBase> operator-=(const std::shared_ptr<RSLayerPropertyBase>& a,
    const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Minus(b);
}

std::shared_ptr<RSLayerPropertyBase> operator*=(const std::shared_ptr<RSLayerPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Multiply(scale);
}

std::shared_ptr<RSLayerPropertyBase> operator+(const std::shared_ptr<const RSLayerPropertyBase>& a,
    const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    const auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Add(b);
}

std::shared_ptr<RSLayerPropertyBase> operator-(const std::shared_ptr<const RSLayerPropertyBase>& a,
    const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    const auto clone = a->Clone();
    if (clone == nullptr) {
        return {};
    }
    return clone->Minus(b);
}

std::shared_ptr<RSLayerPropertyBase> operator*(const std::shared_ptr<const RSLayerPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    const auto clone = value->Clone();
    if (clone == nullptr) {
        return {};
    }

    return clone->Multiply(scale);
}

bool operator==(const std::shared_ptr<const RSLayerPropertyBase>& a, const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }

    return a->IsEqual(b);
}

bool operator!=(const std::shared_ptr<const RSLayerPropertyBase>& a, const std::shared_ptr<const RSLayerPropertyBase>& b)
{
    if (a == nullptr) {
        return false;
    }

    return !a->IsEqual(b);
}
} // namespace Rosen
} // namespace OHOS
