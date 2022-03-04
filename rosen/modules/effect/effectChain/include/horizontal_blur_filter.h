/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HORIZONTAL_BLUR_FILTER_H
#define HORIZONTAL_BLUR_FILTER_H

#include "algo_filter.h"

namespace OHOS {
namespace Rosen {
class HorizontalBlurFilter : public AlgoFilter {
public:
    static constexpr int RADIUS = 3;
    static constexpr float DEFAULT_WEIGHT_ONE = 0.227f;
    static constexpr float DEFAULT_WEIGHT_TWO = 0.316f;
    static constexpr float DEFAULT_WEIGHT_THREE = 0.070f;
    static constexpr float DEFAULT_OFFSET_ONE = 0.0;
    static constexpr float DEFAULT_OFFSET_TWO = 1.385f;
    static constexpr float DEFAULT_OFFSET_THREE = 3.231f;

    HorizontalBlurFilter();
    void SetValue(const std::string& key, void *value, int size) override;
    std::string GetFragmentShader() override;
    std::string GetVertexShader() override;

private:
    void LoadFilterParams() override;
    GLint weightID_ = 0;
    GLint offsetID_ = 0;
    float weight_[RADIUS] = {DEFAULT_WEIGHT_ONE, DEFAULT_WEIGHT_TWO, DEFAULT_WEIGHT_THREE};
    float offset_[RADIUS] = {DEFAULT_OFFSET_ONE, DEFAULT_OFFSET_TWO, DEFAULT_OFFSET_THREE};
};
} // namespace Rosen
} // namespace OHOS
#endif // HORIZONTAL_BLUR_FILTER_H

