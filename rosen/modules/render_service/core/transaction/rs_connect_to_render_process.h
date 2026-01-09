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

#ifndef RENDER_SERVICE_CORE_TRANSACTION_RS_CONNECT_TO_RENDER_PROCESS_H
#define RENDER_SERVICE_CORE_TRANSACTION_RS_CONNECT_TO_RENDER_PROCESS_H

#include "transaction/zidl/rs_connect_to_render_process_stub.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "render_server/rs_render_service.h"
#include "core/rs_render_pipeline_agent.h"

namespace OHOS {
namespace Rosen {
class RSConnectToRenderProcess : public RSConnectToRenderProcessStub {
public:
    RSConnectToRenderProcess(sptr<RSRenderPipelineAgent> renderPipelineAgent);
    RSConnectToRenderProcess() = default;
    virtual ~RSConnectToRenderProcess() noexcept = default;
private:
    sptr<RSIClientToRenderConnection> CreateRenderConnection(const sptr<RSIConnectionToken>& token) override;
    
    sptr<RSRenderPipelineAgent> renderPipelineAgent_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_TRANSACTION_RS_CONNECT_TO_RENDER_PROCESS_H