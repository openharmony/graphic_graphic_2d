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

    /**
     * @brief Create a Dash PathEffect. Note: only affects stroked paths.
     *
     * @param intervals array containing an even number of entries (>=2), with
     * the even indices specifying the length of "on" intervals, and the odd
     * indices specifying the length of "off" intervals. This array will be
     * copied in Make, and can be disposed of freely after.
     * @param count number of elements in the intervals array.
     * @param phase offset into the intervals array (mod the sum of all of the intervals).
     * For example: if intervals[] = {10, 20}, count = 2, and phase = 25,
     * this will set up a dashed path like so:
     * 5 pixels off
     * 10 pixels on
     * 20 pixels off
     * 10 pixels on
     * 20 pixels off
     * ...
     * A phase of -5, 25, 55, 85, etc. would all result in the same path,
     * because the sum of all the intervals is 30.
     * @return A shared pointer to PathEffect.
     */
    static std::shared_ptr<PathEffect> CreateDashPathEffect(const scalar intervals[], int count, scalar phase);
    static std::shared_ptr<PathEffect> CreatePathDashEffect(
        const Path& path, scalar advance, scalar phase, PathDashStyle style);

    /**
     * @brief Create a Corner Path Effect.
     *
     * @param radius radius must be > 0 to have an effect.
     * It specifies the distance from each corner that should be "rounded".
     * @return A shared pointer to PathEffect.
     */
    static std::shared_ptr<PathEffect> CreateCornerPathEffect(scalar radius);

    /**
     * @brief Create a Discrete PathEffect object.
     * This path effect chops a path into discrete segments, and randomly displaces them.
     * Break the path into segments of segLength length, and randomly move the endpoints
     * away from the original path by a maximum of deviation.
     * Note: works on filled or framed paths.
     *
     * @param segLength
     * @param dev
     * @param seedAssist This is a caller-supplied seedAssist that modifies
     * the seed value that is used to randomize the path segments' endpoints.
     * If not supplied it defaults to 0, in which case filtering a path multiple
     * times will result in the same set of segments (this is useful for testing).
     * If a caller does not want this behaviour they can pass in a different seedAssist
     * to get a different set of path segments.
     * @return A shared pointer to PathEffect.
     */
    static std::shared_ptr<PathEffect> CreateDiscretePathEffect(scalar segLength, scalar dev, uint32_t seedAssist = 0);

    /**
     * @brief Create a Sum PathEffect object that apples each effect (first and second) to the original path,
     * and returns a path with the sum of these.
     *
     * result = first(path) + second(path)
     *
     * @param e1 first PathEffect
     * @param e2 second PathEffect
     * @return std::shared_ptr<PathEffect>
     */
    static std::shared_ptr<PathEffect> CreateSumPathEffect(PathEffect& e1, PathEffect& e2);
    /**
     * @brief Create a Compose PathEffect that applies the inner effect to the path, and then applies the
     *  outer effect to the result of the inner's.
     *
     * result = outer(inner(path))
     * @return a share pointer to PathEffect
     */
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