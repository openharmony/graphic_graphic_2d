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

#ifndef RECORDING_SHADER_EFFECT_H
#define RECORDING_SHADER_EFFECT_H

#include "effect/shader_effect.h"
#include "recording/shader_effect_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingShaderEffect : public ShaderEffect {
public:
    /*
     * @brief        Creates a CreateColorShaderOpItem to add to the ShaderEffectCmdList.
     * @param color  32-bit ARGB color value.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateColorShader(ColorQuad color);

    /*
     * @brief       Creates a CreateBlendShaderOpItem to add to the ShaderEffectCmdList.
     * @param dst   To Blend.
     * @param src   To Blend.
     * @param mode  The mode of Blend.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateBlendShader(const ShaderEffect& dst,
        const ShaderEffect& src, BlendMode mode);

    /*
     * @brief           Creates a CreateImageShaderOpItem to add to the ShaderEffectCmdList.
     * @param image     Dimensions are taken from Image.
     * @param tileX     Tiling in the x direction.
     * @param tileY     Tiling in the y direction.
     * @param sampling  Sampling Options.
     * @param matrix    Image transformation matrix.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateImageShader(
        const Image& image, TileMode tileX, TileMode tileY, const SamplingOptions& sampling, const Matrix& matrix);

    /*
     * @brief          Creates a CreatePictureShaderOpItem to add to the ShaderEffectCmdList.
     * @param picture  Shader will draw with this picture.
     * @param tileX    Tiling in the x direction.
     * @param tileY    Tiling in the y direction.
     * @param mode     How to filter the tiles.
     * @param matrix   Used when sampling.
     * @param rect     The tile rectangle in picture coordinates.
     */
    static std::shared_ptr<RecordingShaderEffect> CreatePictureShader(const Picture& picture,
        TileMode tileX, TileMode tileY, FilterMode mode, const Matrix& matrix, const Rect& rect);

    /*
     * @brief          Creates a CreateLinearGradientOpItem to add to the ShaderEffectCmdList.
     * @param startPt  The start point for the gradient.
     * @param endPt    The end point for the gradient.
     * @param colors   The vector of colors, to be distributed between the two points.
     * @param pos      The vector of scalar. If null, the colors are evenly distributed between the start and end point.
                       If not null, the values must be between 0.0 and 1.0.
     * @param mode     The mode of tiling.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateLinearGradient(const Point& startPt, const Point& endPt,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode);

    /*
     * @brief           Creates a CreateRadialGradientOpItem to add to the ShaderEffectCmdList.
     * @param centerPt  The center point of the circle for this gradient.
     * @param radius    Must be positive, the radius of the gradient circle.
     * @param colors    The vector of colors, to be distributed between the center and the edge of the circle.
     * @param pos       The vector of scalar. If NULL, the colors are evenly distributed between the center
                        and edges of the circle. If not NULL, the value must be between 0.0 and 1.0.
     * @param mode      The mode of tiling.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateRadialGradient(const Point& centerPt, scalar radius,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode);
    /*
     * @brief              Creates a CreateTwoPointConicalOpItem to add to the ShaderEffectCmdList.
     * @param startPt      The center point for the start circle.
     * @param startRadius  The radius for the start circle.
     * @param endPt        The center point for the end circle.
     * @param endRadius    The radius for the end circle.
     * @param colors       The vector of colors, to be distributed between the start and end circle.
     * @param pos          The vector of scalar. If NULL, the colors are evenly distributed between the start and end
                           circle. If not NULL, the value must be between 0.0 and 1.0.
     * @param mode         The mode of tiling.
     * @return
     */
    static std::shared_ptr<RecordingShaderEffect> CreateTwoPointConical(const Point& startPt, scalar startRadius,
        const Point& endPt, scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos,
        TileMode mode);
    /*
     * @brief             Creates a CreateSweepGradientOpItem to add to the ShaderEffectCmdList.
     * @param centerPt    The center point for the sweep.
     * @param colors      The vector of colors, to be distributed around the center point.
     * @param pos         The vector of scalar. If NULL, the colors are evenly distributed between the start
                          and end circle. If not NULL, the value must be between 0.0 and 1.0.
     * @param mode        The mode of tiling.
     * @param startAngle  The start angle.
     * @param endAngle    The end angle.
     */
    static std::shared_ptr<RecordingShaderEffect> CreateSweepGradient(const Point& centerPt,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
        scalar endAngle);

    RecordingShaderEffect() noexcept;
    ~RecordingShaderEffect() override = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    /*
     * @brief  Gets the pionter to ShaderEffectCmdList.
     */
    std::shared_ptr<ShaderEffectCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<ShaderEffectCmdList> cmdList_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
