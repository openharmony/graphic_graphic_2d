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
#ifndef RS_DELEGATE_COMPOSITE_COMMAND_H
#define RS_DELEGATE_COMPOSITE_COMMAND_H

#ifndef ROSEN_CROSS_PLATFORM
#include <vector>
#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "transaction/transaction.h"

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
    TransactionBufferCommand(RectF rect, bool isSrcRect, NodeId nodeId);

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

    enum class CmdType : int32_t {
        INVALID = -1,
        SET_BUFFER = 0,
        SET_RECT,
    };

    // common
    CmdType cmdType_ = CmdType::INVALID;
    NodeId nodeId_ = 0;

    // setBuffer
    void ProcessCmdTypeSetBuffer(RSContext& context);
    void ProcessCmdTypeSetBufferwithAttach(sptr<Transaction> transaction,
        sptr<IConsumerSurface> consumer, const RSTransactionConfig& config);
    static RSCommand* UnmarshallingCmdTypeSetBuffer(MessageParcel* messageParcel);
    bool MarshallingCmdTypeSetBuffer(MessageParcel* messageParcel) const;
    std::vector<RSTransactionConfig> configList_;

    // set rect
    void ProcessCmdTypeSetRect(RSContext& context);
    static RSCommand* UnmarshallingCmdTypeSetRect(MessageParcel* messageParcel);
    bool MarshallingCmdTypeSetRect(MessageParcel* messageParcel) const;
    bool isSrcRect_ = true;
    RectF rect_;

    // rs command define
    using Register = RSCommandRegister<RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::TRANSACTION_BUFFER, Unmarshalling>;
    static Register instance_;
};

class RSB_EXPORT SurfaceTransactionCommand : public RSCommand {
public:
    SurfaceTransactionCommand(uint64_t srcId, uint64_t seqNum, pid_t pid, pid_t tid);

    void Process(RSContext& context) override;
    [[nodiscard]] static RSCommand* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    uint16_t GetType() const override
    {
        return RSCommandType::DELEGATE_COMPOSITE;
    }

    uint16_t GetSubType() const override
    {
        return RSDelegateCompositeCommandType::SURFACE_TRANSACTION_CMD;
    }

    RSCommandPermissionType GetAccessPermission() const override
    {
        return RSCommandPermissionType::PERMISSION_APP;
    }
private:
    uint64_t commandSrcId_ = 0;
    uint64_t commandSeqNum_ = 0;
    pid_t commandSendPid_ = 0;
    pid_t commandSendTid_ = 0;
    using Register = RSCommandRegister<RSCommandType::DELEGATE_COMPOSITE,
        RSDelegateCompositeCommandType::SURFACE_TRANSACTION_CMD, Unmarshalling>;
    static Register instance_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif // RS_DELEGATE_COMPOSITE_COMMAND_H
