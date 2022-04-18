/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    for (auto& command : commands_) {
        success &= command->Marshalling(parcel);
        if (!success) {
            ROSEN_LOGE("failed RSTransactionData::Marshalling type:%s", command->PrintType().c_str());
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
}

void RSTransactionData::AddCommand(std::unique_ptr<RSCommand>& command)
{
    commands_.emplace_back(std::move(command));
}

void RSTransactionData::AddCommand(std::unique_ptr<RSCommand>&& command)
{
    commands_.emplace_back(std::move(command));
}

#ifdef ROSEN_OHOS
bool RSTransactionData::UnmarshallingCommand(Parcel& parcel)
{
    uint16_t commandType = 0;
    uint16_t commandSubType = 0;
    bool isNotFinished = true;
    commands_.clear();
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
            ROSEN_LOGE("failed RSTransactionData::UnmarshallingCommand, type=%d subtype=%d", commandType, commandSubType);
            break;
        }
        AddCommand(std::unique_ptr<RSCommand>(command));
    }
    return !isNotFinished;
}

#endif // ROSEN_OHOS

} // namespace Rosen
} // namespace OHOS
