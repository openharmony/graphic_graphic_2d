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

#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_property.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcessAgent"

namespace OHOS {
namespace Rosen {
bool RSRenderProcessAgent::NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
    const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    return renderProcess_.handler_->PostTask([this, screenProperty, renderToComposerConn, composerToRenderConn] {
        RS_LOGI("%{public}s: screenId[%{public}" PRIu64 "] width[%{public}d] height[%{public}d]", __func__,
            screenProperty->GetScreenId(), screenProperty->GetWidth(), screenProperty->GetHeight());
        renderProcess_.renderPipeline_->OnScreenConnected(
            screenProperty, renderToComposerConn, composerToRenderConn, nullptr);
    });
}

bool RSRenderProcessAgent::NotifyScreenDisconnectInfoToRender(ScreenId screenId)
{
    return renderProcess_.handler_->PostTask([this, screenId] {
        RS_LOGI("%{public}s: screenId[%{public}" PRIu64 "]", __func__, screenId);
        renderProcess_.renderPipeline_->OnScreenDisconnected(screenId);
    });
}

bool RSRenderProcessAgent::NotifyScreenPropertyChangedInfoToRender(
    ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& screenProperty)
{
    return renderProcess_.handler_->PostTask([this, id, type, screenProperty] {
        renderProcess_.renderPipeline_->OnScreenPropertyChanged(id, type, screenProperty);
    });
}
} // namespace Rosen
} // namespace OHOS
