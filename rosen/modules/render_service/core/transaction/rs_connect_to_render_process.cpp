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

#include "rs_connect_to_render_process.h"

#include "transaction/rs_client_to_render_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcess"

namespace OHOS {
namespace Rosen {
sptr<RSIClientToRenderConnection> RSConnectToRenderProcess::CreateRenderConnection(
    const sptr<RSIConnectionToken>& token)
{
    if (!token) {
        RS_LOGE("%{public}s: token is nullptr.", __func__);
        return nullptr;
    }
    auto tokenObj = token->AsObject();
    if (auto renderConnection = renderPipelineAgent_->FindClientToRenderConnection(tokenObj)) {
        return renderConnection;
    }
    pid_t remotePid = GetCallingPid();
    auto newRenderConn = sptr<RSClientToRenderConnection>::MakeSptr(remotePid, renderPipelineAgent_, tokenObj);
    renderPipelineAgent_->AddTransactionDataPidInfo(remotePid);
    renderPipelineAgent_->AddConnection(tokenObj, newRenderConn);
    return newRenderConn;
}
} // namespace Rosen
} // namespace OHOS