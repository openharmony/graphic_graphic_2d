/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSShader> RSShader::CreateRSShader()
{
    return std::make_shared<RSShader>();
}

std::shared_ptr<RSShader> RSShader::CreateRSShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader)
{
    auto rsShader = std::make_shared<RSShader>();
    rsShader->SetDrawingShader(drShader);
    return rsShader;
}

void RSShader::SetDrawingShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader)
{
    drShader_ = drShader;
}

const std::shared_ptr<Drawing::ShaderEffect>& RSShader::GetDrawingShader() const
{
    return drShader_;
}
} // namespace Rosen
} // namespace OHOS