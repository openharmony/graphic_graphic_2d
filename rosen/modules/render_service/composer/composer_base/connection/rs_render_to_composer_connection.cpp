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

#include "rs_render_to_composer_connection.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSRenderToComposerConnection::RSRenderToComposerConnection(const std::string& name, uint64_t screenId,
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent)
    : screenId_(screenId), rsRenderComposerAgent_(rsRenderComposerAgent)
{}

void RSRenderToComposerConnection::CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData)
{
    RS_TRACE_NAME_FMT("RSRenderToComposerConnection::CommitLayers screenId:%" PRIu64, screenId_);
    if (transactionData.get() == nullptr) {
        RS_LOGE("RSRenderToComposerConnection::CommitLayers param nullptr");
        return;
    }
    rsRenderComposerAgent_->ComposerProcess(std::move(transactionData));
}
} // namespace Rosen
} // namespace OHOS