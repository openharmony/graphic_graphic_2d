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

#include "mem_param.h"

namespace OHOS::Rosen {
std::string MEMParam::GetRSWatchPoint() const
{
    return rsWatchPoint_;
}

void MEMParam::SetRSWatchPoint(std::string rsWatchPoint)
{
    rsWatchPoint_ = rsWatchPoint;
}

bool MEMParam::IsReclaimEnabled()
{
    return isReclaimEnabled_;
}

void MEMParam::SetReclaimEnabled(bool isEnabled)
{
    isReclaimEnabled_ = isEnabled;
}
} // namespace OHOS::Rosen