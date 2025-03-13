/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "command/rs_command_verify_helper.h"

namespace OHOS {
namespace Rosen {

constexpr int SURFACENODE_CREAET_LIMIT = 500;

RsCommandVerifyHelper &RsCommandVerifyHelper::GetInstance()
{
    static RsCommandVerifyHelper instance;
    return instance;
}

void RsCommandVerifyHelper::RegisterNonSystemPid(pid_t pid)
{
    std::unique_lock<std::mutex> lock(commandMutex_);
    if (nonSystemSurfaceNodeCreateCnt_.count(pid) == 0) {
        nonSystemSurfaceNodeCreateCnt_[pid] = 0;
    }
}

void RsCommandVerifyHelper::AddSurfaceNodeCreateCnt(pid_t pid)
{
    std::unique_lock<std::mutex> lock(commandMutex_);
    if (nonSystemSurfaceNodeCreateCnt_.find(pid) != nonSystemSurfaceNodeCreateCnt_.end()) {
        if (nonSystemSurfaceNodeCreateCnt_[pid] < SURFACENODE_CREAET_LIMIT) {
            nonSystemSurfaceNodeCreateCnt_[pid]++;
        }
    }
}

void RsCommandVerifyHelper::SubSurfaceNodeCreateCnt(pid_t pid)
{
    std::unique_lock<std::mutex> lock(commandMutex_);
    if (nonSystemSurfaceNodeCreateCnt_.find(pid) != nonSystemSurfaceNodeCreateCnt_.end()) {
        if (nonSystemSurfaceNodeCreateCnt_[pid] > 0) {
            nonSystemSurfaceNodeCreateCnt_[pid]--;
        }
    }
}

void RsCommandVerifyHelper::RemoveCntWithPid(pid_t pid)
{
    std::unique_lock<std::mutex> lock(commandMutex_);
    nonSystemSurfaceNodeCreateCnt_.erase(pid);
}

bool RsCommandVerifyHelper::IsSurfaceNodeCreateCommandVaild(pid_t pid)
{
    std::unique_lock<std::mutex> lock(commandMutex_);
    if ((nonSystemSurfaceNodeCreateCnt_.find(pid) != nonSystemSurfaceNodeCreateCnt_.end())) {
        if (nonSystemSurfaceNodeCreateCnt_[pid] >= SURFACENODE_CREAET_LIMIT) {
            return false;
        }
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
