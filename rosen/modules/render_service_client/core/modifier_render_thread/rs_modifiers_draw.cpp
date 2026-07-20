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

#include "modifier_render_thread/rs_modifiers_draw.h"

#include "sandbox_utils.h"

#include "command/rs_delegate_composite_command.h"

namespace OHOS::Rosen {
void RSModifiersDraw::ConvertTransaction(
    std::unique_ptr<RSTransactionData>& transactionData, std::vector<RSTransactionConfig>& transactionConfigList)
{
    if (transactionData != nullptr && !transactionConfigList.empty()) {
        // Static is safe: GetRealPid() is constant within a process, and this nodeId
        // is only used to pass server-side PID validation (ExtractPid(nodeId) == callingPid).
        // Left shift by 32 to match the standard NodeId format (pid << 32 | sequence).
        static NodeId nodeId = (NodeId)GetRealPid() << 32;
        transactionData->AddCommand(
            std::make_unique<TransactionBufferCommand>(transactionConfigList, nodeId), nodeId, FollowType::NONE);
    }
}
} // namespace OHOS::Rosen