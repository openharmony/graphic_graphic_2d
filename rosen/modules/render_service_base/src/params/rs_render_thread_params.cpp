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

#include "params/rs_render_thread_params.h"

namespace OHOS::Rosen {
std::shared_ptr<Drawing::Image> RSRenderThreadParams::GetWatermark(const std::string& name) const
{
    auto iter = surfaceWatermarks_.find(name);
    if (iter == surfaceWatermarks_.end()) {
        return nullptr;
    }
    return iter->second.first;
}

void RSRenderThreadParams::SetWatermarks(std::unordered_map<std::string,
    std::pair<std::shared_ptr<Drawing::Image>, pid_t>>& watermarks)
{
    surfaceWatermarks_ = watermarks;
}

void RSRenderThreadParams::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

bool RSRenderThreadParams::IsSecurityDisplay() const
{
    return isSecurityDisplay_;
}

} // namespace OHOS::Rosen
