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
    std::shared_ptr<RSShader> rsShader(new RSShader());
    return rsShader;
}

std::shared_ptr<RSShader> RSShader::CreateRSShader(const sk_sp<SkShader>& skShader)
{
    std::shared_ptr<RSShader> rsShader(new RSShader());
    rsShader->SetSkShader(skShader);
    return rsShader;
}

void RSShader::SetSkShader(const sk_sp<SkShader>& skShader)
{
    skShader_ = skShader;
}

const sk_sp<SkShader>& RSShader::GetSkShader() const
{
    return skShader_;
}
} // namespace Rosen
} // namespace OHOS