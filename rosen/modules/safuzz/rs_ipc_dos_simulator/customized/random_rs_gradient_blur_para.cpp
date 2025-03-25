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

#include "customized/random_rs_gradient_blur_para.h"

#include "random/random_data.h"
#include "random/random_engine.h"
#include "render/rs_gradient_blur_para.h"

namespace OHOS {
namespace Rosen {
namespace {
GradientDirection GetRandomGradientDirection()
{
    static constexpr int GRADIENT_DIRECTION_INDEX_MAX = 10;
    return static_cast<GradientDirection>(RandomEngine::GetRandomIndex(GRADIENT_DIRECTION_INDEX_MAX));
}

std::vector<std::pair<float, float>> GetRandomFloatAndFloatPairVector()
{
    std::vector<std::pair<float, float>> vector;
    int vectorLength = RandomEngine::GetRandomMiddleVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        float x = RandomData::GetRandomFloat();
        float y = RandomData::GetRandomFloat();
        auto item = std::make_pair(x, y);
        vector.emplace_back(item);
    }
    return vector;
}
} // namespace

std::shared_ptr<RSLinearGradientBlurPara> RandomRSLinearGradientBlurPara::GetRandomRSLinearGradientBlurPara()
{
    std::shared_ptr<RSLinearGradientBlurPara> para = std::make_shared<RSLinearGradientBlurPara>(
        RandomData::GetRandomFloat(), GetRandomFloatAndFloatPairVector(), GetRandomGradientDirection());
    return para;
}
} // namespace Rosen
} // namespace OHOS
