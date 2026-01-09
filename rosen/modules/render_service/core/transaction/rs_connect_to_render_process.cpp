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
#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSConnectToRenderProcess"

namespace OHOS {
namespace Rosen {

RSConnectToRenderProcess::RSConnectToRenderProcess(sptr<RSRenderPipelineAgent> renderPipelineAgent)
    : renderPipelineAgent_(renderPipelineAgent)
{}

sptr<RSIClientToRenderConnection> RSConnectToRenderProcess::CreateRenderConnection(const sptr<RSIConnectionToken>& token)
{
    if (!token) {
        RS_LOGE("RSConnectToRenderProcess::CreateRenderConnection CreateRenderConnection failed, token is nullptr.");
        return nullptr;
    }
    RS_LOGI("RSConnectToRenderProcess::%{public}s, %{public}p", __func__, token.GetRefPtr());
    pid_t remotePid = GetCallingPid();
    auto tokenObj = token->AsObject();
    if (renderPipelineAgent_ == nullptr) {
        RS_LOGW("RSClientToRenderConnection: renderPipelineAgent_ is nullptr");
    }
    sptr<RSIClientToRenderConnection> renderConnection = renderPipelineAgent_->FindClientToRenderConnection(tokenObj);
    if (renderConnection != nullptr) {
        return renderConnection;
    }
    RS_LOGI("RSConnectToRenderProcess::CreateRenderConnection");
    sptr<RSIClientToRenderConnection> newRenderConn =
        new RSClientToRenderConnection(remotePid, renderPipelineAgent_, tokenObj);
    renderPipelineAgent_->AddTransactionDataPidInfo(remotePid);
    renderPipelineAgent_->AddConnection(tokenObj, newRenderConn);
    RS_LOGI("RSConnectToRenderProcess::%{public}s, has not the same token one %{public}p, return %{public}p.", __func__,
        tokenObj.GetRefPtr(), newRenderConn.GetRefPtr());
    return newRenderConn;
}
} // namespace Rosen
} // namespace OHOS