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

int32_t HdiLayer::InitDevice()
{
    if (device_ != nullptr) {
        return DISPLAY_SUCCESS;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("device_ init failed.");
        return DISPLAY_NULL_PTR;
    }
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetHdiDeviceMock(Base::HdiDevice* hdiDeviceMock)
{
    if (hdiDeviceMock == nullptr) {
        HLOGE("Input HdiDevice is nullptr");
        return DISPLAY_NULL_PTR;
    }

    if (device_ != nullptr) {
        HLOGD("device_ has been initialized");
        return DISPLAY_SUCCESS;
    }

    device_ = hdiDeviceMock;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::CreateLayer(const LayerInfoPtr &layerInfo)
{
    GraphicLayerInfo hdiLayerInfo = {
        .width = layerInfo->GetLayerSize().w,
        .height = layerInfo->GetLayerSize().h,
        .type = GRAPHIC_LAYER_TYPE_GRAPHIC,
        .pixFormat = GRAPHIC_PIXEL_FMT_RGBA_8888,
    };

    int32_t retCode = InitDevice();
    if (retCode != DISPLAY_SUCCESS) {
        return DISPLAY_NULL_PTR;
    }

    uint32_t layerId = INT_MAX;
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

    int32_t retCode = InitDevice();
    if (retCode != DISPLAY_SUCCESS) {
        return;
    }

    retCode = device_->CloseLayer(screenId_, layerId_);
    if (retCode != DISPLAY_SUCCESS) {
        HLOGE("Close hwc layer[%{public}u] failed, ret is %{public}d", layerId_, retCode);
    }

    HLOGD("Close hwc layer succeed, layerId is %{public}u", layerId_);
}

int32_t HdiLayer::SetLayerAlpha()
{
    if (doLayerInfoCompare_) {
        const GraphicLayerAlpha& layerAlpha1 = layerInfo_->GetAlpha();
        const GraphicLayerAlpha& layerAlpha2 = prevLayerInfo_->GetAlpha();
        bool isSame = layerAlpha1.enGlobalAlpha == layerAlpha2.enGlobalAlpha &&
                      layerAlpha1.enPixelAlpha == layerAlpha2.enPixelAlpha &&
                      layerAlpha1.alpha0 == layerAlpha2.alpha0 && layerAlpha1.alpha1 == layerAlpha2.alpha1 &&
                      layerAlpha1.gAlpha == layerAlpha2.gAlpha;
        if (isSame) {
            return DISPLAY_SUCCESS;
        }
    }

    int32_t ret = device_->SetLayerAlpha(screenId_, layerId_, layerInfo_->GetAlpha());
    return ret;
}

bool HdiLayer::IsSameRect(const IRect& rect1, const IRect& rect2)
{
    return rect1.x == rect2.x && rect1.y == rect2.y && rect1.w == rect2.w && rect1.h == rect2.h;
}

int32_t HdiLayer::SetLayerSize()
{
    if (doLayerInfoCompare_ && IsSameRect(layerInfo_->GetLayerSize(), prevLayerInfo_->GetLayerSize())) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerSize(screenId_, layerId_, layerInfo_->GetLayerSize());
    return ret;
}

int32_t HdiLayer::SetTransformMode()
{
    if (layerInfo_->GetTransformType() == GraphicTransformType::GRAPHIC_ROTATE_BUTT || (doLayerInfoCompare_ &&
        layerInfo_->GetTransformType() == prevLayerInfo_->GetTransformType())) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetTransformMode(screenId_, layerId_, layerInfo_->GetTransformType());
    return ret;
}

int32_t HdiLayer::SetLayerVisibleRegion()
{
    if (doLayerInfoCompare_ && IsSameRect(layerInfo_->GetVisibleRegion(), prevLayerInfo_->GetVisibleRegion()) &&
        layerInfo_->GetVisibleNum() == prevLayerInfo_->GetVisibleNum()) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerVisibleRegion(screenId_, layerId_, layerInfo_->GetVisibleNum(),
                                                 layerInfo_->GetVisibleRegion());
    return ret;
}

int32_t HdiLayer::SetLayerDirtyRegion()
{
    if (doLayerInfoCompare_ && IsSameRect(layerInfo_->GetDirtyRegion(), prevLayerInfo_->GetDirtyRegion())) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerDirtyRegion(screenId_, layerId_, layerInfo_->GetDirtyRegion());
    return ret;
}

int32_t HdiLayer::SetLayerBuffer()
{
    if (layerInfo_->GetBuffer() == nullptr || (doLayerInfoCompare_ &&
        layerInfo_->GetBuffer() == prevLayerInfo_->GetBuffer() &&
        layerInfo_->GetAcquireFence() == prevLayerInfo_->GetAcquireFence())) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerBuffer(screenId_, layerId_, layerInfo_->GetBuffer()->GetBufferHandle(),
                                          layerInfo_->GetAcquireFence());
    return ret;
}

int32_t HdiLayer::SetLayerCompositionType()
{
    if (doLayerInfoCompare_ && layerInfo_->GetCompositionType() == prevLayerInfo_->GetCompositionType()) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerCompositionType(screenId_, layerId_, layerInfo_->GetCompositionType());
    return ret;
}

int32_t HdiLayer::SetLayerBlendType()
{
    if (doLayerInfoCompare_ && layerInfo_->GetBlendType() == prevLayerInfo_->GetBlendType()) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerBlendType(screenId_, layerId_, layerInfo_->GetBlendType());
    return ret;
}

int32_t HdiLayer::SetLayerCrop()
{
    if (doLayerInfoCompare_ && IsSameRect(layerInfo_->GetCropRect(), prevLayerInfo_->GetCropRect())) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerCrop(screenId_, layerId_, layerInfo_->GetCropRect());
    return ret;
}

int32_t HdiLayer::SetLayerZorder()
{
    if (doLayerInfoCompare_ && layerInfo_->GetZorder() == prevLayerInfo_->GetZorder()) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerZorder(screenId_, layerId_, layerInfo_->GetZorder());
    return ret;
}

int32_t HdiLayer::SetLayerPreMulti()
{
    if (doLayerInfoCompare_ && layerInfo_->IsPreMulti() == prevLayerInfo_->IsPreMulti()) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerPreMulti(screenId_, layerId_, layerInfo_->IsPreMulti());
    return ret;
}

int32_t HdiLayer::SetLayerColorTransform()
{
    if (doLayerInfoCompare_ && layerInfo_->GetColorTransform() == prevLayerInfo_->GetColorTransform()) {
        return DISPLAY_SUCCESS;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColorTransform(screenId_, layerId_, layerInfo_->GetColorTransform());
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerColorDataSpace()
{
    if (doLayerInfoCompare_ && layerInfo_->GetColorDataSpace() == prevLayerInfo_->GetColorDataSpace()) {
        return DISPLAY_SUCCESS;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColorDataSpace(screenId_, layerId_, layerInfo_->GetColorDataSpace());
    return DISPLAY_SUCCESS;
}

bool HdiLayer::IsSameLayerMetaData()
{
    bool isSame = false;
    std::vector<GraphicHDRMetaData>& metaData = layerInfo_->GetMetaData();
    std::vector<GraphicHDRMetaData>& prevMetaData = prevLayerInfo_->GetMetaData();
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

int32_t HdiLayer::SetLayerMetaData()
{
    if (doLayerInfoCompare_) {
        bool isSame = IsSameLayerMetaData();
        if (isSame) {
            return DISPLAY_SUCCESS;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaData(screenId_, layerId_, layerInfo_->GetMetaData());
    return DISPLAY_SUCCESS;
}


bool HdiLayer::IsSameLayerMetaDataSet()
{
    bool isSame = false;
    GraphicHDRMetaDataSet &metaDataSet = layerInfo_->GetMetaDataSet();
    GraphicHDRMetaDataSet &prevMetaDataSet = prevLayerInfo_->GetMetaDataSet();
    if (metaDataSet.key == prevMetaDataSet.key &&
        metaDataSet.metaData.size() == prevMetaDataSet.metaData.size()) {
        isSame = true;
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

int32_t HdiLayer::SetLayerMetaDataSet()
{
    if (doLayerInfoCompare_) {
        bool isSame = IsSameLayerMetaDataSet();
        if (isSame) {
            return DISPLAY_SUCCESS;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaDataSet(screenId_, layerId_, layerInfo_->GetMetaDataSet().key,
                                 layerInfo_->GetMetaDataSet().metaData);
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerTunnelHandle()
{
    if (!layerInfo_->GetTunnelHandleChange()) {
        return DISPLAY_SUCCESS;
    }
    int32_t ret = DISPLAY_SUCCESS;
    if (layerInfo_->GetTunnelHandle() == nullptr) {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, nullptr);
    } else {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, layerInfo_->GetTunnelHandle()->GetHandle());
    }
    return ret;
}

int32_t HdiLayer::SetLayerPresentTimestamp()
{
    if (supportedPresentTimestamptype_ == GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED) {
        return DISPLAY_SUCCESS;
    }
    layerInfo_->SetIsSupportedPresentTimestamp(true);
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = device_->GetPresentTimestamp(screenId_, layerId_, timestamp);
    CheckRet(ret, "GetPresentTimestamp");
    if (ret == DISPLAY_SUCCESS) {
        layerInfo_->SetPresentTimestamp(timestamp);
    }
    return ret;
}

int32_t HdiLayer::SetHdiLayerInfo()
{
    /*
        Some hardware platforms may not support all layer settings.
        If the current function is not supported, continue other layer settings.
     */
    int32_t ret = InitDevice();
    if (ret != DISPLAY_SUCCESS || layerInfo_ == nullptr) {
        return DISPLAY_FAILURE;
    }

    // All layer properities need to set to hwc when the layer is created firstly or the previous layer's composition
    // type is COMPOSITION_DEVICE for COMPOSITION_DEVICE can not reuse COMPOSITION_CLIENT layers info.
    doLayerInfoCompare_ = prevLayerInfo_ != nullptr &&
                          prevLayerInfo_->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE;

    ret = SetLayerAlpha();
    CheckRet(ret, "SetLayerAlpha");
    ret = SetLayerSize();
    CheckRet(ret, "SetLayerSize");
    ret = SetTransformMode();
    CheckRet(ret, "SetTransformMode");
    ret = SetLayerVisibleRegion();
    CheckRet(ret, "SetLayerVisibleRegion");
    ret = SetLayerDirtyRegion();
    CheckRet(ret, "SetLayerDirtyRegion");
    ret = SetLayerBuffer();
    CheckRet(ret, "SetLayerBuffer");
    ret = SetLayerCompositionType();
    CheckRet(ret, "SetLayerCompositionType");
    ret = SetLayerBlendType();
    CheckRet(ret, "SetLayerBlendType");
    ret = SetLayerCrop();
    CheckRet(ret, "SetLayerCrop");
    ret = SetLayerZorder();
    CheckRet(ret, "SetLayerZorder");
    ret = SetLayerPreMulti();
    CheckRet(ret, "SetLayerPreMulti");
    ret = SetLayerColorTransform();
    CheckRet(ret, "SetLayerColorTransform");
    ret = SetLayerColorDataSpace();
    CheckRet(ret, "SetLayerColorDataSpace");
    ret = SetLayerMetaData();
    CheckRet(ret, "SetLayerMetaData");
    ret = SetLayerMetaDataSet();
    CheckRet(ret, "SetLayerMetaDataSet");
    ret = SetLayerTunnelHandle();
    CheckRet(ret, "SetLayerTunnelHandle");
    ret = SetLayerPresentTimestamp();
    CheckRet(ret, "SetLayerPresentTimestamp");
    return DISPLAY_SUCCESS;
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

void HdiLayer::UpdateCompositionType(GraphicCompositionType type)
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

void HdiLayer::ClearDump()
{
    presentTimeRecords.fill(0);
}
} // namespace Rosen
} // namespace OHOS
