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

#include "rs_render_process_manager_agent.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessManagerAgent"

namespace OHOS {
namespace Rosen {
RSRenderProcessManagerAgent::RSRenderProcessManagerAgent(sptr<RSRenderProcessManager> renderProcessManager)
    : renderProcessManager_(renderProcessManager) {}

sptr<RSIServiceToRenderConnection> RSRenderProcessManagerAgent::GetServiceToRenderConn(ScreenId screenId) const
{
    return renderProcessManager_->GetServiceToRenderConn(screenId);
}

std::vector<sptr<RSIServiceToRenderConnection>> RSRenderProcessManagerAgent::GetServiceToRenderConns() const
{
    return renderProcessManager_->GetServiceToRenderConns();
}
} // namespace Rosen
} // namespace OHOS