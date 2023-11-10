/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_render_service_security_utils.h"

namespace OHOS {
namespace Rosen {

uint32_t RSRenderServiceSecurityUtils::GetCodeAccessCounter(uint32_t code) const
{
    std::lock_guard<std::mutex> lock(accessCounterMutex_);
    return accessCounter_.count(code) == 0 ? 0 : accessCounter_.at(code);
}

void RSRenderServiceSecurityUtils::IncreaseAccessCounter(uint32_t code)
{
    std::lock_guard<std::mutex> lock(accessCounterMutex_);
    accessCounter_[code] = accessCounter_.count(code) == 0 ? 1 : accessCounter_[code] + 1;
}

} // namespace Rosen
} // namespace OHOS
