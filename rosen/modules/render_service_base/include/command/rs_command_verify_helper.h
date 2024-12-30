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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_VERIFY_HELPER_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_VERIFY_HELPER_H

#include <mutex>
#include <map>
#include <unordered_map>

#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RsCommandVerifyHelper {
public:
    static RsCommandVerifyHelper &GetInstance();
    void RegisterNonSystemPid(pid_t pid);
    void AddSurfaceNodeCreateCnt(pid_t pid);
    void SubSurfaceNodeCreateCnt(pid_t pid);
    void RemoveCntWithPid(pid_t pid);
    bool IsSurfaceNodeCreateCommandVaild(pid_t pid);

private:
    RsCommandVerifyHelper() = default;
    ~RsCommandVerifyHelper() = default;
    RsCommandVerifyHelper(const RsCommandVerifyHelper&) = delete;
    RsCommandVerifyHelper(const RsCommandVerifyHelper&&) = delete;
    RsCommandVerifyHelper& operator=(const RsCommandVerifyHelper&) = delete;
    RsCommandVerifyHelper& operator=(const RsCommandVerifyHelper&&) = delete;
    // the number of times a non-system app can call surfaceNode create
    std::unordered_map<pid_t, uint32_t> nonSystemSurfaceNodeCreateCnt_;
    std::mutex commandMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_VERIFY_HELPER_H
