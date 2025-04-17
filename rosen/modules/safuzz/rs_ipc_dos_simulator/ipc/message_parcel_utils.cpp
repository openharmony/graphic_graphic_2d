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

#include "ipc/message_parcel_utils.h"

#include <vector>

#include "common/safuzz_log.h"
#include "ipc/message_parcel_basic_type_utils.h"
#include "ipc/message_parcel_customized_type_utils.h"
#include "tools/common_utils.h"

namespace OHOS {
namespace Rosen {
bool MessageParcelUtils::SetOption(MessageOption& messageOption, const std::string& option)
{
    if (option == "TF_SYNC") {
        messageOption.SetFlags(MessageOption::TF_SYNC);
        return true;
    }
    if (option == "TF_ASYNC") {
        messageOption.SetFlags(MessageOption::TF_ASYNC);
        return true;
    }
    SAFUZZ_LOGE("MessageParcelUtils::SetOption unrecognized option %s", option.c_str());
    return false;
}

bool MessageParcelUtils::WriteParams(MessageParcel& messageParcel, const TestCaseParams& testCaseParams)
{
    std::vector<std::string> typeDescVec = CommonUtils::SplitString(testCaseParams.inputParams, ';');
    for (const std::string& typeDesc : typeDescVec) {
        if (!WriteType(messageParcel, typeDesc, testCaseParams)) {
            SAFUZZ_LOGE("MessageParcelUtils::WriteParams %s failed", typeDesc.c_str());
            return false;
        }
    }
    return true;
}

bool MessageParcelUtils::WriteParamsToVector(std::vector<std::shared_ptr<MessageParcel>>& messageParcels,
    const TestCaseParams& testCaseParams)
{
    std::vector<std::string> typeDescVec = CommonUtils::SplitString(testCaseParams.inputParams, ';');
    for (const std::string& typeDesc : typeDescVec) {
        if (!WriteTypeToVector(messageParcels, typeDesc, testCaseParams)) {
            SAFUZZ_LOGE("MessageParcelUtils::WriteParamsToVector %s failed", typeDesc.c_str());
            return false;
        }
    }
    return true;
}

bool MessageParcelUtils::WriteType(MessageParcel& messageParcel, const std::string& typeDesc,
    const TestCaseParams& testCaseParams)
{
    auto writeRandomBasicTypeFunction = MessageParcelBasicTypeUtils::GetWriteFunctionByTypeName(typeDesc);
    if (writeRandomBasicTypeFunction != nullptr) {
        if (!writeRandomBasicTypeFunction(messageParcel)) {
            SAFUZZ_LOGE("MessageParcelUtils::WriteType %s failed", typeDesc.c_str());
            return false;
        }
        return true;
    }

    auto writeRandomCustomizedTypeFunction = MessageParcelCustomizedTypeUtils::GetWriteFunctionByTypeName(typeDesc);
    if (writeRandomCustomizedTypeFunction != nullptr) {
        if (!writeRandomCustomizedTypeFunction(messageParcel, testCaseParams)) {
            SAFUZZ_LOGE("MessageParcelUtils::WriteType %s failed", typeDesc.c_str());
            return false;
        }
        return true;
    }

    SAFUZZ_LOGE("MessageParcelUtils::WriteType %s can not be identified", typeDesc.c_str());
    return false;
}

bool MessageParcelUtils::WriteTypeToVector(std::vector<std::shared_ptr<MessageParcel>>& messageParcels,
    const std::string& typeDesc, const TestCaseParams& testCaseParams)
{
    auto writeRandomCustomizedTypeToVectorFunction =
        MessageParcelCustomizedTypeUtils::GetWriteToVectorFunctionByTypeName(typeDesc);
    if (writeRandomCustomizedTypeToVectorFunction != nullptr) {
        if (!writeRandomCustomizedTypeToVectorFunction(messageParcels, testCaseParams)) {
            SAFUZZ_LOGE("MessageParcelUtils::WriteTypeToVector %s failed", typeDesc.c_str());
            return false;
        }
        return true;
    }

    SAFUZZ_LOGE("MessageParcelUtils::WriteTypeToVector %s can not be identified", typeDesc.c_str());
    return false;
}
} // namespace Rosen
} // namespace OHOS
