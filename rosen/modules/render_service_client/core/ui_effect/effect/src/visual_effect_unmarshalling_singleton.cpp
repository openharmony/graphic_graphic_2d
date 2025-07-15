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

#include "ui_effect/effect/include/visual_effect_unmarshalling_singleton.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

VisualEffectUnmarshallingSingleton& VisualEffectUnmarshallingSingleton::GetInstance()
{
    static VisualEffectUnmarshallingSingleton instance;
    return instance;
}

void VisualEffectUnmarshallingSingleton::RegisterCallback(
    uint16_t type, VisualEffectUnmarshallingSingleton::UnmarshallingFunc func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    unmarshallingFuncs_.insert_or_assign(type, func);
    RS_LOGD("[ui_effect] VisualEffect register unmarshalling callback, type is %{public}hu", type);
}

void VisualEffectUnmarshallingSingleton::UnregisterCallback(uint16_t type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    unmarshallingFuncs_.erase(type);
    RS_LOGD("[ui_effect] VisualEffect unregister unmarshalling callback, type is %{public}hu", type);
}

VisualEffectUnmarshallingSingleton::UnmarshallingFunc VisualEffectUnmarshallingSingleton::GetCallback(uint16_t type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = unmarshallingFuncs_.find(type);
    if (it != unmarshallingFuncs_.end()) {
        return it->second;
    }
    RS_LOGE("[ui_effect] VisualEffectUnmarshallingSingleton GetCallback find failed, type is %{public}hu", type);
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS