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

#ifndef HGM_UPDATE_CALLBACK_H
#define HGM_UPDATE_CALLBACK_H

#include <functional>
#include <mutex>
#include <vector>

namespace OHOS {
namespace Rosen {
class HgmUpdateCallback {
public:
    using UpdateCallback = std::function<void(HgmUpdateCallback&)>;
    HgmUpdateCallback() = default;
    virtual ~HgmUpdateCallback() = default;

    void RegisterUpdateCallback(const UpdateCallback& callback)
    {
        if (callback == nullptr) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        updateCallback_.emplace_back(callback);
    }
    virtual void OnUpdate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& callback : updateCallback_) {
            if (callback != nullptr) {
                callback(*this);
            }
        }
    }
protected:
    mutable std::mutex mutex_;
    std::vector<UpdateCallback> updateCallback_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_UPDATE_CALLBACK_H