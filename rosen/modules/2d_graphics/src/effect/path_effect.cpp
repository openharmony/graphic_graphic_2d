/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "effect/path_effect.h"

#include "impl_factory.h"
#include "impl_interface/path_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
PathEffect::PathEffect(PathEffectType t, const scalar intervals[], int count, scalar phase) noexcept : PathEffect()
{
    type_ = t;
    impl_->InitWithDash(intervals, count, phase);
}

PathEffect::PathEffect(PathEffectType t, const Path& path, scalar advance, scalar phase, PathDashStyle style) noexcept
    : PathEffect()
{
    type_ = t;
    impl_->InitWithPathDash(path, advance, phase, style);
}

PathEffect::PathEffect(PathEffectType t, scalar radius) noexcept : PathEffect()
{
    type_ = t;
    impl_->InitWithCorner(radius);
}

PathEffect::PathEffect(PathEffectType t, PathEffect& e1, PathEffect& e2) noexcept : PathEffect()
{
    type_ = t;
    if (type_ == PathEffect::PathEffectType::SUM) {
        impl_->InitWithSum(e1, e2);
    } else if(type_ == PathEffect::PathEffectType::COMPOSE) {
        impl_->InitWithCompose(e1, e2);
    }
}

PathEffect::PathEffect() noexcept
    : type_(PathEffect::PathEffectType::NO_TYPE),
    impl_(ImplFactory::CreatePathEffectImpl()) {}

PathEffect::PathEffectType PathEffect::GetType() const
{
    return type_;
}

std::shared_ptr<PathEffect> PathEffect::CreateDashPathEffect(const scalar intervals[], int count, scalar phase)
{
    return std::make_shared<PathEffect>(PathEffect::PathEffectType::DASH, intervals, count, phase);
}

std::shared_ptr<PathEffect> PathEffect::CreatePathDashEffect(const Path& path, scalar advance,
                                                             scalar phase, PathDashStyle style)
{
    return std::make_shared<PathEffect>(PathEffect::PathEffectType::PATH_DASH, path, advance, phase, style);
}

std::shared_ptr<PathEffect> PathEffect::CreateCornerPathEffect(scalar radius)
{
    return std::make_shared<PathEffect>(PathEffect::PathEffectType::CORNER, radius);
}

std::shared_ptr<PathEffect> PathEffect::CreateSumPathEffect(PathEffect& e1, PathEffect& e2)
{
    return std::make_shared<PathEffect>(PathEffect::PathEffectType::SUM, e1, e2);
}

std::shared_ptr<PathEffect> PathEffect::CreateComposePathEffect(PathEffect& e1, PathEffect& e2)
{
    return std::make_shared<PathEffect>(PathEffect::PathEffectType::COMPOSE, e1, e2);
}
}
}
}