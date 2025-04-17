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

#include "customized/random_rs_shader.h"

#include <memory>

#include "draw/color.h"
#include "effect/shader_effect.h"
#include "ext/gex_dot_matrix_shader_params.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "render/rs_dot_matrix_shader.h"
#include "render/rs_flow_light_sweep_shader.h"

namespace OHOS {
namespace Rosen {
namespace {
std::vector<Drawing::Color> GetRandomDrawingColorVector()
{
    std::vector<Drawing::Color> vector;
    int vectorLength = RandomEngine::GetRandomMiddleVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        vector.emplace_back(RandomData::GetRandomDrawingColor());
    }
    return vector;
}

std::vector<Drawing::Point> GetRandomDrawingPointVector()
{
    std::vector<Drawing::Point> vector;
    int vectorLength = RandomEngine::GetRandomMiddleVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        vector.emplace_back(RandomData::GetRandomDrawingPoint());
    }
    return vector;
}

std::vector<std::pair<Drawing::Color, float>> GetRandomDrawingColorAndFloatPairVector()
{
    std::vector<std::pair<Drawing::Color, float>> vector;
    int vectorLength = RandomEngine::GetRandomMiddleVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        vector.emplace_back(std::make_pair(RandomData::GetRandomDrawingColor(), RandomData::GetRandomFloat()));
    }
    return vector;
}

DotMatrixShaderParams GetRandomDotMatrixShaderParams()
{
    static constexpr int DOT_MATRIX_EFFECT_TYPE_INDEX_MAX = 2;
    DotMatrixShaderParams params;
    params.effectType_ =
        static_cast<DotMatrixEffectType>(RandomEngine::GetRandomIndex(DOT_MATRIX_EFFECT_TYPE_INDEX_MAX));
    params.normalParams_.dotColor_ = RandomData::GetRandomDrawingColor();
    params.normalParams_.dotRadius_ = RandomData::GetRandomFloat();
    params.normalParams_.dotSpacing_ = RandomData::GetRandomFloat();
    params.normalParams_.bgColor_ = RandomData::GetRandomDrawingColor();
    switch (params.effectType_) {
        case DotMatrixEffectType::RIPPLE: {
            params.rippleEffectParams_.inverseEffect_ = RandomData::GetRandomBool();
            params.rippleEffectParams_.pathWidth_ = RandomData::GetRandomFloat();
            params.rippleEffectParams_.effectColors_ = GetRandomDrawingColorVector();
            params.rippleEffectParams_.colorFractions_ = RandomData::GetRandomFloatVector();
            params.rippleEffectParams_.startPoints_ = GetRandomDrawingPointVector();
        }
        case DotMatrixEffectType::ROTATE: {
            params.rotateEffectParams_.effectColors_ = GetRandomDrawingColorVector();
            params.rotateEffectParams_.pathDirection_ = static_cast<DotMatrixDirection>(
                RandomEngine::GetRandomIndex(static_cast<int>(DotMatrixDirection::MAX)));
        }
        default: {}
    }
    return params;
}

GEXFlowLightSweepParams GetRandomGEXFlowLightSweepParams()
{
    GEXFlowLightSweepParams params;
    params.effectColors_ = GetRandomDrawingColorAndFloatPairVector();
    return params;
}

Drawing::ShaderEffect::ShaderEffectType GetRandomDrawingShaderEffectType()
{
    static constexpr int DRAWING_SHADER_EFFECT_TYPE_INDEX_MAX = 11;
    return static_cast<Drawing::ShaderEffect::ShaderEffectType>(
        RandomEngine::GetRandomIndex(DRAWING_SHADER_EFFECT_TYPE_INDEX_MAX));
}
} // namespace

std::shared_ptr<RSShader> RandomRSShader::GetRandomRSShader()
{
    static const std::vector<std::function<std::shared_ptr<RSShader>()>> rsShaderRandomGenerator = {
        GetRandomRSDotMatrixShader,
        GetRandomRSFlowLightSweepShader,
        GetRandomRSDrawingShader,
    };
    int index = RandomEngine::GetRandomIndex(rsShaderRandomGenerator.size() - 1);
    auto shader = rsShaderRandomGenerator[index]();
    if (shader && RandomData::GetRandomBool()) {
        shader->SetDrawingShader(std::make_shared<Drawing::ShaderEffect>(GetRandomDrawingShaderEffectType()));
    }
    return shader;
}

std::shared_ptr<RSShader> RandomRSShader::GetRandomRSDotMatrixShader()
{
    auto params = GetRandomDotMatrixShaderParams();
    Parcel parcel;
    if (!params.Marshalling(parcel)) {
        return nullptr;
    }

    std::shared_ptr<RSDotMatrixShader> rsShader = std::make_shared<RSDotMatrixShader>();
    rsShader->type_ = RSShader::ShaderType::DOT_MATRIX;
    bool needReset = false;
    if (!rsShader->Unmarshalling(parcel, needReset)) {
        return nullptr;
    }
    return rsShader;
}

std::shared_ptr<RSShader> RandomRSShader::GetRandomRSFlowLightSweepShader()
{
    auto params = GetRandomGEXFlowLightSweepParams();
    Parcel parcel;
    if (!params.Marshalling(parcel)) {
        return nullptr;
    }

    std::shared_ptr<RSFlowLightSweepShader> rsShader = std::make_shared<RSFlowLightSweepShader>();
    rsShader->type_ = RSShader::ShaderType::FLOW_LIGHT_SWEEP;
    bool needReset = false;
    if (!rsShader->Unmarshalling(parcel, needReset)) {
        return nullptr;
    }
    return rsShader;
}

std::shared_ptr<RSShader> RandomRSShader::GetRandomRSDrawingShader()
{
    return RSShader::CreateRSShader();
}
} // namespace Rosen
} // namespace OHOS
