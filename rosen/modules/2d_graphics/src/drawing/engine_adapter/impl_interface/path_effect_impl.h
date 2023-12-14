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

#ifndef PATH_EFFECT_IMPL_H
#define PATH_EFFECT_IMPL_H

#include <memory>
#include <cstdint>

#include "base_impl.h"

#include "draw/path.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
class PathEffect;
enum class PathDashStyle;
class PathEffectImpl : public BaseImpl {
public:
    PathEffectImpl() noexcept {}
    ~PathEffectImpl() override {}

    virtual void InitWithDash(const scalar intervals[], int count, scalar phase) = 0;
    virtual void InitWithPathDash(const Path& path, scalar advance, scalar phase, PathDashStyle style) = 0;
    virtual void InitWithCorner(scalar radius) = 0;
    virtual void InitWithDiscrete(scalar segLength, scalar dev, uint32_t seedAssist) = 0;
    virtual void InitWithSum(const PathEffect& e1, const PathEffect& e2) = 0;
    virtual void InitWithCompose(const PathEffect& e1, const PathEffect& e2) = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif