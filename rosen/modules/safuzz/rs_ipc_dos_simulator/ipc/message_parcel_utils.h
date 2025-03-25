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

#ifndef SAFUZZ_MESSAGE_PARCEL_UTILS_H
#define SAFUZZ_MESSAGE_PARCEL_UTILS_H

#include <string>

#include "configs/test_case_config.h"
#include "tools/ipc_utils.h"

namespace OHOS {
namespace Rosen {
class MessageParcelUtils {
public:
    static bool SetOption(MessageOption& messageOption, const std::string& option);
    static bool WriteParams(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteParamsToVector(std::vector<std::shared_ptr<MessageParcel>>& messageParcels,
        const TestCaseParams& testCaseParams);

private:
    static bool WriteType(MessageParcel& messageParcel, const std::string& typeDesc,
        const TestCaseParams& testCaseParams);
    static bool WriteTypeToVector(std::vector<std::shared_ptr<MessageParcel>>& messageParcels,
        const std::string& typeDesc, const TestCaseParams& testCaseParams);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_MESSAGE_PARCEL_UTILS_H
