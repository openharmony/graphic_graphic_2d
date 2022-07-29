/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_layer.h"

#include "hdi_log.h"
#include "hdi_device.h"

namespace OHOS {
namespace Rosen {

/* rs create layer and set layer info begin */
std::shared_ptr<HdiLayer> HdiLayer::CreateHdiLayer(uint32_t screenId)
{
    return std::make_shared<HdiLayer>(screenId);
}

HdiLayer::HdiLayer(uint32_t screenId) : screenId_(screenId)
{
    prevSbuffer_ = new LayerBufferInfo();
    currSbuffer_ = new LayerBufferInfo();
}

HdiLayer::~HdiLayer()
{
    CloseLayer();
}

bool HdiLayer::Init(const LayerInfoPtr &layerInfo)
{
    if (layerInfo == nullptr) {
        return false;
    }

    if (CreateLayer(layerInfo) != DISPLAY_SUCCESS) {
        return false;
    }

    return true;
}

int32_t HdiLayer::CreateLayer(const LayerInfoPtr &layerInfo)
{
    LayerInfo hdiLayerInfo = {
        .width = layerInfo->GetLayerSize().w,
        .height = layerInfo->GetLayerSize().h,
        .type = LAYER_TYPE_GRAPHIC,
        .pixFormat = PIXEL_FMT_RGBA_8888,
    };

    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        HLOGE("device is null");
        return DISPLAY_NULL_PTR;
    }

    uint32_t layerId = 0;
    int32_t ret = device->CreateLayer(screenId_, hdiLayerInfo, layerId);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Create hwc layer failed, ret is %{public}d", ret);
        return ret;
    }

    layerId_ = layerId;

    HLOGD("Create hwc layer succeed, layerId is %{public}u", layerId_);

    CheckRet(device->GetSupportedPresentTimestampType(screenId_, layerId_, supportedPresentTimestamptype_),
             "GetSupportedPresentTimestamp");
    return ret;
}

void HdiLayer::CloseLayer()
{
    if (layerId_ == INT_MAX) {
        HLOGI("this layer has not been created");
        return;
    }

    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        HLOGE("device is null");
        return;
    }

    int32_t ret = device->CloseLayer(screenId_, layerId_);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Close hwc layer[%{public}u] failed, ret is %{public}d", layerId_, ret);
    }

    HLOGD("Close hwc layer succeed, layerId is %{public}u", layerId_);
}

void HdiLayer::SetLayerTunnelHandle()
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr || layerInfo_ == nullptr) {
        return;
    }
    if (!layerInfo_->GetTunnelHandleChange()) {
        return;
    }
    int32_t ret = DISPLAY_SUCCESS;
    if (layerInfo_->GetTunnelHandle() == nullptr) {
        ret = device->SetLayerTunnelHandle(screenId_, layerId_, nullptr);
    } else {
        ret = device->SetLayerTunnelHandle(screenId_, layerId_,
                                           layerInfo_->GetTunnelHandle()->GetHandle());
    }
    CheckRet(ret, "SetLayerTunnelHandle");
}

void HdiLayer::SetLayerPresentTimestamp()
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr || layerInfo_ == nullptr) {
        return;
    }
    if (supportedPresentTimestamptype_ == PresentTimestampType::HARDWARE_DISPLAY_PTS_UNSUPPORTED) {
        return;
    }
    layerInfo_->SetIsSupportedPresentTimestamp(true);
    PresentTimestamp timestamp = {HARDWARE_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = device->GetPresentTimestamp(screenId_, layerId_, timestamp);
    CheckRet(ret, "GetPresentTimestamp");
    if (ret == DISPLAY_SUCCESS) {
        layerInfo_->SetPresentTimestamp(timestamp);
    }
}

void HdiLayer::SetHdiLayerInfo()
{
    /*
        Some hardware platforms may not support all layer settings.
        If the current function is not supported, continue other layer settings.
     */

    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr || layerInfo_ == nullptr) {
        return;
    }

    int32_t ret = device->SetLayerAlpha(screenId_, layerId_, layerInfo_->GetAlpha());
    CheckRet(ret, "SetLayerAlpha");

    ret = device->SetLayerSize(screenId_, layerId_, layerInfo_->GetLayerSize());
    CheckRet(ret, "SetLayerSize");

    if (layerInfo_->GetTransformType() != TransformType::ROTATE_BUTT) {
        ret = device->SetTransformMode(screenId_, layerId_, layerInfo_->GetTransformType());
        CheckRet(ret, "SetTransformMode");
    }

    ret = device->SetLayerVisibleRegion(screenId_, layerId_, layerInfo_->GetVisibleNum(),
                                         layerInfo_->GetVisibleRegion());
    CheckRet(ret, "SetLayerVisibleRegion");

    ret = device->SetLayerDirtyRegion(screenId_, layerId_, layerInfo_->GetDirtyRegion());
    CheckRet(ret, "SetLayerDirtyRegion");

    if (layerInfo_->GetBuffer() != nullptr) {
        ret = device->SetLayerBuffer(screenId_, layerId_, layerInfo_->GetBuffer()->GetBufferHandle(),
                                     layerInfo_->GetAcquireFence());
        CheckRet(ret, "SetLayerBuffer");
    }

    ret = device->SetLayerCompositionType(screenId_, layerId_, layerInfo_->GetCompositionType());
    CheckRet(ret, "SetLayerCompositionType");

    ret = device->SetLayerBlendType(screenId_, layerId_, layerInfo_->GetBlendType());
    CheckRet(ret, "SetLayerBlendType");

    ret = device->SetLayerCrop(screenId_, layerId_, layerInfo_->GetCropRect());
    CheckRet(ret, "SetLayerCrop");

    ret = device->SetLayerZorder(screenId_, layerId_, layerInfo_->GetZorder());
    CheckRet(ret, "SetLayerZorder");

    ret = device->SetLayerPreMulti(screenId_, layerId_, layerInfo_->IsPreMulti());
    CheckRet(ret, "SetLayerPreMulti");

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    ret = device->SetLayerColorTransform(screenId_, layerId_, layerInfo_->GetColorTransform());
    ret = device->SetLayerColorDataSpace(screenId_, layerId_, layerInfo_->GetColorDataSpace());
    ret = device->SetLayerMetaData(screenId_, layerId_, layerInfo_->GetMetaData());
    ret = device->SetLayerMetaDataSet(screenId_, layerId_, layerInfo_->GetMetaDataSet().key,
                                      layerInfo_->GetMetaDataSet().metaData);

    SetLayerTunnelHandle();

    SetLayerPresentTimestamp();
}

uint32_t HdiLayer::GetLayerId() const
{
    return layerId_;
}

const LayerInfoPtr& HdiLayer::GetLayerInfo()
{
    return layerInfo_;
}

void HdiLayer::SetLayerStatus(bool inUsing)
{
    isInUsing_ = inUsing;
}

bool HdiLayer::GetLayerStatus() const
{
    return isInUsing_;
}

void HdiLayer::UpdateLayerInfo(const LayerInfoPtr &layerInfo)
{
    if (layerInfo == nullptr) {
        return;
    }

    /* If the layer is updated, it indicates that the layer will be used
     * in the frame. Mark it.
     */

    isInUsing_ = true;
    layerInfo_ = layerInfo;

    prevSbuffer_->sbuffer_ = currSbuffer_->sbuffer_;
    prevSbuffer_->acquireFence_ = currSbuffer_->acquireFence_;

    currSbuffer_->sbuffer_ = layerInfo_->GetBuffer();
    currSbuffer_->acquireFence_ = layerInfo_->GetAcquireFence();
}

sptr<SyncFence> HdiLayer::GetReleaseFence() const
{
    if (currSbuffer_ == nullptr) {
        return SyncFence::INVALID_FENCE;
    }
    return currSbuffer_->releaseFence_;
}

void HdiLayer::RecordPresentTime(int64_t timestamp)
{
    if (currSbuffer_->sbuffer_ != prevSbuffer_->sbuffer_) {
        presentTimeRecords[count] = timestamp;
        count = (count + 1) % FRAME_RECORDS_NUM;
    }
}

void HdiLayer::MergeWithFramebufferFence(const sptr<SyncFence> &fbAcquireFence)
{
    if (fbAcquireFence != nullptr) {
        currSbuffer_->releaseFence_ = Merge(currSbuffer_->releaseFence_, fbAcquireFence);
    }
}

void HdiLayer::MergeWithLayerFence(const sptr<SyncFence> &layerReleaseFence)
{
    if (layerReleaseFence != nullptr) {
        currSbuffer_->releaseFence_ = layerReleaseFence;
    }
}

void HdiLayer::UpdateCompositionType(CompositionType type)
{
    if (layerInfo_ == nullptr) {
        return;
    }

    layerInfo_->SetCompositionType(type);
}
/* backend get layer info end */

sptr<SyncFence> HdiLayer::Merge(const sptr<SyncFence> &fence1, const sptr<SyncFence> &fence2)
{
    return SyncFence::MergeFence("ReleaseFence", fence1, fence2);
}

void HdiLayer::CheckRet(int32_t ret, const char* func)
{
    if (ret != DISPLAY_SUCCESS) {
        HLOGD("call hdi %{public}s failed, ret is %{public}d", func, ret);
    }
}

void HdiLayer::Dump(std::string &result)
{
    const uint32_t offset = count;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + i) % FRAME_RECORDS_NUM;
        result += std::to_string(presentTimeRecords[order]) + "\n";
    }
}

} // namespace Rosen
} // namespace OHOS
