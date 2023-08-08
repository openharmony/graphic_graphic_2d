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

#include "recording/recording_shader_effect.h"

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingShaderEffect::RecordingShaderEffect() noexcept : cmdList_(std::make_shared<ShaderEffectCmdList>()) {}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateColorShader(ColorQuad color)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    shaderEffect->GetCmdList()->AddOp<CreateColorShaderOpItem>(color);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateBlendShader(
    const ShaderEffect& dst, const ShaderEffect& src, BlendMode mode)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto dstHandle = CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*shaderEffect->GetCmdList(), dst);
    auto srcHandle = CmdListHelper::AddRecordedToCmdList<RecordingShaderEffect>(*shaderEffect->GetCmdList(), src);

    shaderEffect->GetCmdList()->AddOp<CreateBlendShaderOpItem>(dstHandle, srcHandle, mode);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateImageShader(
    const Image& image, TileMode tileX, TileMode tileY, const SamplingOptions& sampling, const Matrix& matrix)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto imageHandle = CmdListHelper::AddImageToCmdList(*shaderEffect->GetCmdList(), image);

    shaderEffect->GetCmdList()->AddOp<CreateImageShaderOpItem>(imageHandle, tileX, tileY, sampling, matrix);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreatePictureShader(const Picture& picture,
    TileMode tileX, TileMode tileY, FilterMode mode, const Matrix& matrix, const Rect& rect)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto pictureHandle = CmdListHelper::AddPictureToCmdList(*shaderEffect->GetCmdList(), picture);

    shaderEffect->GetCmdList()->AddOp<CreatePictureShaderOpItem>(pictureHandle, tileX, tileY, mode, matrix, rect);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateLinearGradient(const Point& startPt,
    const Point& endPt, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*shaderEffect->GetCmdList(), colors);
    auto posData = CmdListHelper::AddVectorToCmdList<scalar>(*shaderEffect->GetCmdList(), pos);

    shaderEffect->GetCmdList()->AddOp<CreateLinearGradientOpItem>(startPt, endPt, colorsData, posData, mode);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateRadialGradient(const Point& centerPt,
    scalar radius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*shaderEffect->GetCmdList(), colors);
    auto posData = CmdListHelper::AddVectorToCmdList<scalar>(*shaderEffect->GetCmdList(), pos);

    shaderEffect->GetCmdList()->AddOp<CreateRadialGradientOpItem>(centerPt, radius, colorsData, posData, mode);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateTwoPointConical(
    const Point& startPt, scalar startRadius, const Point& endPt, scalar endRadius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*shaderEffect->GetCmdList(), colors);
    auto posData = CmdListHelper::AddVectorToCmdList<scalar>(*shaderEffect->GetCmdList(), pos);

    shaderEffect->GetCmdList()->AddOp<CreateTwoPointConicalOpItem>(
        startPt, startRadius, endPt, endRadius, colorsData, posData, mode);
    return shaderEffect;
}

std::shared_ptr<RecordingShaderEffect> RecordingShaderEffect::CreateSweepGradient(
    const Point& centerPt, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos,
    TileMode mode, scalar startAngle, scalar endAngle)
{
    auto shaderEffect = std::make_shared<RecordingShaderEffect>();
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*shaderEffect->GetCmdList(), colors);
    auto posData = CmdListHelper::AddVectorToCmdList<scalar>(*shaderEffect->GetCmdList(), pos);

    shaderEffect->GetCmdList()->AddOp<CreateSweepGradientOpItem>(
        centerPt, colorsData, posData, mode, startAngle, endAngle);
    return shaderEffect;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
