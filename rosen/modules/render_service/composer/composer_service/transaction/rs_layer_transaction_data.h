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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H

#include <memory>
#include <mutex>
#include <vector>
#include "common/rs_macros.h"
#include "message_parcel.h"
#include "params/rs_render_params.h"
#include "rs_layer_parcel.h"

namespace OHOS::Rosen {
class RSComposerContext;
#define RSLayerId uint64_t

struct ComposerScreenInfo {
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t width = 0;  // render resolution
    uint32_t height = 0;
    uint32_t phyWidth = 0;  // physical screen resolution
    uint32_t phyHeight = 0;
    bool isSamplingOn = false;
    float samplingTranslateX = 0.f;
    float samplingTranslateY = 0.f;
    float samplingScale = 1.f;
    RectI activeRect;
    RectI maskRect;
    RectI reviseRect;

    float GetRogWidthRatio() const
    {
        return (width == 0) ? 1.f : static_cast<float>(phyWidth) / width;
    }

    float GetRogHeightRatio() const
    {
        return (height == 0) ? 1.f : static_cast<float>(phyHeight) / height;
    }
};
struct ComposerInfo {
    ComposerScreenInfo composerScreenInfo;
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
    ~RSLayerTransactionData();

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
    const ComposerScreenInfo& GetComposerScreenInfo() const
    {
        return composerInfo_.composerScreenInfo;
    }
    PipelineParam GetPipelineParam() const
    {
        return composerInfo_.pipelineParam;
    }
    void SetComposerInfo(ComposerInfo& composerInfo)
    {
        composerInfo_.composerScreenInfo = composerInfo.composerScreenInfo;
        composerInfo_.pipelineParam = composerInfo.pipelineParam;
    }

private:
    bool UnmarshallingRsLayerParcel(OHOS::MessageParcel& parcel);
    bool UnMarshallingRectI(OHOS::MessageParcel& parcel, RectI& rectI);
    bool MarshallingRectI(std::shared_ptr<OHOS::MessageParcel>& parcel, RectI& rectI);
    bool UnMarshallingComposerScreenInfo(OHOS::MessageParcel& parcel);
    bool MarshallingComposerScreenInfo(std::shared_ptr<OHOS::MessageParcel>& parcel);
    bool UnMarshallingPipelineParam(OHOS::MessageParcel& parcel);
    bool MarshallingPipelineParam(std::shared_ptr<OHOS::MessageParcel>& parcel);
    void AddRSLayerParcel(RSLayerId layerId, std::shared_ptr<RSLayerParcel>& layerParcel);

    std::vector<std::tuple<RSLayerId, std::shared_ptr<RSLayerParcel>>> payload_ = {};
    uint64_t timestamp_ = 0;
    pid_t pid_ = 0;
    uint64_t index_ = 0;
    ComposerInfo composerInfo_;
    mutable size_t marshallingIndex_ = 0;
    mutable std::mutex rsLayerParcelMutex_;
    friend class RSLayerTransactionHandler;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H