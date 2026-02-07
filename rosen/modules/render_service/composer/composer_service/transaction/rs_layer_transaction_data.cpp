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

#undef LOG_TAG
#define LOG_TAG "RSLayerTransactionData"
namespace OHOS {
namespace Rosen {
static constexpr size_t PARCEL_MAX_CAPACITY = 4000 * 1024; // upper bound of parcel capacity
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

bool RSLayerTransactionData::UnMarshallingComposerScreenInfo(OHOS::MessageParcel& parcel)
{
    bool result = parcel.ReadUint64(composerInfo_.composerScreenInfo.id);
    result = result && parcel.ReadUint32(composerInfo_.composerScreenInfo.width);
    result = result && parcel.ReadUint32(composerInfo_.composerScreenInfo.height);
    result = result && parcel.ReadUint32(composerInfo_.composerScreenInfo.phyWidth);
    result = result && parcel.ReadUint32(composerInfo_.composerScreenInfo.phyHeight);
    result = result && parcel.ReadBool(composerInfo_.composerScreenInfo.isSamplingOn);
    result = result && parcel.ReadFloat(composerInfo_.composerScreenInfo.samplingTranslateX);
    result = result && parcel.ReadFloat(composerInfo_.composerScreenInfo.samplingTranslateY);
    result = result && parcel.ReadFloat(composerInfo_.composerScreenInfo.samplingScale);
    result = result && UnMarshallingRectI(parcel, composerInfo_.composerScreenInfo.activeRect);
    result = result && UnMarshallingRectI(parcel, composerInfo_.composerScreenInfo.maskRect);
    result = result && UnMarshallingRectI(parcel, composerInfo_.composerScreenInfo.reviseRect);

    return result;
}

bool RSLayerTransactionData::MarshallingComposerScreenInfo(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    bool result = parcel->WriteUint64(composerInfo_.composerScreenInfo.id);
    result = result && parcel->WriteUint32(composerInfo_.composerScreenInfo.width);
    result = result && parcel->WriteUint32(composerInfo_.composerScreenInfo.height);
    result = result && parcel->WriteUint32(composerInfo_.composerScreenInfo.phyWidth);
    result = result && parcel->WriteUint32(composerInfo_.composerScreenInfo.phyHeight);
    result = result && parcel->WriteBool(composerInfo_.composerScreenInfo.isSamplingOn);
    result = result && parcel->WriteFloat(composerInfo_.composerScreenInfo.samplingTranslateX);
    result = result && parcel->WriteFloat(composerInfo_.composerScreenInfo.samplingTranslateY);
    result = result && parcel->WriteFloat(composerInfo_.composerScreenInfo.samplingScale);
    result = result && MarshallingRectI(parcel, composerInfo_.composerScreenInfo.activeRect);
    result = result && MarshallingRectI(parcel, composerInfo_.composerScreenInfo.maskRect);
    result = result && MarshallingRectI(parcel, composerInfo_.composerScreenInfo.reviseRect);

    return result;
}

bool RSLayerTransactionData::UnMarshallingPipelineParam(OHOS::MessageParcel& parcel)
{
    bool flag = parcel.ReadUint64(composerInfo_.pipelineParam.frameTimestamp) &&
        parcel.ReadInt64(composerInfo_.pipelineParam.actualTimestamp) &&
        parcel.ReadUint64(composerInfo_.pipelineParam.fastComposeTimeStampDiff) &&
        parcel.ReadUint64(composerInfo_.pipelineParam.vsyncId) &&
        parcel.ReadBool(composerInfo_.pipelineParam.isForceRefresh) &&
        parcel.ReadBool(composerInfo_.pipelineParam.hasGameScene) &&
        parcel.ReadUint32(composerInfo_.pipelineParam.pendingScreenRefreshRate) &&
        parcel.ReadUint64(composerInfo_.pipelineParam.pendingConstraintRelativeTime) &&
        parcel.ReadBool(composerInfo_.pipelineParam.hasLppVideo) &&
        parcel.ReadUint32(composerInfo_.pipelineParam.SurfaceFpsOpNum);

    composerInfo_.pipelineParam.SurfaceFpsOpList = std::vector<SurfaceFpsOp>(
        composerInfo_.pipelineParam.SurfaceFpsOpNum);
    for (uint32_t i = 0; i < composerInfo_.pipelineParam.SurfaceFpsOpNum; i++) {
        flag = flag && parcel.ReadUint32(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceFpsOpType) &&
        parcel.ReadUint64(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceNodeId) &&
        parcel.ReadString(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceName);
    }

    if (!flag) {
        RS_LOGE("%{public}s failed", __func__);
    }
    return flag;
}

bool RSLayerTransactionData::MarshallingPipelineParam(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    bool flag = parcel->WriteUint64(composerInfo_.pipelineParam.frameTimestamp) &&
        parcel->WriteInt64(composerInfo_.pipelineParam.actualTimestamp) &&
        parcel->WriteUint64(composerInfo_.pipelineParam.fastComposeTimeStampDiff)&&
        parcel->WriteUint64(composerInfo_.pipelineParam.vsyncId) &&
        parcel->WriteBool(composerInfo_.pipelineParam.isForceRefresh) &&
        parcel->WriteBool(composerInfo_.pipelineParam.hasGameScene) &&
        parcel->WriteUint32(composerInfo_.pipelineParam.pendingScreenRefreshRate) &&
        parcel->WriteUint64(composerInfo_.pipelineParam.pendingConstraintRelativeTime) &&
        parcel->WriteBool(composerInfo_.pipelineParam.hasLppVideo) &&
        parcel->WriteUint32(composerInfo_.pipelineParam.SurfaceFpsOpNum);

    for (uint32_t i = 0; i < composerInfo_.pipelineParam.GetSurfaceFpsOpNum(); i++) {
        flag = flag && parcel->WriteUint32(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceFpsOpType) &&
        parcel->WriteUint64(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceNodeId) &&
        parcel->WriteString(composerInfo_.pipelineParam.SurfaceFpsOpList[i].surfaceName);
    }

    if (!flag) {
        RS_LOGE("%{public}s failed", __func__);
    }
    return flag;
}

bool RSLayerTransactionData::Marshalling(std::shared_ptr<OHOS::MessageParcel>& parcel)
{
    if (parcel == nullptr) {
        RS_LOGE("%{public}s parcel nullptr", __func__);
        return false;
    }
    parcel->SetMaxCapacity(PARCEL_MAX_CAPACITY);
    std::unique_lock<std::mutex> lock(rsLayerParcelMutex_);
    bool success = parcel->WriteUint64(static_cast<uint64_t>(payload_.size()));
    while (marshallingIndex_ < payload_.size()) {
        auto& [layerId, rsLayerParcel] = payload_[marshallingIndex_];
        success = success && parcel->WriteUint64(layerId);
        if (!rsLayerParcel) {
            parcel->WriteUint8(0);  // has not rsLayerParcel
            RS_LOGE("%{public}s rsLayerParcel is nullptr", __func__);
        } else if (rsLayerParcel->indexVerifier_ != marshallingIndex_) {
            parcel->WriteUint8(0);  // has not rsLayerParcel
            RS_LOGE("%{public}s indexVerifier is wrong",__func__);
        } else {
            parcel->WriteUint8(1);
            success = success && rsLayerParcel->Marshalling(*parcel);
        }
        if (!success) {
            RS_LOGE("%{public}s parcel write error", __func__);
            return false;
        }
        ++marshallingIndex_;
    }
    success = success && parcel->WriteUint64(timestamp_);
    success = success && parcel->WriteInt32(pid_);
    success = success && parcel->WriteUint64(index_);
    success = success && MarshallingComposerScreenInfo(parcel);
    success = success && MarshallingPipelineParam(parcel);
    if (!success) {
        RS_LOGE("%{public}s failed", __func__);
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
            RS_LOGE("%{public}s limit timestamps from %{public}" PRIu64 " to "
                "%{public}" PRIu64, __func__, transactionData->timestamp_, now + MAX_ADVANCE_TIME);
        }
        transactionData->timestamp_ = std::min(now + MAX_ADVANCE_TIME, transactionData->timestamp_);
        return transactionData;
    }
    RS_LOGE("%{public}s failed.",__func__);
    delete transactionData;
    return nullptr;
}

bool RSLayerTransactionData::UnmarshallingRsLayerParcel(OHOS::MessageParcel& parcel)
{
    Clear();
    uint64_t payloadSize = 0;
    if (!parcel.ReadUint64(payloadSize)) {
        RS_LOGE("%{public}s read payloadSize failed", __func__);
        return false;
    }
    RSLayerId layerId = 0;
    uint8_t hasRsLayerParcel = 0;
    uint16_t rsLayerParcelType = 0;

    size_t len = static_cast<size_t>(payloadSize);
    if (len > PARCEL_MAX_CAPACITY) {
        RS_LOGE("%{public}s fail read vector, size:%{public}zu", __func__, len);
        return false;
    }
    std::unique_lock<std::mutex> payloadLock(rsLayerParcelMutex_, std::defer_lock);
    for (size_t i = 0; i < len; i++) {
        if (!parcel.ReadUint64(layerId) || !parcel.ReadUint8(hasRsLayerParcel)) {
            RS_LOGE("%{public}s cannot read layerId or hasRsLayerParcel", __func__);
            return false;
        }
        if (hasRsLayerParcel) {
            if (!parcel.ReadUint16(rsLayerParcelType)) {
                return false;
            }
            auto func = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(rsLayerParcelType);
            if (func == nullptr) {
                RS_LOGE("%{public}s func is nullptr, type:%{public}d", __func__, rsLayerParcelType);
                return false;
            }
            auto rsLayerParcel = (*func)(parcel);
            if (rsLayerParcel == nullptr) {
                RS_LOGE("%{public}s func failed, type:%{public}d", __func__, rsLayerParcelType);
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
        parcel.ReadUint64(index_) && UnMarshallingComposerScreenInfo(parcel) && UnMarshallingPipelineParam(parcel);
    if (!flag) {
        RS_LOGE("%{public}s failed", __func__);
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

void RSLayerTransactionData::AddRSLayerParcel(RSLayerId layerId, std::shared_ptr<RSLayerParcel>& layerParcel)
{
    std::unique_lock<std::mutex> lock(rsLayerParcelMutex_);
    layerParcel->indexVerifier_ = payload_.size();
    payload_.emplace_back(layerId, std::move(layerParcel));
}
} // namespace Rosen
} // namespace OHOS