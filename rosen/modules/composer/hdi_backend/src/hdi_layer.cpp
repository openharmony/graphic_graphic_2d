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

    if (prevSbuffer_ == nullptr) {
        prevSbuffer_ = new LayerBufferInfo();
    }

    if (currSbuffer_ == nullptr) {
        currSbuffer_ = new LayerBufferInfo();
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

    uint32_t layerId = 0;
    int32_t ret = HdiDevice::GetInstance()->CreateLayer(screenId_, hdiLayerInfo, layerId);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Create hwc layer failed, ret is %{public}d", ret);
        return ret;
    }

    layerId_ = layerId;

    HLOGD("Create hwc layer succeed, layerId is %{public}u", layerId_);

    return ret;
}

void HdiLayer::CloseLayer()
{
    if (layerId_ == INT_MAX) {
        HLOGI("this layer has not been created");
        return;
    }

    int32_t ret = HdiDevice::GetInstance()->CloseLayer(screenId_, layerId_);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Close hwc layer[%{public}u] failed, ret is %{public}d", layerId_, ret);
    }

    HLOGD("Close hwc layer succeed, layerId is %{public}u", layerId_);
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

    ret = device->SetLayerBuffer(screenId_, layerId_, layerInfo_->GetBuffer()->GetBufferHandle(),
                                  layerInfo_->GetAcquireFence());
    CheckRet(ret, "SetLayerBuffer");

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
}

int32_t HdiLayer::SetLayerColorTransform(const float *matrix) const
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr || matrix == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device->SetLayerColorTransform(screenId_, layerId_, matrix);
}

int32_t HdiLayer::SetLayerColorDataSpace(ColorDataSpace colorSpace) const
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device->SetLayerColorDataSpace(screenId_, layerId_, colorSpace);
}

int32_t HdiLayer::GetLayerColorDataSpace(ColorDataSpace &colorSpace) const
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device->GetLayerColorDataSpace(screenId_, layerId_, colorSpace);
}

int32_t HdiLayer::SetLayerMetaData(const std::vector<HDRMetaData> &metaData) const
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device->SetLayerMetaData(screenId_, layerId_, metaData);
}

int32_t HdiLayer::SetLayerMetaDataSet(HDRMetadataKey key, const std::vector<uint8_t> &metaData) const
{
    HdiDevice *device = HdiDevice::GetInstance();
    if (device == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device->SetLayerMetaDataSet(screenId_, layerId_, key, metaData);
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

    currSbuffer_->sbuffer_ = layerInfo_->GetBuffer();
    currSbuffer_->acquireFence_ = layerInfo_->GetAcquireFence();
    prevSbuffer_->sbuffer_ = layerInfo_->GetPreBuffer();
    prevSbuffer_->acquireFence_ = layerInfo_->GetPreAcquireFence();
}

void HdiLayer::ReleaseBuffer()
{
    // check gpu buffer release
    if (currSbuffer_->sbuffer_ != prevSbuffer_->sbuffer_) {
        SurfaceError ret = ReleasePrevBuffer();
        if (ret != SURFACE_ERROR_OK) {
            HLOGW("ReleaseBuffer failed, ret is %{public}d", ret);
        }

        /* copy currSbuffer to prevSbuffer */
        prevSbuffer_->releaseFence_ = currSbuffer_->releaseFence_;
    } else {
        prevSbuffer_->acquireFence_ = Merge(currSbuffer_->acquireFence_, prevSbuffer_->acquireFence_);
        prevSbuffer_->releaseFence_ = Merge(currSbuffer_->releaseFence_, prevSbuffer_->releaseFence_);
    }
}

SurfaceError HdiLayer::ReleasePrevBuffer()
{
    /*
     * The first time, prevSbuffer is empty and the releaseFence is invalid.
     * Therefore, we do not release the buffer.
     */

    if (prevSbuffer_->sbuffer_ == nullptr) {
        return SURFACE_ERROR_OK;
    }

    if (layerInfo_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    int32_t fenceFd = prevSbuffer_->releaseFence_->Dup();

    SurfaceError ret = layerInfo_->GetSurface()->ReleaseBuffer(prevSbuffer_->sbuffer_, fenceFd);
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }

    prevSbuffer_->sbuffer_ = nullptr;
    prevSbuffer_->releaseFence_ = nullptr;

    return ret;
}

void HdiLayer::RecordPresentTime(const sptr<SyncFence> &fbFence)
{
    if (currSbuffer_->sbuffer_ != prevSbuffer_->sbuffer_) {
        presentTimeRecords[count].presentFence = fbFence;
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
        prevSbuffer_->releaseFence_ = Merge(prevSbuffer_->releaseFence_, layerReleaseFence);
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
        if (presentTimeRecords[order].presentFence != SyncFence::INVALID_FENCE) {
            presentTimeRecords[order].presentTime = presentTimeRecords[order].presentFence->SyncFileReadTimestamp();
            presentTimeRecords[order].presentFence = SyncFence::INVALID_FENCE;
        }
        result += std::to_string(presentTimeRecords[order].presentTime) + "\n";
    }
}

} // namespace Rosen
} // namespace OHOS
