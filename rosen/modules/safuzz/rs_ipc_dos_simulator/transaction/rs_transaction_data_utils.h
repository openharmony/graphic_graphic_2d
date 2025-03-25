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

#ifndef SAFUZZ_RS_TRANSACTION_DATA_UTILS_H
#define SAFUZZ_RS_TRANSACTION_DATA_UTILS_H

#include <string>
#include <utility>
#include <vector>

#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSTransactionDataUtils {
public:
    static std::unique_ptr<RSTransactionData> CreateTransactionDataFromCommandList(const std::string& commandList,
        int commandListRepeat);

private:
    static bool GetCommandRepeatVecFromCommandList(std::vector<std::pair<std::string, int>>& commandRepeatVec,
        const std::string& commandList);
    static std::function<bool(std::unique_ptr<RSTransactionData>&)> GetAddFunctionByCommandName(
        const std::string& commandName);
};

class RSTransactionDataVariant : public RSTransactionData {
public:
    bool Marshalling(Parcel& parcel) const override;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_TRANSACTION_DATA_UTILS_H
