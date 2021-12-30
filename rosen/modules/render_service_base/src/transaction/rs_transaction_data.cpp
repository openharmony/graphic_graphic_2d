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

namespace OHOS {
namespace Rosen {
RSTransactionData::~RSTransactionData() noexcept
{
}

#ifdef ROSEN_OHOS
RSTransactionData* RSTransactionData::Unmarshalling(Parcel& parcel)
{
    auto transactionData = new RSTransactionData();
    return transactionData;
}

bool RSTransactionData::Marshalling(Parcel& parcel) const
{
    bool success = true;
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
    return false;
}

#endif // ROSEN_OHOS

} // namespace Rosen
} // namespace OHOS
