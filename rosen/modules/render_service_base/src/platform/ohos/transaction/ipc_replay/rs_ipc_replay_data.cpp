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

#include "platform/ohos/transaction/ipc_replay/rs_ipc_replay_data.h"

#include "transaction/rs_marshalling_helper.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcReplayData"

namespace OHOS {
namespace Rosen {
bool SetWatermarkReplayData::Marshalling(Parcel& parcel) const
{
    return RSMarshallingHelper::Marshalling(parcel, pid_) &&
           RSMarshallingHelper::Marshalling(parcel, name_) &&
           RSMarshallingHelper::Marshalling(parcel, watermark_) &&
           RSMarshallingHelper::Marshalling(parcel, success_);
}

SetWatermarkReplayData* SetWatermarkReplayData::Unmarshalling(Parcel& parcel)
{
    auto result = std::make_unique<SetWatermarkReplayData>();
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->pid_)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->name_)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->watermark_)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, result->success_)) {
        return nullptr;
    }
    return result.release();
}
} // namespace Rosen
} // namespace OHOS
