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

#ifndef PATH_EFFECT_H
#define PATH_EFFECT_H

#include "draw/path.h"
#include "drawing/engine_adapter/impl_interface/path_effect_impl.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathDashStyle {
    TRANSLATE,
    ROTATE,
    MORPH,
};
class DRAWING_API PathEffect {
public:
    enum class PathEffectType {
        NO_TYPE,
        DASH,
        PATH_DASH,
        CORNER,
        DISCRETE,
        SUM,
        COMPOSE,
    };

    static std::shared_ptr<PathEffect> CreateDashPathEffect(const scalar intervals[], int count, scalar phase);
    static std::shared_ptr<PathEffect> CreatePathDashEffect(
        const Path& path, scalar advance, scalar phase, PathDashStyle style);
    static std::shared_ptr<PathEffect> CreateCornerPathEffect(scalar radius);
    static std::shared_ptr<PathEffect> CreateDiscretePathEffect(scalar segLength, scalar dev, uint32_t seedAssist = 0);
    static std::shared_ptr<PathEffect> CreateSumPathEffect(PathEffect& e1, PathEffect& e2);
    static std::shared_ptr<PathEffect> CreateComposePathEffect(PathEffect& e1, PathEffect& e2);

    virtual ~PathEffect() {}
    PathEffectType GetType() const;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }
    PathEffect(PathEffectType t, const scalar intervals[], int count, scalar phase) noexcept;
    PathEffect(PathEffectType t, const Path& path, scalar advance, scalar phase, PathDashStyle style) noexcept;
    PathEffect(PathEffectType t, scalar radius) noexcept;
    PathEffect(PathEffectType t, scalar segLength, scalar dev, uint32_t seedAssist) noexcept;
    PathEffect(PathEffectType t, PathEffect& e1, PathEffect& e2) noexcept;
    PathEffect(PathEffectType t) noexcept;

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);
protected:
    PathEffect() noexcept;

private:
    PathEffectType type_;
    std::shared_ptr<PathEffectImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif