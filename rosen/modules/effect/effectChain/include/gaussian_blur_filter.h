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

#ifndef GAUSSIAN_BLUR_FILTER_H
#define GAUSSIAN_BLUR_FILTER_H

#include <algorithm>
#include "algo_filter.h"
#include "horizontal_blur_filter.h"
#include "vertical_blur_filter.h"
#include "scale_filter.h"

namespace OHOS {
namespace Rosen {
class GaussianBlurFilter : public AlgoFilter {
public:
    static constexpr int RADIUS = 3;
    static constexpr float DOWNSAMPLE_FACTOR = 0.25f;
    static constexpr float INVERSE_DOWNSAMPLE_FACTOR = 1.0f / DOWNSAMPLE_FACTOR;
    GaussianBlurFilter();
    ~GaussianBlurFilter();
    void SetValue(const std::string& key, void* value, int size) override;

private:
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    void DoProcess(ProcessData& data) override;
    void LoadFilterParams() override {};
    ScaleFilter* upSampleFilter_ = nullptr;
    ScaleFilter* downSampleFilter_ = nullptr;
    HorizontalBlurFilter* horizontalBlurFilter_ = nullptr;
    VerticalBlurFilter* verticalBlurFilter_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // GAUSSIAN_BLUR_FILTER_H

