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

#ifndef RS_BEHIND_WINDOW_FILTER_MANAGER_H
#define RS_BEHIND_WINDOW_FILTER_MANAGER_H

#include <atomic>
#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RS_EXPORT RSBehindWindowFilterManager {
public:
    static RSBehindWindowFilterManager& Instance();

    bool IsBehindWindowFilterEnabledByCCM() const
    {
        return isBehindWindowFilterEnabledCCM_.load();
    }

    void SetBehindWindowFilterEnabledByCCM(bool enabled)
    {
        isBehindWindowFilterEnabledCCM_.store(enabled);
    }

private:
    RSBehindWindowFilterManager() = default;
    ~RSBehindWindowFilterManager() = default;
    RSBehindWindowFilterManager(const RSBehindWindowFilterManager&);
    RSBehindWindowFilterManager(const RSBehindWindowFilterManager&&);
    RSBehindWindowFilterManager& operator=(const RSBehindWindowFilterManager&);
    RSBehindWindowFilterManager& operator=(const RSBehindWindowFilterManager&&);

    std::atomic<bool> isBehindWindowFilterEnabledCCM_ = true;
};
} // namespace OHOS::Rosen
#endif // RS_BEHIND_WINDOW_FILTER_MANAGER_H
