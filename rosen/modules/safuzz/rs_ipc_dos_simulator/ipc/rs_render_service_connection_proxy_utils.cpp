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

#include "ipc/rs_render_service_connection_proxy_utils.h"

#include "common/safuzz_log.h"
#include "configs/safuzz_config.h"
#include "random/random_engine.h"
#include "rs_trace.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_ashmem_helper_utils.h"

namespace OHOS {
namespace Rosen {
bool RSRenderServiceConnectionProxyVariant::FillParcelVectorWithTransactionData(
    std::unique_ptr<RSTransactionData>& data, std::vector<std::shared_ptr<MessageParcel>>& parcelVector)
{
    if (data == nullptr) {
        SAFUZZ_LOGE("RSRenderServiceConnectionProxyVariant::FillParcelVectorWithTransactionData "
            "transaction data is nullptr");
        return false;
    }

    auto func = [&parcelVector, &data]() -> bool {
        std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
        if (!FillParcelWithTransactionData(data, parcel)) {
            SAFUZZ_LOGE("RSRenderServiceConnectionProxyVariant::FillParcelVectorWithTransactionData "
                "FillParcelWithTransactionData failed");
            return false;
        }
        parcelVector.emplace_back(parcel);
        return true;
    };

    if (data->IsEmpty()) {
        SAFUZZ_LOGW("RSRenderServiceConnectionProxyVariant::FillParcelVectorWithTransactionData "
            "commit empty transaction data");
        if (!func()) {
            return false;
        }
    } else {
        while (data->GetMarshallingIndex() < data->GetCommandCount()) {
            if (!func()) {
                return false;
            }
        }
    }
    return true;
}

bool RSRenderServiceConnectionProxyVariant::FillParcelWithTransactionData(std::unique_ptr<RSTransactionData>& data,
    std::shared_ptr<MessageParcel>& parcel)
{
    static constexpr float CHOOSE_RANDOM_PROBABILITY = 0.5;

    if (data == nullptr) {
        SAFUZZ_LOGE("RSRenderServiceConnectionProxyVariant::FillParcelWithTransactionData transaction data is nullptr");
        return false;
    }

    // write a flag at the begin of parcel to identify parcel type
    // 0: indicate normal parcel
    // 1: indicate ashmem parcel
    if (!parcel->WriteInt32(0)) {
        SAFUZZ_LOGE("RSRenderServiceConnectionProxyVariant::FillParcelWithTransactionData WriteInt32 failed");
        return false;
    }

    if (!RSMarshallingHelper::MarshallingTransactionVer(*parcel)) {
        ROSEN_LOGE("FillParcelWithTransactionData WriteVersionHeader failed!");
        return false;
    }

    // 1. marshalling RSTransactionData
    RS_TRACE_BEGIN("[SAFUZZ] MarshRSTransactionData cmdCount:" + std::to_string(data->GetCommandCount()) +
        " index:" + std::to_string(data->GetIndex()));
    bool success = parcel->WriteParcelable(data.get());
    RS_TRACE_END();
    if (!success) {
        SAFUZZ_LOGE("RSRenderServiceConnectionProxyVariant::FillParcelWithTransactionData WriteParcelable failed");
        return false;
    }

    // 2. convert parcel to new ashmem parcel if size over threshold
    std::shared_ptr<MessageParcel> ashmemParcel = nullptr;
    if (parcel->GetDataSize() > ASHMEM_SIZE_THRESHOLD) {
        if (RandomEngine::ChooseByProbability(CHOOSE_RANDOM_PROBABILITY)) {
            ashmemParcel = RSAshmemHelperVariant::CreateAshmemParcel(parcel);
        } else {
            ashmemParcel = RSAshmemHelper::CreateAshmemParcel(parcel);
        }
    }
    if (ashmemParcel != nullptr) {
        parcel = ashmemParcel;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
