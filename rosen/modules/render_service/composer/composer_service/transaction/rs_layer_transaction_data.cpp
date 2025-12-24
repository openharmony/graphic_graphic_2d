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

#include "rs_layer_transaction_data.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "rs_layer_parcel.h"
#include "rs_layer_parcel_factory.h"
#include "rs_surface_layer_parcel.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
static constexpr size_t PARCEL_MAX_CPACITY = 4000 * 1024; // upper bound of parcel capacity
static constexpr uint64_t MAX_ADVANCE_TIME = 1000000000;  // one second advance most

RSLayerTransactionData::~RSLayerTransactionData()
{
    Clear();
}

bool RSLayerTransactionData::UnMarshallingRectI(OHOS::MessageParcel& parcel, RectI& rectI)
{
    return parcel.ReadInt32(rectI.left_) && parcel.ReadInt32(rectI.top_) &&
        parcel.ReadInt32(rectI.width_) && parcel.ReadInt32(rectI.height_);
}

bool RSLayerTransactionData::MarshallingRectI(std::shared_ptr<OHOS::MessageParcel>& parcel, RectI& rectI)
{
    return parcel->WriteInt32(rectI.left_) && parcel->WriteInt32(rectI.top_) &&
        parcel->WriteInt32(rectI.width_) && parcel->WriteInt32(rectI.height_);
}

bool RSLayerTransactionData::UnMarshallingScreenInfo(OHOS::MessageParcel& parcel)
{
    bool result = parcel.ReadBool(isScreenInfoChanged_);
    if (isScreenInfoChanged_) {
        result = result && parcel.ReadUint64(screenInfo_.id);
        result = result && parcel.ReadUint32(screenInfo_.width);
        result = result && parcel.ReadUint32(screenInfo_.height);
        result = result && parcel.ReadUint32(screenInfo_.phyWidth);
        result = result && parcel.ReadUint32(screenInfo_.phyHeight);
        result = result && parcel.ReadInt32(screenInfo_.offsetX);
        result = result && parcel.ReadInt32(screenInfo_.offsetY);
        result = result && parcel.ReadBool(screenInfo_.isSamplingOn);
        result = result && parcel.ReadInt32(screenInfo_.samplingDistance);
        result = result && parcel.ReadFloat(screenInfo_.samplingTranslateX);
        result = result && parcel.ReadFloat(screenInfo_.samplingTranslateY);
        result = result && parcel.ReadFloat(screenInfo_.samplingScale);
        uint32_t value = 0;
        result = result && parcel.ReadUint32(value);
        screenInfo_.colorGamut = static_cast<ScreenColorGamut>(value);
        result = result && parcel.ReadUint32(value);
        screenInfo_.state = static_cast<ScreenState>(value);
        result = result && parcel.ReadUint32(value);
        screenInfo_.rotation = static_cast<ScreenRotation>(value);
        uint32_t whiteListSize = 0;
        uint64_t white = 0;
        result = result && parcel.ReadUint32(whiteListSize);
        for (uint32_t i = 0; i < whiteListSize; i++) {
            result = result && parcel.ReadUint64(white);
            screenInfo_.whiteList.insert(white);
        }
        result = result && UnMarshallingRectI(parcel, screenInfo_.activeRect);
        result = result && UnMarshallingRectI(parcel, screenInfo_.maskRect);
        result = result && UnMarshallingRectI(parcel, screenInfo_.reviseRect);
        result = result && parcel.ReadUint32(screenInfo_.skipFrameInterval);
        result = result && parcel.ReadUint32(screenInfo_.expectedRefreshRate);
        result = result && parcel.ReadUint32(value);
        screenInfo_.skipFrameStrategy = static_cast<SkipFrameStrategy>(value);
        result = result && parcel.ReadUint32(value);
        screenInfo_.pixelFormat = static_cast<GraphicPixelFormat>(value);
        result = result && parcel.ReadUint32(value);
        screenInfo_.hdrFormat = static_cast<ScreenHDRFormat>(value);
        result = result && parcel.ReadBool(screenInfo_.enableVisibleRect);
        result = result && parcel.ReadUint32(value);
        screenInfo_.powerStatus = static_cast<ScreenPowerStatus>(value);
    }
    return result;
}

bool RSLayerTransactionData::MarshallingScreenInfo(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    bool result = parcel->WriteBool(isScreenInfoChanged_);
    if (isScreenInfoChanged_) {
        result = result && parcel->WriteUint64(screenInfo_.id);
        result = result && parcel->WriteUint32(screenInfo_.width);
        result = result && parcel->WriteUint32(screenInfo_.height);
        result = result && parcel->WriteUint32(screenInfo_.phyWidth);
        result = result && parcel->WriteUint32(screenInfo_.phyHeight);
        result = result && parcel->WriteInt32(screenInfo_.offsetX);
        result = result && parcel->WriteInt32(screenInfo_.offsetY);
        result = result && parcel->WriteBool(screenInfo_.isSamplingOn);
        result = result && parcel->WriteInt32(screenInfo_.samplingDistance);
        result = result && parcel->WriteFloat(screenInfo_.samplingTranslateX);
        result = result && parcel->WriteFloat(screenInfo_.samplingTranslateY);
        result = result && parcel->WriteFloat(screenInfo_.samplingScale);
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.colorGamut));
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.state));
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.rotation));
        result = result && parcel->WriteUint32(screenInfo_.whiteList.size());
        for (auto it = screenInfo_.whiteList.begin(); it != screenInfo_.whiteList.end(); ++it) {
            result = result && parcel->WriteUint64(*it);
        }
        result = result && MarshallingRectI(parcel, screenInfo_.activeRect);
        result = result && MarshallingRectI(parcel, screenInfo_.maskRect);
        result = result && MarshallingRectI(parcel, screenInfo_.reviseRect);
        result = result && parcel->WriteUint32(screenInfo_.skipFrameInterval);
        result = result && parcel->WriteUint32(screenInfo_.expectedRefreshRate);
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.skipFrameStrategy));
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.pixelFormat));
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.hdrFormat));
        result = result && parcel->WriteBool(screenInfo_.enableVisibleRect);
        result = result && parcel->WriteUint32(static_cast<uint32_t>(screenInfo_.powerStatus));
    }
    return result;
}

bool RSLayerTransactionData::UnMarshallingPipelineParam(OHOS::MessageParcel& parcel)
{
    bool flag = parcel.ReadUint64(pipelineParam_.frameTimestamp) &&
        parcel.ReadInt64(pipelineParam_.actualTimestamp) &&
        parcel.ReadUint64(pipelineParam_.fastComposeTimeStampDiff) &&
        parcel.ReadUint64(pipelineParam_.vsyncId) &&
        parcel.ReadBool(pipelineParam_.isForceRefresh) &&
        parcel.ReadBool(pipelineParam_.hasGameScene) &&
        parcel.ReadUint32(pipelineParam_.pendingScreenRefreshRate) &&
        parcel.ReadUint64(pipelineParam_.pendingConstraintRelativeTime) &&
        parcel.ReadUint32(pipelineParam_.SurfaceFpsOpNum);

    pipelineParam_.SurfaceFpsOpList = std::vector<SurfaceFpsOp>(pipelineParam_.SurfaceFpsOpNum);
    for (int i = 0; i < pipelineParam_.SurfaceFpsOpNum; i++) {
        flag = flag && parcel.ReadUint32(pipelineParam_.SurfaceFpsOpList[i].surfaceFpsOpType) &&
        parcel.ReadUint64(pipelineParam_.SurfaceFpsOpList[i].surfaceNodeId) &&
        parcel.ReadString(pipelineParam_.SurfaceFpsOpList[i].surfaceName);
    }

    if (!flag) {
        RS_LOGE("RSLayerTransactionData::UnMarshallingPipelineParam failed");
    }
    return flag;
}

bool RSLayerTransactionData::MarshallingPipelineParam(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    bool flag = parcel->WriteUint64(pipelineParam_.frameTimestamp) &&
        parcel->WriteInt64(pipelineParam_.actualTimestamp) &&
        parcel->WriteUint64(pipelineParam_.fastComposeTimeStampDiff)&&
        parcel->WriteUint64(pipelineParam_.vsyncId) &&
        parcel->WriteBool(pipelineParam_.isForceRefresh) &&
        parcel->WriteBool(pipelineParam_.hasGameScene) &&
        parcel->WriteUint32(pipelineParam_.pendingScreenRefreshRate) &&
        parcel->WriteUint64(pipelineParam_.pendingConstraintRelativeTime) &&
        parcel->WriteUint32(pipelineParam_.SurfaceFpsOpNum);

    for (int i = 0; i < pipelineParam_.GetSurfaceFpsOpNum(); i++) {
        flag = flag && parcel->WriteUint32(pipelineParam_.SurfaceFpsOpList[i].surfaceFpsOpType) &&
        parcel->WriteUint64(pipelineParam_.SurfaceFpsOpList[i].surfaceNodeId) &&
        parcel->WriteString(pipelineParam_.SurfaceFpsOpList[i].surfaceName);
    }

    if (!flag) {
        RS_LOGE("RSLayerTransactionData::MarshallingPipelineParam failed");
    }
    return flag;
}

bool RSLayerTransactionData::Marshalling(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    if (parcel == nullptr) {
        RS_LOGE("RSLayerTransactionData::Marshalling parcel nullptr");
        return false;
    }
    parcel->SetMaxCapacity(PARCEL_MAX_CPACITY);
    std::unique_lock<std::mutex> lock(rsLayerParcelMutex_);
    bool success = parcel->WriteInt32(static_cast<int32_t>(payload_.size()));
    while (marshallingIndex_ < payload_.size()) {
        auto& [layerId, rsLayerParcel] = payload_[marshallingIndex_];
        success = success && parcel->WriteUint64(layerId);
        if (!rsLayerParcel) {
            parcel->WriteUint8(0);  // has not rsLayerParcel
            RS_LOGE("RSLayerTransactionData::Marshalling rsLayerParcel is nullptr");
        } else if (rsLayerParcel->indexVerifier_ != marshallingIndex_) {
            parcel->WriteUint8(0);  // has not rsLayerParcel
            RS_LOGE("RSLayerTransactionData::Marshalling indexVerifier is wrong");
        } else {
            parcel->WriteUint8(1);
            success = success && rsLayerParcel->Marshalling(*parcel);
        }
        if (!success) {
            RS_LOGE("RSLayerTransactionData::Marshalling parcel write error");
            return false;
        }
        ++marshallingIndex_;
    }
    success = success && parcel->WriteUint64(timestamp_);
    success = success && parcel->WriteInt32(pid_);
    success = success && parcel->WriteUint64(index_);
    success = success && MarshallingScreenInfo(parcel);
    success = success && MarshallingPipelineParam(parcel);
    if (!success) {
        RS_LOGE("RSLayerTransactionData::Marshalling failed");
    }
    return success;
}

RSLayerTransactionData* RSLayerTransactionData::Unmarshalling(OHOS::MessageParcel& parcel)
{
    auto transactionData = new RSLayerTransactionData();
    if (transactionData->UnmarshallingRsLayerParcel(parcel)) {
        uint64_t now = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
        if (transactionData->timestamp_ > now + MAX_ADVANCE_TIME) {
            RS_LOGE("RSLayerTransactionData::Unmarshalling limit timestamps from %{public}" PRIu64 " to "
                "%{public}" PRIu64 " ", transactionData->timestamp_, now + MAX_ADVANCE_TIME);
        }
        transactionData->timestamp_ = std::min(now + MAX_ADVANCE_TIME, transactionData->timestamp_);
        return transactionData;
    }
    RS_LOGE("RSLayerTransactionData::Unmarshalling failed.");
    delete transactionData;
    return nullptr;
}

bool RSLayerTransactionData::UnmarshallingRsLayerParcel(OHOS::MessageParcel& parcel)
{
    Clear();
    int32_t payloadSize = 0;
    if (!parcel.ReadInt32(payloadSize)) {
        RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel read payloadSize failed");
        return false;
    }
    RSLayerId layerId = 0;
    uint8_t hasRsLayerParcel = 0;
    uint16_t rsLayerParcelType = 0;

    size_t readableSize = parcel.GetReadableBytes();
    size_t len = static_cast<size_t>(payloadSize);
    if (len > readableSize || len > payload_.max_size()) {
        RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel fail read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return false;
    }
    std::unique_lock<std::mutex> payloadLock(rsLayerParcelMutex_, std::defer_lock);
    for (size_t i = 0; i < len; i++) {
        if (!parcel.ReadUint64(layerId) || !parcel.ReadUint8(hasRsLayerParcel)) {
            RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel cannot read layerId or hasRsLayerParcel");
            return false;
        }
        if (hasRsLayerParcel) {
            if (!parcel.ReadUint16(rsLayerParcelType)) {
                return false;
            }
            auto func = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(rsLayerParcelType);
            if (func == nullptr) {
                RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel func is nullptr, type:%{public}d", rsLayerParcelType);
                return false;
            }
            auto rsLayerParcel = (*func)(parcel);
            if (rsLayerParcel == nullptr) {
                RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel func failed, type:%{public}d", rsLayerParcelType);
                return false;
            }
            payloadLock.lock();
            payload_.emplace_back(layerId, std::move(rsLayerParcel));
            payloadLock.unlock();
        } else {
            continue;
        }
    }
    bool flag = parcel.ReadUint64(timestamp_) && parcel.ReadInt32(pid_) &&
        parcel.ReadUint64(index_) && UnMarshallingScreenInfo(parcel) && UnMarshallingPipelineParam(parcel);
    if (!flag) {
        RS_LOGE("RSLayerTransactionData::UnmarshallingRsLayerParcel failed");
    }
    return flag;
}

void RSLayerTransactionData::Clear()
{
    std::unique_lock<std::mutex> lock(rsLayerParcelMutex_);
    payload_.clear();
    payload_.shrink_to_fit();
    timestamp_ = 0;
}

void RSLayerTransactionData::AddRSLayerParcel(std::shared_ptr<RSLayerParcel>& layerParcel, RSLayerId layerId)
{
    std::unique_lock<std::mutex> lock(rsLayerParcelMutex_);
    layerParcel->indexVerifier_ = payload_.size();
    payload_.emplace_back(layerId, std::move(layerParcel));
}
} // namespace Rosen
} // namespace OHOS