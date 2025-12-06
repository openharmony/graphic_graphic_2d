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

#include "pipeline/rs_render_node_map.h"
#include "screen_manager/screen_types.h"

#include <mutex>
#include <unordered_map>

namespace OHOS {
namespace Rosen {
class RSPowerOffRenderSkipManager {
public:
    static RSPowerOffRenderSkipManager& Instance();
    void CheckRenderSkipStatus(const RSRenderNodeMap& nodeMap);
    bool GetScreenRenderSkipStatus(ScreenId screenId) const;
    bool GetAllScreenRenderSkipStatus() const;
private:
    RSPowerOffRenderSkipManager() = default;
    ~RSPowerOffRenderSkipManager() = default;
    RSPowerOffRenderSkipManager(const RSPowerOffRenderSkipManager&) = delete;
    RSPowerOffRenderSkipManager& operator=(const RSPowerOffRenderSkipManager&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<ScreenId, bool> screenRenderSkipStatus_;
    bool allScreenSkipStatus_ = false;
    bool disableRenderControl_ = false;
};
} // namespace Rosen
} // namespace OHOS