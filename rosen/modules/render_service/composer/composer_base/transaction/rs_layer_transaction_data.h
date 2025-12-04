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

#ifndef RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H
#define RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H

#include <memory>
#include <mutex>
#include <vector>
#include "common/rs_macros.h"
#include "message_parcel.h"
#include "params/rs_render_params.h"
#include "rs_layer_parcel.h"
#include "screen_manager/rs_screen_info.h"

namespace OHOS::Rosen {
class RSLayerContext;
#define RSLayerId uint64_t

struct CommitLayerInfo {
    ScreenInfo screenInfo;
    PipelineParam pipelineParam;
};

class RSB_EXPORT RSLayerTransactionData {
public:
    RSLayerTransactionData() = default;
    RSLayerTransactionData(const RSLayerTransactionData&) = delete;
    RSLayerTransactionData& operator=(const RSLayerTransactionData&) = delete;
    RSLayerTransactionData(RSLayerTransactionData&& other) :
        payload_(std::move(other.payload_)), timestamp_(std::move(other.timestamp_)),
        pid_(other.pid_), index_(other.index_) {}
    ~RSLayerTransactionData() = default;

    [[nodiscard]]static RSLayerTransactionData* Unmarshalling(OHOS::MessageParcel& parcel);
    bool Marshalling(std::shared_ptr<OHOS::MessageParcel>& parcel);

    uint64_t GetCommandCount() const
    {
        return payload_.size();
    }
    bool IsEmpty() const
    {
        return payload_.empty();
    }
    void Clear();
    uint64_t GetTimestamp() const
    {
        return timestamp_;
    }
    void SetTimestamp(const uint64_t timestamp)
    {
        timestamp_ = timestamp;
    }
    void SetSendingPid(pid_t pid)
    {
        pid_ = pid;
    }
    pid_t GetSendingPid() const
    {
        return pid_;
    }
    void SetIndex(uint64_t index)
    {
        index_ = index;
    }
    uint64_t GetIndex() const
    {
        return index_;
    }
    size_t GetMarshallingIndex() const
    {
        return marshallingIndex_;
    }
    std::vector<std::tuple<RSLayerId, std::shared_ptr<RSLayerParcel>>>& GetPayload()
    {
        return payload_;
    }
    ScreenInfo GetScreenInfo()
    {
        return screenInfo_;
    }
    void SetScreenInfo(ScreenInfo& screenInfo)
    {
        screenInfo_ = screenInfo;
    }
    void SetIsScreenInfoChanged(bool isChanged)
    {
        isScreenInfoChanged_ = isChanged;
    }
    bool GetIsScreenInfoChanged()
    {
        return isScreenInfoChanged_;
    }
    PipelineParam GetPipelineParam() const
    {
        return pipelineParam_;
    }
    void SetPipelineParam(PipelineParam& pipelineParam)
    {
        pipelineParam_ = pipelineParam;
    }
private:
    bool UnmarshallingRsLayerParcel(OHOS::MessageParcel& parcel);
    bool UnMarshallingRectI(OHOS::MessageParcel& parcel, RectI& rectI);
    bool MarshallingRectI(std::shared_ptr<OHOS::MessageParcel>& parcel, RectI& rectI);
    bool UnMarshallingScreenInfo(OHOS::MessageParcel& parcel);
    bool MarshallingScreenInfo(std::shared_ptr<OHOS::MessageParcel>& parcel);
    bool UnMarshallingPipelineParam(OHOS::MessageParcel& parcel);
    bool MarshallingPipelineParam(std::shared_ptr<OHOS::MessageParcel>& parcel);
    void AddRsLayerParcel(std::shared_ptr<RSLayerParcel>& layerParcel, RSLayerId layerId);

    std::vector<std::tuple<RSLayerId, std::shared_ptr<RSLayerParcel>>> payload_ = {};
    uint64_t timestamp_ = 0;
    pid_t pid_ = 0;
    uint64_t index_ = 0;
    bool isScreenInfoChanged_ = false;
    ScreenInfo screenInfo_;
    PipelineParam pipelineParam_;
    mutable size_t marshallingIndex_ = 0;
    int32_t parentPid_ = -1;
    uint32_t parcelNumber_ = 0;
    mutable std::mutex rsLayerParcelMutex_;
    friend class RSLayerTransactionHandler;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H