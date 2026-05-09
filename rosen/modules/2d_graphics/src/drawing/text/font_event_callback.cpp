/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "text/font_event_callback.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

FontEventCallbackManager& FontEventCallbackManager::GetInstance()
{
    static FontEventCallbackManager instance;
    return instance;
}

void FontEventCallbackManager::RegisterUnloadFontFinishCallback(FontEventCallback callback)
{
    unloadFinishCallbacks_.AddCallback(std::move(callback));
}

void FontEventCallbackManager::OnUnloadFontFinish(const FontEventInfo& info)
{
    unloadFinishCallbacks_.ExecuteCallbacks(info);
}

void FontEventCallbackManager::CallbackSet::AddCallback(FontEventCallback cb)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (cb != nullptr) {
        callbacks.insert(std::move(cb));
    }
}

void FontEventCallbackManager::CallbackSet::ExecuteCallbacks(const FontEventInfo& info) const
{
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& cb : callbacks) {
        cb(info);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS