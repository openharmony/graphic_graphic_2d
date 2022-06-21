/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "transaction/rs_transaction_data.h"

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSTransactionData::~RSTransactionData() noexcept
{
}

#ifdef ROSEN_OHOS
RSTransactionData* RSTransactionData::Unmarshalling(Parcel& parcel)
{
    auto transactionData = new RSTransactionData();
    if (transactionData->UnmarshallingCommand(parcel)) {
        return transactionData;
    }
    ROSEN_LOGE("RSTransactionData Unmarshalling Failed");
    delete transactionData;
    return nullptr;
}

bool RSTransactionData::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success = success && parcel.WriteInt32(static_cast<int32_t>(commands_.size()));
    for (auto followType : followTypes_) {
        success = success && parcel.WriteUint32(static_cast<uint8_t>(followType));
    }
    success = success && parcel.WriteUInt64Vector(nodeIds_);
    success = success && parcel.WriteUint64(timestamp_);
    for (auto& command : commands_) {
        success = command->Marshalling(parcel);
        if (!success) {
            ROSEN_LOGE("failed RSTransactionData::Marshalling type:%s", command->PrintType().c_str());
            break;
        }
    }
    return success;
}
#endif // ROSEN_OHOS

void RSTransactionData::Process(RSContext& context)
{
    for (auto& command : commands_) {
        if (command != nullptr) {
            command->Process(context);
        }
    }
}

void RSTransactionData::Clear()
{
    commands_.clear();
    followTypes_.clear();
    nodeIds_.clear();
    timestamp_ = 0;
}

void RSTransactionData::AddCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    commands_.emplace_back(std::move(command));
    followTypes_.push_back(followType);
    nodeIds_.push_back(nodeId);
}

void RSTransactionData::AddCommand(std::unique_ptr<RSCommand>&& command, NodeId nodeId, FollowType followType)
{
    commands_.emplace_back(std::move(command));
    followTypes_.push_back(followType);
    nodeIds_.push_back(nodeId);
}

#ifdef ROSEN_OHOS
bool RSTransactionData::UnmarshallingCommand(Parcel& parcel)
{
    uint16_t commandType = 0;
    uint16_t commandSubType = 0;
    int commandSize = 0;
    if (!parcel.ReadInt32(commandSize)) {
        ROSEN_LOGE("RSTransactionData::UnmarshallingCommand cannot read commandSize");
        return false;
    }
    uint8_t followType = 0;
    bool isNotFinished = true;
    Clear();

    for (int i = 0; i < commandSize; i++) {
        if (!parcel.ReadUint8(followType)) {
            ROSEN_LOGE("RSTransactionData::UnmarshallingCommand cannot read followType");
            return false;
        }
        followTypes_.push_back(static_cast<FollowType>(followType));
    }

    if (!(parcel.ReadUInt64Vector(&nodeIds_) && parcel.ReadUint64(timestamp_))) {
        ROSEN_LOGE("RSTransactionData::UnmarshallingCommand cannot read nodeIds or timestamp");
        return false;
    }
    while (isNotFinished) {
        if (!(parcel.ReadUint16(commandType) && parcel.ReadUint16(commandSubType))) {
            isNotFinished = false;
            break;
        }
        auto func = RSCommandFactory::Instance().GetUnmarshallingFunc(commandType, commandSubType);
        if (func == nullptr) {
            break;
        }
        auto command = (*func)(parcel);
        if (command == nullptr) {
            ROSEN_LOGE("failed RSTransactionData::UnmarshallingCommand, type=%d subtype=%d", commandType,
                commandSubType);
            break;
        }
        commands_.emplace_back(std::unique_ptr<RSCommand>(command));
    }
    return !isNotFinished;
}

#endif // ROSEN_OHOS

} // namespace Rosen
} // namespace OHOS
