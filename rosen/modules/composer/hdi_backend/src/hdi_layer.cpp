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

RosenError HdiLayer::InitDevice()
{
    if (device_ != nullptr) {
        return ROSEN_ERROR_OK;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("device_ init failed.");
        return ROSEN_ERROR_NOT_INIT;
    }
    return ROSEN_ERROR_OK;
}

int32_t HdiLayer::CreateLayer(const LayerInfoPtr &layerInfo)
{
    LayerInfo hdiLayerInfo = {
        .width = layerInfo->GetLayerSize().w,
        .height = layerInfo->GetLayerSize().h,
        .type = LAYER_TYPE_GRAPHIC,
        .pixFormat = PIXEL_FMT_RGBA_8888,
    };

    RosenError retCode = InitDevice();
    if (retCode != ROSEN_ERROR_OK) {
        return DISPLAY_NULL_PTR;
    }

    uint32_t layerId = 0;
    int32_t ret = device_->CreateLayer(screenId_, hdiLayerInfo, layerId);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Create hwc layer failed, ret is %{public}d", ret);
        return ret;
    }

    layerId_ = layerId;

    HLOGD("Create hwc layer succeed, layerId is %{public}u", layerId_);

    CheckRet(device_->GetSupportedPresentTimestampType(screenId_, layerId_, supportedPresentTimestamptype_),
             "GetSupportedPresentTimestamp");
    return ret;
}

void HdiLayer::CloseLayer()
{
    if (layerId_ == INT_MAX) {
        HLOGI("this layer has not been created");
        return;
    }

    RosenError retCode = InitDevice();
    if (retCode != ROSEN_ERROR_OK) {
        return;
    }

    int32_t ret = device_->CloseLayer(screenId_, layerId_);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("Close hwc layer[%{public}u] failed, ret is %{public}d", layerId_, ret);
    }

    HLOGD("Close hwc layer succeed, layerId is %{public}u", layerId_);
}

void HdiLayer::SetLayerAlpha()
{
    if (prevLayerInfo_ != nullptr) {
        LayerAlpha& layerAlpha1 = layerInfo_->GetAlpha();
        LayerAlpha& layerAlpha2 = prevLayerInfo_->GetAlpha();
        bool isSame = layerAlpha1.enGlobalAlpha == layerAlpha2.enGlobalAlpha &&
                      layerAlpha1.enPixelAlpha == layerAlpha2.enPixelAlpha &&
                      layerAlpha1.alpha0 == layerAlpha2.alpha0 && layerAlpha1.alpha1 == layerAlpha2.alpha1 &&
                      layerAlpha1.gAlpha == layerAlpha2.gAlpha;
        if (isSame) {
            return;
        }
    }

    int32_t ret = device_->SetLayerAlpha(screenId_, layerId_, layerInfo_->GetAlpha());
    CheckRet(ret, "SetLayerAlpha");
}

bool HdiLayer::IsSameRect(const IRect& rect1, const IRect& rect2)
{
    return rect1.x == rect2.x && rect1.y == rect2.y && rect1.w == rect2.w && rect1.h == rect2.h;
}

void HdiLayer::SetLayerSize()
{
    if (prevLayerInfo_ != nullptr &&
        IsSameRect(layerInfo_->GetLayerSize(), prevLayerInfo_->GetLayerSize())) {
        return;
    }

    int32_t ret = device_->SetLayerSize(screenId_, layerId_, layerInfo_->GetLayerSize());
    CheckRet(ret, "SetLayerSize");
}

void HdiLayer::SetTransformMode()
{
    if (layerInfo_->GetTransformType() == TransformType::ROTATE_BUTT || (prevLayerInfo_ != nullptr &&
        layerInfo_->GetTransformType() == prevLayerInfo_->GetTransformType())) {
        return;
    }

    int32_t ret = device_->SetTransformMode(screenId_, layerId_, layerInfo_->GetTransformType());
    CheckRet(ret, "SetTransformMode");
}

void HdiLayer::SetLayerVisibleRegion()
{
    if (prevLayerInfo_ != nullptr && IsSameRect(layerInfo_->GetVisibleRegion(), prevLayerInfo_->GetVisibleRegion()) &&
        layerInfo_->GetVisibleNum() == prevLayerInfo_->GetVisibleNum()) {
        return;
    }

    int32_t ret = device_->SetLayerVisibleRegion(screenId_, layerId_, layerInfo_->GetVisibleNum(),
                                                 layerInfo_->GetVisibleRegion());
    CheckRet(ret, "SetLayerVisibleRegion");
}

void HdiLayer::SetLayerDirtyRegion()
{
    if (prevLayerInfo_ != nullptr && IsSameRect(layerInfo_->GetDirtyRegion(), prevLayerInfo_->GetDirtyRegion())) {
        return;
    }

    int32_t ret = device_->SetLayerDirtyRegion(screenId_, layerId_, layerInfo_->GetDirtyRegion());
    CheckRet(ret, "SetLayerDirtyRegion");
}

void HdiLayer::SetLayerBuffer()
{
    if (layerInfo_->GetBuffer() == nullptr || (prevLayerInfo_ != nullptr &&
        layerInfo_->GetBuffer() == prevLayerInfo_->GetBuffer() &&
        layerInfo_->GetAcquireFence() == prevLayerInfo_->GetAcquireFence())) {
        return;
    }

    int32_t ret = device_->SetLayerBuffer(screenId_, layerId_, layerInfo_->GetBuffer()->GetBufferHandle(),
                                          layerInfo_->GetAcquireFence());
    CheckRet(ret, "SetLayerBuffer");
}

void HdiLayer::SetLayerCompositionType()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->GetCompositionType() == prevLayerInfo_->GetCompositionType()) {
        return;
    }

    int32_t ret = device_->SetLayerCompositionType(screenId_, layerId_, layerInfo_->GetCompositionType());
    CheckRet(ret, "SetLayerCompositionType");
}

void HdiLayer::SetLayerBlendType()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->GetBlendType() == prevLayerInfo_->GetBlendType()) {
        return;
    }

    int32_t ret = device_->SetLayerBlendType(screenId_, layerId_, layerInfo_->GetBlendType());
    CheckRet(ret, "SetLayerBlendType");
}

void HdiLayer::SetLayerCrop()
{
    if (prevLayerInfo_ != nullptr && IsSameRect(layerInfo_->GetCropRect(), prevLayerInfo_->GetCropRect())) {
        return;
    }

    int32_t ret = device_->SetLayerCrop(screenId_, layerId_, layerInfo_->GetCropRect());
    CheckRet(ret, "SetLayerCrop");
}

void HdiLayer::SetLayerZorder()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->GetZorder() == prevLayerInfo_->GetZorder()) {
        return;
    }

    int32_t ret = device_->SetLayerZorder(screenId_, layerId_, layerInfo_->GetZorder());
    CheckRet(ret, "SetLayerZorder");
}

void HdiLayer::SetLayerPreMulti()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->IsPreMulti() == prevLayerInfo_->IsPreMulti()) {
        return;
    }

    int32_t ret = device_->SetLayerPreMulti(screenId_, layerId_, layerInfo_->IsPreMulti());
    CheckRet(ret, "SetLayerPreMulti");
}

void HdiLayer::SetLayerColorTransform()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->GetColorTransform() == prevLayerInfo_->GetColorTransform()) {
        return;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColorTransform(screenId_, layerId_, layerInfo_->GetColorTransform());
}

void HdiLayer::SetLayerColorDataSpace()
{
    if (prevLayerInfo_ != nullptr && layerInfo_->GetColorDataSpace() == prevLayerInfo_->GetColorDataSpace()) {
        return;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColorDataSpace(screenId_, layerId_, layerInfo_->GetColorDataSpace());
}

bool HdiLayer::IsSameLayerMetaData()
{
    bool isSame = false;
    std::vector<HDRMetaData>& metaData = layerInfo_->GetMetaData();
    std::vector<HDRMetaData>& prevMetaData = prevLayerInfo_->GetMetaData();
    if (metaData.size() == prevMetaData.size()) {
        isSame = true;
        size_t metaDeataSize = metaData.size();
        for (size_t i = 0; i < metaDeataSize; i++) {
            if (metaData[i].key != prevMetaData[i].key || metaData[i].value != prevMetaData[i].value) {
                isSame = false;
                break;
            }
        }
    }
    return isSame;
}

void HdiLayer::SetLayerMetaData()
{
    if (prevLayerInfo_ != nullptr) {
        bool isSame = IsSameLayerMetaData();
        if (isSame) {
            return;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaData(screenId_, layerId_, layerInfo_->GetMetaData());
}


bool HdiLayer::IsSameLayerMetaDataSet()
{
    bool isSame = false;
    HDRMetaDataSet &metaDataSet = layerInfo_->GetMetaDataSet();
    HDRMetaDataSet &prevMetaDataSet = prevLayerInfo_->GetMetaDataSet();
    if (metaDataSet.key == prevMetaDataSet.key &&
        metaDataSet.metaData.size() == prevMetaDataSet.metaData.size()) {
        bool isSame = true;
        size_t metaDeataSetSize = metaDataSet.metaData.size();
        for (size_t i = 0; i < metaDeataSetSize; i++) {
            if (metaDataSet.metaData[i] != prevMetaDataSet.metaData[i]) {
                isSame = false;
                break;
            }
        }
    }
    return isSame;
}

void HdiLayer::SetLayerMetaDataSet()
{
    if (prevLayerInfo_ != nullptr) {
        bool isSame = IsSameLayerMetaDataSet();
        if (isSame) {
            return;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaDataSet(screenId_, layerId_, layerInfo_->GetMetaDataSet().key,
                                 layerInfo_->GetMetaDataSet().metaData);
}

void HdiLayer::SetLayerTunnelHandle()
{
    if (!layerInfo_->GetTunnelHandleChange()) {
        return;
    }
    int32_t ret = DISPLAY_SUCCESS;
    if (layerInfo_->GetTunnelHandle() == nullptr) {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, nullptr);
    } else {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, layerInfo_->GetTunnelHandle()->GetHandle());
    }
    CheckRet(ret, "SetLayerTunnelHandle");
}

void HdiLayer::SetLayerPresentTimestamp()
{
    if (supportedPresentTimestamptype_ == PresentTimestampType::HARDWARE_DISPLAY_PTS_UNSUPPORTED) {
        return;
    }
    layerInfo_->SetIsSupportedPresentTimestamp(true);
    PresentTimestamp timestamp = {HARDWARE_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = device_->GetPresentTimestamp(screenId_, layerId_, timestamp);
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
    RosenError ret = InitDevice();
    if (ret != ROSEN_ERROR_OK || layerInfo_ == nullptr) {
        return;
    }

    SetLayerAlpha();
    SetLayerSize();
    SetTransformMode();
    SetLayerVisibleRegion();
    SetLayerDirtyRegion();
    SetLayerBuffer();
    SetLayerCompositionType();
    SetLayerBlendType();
    SetLayerCrop();
    SetLayerZorder();
    SetLayerPreMulti();
    SetLayerColorTransform();
    SetLayerColorDataSpace();
    SetLayerMetaData();
    SetLayerMetaDataSet();
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

void HdiLayer::SavePrevLayerInfo()
{
    if (prevLayerInfo_ == nullptr) {
        prevLayerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
    }
    prevLayerInfo_->CopyLayerInfo(layerInfo_);
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
