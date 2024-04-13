/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "render/rs_aibar_shader_filter.h"

#include <unordered_map>

#include "src/core/SkOpts.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_kawase_blur.h"

#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"

namespace OHOS {
namespace Rosen {
RSAIBarShaderFilter::RSAIBarShaderFilter()
{
    type_ = ShaderFilterType::AIBAR;
}
RSAIBarShaderFilter::~RSAIBarShaderFilter() = default;

std::vector<float> RSAIBarShaderFilter::GetAiInvertCoef()
{
    const auto& aiInvertCoef = RSSystemProperties::GetAiInvertCoef();
    if (!IsAiInvertCoefValid(aiInvertCoef)) {
        static std::vector<float> aiInvertCoefDefault {0.0, 1.0, 0.55, 0.4, 1.6, 45.0};
        return aiInvertCoefDefault;
    }
    return aiInvertCoef;
}

void RSAIBarShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto aiBarPara = GetAiInvertCoef();
    auto aiBarFilter = std::make_shared<Drawing::GEVisualEffect>("AIBAR", Drawing::DrawingPaintType::BRUSH);
    aiBarFilter->SetParam("AIBAR_LOW", aiBarPara[0]); // aiBarPara[0] is low
    aiBarFilter->SetParam("AIBAR_HIGH", aiBarPara[1]); // aiBarPara[1] is high
    aiBarFilter->SetParam("AIBAR_THRESHOLD", aiBarPara[2]); // aiBarPara[2] is threshold
    aiBarFilter->SetParam("AIBAR_OPACITY", aiBarPara[3]); // aiBarPara[3] is opacity
    aiBarFilter->SetParam("AIBAR_SATURATION", aiBarPara[4]); // aiBarPara[4] is saturation
 
    visualEffectContainer->AddToChainedFilter(aiBarFilter);
}

bool RSAIBarShaderFilter::IsAiInvertCoefValid(const std::vector<float>& aiInvertCoef)
{
    return ROSEN_LNE(aiInvertCoef[0], aiInvertCoef[1]) &&
        ROSEN_GE(aiInvertCoef[0], 0.0) && ROSEN_LE(aiInvertCoef[0], 1.0) && // aiInvertCoef[0] is low
        ROSEN_GE(aiInvertCoef[1], 0.0) && ROSEN_LE(aiInvertCoef[1], 1.0) && // aiInvertCoef[1] is high
        ROSEN_GE(aiInvertCoef[2], 0.0) && ROSEN_LE(aiInvertCoef[2], 1.0) && // aiInvertCoef[2] is threshold
        ROSEN_GE(aiInvertCoef[3], 0.0) && ROSEN_LE(aiInvertCoef[3], 1.0) && // aiInvertCoef[3] is opacity
        ROSEN_GE(aiInvertCoef[4], 0.0) && ROSEN_LE(aiInvertCoef[4], 2.0) && // aiInvertCoef[4] is saturation <= 2.0
        ROSEN_GNE(aiInvertCoef[5], 0.0); // aiInvertCoef[5] is filter_radius
}
} // namespace Rosen
} // namespace OHOS