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

#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "common/rs_background_thread.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSFirstFrameNotifier& RSFirstFrameNotifier::GetInstance()
{
    static RSFirstFrameNotifier instance;
    return instance;
}

void RSFirstFrameNotifier::RegisterFirstFrameCommitCallback(
    pid_t pid, const sptr<RSIFirstFrameCommitCallback>& callback)
{
    std::unique_lock<std::shared_mutex> lock(callbacksMutex_);
    if (callback == nullptr) {
        if (firstFrameCommitCallbacks_.find(pid) != firstFrameCommitCallbacks_.end()) {
            firstFrameCommitCallbacks_.erase(pid);
            RS_LOGD("FirstFrameCommitCallback unregister succ.");
        }
        return ;
    }
    firstFrameCommitCallbacks_[pid] = callback;
    RS_LOGD("FirstFrameCommitCallback: add a remote callback succeed.");
}

void RSFirstFrameNotifier::OnFirstFrameCommitCallback(ScreenId screenId)
{
    std::shared_lock<std::shared_mutex> lock(callbacksMutex_);
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    RS_TRACE_NAME_FMT("OnFirstFrameCommitCallback screenId:%" PRIu64 ", timestamp:%" PRId64 ".",
        screenId, timestamp);
    for (const auto& callback : firstFrameCommitCallbacks_) {
        if (callback.second != nullptr) {
            callback.second->OnFirstFrameCommit(screenId, timestamp);
        }
    }
}

void RSFirstFrameNotifier::ExecIfFirstFrameCommit(ScreenId screenId)
{
    std::unique_lock<std::mutex> lock(screensMutex_);
    if (firstFrameCommitScreens_.find(screenId) != firstFrameCommitScreens_.end()) {
        firstFrameCommitScreens_.erase(screenId);
        RS_TRACE_NAME_FMT("ExecIfFirstFrameCommit screenId:%" PRIu64 ".", screenId);
        RS_LOGD("ExecIfFirstFrameCommit screenId:%{public}" PRIu64 ".", screenId);
        RSBackgroundThread::Instance().PostTask([this, screenId]() {
            OnFirstFrameCommitCallback(screenId);
        });
    }
}

void RSFirstFrameNotifier::AddFirstFrameCommitScreen(ScreenId screenId)
{
    std::unique_lock<std::mutex> lock(screensMutex_);
    firstFrameCommitScreens_.insert(screenId);
}
}