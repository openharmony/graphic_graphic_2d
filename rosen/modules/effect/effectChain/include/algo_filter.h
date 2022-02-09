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

#ifndef ALGO_FILTER_H
#define ALGO_FILTER_H

#include "cJSON.h"
#include "filter.h"
#include "mesh.h"

namespace OHOS {
namespace Rosen {
class AlgoFilter : public Filter {
public:
    static constexpr int DRAW_ELEMENTS_NUMBER = 6;
    AlgoFilter();
    virtual ~AlgoFilter();
    virtual FILTER_TYPE GetFilterType() override;
    virtual void SetValue(const std::string& key, void* value, int size) override {};

protected:
    void Use();
    virtual std::string GetVertexShader() = 0;
    virtual std::string GetFragmentShader() = 0;
    virtual void LoadFilterParams() = 0;
    virtual void DoProcess(ProcessData& data) override;
    virtual void Prepare(ProcessData& data);
    virtual void Draw(ProcessData& data);
    virtual void CreateProgram(const std::string& vertexString, const std::string& fragmentString);
    Program* program_ = nullptr;
    Mesh* mesh_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ALGO_FILTER_H