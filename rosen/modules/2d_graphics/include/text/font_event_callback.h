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

#ifndef FONT_EVENT_CALLBACK_H
#define FONT_EVENT_CALLBACK_H

#include <functional>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <string>

#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct DRAWING_API FontEventInfo {
    std::string familyName;
    std::vector<uint32_t> uniqueIds;

    FontEventInfo() = default;
    explicit FontEventInfo(const std::string& name) : familyName(name) {}
};

using FontEventCallback = void (*)(const FontEventInfo&);

class DRAWING_API FontEventCallbackManager {
public:
    static FontEventCallbackManager& GetInstance();

    void RegisterUnloadFontFinishCallback(FontEventCallback callback);

    void OnUnloadFontFinish(const FontEventInfo& info);

private:
    FontEventCallbackManager() = default;
    ~FontEventCallbackManager() = default;
    FontEventCallbackManager(const FontEventCallbackManager&) = delete;
    FontEventCallbackManager& operator=(const FontEventCallbackManager&) = delete;

    struct CallbackSet {
        mutable std::mutex mutex;
        std::unordered_set<FontEventCallback> callbacks;

        void AddCallback(FontEventCallback cb);
        void ExecuteCallbacks(const FontEventInfo& info) const;
    };

    CallbackSet unloadFinishCallbacks_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // FONT_EVENT_CALLBACK_H