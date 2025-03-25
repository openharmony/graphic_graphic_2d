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

#ifndef SAFUZZ_THREAD_H
#define SAFUZZ_THREAD_H

#include <cstdint>
#include <unistd.h>

#include "configs/safuzz_config.h"
#include "tools/common_utils.h"
#include "tools/ipc_utils.h"

namespace OHOS {
namespace Rosen {
struct SaFuzzThreadParams {
    sptr<IRemoteObject> sa = nullptr;
    uint32_t code = UINT32_MAX;
    MessageParcel* data = nullptr;
    MessageParcel* reply = nullptr;
    MessageOption* option = nullptr;
    int loops = 1;
};

void* SaFuzzThread(void* threadParams)
{
    SaFuzzThreadParams* params = static_cast<SaFuzzThreadParams*>(threadParams);
    if (params == nullptr || params->sa == nullptr || params->data == nullptr || params->reply == nullptr ||
        params->option == nullptr) {
        SAFUZZ_LOGE("SaFuzzThread failed: invalid params");
        return nullptr;
    }

    int status = NO_ERROR;
    for (int i = 0; i < params->loops; ++i) {
        status = params->sa->SendRequest(params->code, *(params->data), *(params->reply), *(params->option));
    }
    SAFUZZ_LOGI("SaFuzzThread end: code %d, status %d", params->code, status);
    return nullptr;
}

struct MultiDataSaFuzzThreadParams {
    sptr<IRemoteObject> sa = nullptr;
    uint32_t code = UINT32_MAX;
    std::vector<std::shared_ptr<MessageParcel>>* data = nullptr;
    MessageParcel* reply = nullptr;
    MessageOption* option = nullptr;
    int loops = 1;
};

void* MultiDataSaFuzzThread(void* threadParams)
{
    MultiDataSaFuzzThreadParams* params = static_cast<MultiDataSaFuzzThreadParams*>(threadParams);
    if (params == nullptr || params->sa == nullptr || params->data == nullptr || params->reply == nullptr ||
        params->option == nullptr) {
        SAFUZZ_LOGE("MultiDataSaFuzzThread failed: invalid params");
        return nullptr;
    }

    std::vector<int> statusVector;
    for (int i = 0; i < params->loops; ++i) {
        statusVector.clear();
        for (const auto& parcel : *(params->data)) {
            if (parcel == nullptr) {
                continue;
            }
            int status = NO_ERROR;
            int retryCount = 0;
            do {
                status = params->sa->SendRequest(params->code, *parcel, *(params->reply), *(params->option));
                if (status != NO_ERROR && retryCount < MAX_RETRY_COUNT) {
                    retryCount++;
                    usleep(RETRY_WAIT_TIME_US);
                } else if (status != NO_ERROR) {
                    SAFUZZ_LOGE("MultiDataSaFuzzThread SendRequest failed: status %d, retryCount %d, data size %zu",
                        status, retryCount, parcel->GetDataSize());
                    return nullptr;
                }
            } while (status != NO_ERROR);
            statusVector.push_back(status);
        }
    }
    std::string statusVectorDesc = CommonUtils::PrintVector<int>(statusVector, CommonUtils::BASIC_PRINTER<int>);
    SAFUZZ_LOGI("MultiDataSaFuzzThread end: code %d, statusVector %s", params->code, statusVectorDesc.c_str());
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_THREAD_H
