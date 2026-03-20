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

#include "rs_render_process_agent.h"

// #include "dfx/rs_process_dump_manager.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "screen_manager/rs_screen_property.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessAgent"

namespace OHOS {
namespace Rosen {
RSRenderProcessAgent::RSRenderProcessAgent(RSRenderProcess& renderProcess) :
    renderProcess_(renderProcess) {}

int32_t RSRenderProcessAgent::NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
    const sptr<IRSRenderToComposerConnection>& composerConn)
{
    auto handler = renderProcess_.handler_;
    handler->PostTask([this, screenProperty, composerConn] {
        // std::shared_ptr<HgmClient> hgmClient = HgmClient::Create(renderProcess_.renderToServiceConnection_);
        // RSProcessDumpManager::GetInstance().SetRenderToServiceConnection(renderProcess_.renderToServiceConnection_);
        // std::shared_ptr<RSRenderComposerClient> composerClient = nullptr;
        // if (composerConn) {
        //     composerClient = RSRenderComposerClient::Create(true, composerConn);
        // }
        // RS_LOGI("dmulti_process %{public}s: screenId[%{public}llu] width[%{public}d] height[%{public}d]",
        //     __func__, screenProperty->GetScreenId(), screenProperty->GetWidth(), screenProperty->GetHeight());
        renderProcess_.renderPipeline_->OnScreenConnected(screenProperty, composerClient, hgmClient);
    });
    return 0;
}

int32_t RSRenderProcessAgent::NotifyScreenDisconnectInfoToRender(ScreenId screenId)
{
    auto handler = renderProcess_.handler_;
    handler->PostTask([this, screenId] {
        RS_LOGI("dmulti_process %{public}s: screenId[%{public}llu]", __func__, screenId);
        renderProcess_.renderPipeline_->OnScreenDisconnected(screenId);
    });
    return 0;
}

int32_t RSRenderProcessAgent::NotifyScreenPropertyChangedInfoToRender(const sptr<RSScreenPropertyBase>& screenProperty)
{
    auto handler = renderProcess_.handler_;
    // handler->PostTask([this, screenProperty] {
    //     RS_LOGI("dmulti_process %{public}s: screenId[%{public}llu] width[%{public}d] height[%{public}d]",
    //         __func__, screenProperty->GetScreenId(), screenProperty->GetWidth(), screenProperty->GetHeight());
    //     renderProcess_.renderPipeline_->OnScreenPropertyChanged(screenProperty);
    // });
    return 0;
}
} // namespace Rosen
} // namespace OHOS
