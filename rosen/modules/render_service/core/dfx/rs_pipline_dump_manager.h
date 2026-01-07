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
#ifndef RENDER_PROCESS_DFX_RS_PIPLINE_DUMP_MANAGER_H
#define RENDER_PROCESS_DFX_RS_PIPLINE_DUMP_MANAGER_H

#include "gfx/dump/rs_dump_manager.h"
#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSPiplineDumpManager : public RSDumpManager {
public:
    RSPiplineDumpManager() {};
    ~RSPiplineDumpManager() = default;

    // Execute a command
    void CmdExec(std::unordered_set<std::u16string>& argSets, std::string &out, sptr<RSIDumpCallback> callback);
    void SetPid(int pid);
    void SetScreenId(unsigned long screenId);

private:
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    uint64_t screenId_;
    pid_t pid_;
};
}
#endif // RENDER_PROCESS_DFX_RS_PIPLINE_DUMP_MANAGER_H