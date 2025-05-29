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

#ifndef RENDER_SERVICE_BASE_GFX_FIRST_FRAME_NOTIFIER_RS_FIRST_FRAME_NOTIFIER_H
#define RENDER_SERVICE_BASE_GFX_FIRST_FRAME_NOTIFIER_RS_FIRST_FRAME_NOTIFIER_H

#include <array>
#include <mutex>
#include <shared_mutex>
#include <string>

#include "common/rs_common_def.h"
#include "ipc_callbacks/rs_ifirst_frame_commit_callback.h"
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSFirstFrameNotifier {
public:
    static RSFirstFrameNotifier &GetInstance();
    void RegisterFirstFrameCommitCallback(pid_t pid, const sptr<RSIFirstFrameCommitCallback>& callback);
    void OnFirstFrameCommitCallback(ScreenId screenId);
    void ExecIfFirstFrameCommit(ScreenId screenId);
    void AddFirstFrameCommitScreen(ScreenId screenId);
private:
    RSFirstFrameNotifier() = default;
    ~RSFirstFrameNotifier() = default;
    RSFirstFrameNotifier(const RSFirstFrameNotifier&) = delete;
    RSFirstFrameNotifier(const RSFirstFrameNotifier&&) = delete;
    RSFirstFrameNotifier& operator=(const RSFirstFrameNotifier&) = delete;
    RSFirstFrameNotifier& operator=(const RSFirstFrameNotifier&&) = delete;

    std::mutex screensMutex_;
    std::unordered_set<ScreenId> firstFrameCommitScreens_;

    std::shared_mutex callbacksMutex_;
    std::unordered_map<pid_t, sptr<RSIFirstFrameCommitCallback>> firstFrameCommitCallbacks_;
};
}
#endif