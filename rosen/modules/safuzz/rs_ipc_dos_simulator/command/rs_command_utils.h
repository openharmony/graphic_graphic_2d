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

#ifndef SAFUZZ_RS_COMMAND_UTILS_H
#define SAFUZZ_RS_COMMAND_UTILS_H

#include "common/safuzz_log.h"
#include "random/random_data.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
#define DECLARE_ADD_RANDOM(commandCategory, alias) { #alias, commandCategory##Utils::AddRandom##alias }

#define ADD_RANDOM_COMMAND_WITH_PARAM_1(alias, type1)                                        \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }

#define ADD_RANDOM_COMMAND_WITH_PARAM_2(alias, type1, type2)                                 \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1(),                                                  \
            RandomData::GetRandom##type2());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }

#define ADD_RANDOM_COMMAND_WITH_PARAM_3(alias, type1, type2, type3)                          \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1(),                                                  \
            RandomData::GetRandom##type2(),                                                  \
            RandomData::GetRandom##type3());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }

#define ADD_RANDOM_COMMAND_WITH_PARAM_4(alias, type1, type2, type3, type4)                   \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1(),                                                  \
            RandomData::GetRandom##type2(),                                                  \
            RandomData::GetRandom##type3(),                                                  \
            RandomData::GetRandom##type4());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }

#define ADD_RANDOM_COMMAND_WITH_PARAM_5(alias, type1, type2, type3, type4, type5)            \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1(),                                                  \
            RandomData::GetRandom##type2(),                                                  \
            RandomData::GetRandom##type3(),                                                  \
            RandomData::GetRandom##type4(),                                                  \
            RandomData::GetRandom##type5());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }

#define ADD_RANDOM_COMMAND_WITH_PARAM_6(alias, type1, type2, type3, type4, type5, type6)     \
    static bool AddRandom##alias(std::unique_ptr<RSTransactionData>& rsTransactionData)      \
    {                                                                                        \
        if (rsTransactionData == nullptr) {                                                  \
            SAFUZZ_LOGE("RSCommandUtils::AddRandom" #alias " rsTransactionData is nullptr"); \
            return false;                                                                    \
        }                                                                                    \
        std::unique_ptr<RSCommand> command = std::make_unique<alias>(                        \
            RandomData::GetRandom##type1(),                                                  \
            RandomData::GetRandom##type2(),                                                  \
            RandomData::GetRandom##type3(),                                                  \
            RandomData::GetRandom##type4(),                                                  \
            RandomData::GetRandom##type5(),                                                  \
            RandomData::GetRandom##type6());                                                 \
        rsTransactionData->AddCommand(command, 0, FollowType::NONE);                         \
        return true;                                                                         \
    }
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_COMMAND_UTILS_H
