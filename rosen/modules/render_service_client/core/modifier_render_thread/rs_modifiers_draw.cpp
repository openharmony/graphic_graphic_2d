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

#include "command/rs_delegate_composite_command.h"

namespace OHOS::Rosen {
void RSModifiersDraw::ConvertTransaction(
    std::unique_ptr<RSTransactionData>& transactionData, std::vector<RSTransactionConfig>& transactionConfigList)
{
    if (transactionConfigList.empty()) {
        return;
    }

    auto& payload = transactionData->GetPayload();
    for (auto it = payload.rbegin(); it != payload.rend(); ++it) {
        auto& command = std::get<2>(*it); // The element which index 2 is RSCommand.
        if (command == nullptr) {
            continue;
        }
        if (command->GetType() != RSCommandType::DELEGATE_COMPOSITE ||
            command->GetSubType() != RSDelegateCompositeCommandType::TRANSACTION_BUFFER) {
            continue;
        }

        auto transactionBufferCommand = static_cast<TransactionBufferCommand*>(command.get());
        transactionBufferCommand->SetRSTransactionConfigs(transactionConfigList);
        break;
    }
}
} // namespace OHOS::Rosen