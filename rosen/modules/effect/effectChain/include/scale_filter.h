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

#ifndef SCALE_FILTER_H
#define SCALE_FILTER_H

#include <cmath>
#include "algo_filter.h"

namespace OHOS {
namespace Rosen {
class ScaleFilter : public AlgoFilter {
public:
    static constexpr float DEFAULT_SACLE = 1.0f;
    ScaleFilter();
    ~ScaleFilter() {}
    void SetValue(const std::string& key, void* value, int size) override;
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    void SetScale(float scale);

private:
    void DoProcess(ProcessData& data) override;
    void LoadFilterParams() override {};
    float scale_ = DEFAULT_SACLE;
};
} // namespace Rosen
} // namespace OHOS
#endif // SCALE_FILTER_H

