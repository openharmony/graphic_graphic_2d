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

#ifndef SATURATION_FILTER_H
#define SATURATION_FILTER_H

#include "algo_filter.h"

namespace OHOS {
namespace Rosen {
class SaturationFilter : public AlgoFilter {
public:
    // saturation: The degree of saturation or desaturation to apply to the image (0.0 - 2.0, with 1.0 as the default)
    static constexpr float DEFAULT_SATURATION = 1.0f;

    SaturationFilter();
    ~SaturationFilter() {}
    void SetValue(const std::string& key, void* value, int size) override;
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;

private:
    void LoadFilterParams() override;
    float saturation_ = DEFAULT_SATURATION;
    GLint saturationID_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // SATURATION_FILTER_H