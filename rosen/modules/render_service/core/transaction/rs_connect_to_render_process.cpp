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
#include "rs_profiler.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcess"

namespace OHOS {
namespace Rosen {
std::pair<sptr<RSIClientToRenderConnection>, uint64_t> RSConnectToRenderProcess::CreateRenderConnection(
    uint64_t tokenMaskId, const sptr<RSIConnectionToken>& token, bool needRefresh)
{
    if (!token) {
        RS_LOGE("%{public}s: token is nullptr.", __func__);
        return {nullptr, INVALID_TOKEN_MASK_ID};
    }
    auto tokenObj = token->AsObject();
    pid_t remotePid = GetCallingPid();
    RS_PROFILER_ON_CREATE_CONNECTION(remotePid);
    auto [renderConnection, oldTokenMaskId] = renderPipelineAgent_->FindClientToRenderConnection(remotePid);
    if (renderConnection != nullptr) {
        return {renderConnection, INVALID_TOKEN_MASK_ID};
    }
    auto newRenderConn =
        sptr<RSClientToRenderConnection>::MakeSptr(remotePid, renderPipelineAgent_, tokenObj, needRefresh);
    renderPipelineAgent_->AddTransactionDataPidInfo(remotePid);
    renderPipelineAgent_->AddConnection(remotePid, tokenMaskId, tokenObj, newRenderConn);
    return {newRenderConn, INVALID_TOKEN_MASK_ID};
}

std::pair<sptr<RSIClientToRenderConnection>, uint64_t> RSConnectToRenderProcess::FindClientToRenderConnection()
{
    pid_t remotePid = GetCallingPid();
    return renderPipelineAgent_->FindClientToRenderConnection(remotePid);
}
bool RSConnectToRenderProcess::RemoveConnection(uint64_t tokenMaskId)
{
    return true;
}
} // namespace Rosen
} // namespace OHOS