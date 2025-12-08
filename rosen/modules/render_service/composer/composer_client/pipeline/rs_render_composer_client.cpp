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

#include "rs_render_composer_client.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSRenderComposerClient::RSRenderComposerClient(const sptr<RSRenderToComposerConnection>& conn)
{
    rsLayerContext_ = std::make_shared<RSLayerContext>();
    rsLayerContext_->SetRenderComposerClientConnection(conn);
}

std::shared_ptr<RSRenderComposerClient> RSRenderComposerClient::Create(
    const sptr<RSRenderToComposerConnection>& conn)
{
    RS_TRACE_NAME_FMT("RSRenderComposerClient::Create");
    if (conn == nullptr) {
        RS_LOGE("RSRenderComposerClient::Create conn is nullptr");
        return nullptr;
    }
    return std::make_shared<RSRenderComposerClient>(conn);
}

void RSRenderComposerClient::CommitLayers()
{
    rsLayerContext_->CommitLayers();
}

void RSRenderComposerClient::AddLayer(const std::shared_ptr<RSLayer> rsLayer)
{
    rsLayerContext_->AddLayer(rsLayer);
}

void RSRenderComposerClient::ClearAllLayers()
{
    RS_TRACE_NAME_FMT("RSRenderComposerClient::ClearAllLayers");
    rsLayerContext_->ClearAllLayers();
}
} // namespace Rosen
} // namespace OHOS