/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMAND_RS_DELEGATE_COMPOSITE_COMMAND_H
#define RENDER_SERVICE_BASE_COMMAND_RS_DELEGATE_COMPOSITE_COMMAND_H

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "transaction/rs_buffer_transaction.h"

namespace OHOS {
namespace Rosen {
enum RSDelegateCompositeCommandType : uint16_t {
    TRANSACTION_BUFFER = 0,
    SURFACE_TRANSACTION_CMD = 1,
};

class RSB_EXPORT TransactionBufferCommand : public RSCommand {
public:
    TransactionBufferCommand();
    TransactionBufferCommand(NodeId nodeId);
    TransactionBufferCommand(std::vector<RSTransactionConfig>& configs, NodeId nodeId);

    void Process(RSContext& context) override;
    [[nodiscard]] static RSCommand* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    void SetRSTransactionConfigs(std::vector<RSTransactionConfig>& configs);

    void SetNodeId(NodeId nodeId);
    NodeId GetNodeId() const override
    {
        return nodeId_;
    }

    uint16_t GetType() const override
    {
        return RSCommandType::DELEGATE_COMPOSITE;
    }

    uint16_t GetSubType() const override
    {
        return RSDelegateCompositeCommandType::TRANSACTION_BUFFER;
    }

    RSCommandPermissionType GetAccessPermission() const override
    {
        return RSCommandPermissionType::PERMISSION_APP;
    }

private:
    sptr<IConsumerSurface> GetConsumerSurface(RSContext& context, NodeId nodeId);

    using Registrar = RSCommandRegister<RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::TRANSACTION_BUFFER, Unmarshalling>;

    std::vector<RSTransactionConfig> configList_;
    NodeId nodeId_ = 0;
    static Registrar instance_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_COMMAND_RS_DELEGATE_COMPOSITE_COMMAND_H