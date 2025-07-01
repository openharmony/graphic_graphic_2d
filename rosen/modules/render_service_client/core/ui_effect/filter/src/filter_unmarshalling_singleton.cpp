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

#include "ui_effect/filter/include/filter_unmarshalling_singleton.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

FilterUnmarshallingSingleton& FilterUnmarshallingSingleton::GetInstance()
{
    static FilterUnmarshallingSingleton instance;
    return instance;
}

void FilterUnmarshallingSingleton::RegisterCallback(
    uint16_t type, FilterUnmarshallingSingleton::UnmarshallingFunc func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    unmarshallingFuncs_.insert_or_assign(type, func);
}

void FilterUnmarshallingSingleton::UnregisterCallback(uint16_t type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    unmarshallingFuncs_.erase(type);
}

FilterUnmarshallingSingleton::UnmarshallingFunc FilterUnmarshallingSingleton::GetCallback(uint16_t type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = unmarshallingFuncs_.find(type);
    if (it != unmarshallingFuncs_.end()) {
        return it->second;
    }
    RS_LOGE("[ui_effect] FilterUnmarshallingSingleton GetCallback find failed, type is %{public}d", type);
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS