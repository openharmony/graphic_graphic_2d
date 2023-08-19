/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RECORDING_PATH_EFFECT_H
#define RECORDING_PATH_EFFECT_H

#include "effect/path_effect.h"
#include "recording/path_effect_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API RecordingPathEffect : public PathEffect {
public:

    /*
     * @brief            Creates a CreateDashPathEffectOpItem to add to the PathEffectCmdList.
     * @param intervals  Containing an even number array.
     * @param phase      The offset of the intervals array.
     */
    static std::shared_ptr<RecordingPathEffect> CreateDashPathEffect(
        const std::vector<scalar>& intervals, scalar phase);

    /*
     * @brief          Creates a CreatePathDashEffectOpItem to add to the PathEffectCmdList.
     * @param path     To replicate.
     * @param advance  The space distance between path.
     * @param phase    The distance of the start position of path.
     * @param style    Transform path style at each point.
     */
    static std::shared_ptr<RecordingPathEffect> CreatePathDashEffect(
        const Path& path, scalar advance, scalar phase, PathDashStyle style);

    /*
     * @brief         Creates a CreateCornerPathEffectOpItem to add to the PathEffectCmdList.
     * @param radius  Must be greater than zero.
     */
    static std::shared_ptr<RecordingPathEffect> CreateCornerPathEffect(scalar radius);

    /*
     * @brief     Creates a CreateSumPathEffectOpItem to add to the PathEffectCmdList.
     * @param e1  The first PathEffect.
     * @param e2  The second PathEffect.
     */
    static std::shared_ptr<RecordingPathEffect> CreateSumPathEffect(const PathEffect& e1, const PathEffect& e2);

    /*
     * @brief     Creates a CreateComposePathEffectOpItem to add to the PathEffectCmdList.
     * @param e1  To apply the inner effect to the path.
     * @param e2  To apply the outer effect to the inner's result.
     */
    static std::shared_ptr<RecordingPathEffect> CreateComposePathEffect(const PathEffect& e1, const PathEffect& e2);

    RecordingPathEffect() noexcept;
    ~RecordingPathEffect() override = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    /*
     * @brief  Gets the pointer to the PathEffectCmdList.
     */
    std::shared_ptr<PathEffectCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<PathEffectCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
