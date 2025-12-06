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
#include <algorithm>
#include <cstring>
#include <securec.h>
#include "common/rs_optional_trace.h"
#include "hdi_log.h"
#include "rs_surface_layer.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr float SIXTY_SIX_INTERVAL_IN_MS = 66.f;
constexpr float THIRTY_THREE_INTERVAL_IN_MS = 33.f;
constexpr float SIXTEEN_INTERVAL_IN_MS = 16.67f;
constexpr float FPS_TO_MS = 1000000.f;
constexpr size_t MATRIX_SIZE = 9;
const std::vector<float> DEFAULT_MATRIX = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const std::string GENERIC_METADATA_KEY_SDR_NIT = "SDRBrightnessNit";
const std::string GENERIC_METADATA_KEY_SDR_RATIO = "SDRBrightnessRatio";
const std::string GENERIC_METADATA_KEY_BRIGHTNESS_NIT = "BrightnessNit";
const std::string GENERIC_METADATA_KEY_LAYER_LINEAR_MATRIX = "LayerLinearMatrix";
const std::string GENERIC_METADATA_KEY_SOURCE_CROP_TUNING = "SourceCropTuning";
}

template<typename T>
bool Compare(const T& lhs, const T& rhs)
{
    return lhs == rhs;
}

template<>
bool Compare(const GraphicIRect& rect1, const GraphicIRect& rect2)
{
    return rect1.x == rect2.x && rect1.y == rect2.y && rect1.w == rect2.w && rect1.h == rect2.h;
}

template<typename T>
bool IsNeedSetInfoToDevice(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return true;
    }

    for (decltype(lhs.size()) i = 0; i < lhs.size(); i++) {
        if (!Compare(lhs[i], rhs[i])) {
            return true;
        }
    }

    return false;
}

/* rs create layer and set layer info begin */
std::shared_ptr<HdiLayer> HdiLayer::CreateHdiLayer(uint32_t screenId)
{
    return std::make_shared<HdiLayer>(screenId);
}

HdiLayer::HdiLayer(uint32_t screenId) : screenId_(screenId)
{
    currBufferInfo_ = new LayerBufferInfo();
}

HdiLayer::~HdiLayer()
{
    CloseLayer();
}

bool HdiLayer::Init(const std::shared_ptr<RSLayer>& rsLayer)
{
    if (rsLayer == nullptr) {
        return false;
    }

    if (CreateLayer(rsLayer) != GRAPHIC_DISPLAY_SUCCESS) {
        return false;
    }

    return true;
}

int32_t HdiLayer::InitDevice()
{
    if (device_ != nullptr) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("device_ init failed.");
        return GRAPHIC_DISPLAY_NULL_PTR;
    }
    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetHdiDeviceMock(HdiDevice* hdiDeviceMock)
{
    if (hdiDeviceMock == nullptr) {
        HLOGE("Input HdiDevice is nullptr");
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    if (device_ != nullptr) {
        HLOGD("device_ has been initialized");
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    device_ = hdiDeviceMock;
    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::CreateLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    int32_t retCode = InitDevice();
    if (retCode != GRAPHIC_DISPLAY_SUCCESS) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    sptr<IConsumerSurface> surface = rsLayer->GetSurface();
    if (surface == nullptr) {
        if (rsLayer->GetCompositionType() ==
            GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            bufferCacheCountMax_ = 0;
        } else {
            HLOGE("Create layer failed because the consumer surface is nullptr.");
            return GRAPHIC_DISPLAY_NULL_PTR;
        }
    } else {
        // The number of buffers cycle in the surface is larger than the queue size.
        surface->GetCycleBuffersNumber(bufferCacheCountMax_);
    }

    uint32_t layerId = INT_MAX;
    GraphicLayerInfo hdiLayerInfo = {
        .width = rsLayer->GetLayerSize().w,
        .height = rsLayer->GetLayerSize().h,
        .type = rsLayer->GetType(),
        .pixFormat = GRAPHIC_PIXEL_FMT_RGBA_8888,
    };
    int32_t ret = device_->CreateLayer(screenId_, hdiLayerInfo, bufferCacheCountMax_, layerId);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Create hwc layer failed, ret is %{public}d", ret);
        return ret;
    }
    ResetBufferCache();
    bufferCache_.reserve(bufferCacheCountMax_);
    layerId_ = layerId;

    HLOGI("Create hwc layer succeed, layerId is %{public}u", layerId_);

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
    if (retCode != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }

    retCode = device_->CloseLayer(screenId_, layerId_);
    if (retCode != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Close hwc layer[%{public}u] failed, ret is %{public}d", layerId_, retCode);
    }

    HLOGI("Close hwc layer succeed, layerId is %{public}u", layerId_);
}

int32_t HdiLayer::SetLayerAlpha()
{
    if (doLayerInfoCompare_) {
        const GraphicLayerAlpha& layerAlpha1 = rsLayer_->GetAlpha();
        const GraphicLayerAlpha& layerAlpha2 = prevRSLayer_->GetAlpha();
        bool isSame = layerAlpha1.enGlobalAlpha == layerAlpha2.enGlobalAlpha &&
                      layerAlpha1.enPixelAlpha == layerAlpha2.enPixelAlpha &&
                      layerAlpha1.alpha0 == layerAlpha2.alpha0 && layerAlpha1.alpha1 == layerAlpha2.alpha1 &&
                      layerAlpha1.gAlpha == layerAlpha2.gAlpha;
        if (isSame) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    int32_t ret = device_->SetLayerAlpha(screenId_, layerId_, rsLayer_->GetAlpha());
    return ret;
}

int32_t HdiLayer::SetLayerSize()
{
    if (doLayerInfoCompare_ && Compare(rsLayer_->GetLayerSize(), prevRSLayer_->GetLayerSize())) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerSize(screenId_, layerId_, rsLayer_->GetLayerSize());
    return ret;
}

int32_t HdiLayer::SetTransformMode()
{
    if (rsLayer_->GetTransformType() == GraphicTransformType::GRAPHIC_ROTATE_BUTT ||
        (doLayerInfoCompare_ && rsLayer_->GetTransformType() == prevRSLayer_->GetTransformType())) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    GraphicTransformType transFormType = rsLayer_->GetTransformType();
    int32_t ret = device_->SetTransformMode(screenId_, layerId_, transFormType);
    return ret;
}

int32_t HdiLayer::SetLayerVisibleRegion()
{
    bool isNeedSetInfoToDevice = true;
    const std::vector<GraphicIRect>& curVisibles = rsLayer_->GetVisibleRegions();
    if (doLayerInfoCompare_) {
        const std::vector<GraphicIRect>& prevVisibles = prevRSLayer_->GetVisibleRegions();
        if (!IsNeedSetInfoToDevice(curVisibles, prevVisibles)) {
            isNeedSetInfoToDevice = false;
        }
    }

    if (isNeedSetInfoToDevice) {
        return device_->SetLayerVisibleRegion(screenId_, layerId_, curVisibles);
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerDirtyRegion()
{
    bool isNeedSetInfoToDevice = true;
    const std::vector<GraphicIRect>& curDirtyRegions = rsLayer_->GetDirtyRegions();
    if (doLayerInfoCompare_) {
        const std::vector<GraphicIRect>& prevDirtyRegions = prevRSLayer_->GetDirtyRegions();
        if (!IsNeedSetInfoToDevice(curDirtyRegions, prevDirtyRegions)) {
            isNeedSetInfoToDevice = false;
        }
    }

    if (isNeedSetInfoToDevice) {
        return device_->SetLayerDirtyRegion(screenId_, layerId_, curDirtyRegions);
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

bool HdiLayer::CheckAndUpdateLayerBufferCahce(uint32_t sequence, uint32_t& index,
                                              std::vector<uint32_t>& deletingList)
{
    uint32_t bufferCacheSize = (uint32_t)bufferCache_.size();
    for (uint32_t i = 0; i < bufferCacheSize; i++) {
        if (bufferCache_[i] == sequence) {
            index = i;
            return true;
        }
    }

    if (bufferCacheSize >= bufferCacheCountMax_) {
        for (uint32_t i = 0; i < bufferCacheSize; i++) {
            deletingList.push_back(i);
        }
        ResetBufferCache();
    }
    index = (uint32_t)bufferCache_.size();
    bufferCache_.push_back(sequence);
    return false;
}

int32_t HdiLayer::SetLayerBuffer()
{
    RS_OPTIONAL_TRACE_NAME_FMT("SetLayerBuffer(layerid=%u)", layerId_);
    currBuffer_ = rsLayer_->GetBuffer();
    if (currBuffer_ == nullptr) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }
    sptr<SyncFence> currAcquireFence = rsLayer_->GetAcquireFence();
    if (doLayerInfoCompare_) {
        sptr<SurfaceBuffer> prevBuffer = prevRSLayer_->GetBuffer();
        sptr<SyncFence> prevAcquireFence = prevRSLayer_->GetAcquireFence();
        if (currBuffer_ == prevBuffer && currAcquireFence == prevAcquireFence) {
            if (!bufferCleared_) {
                return GRAPHIC_DISPLAY_SUCCESS;
            }
            HLOGW("layerid=%{public}u: force set same buffer(bufferId=%{public}u)", layerId_, currBuffer_->GetSeqNum());
            RS_TRACE_NAME_FMT("layerid=%u: force set same buffer(bufferId=%u)", layerId_, currBuffer_->GetSeqNum());
        }
    }

    uint32_t index = INVALID_BUFFER_CACHE_INDEX;
    std::vector<uint32_t> deletingList = {};
    bool bufferCached = false;
    if (bufferCacheCountMax_ == 0) {
        ClearBufferCache();
        HLOGE("The count of this layer buffer cache is 0.");
    } else {
        bufferCached = CheckAndUpdateLayerBufferCahce(currBuffer_->GetSeqNum(), index, deletingList);
    }

    GraphicLayerBuffer layerBuffer;
    layerBuffer.cacheIndex = index;
    layerBuffer.acquireFence = currAcquireFence;
    layerBuffer.deletingList = deletingList;
    if (bufferCached && index < bufferCacheCountMax_) {
        layerBuffer.handle = nullptr;
    } else {
        layerBuffer.handle = currBuffer_->GetBufferHandle();
        if (layerBuffer.handle == nullptr) {
            HLOGE("Get buffer handle failed.");
        }
    }

    bufferCleared_ = false;
    return device_->SetLayerBuffer(screenId_, layerId_, layerBuffer);
}

int32_t HdiLayer::SetLayerCompositionType()
{
    if (doLayerInfoCompare_ && rsLayer_->GetCompositionType() == prevRSLayer_->GetCompositionType()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerCompositionType(screenId_, layerId_, rsLayer_->GetCompositionType());
    return ret;
}

int32_t HdiLayer::SetLayerBlendType()
{
    if (doLayerInfoCompare_ && rsLayer_->GetBlendType() == prevRSLayer_->GetBlendType()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerBlendType(screenId_, layerId_, rsLayer_->GetBlendType());
    return ret;
}

int32_t HdiLayer::SetLayerCrop()
{
    if (doLayerInfoCompare_ && Compare(rsLayer_->GetCropRect(), prevRSLayer_->GetCropRect())) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerCrop(screenId_, layerId_, rsLayer_->GetCropRect());
    return ret;
}

int32_t HdiLayer::SetLayerZorder()
{
    if (doLayerInfoCompare_ && rsLayer_->GetZorder() == prevRSLayer_->GetZorder()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerZorder(screenId_, layerId_, rsLayer_->GetZorder());
    return ret;
}

int32_t HdiLayer::SetLayerPreMulti()
{
    if (doLayerInfoCompare_ && rsLayer_->IsPreMulti() == prevRSLayer_->IsPreMulti()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    int32_t ret = device_->SetLayerPreMulti(screenId_, layerId_, rsLayer_->IsPreMulti());
    return ret;
}

int32_t HdiLayer::SetLayerColor()
{
    if (doLayerInfoCompare_ && rsLayer_->GetLayerColor().r == prevRSLayer_->GetLayerColor().r &&
        rsLayer_->GetLayerColor().g == prevRSLayer_->GetLayerColor().g &&
        rsLayer_->GetLayerColor().b == prevRSLayer_->GetLayerColor().b &&
        rsLayer_->GetLayerColor().a == prevRSLayer_->GetLayerColor().a) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColor(screenId_, layerId_, rsLayer_->GetLayerColor());
    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerColorTransform()
{
    const std::vector<float>& curMatrix = rsLayer_->GetColorTransform();
    bool isNeedSetInfoToDevice = true;
    if (doLayerInfoCompare_) {
        const std::vector<float>& prevMatrix = prevRSLayer_->GetColorTransform();
        if (!IsNeedSetInfoToDevice(curMatrix, prevMatrix)) {
            isNeedSetInfoToDevice = false;
        }
    }
    if (isNeedSetInfoToDevice) {
        // This method may not be supported, the return value is not check here
        device_->SetLayerColorTransform(screenId_, layerId_, curMatrix);
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerColorDataSpace()
{
    if (doLayerInfoCompare_ && rsLayer_->GetColorDataSpace() == prevRSLayer_->GetColorDataSpace()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerColorDataSpace(screenId_, layerId_, rsLayer_->GetColorDataSpace());
    return GRAPHIC_DISPLAY_SUCCESS;
}

bool HdiLayer::IsSameLayerMetaData()
{
    bool isSame = false;
    const std::vector<GraphicHDRMetaData>& metaData = rsLayer_->GetMetaData();
    const std::vector<GraphicHDRMetaData>& prevMetaData = prevRSLayer_->GetMetaData();
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
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaData(screenId_, layerId_, rsLayer_->GetMetaData());
    return GRAPHIC_DISPLAY_SUCCESS;
}


bool HdiLayer::IsSameLayerMetaDataSet()
{
    bool isSame = false;
    const GraphicHDRMetaDataSet& metaDataSet = rsLayer_->GetMetaDataSet();
    const GraphicHDRMetaDataSet& prevMetaDataSet = prevRSLayer_->GetMetaDataSet();
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
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    // because hdi interface func is not implemented, delete CheckRet to avoid excessive print of log
    device_->SetLayerMetaDataSet(screenId_, layerId_, rsLayer_->GetMetaDataSet().key,
                                 rsLayer_->GetMetaDataSet().metaData);
    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerTunnelHandle()
{
    if (!rsLayer_->GetTunnelHandleChange()) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }
    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    if (rsLayer_->GetTunnelHandle() == nullptr) {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, nullptr);
    } else {
        ret = device_->SetLayerTunnelHandle(screenId_, layerId_, rsLayer_->GetTunnelHandle()->GetHandle());
    }
    return ret;
}

int32_t HdiLayer::SetTunnelLayerId()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetTunnelLayerId() == prevRSLayer_->GetTunnelLayerId()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }
 
    int32_t ret = device_->SetTunnelLayerId(screenId_, layerId_, rsLayer_->GetTunnelLayerId());
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetTunnelLayerProperty()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetTunnelLayerProperty() == prevRSLayer_->GetTunnelLayerProperty()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    int32_t ret = device_->SetTunnelLayerProperty(screenId_, layerId_, rsLayer_->GetTunnelLayerProperty());
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetLayerPresentTimestamp()
{
    if (supportedPresentTimestamptype_ == GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }
    rsLayer_->SetIsSupportedPresentTimestamp(true);
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = device_->GetPresentTimestamp(screenId_, layerId_, timestamp);
    GraphicPresentTimestamp graphicTimestamp = {
        .type = static_cast<GraphicPresentTimestampType>(timestamp.type),
        .time = timestamp.time,
    };

    CheckRet(ret, "GetPresentTimestamp");
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        rsLayer_->SetPresentTimestamp(graphicTimestamp);
    }
    return ret;
}

int32_t HdiLayer::SetLayerMaskInfo()
{
    return device_->SetLayerMaskInfo(screenId_, layerId_, static_cast<uint32_t>(rsLayer_->GetLayerMaskInfo()));
}

int32_t HdiLayer::SetHdiLayerInfo(bool isActiveRectSwitching)
{
    /*
        Some hardware platforms may not support all layer settings.
        If the current function is not supported, continue other layer settings.
     */
    int32_t ret = InitDevice();
    if (ret != GRAPHIC_DISPLAY_SUCCESS || rsLayer_ == nullptr) {
        return GRAPHIC_DISPLAY_FAILURE;
    }

    // All layer properities need to set to hwc when the layer is created firstly or the previous layer's composition
    // type is COMPOSITION_DEVICE for COMPOSITION_DEVICE can not reuse COMPOSITION_CLIENT layers info.
    doLayerInfoCompare_ = prevRSLayer_ != nullptr &&
                          prevRSLayer_->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE &&
                          !isActiveRectSwitching;

    ret = SetLayerAlpha();
    CheckRet(ret, "SetLayerAlpha");
    ret = SetLayerSize();
    CheckRet(ret, "SetLayerSize");
    ret = SetTransformMode();
    CheckRet(ret, "SetTransformMode");
    ret = SetLayerVisibleRegion();
    CheckRet(ret, "SetLayerVisibleRegion");
    // The crop needs to be set in the first order
    ret = SetLayerCrop();
    CheckRet(ret, "SetLayerCrop");
    // The data space contained in the layerbuffer needs to be set in the second order
    ret = SetLayerBuffer();
    CheckRet(ret, "SetLayerBuffer");
    // The dirty region needs to be set in the third order
    ret = SetLayerDirtyRegion();
    CheckRet(ret, "SetLayerDirtyRegion");
    ret = SetLayerCompositionType();
    CheckRet(ret, "SetLayerCompositionType");
    ret = SetLayerBlendType();
    CheckRet(ret, "SetLayerBlendType");
    ret = SetLayerZorder();
    CheckRet(ret, "SetLayerZorder");
    ret = SetLayerPreMulti();
    CheckRet(ret, "SetLayerPreMulti");
    ret = SetLayerColor();
    CheckRet(ret, "SetLayerColor");
    // This method may not be supported, the return value is not check here
    (void)SetLayerColorTransform();
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
    ret = SetLayerMaskInfo();
    CheckRet(ret, "SetLayerMask");
    ret = SetPerFrameParameters();
    CheckRet(ret, "SetPerFrameParameters");

    return GRAPHIC_DISPLAY_SUCCESS;
}

uint32_t HdiLayer::GetLayerId() const
{
    return layerId_;
}

std::shared_ptr<RSLayer> HdiLayer::GetRSLayer()
{
    return rsLayer_;
}

void HdiLayer::SetLayerStatus(bool inUsing)
{
    isInUsing_ = inUsing;
}

bool HdiLayer::GetLayerStatus() const
{
    return isInUsing_;
}

void HdiLayer::UpdateRSLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    if (rsLayer == nullptr) {
        return;
    }

    /* If the layer is updated, it indicates that the layer will be used
     * in the frame. Mark it.
     */

    isInUsing_ = true;
    rsLayer_ = rsLayer;

    prevSbuffer_ = currBufferInfo_->sbuffer_;
    currBufferInfo_->sbuffer_ = rsLayer_->GetBuffer();
}

void HdiLayer::SetReleaseFence(const sptr<SyncFence>& layerReleaseFence)
{
    if (currBufferInfo_ == nullptr || layerReleaseFence == nullptr) {
        return;
    }
    currBufferInfo_->releaseFence_ = layerReleaseFence;
}

sptr<SyncFence> HdiLayer::GetReleaseFence() const
{
    if (currBufferInfo_ == nullptr) {
        return SyncFence::InvalidFence();
    }
    return currBufferInfo_->releaseFence_;
}

bool HdiLayer::RecordPresentTime(int64_t timestamp)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (currBufferInfo_->sbuffer_ != prevSbuffer_) {
        presentTimeRecords_[count_].presentTime = timestamp;
        presentTimeRecords_[count_].windowsName = rsLayer_->GetWindowsName();
        count_ = (count_ + 1) % FRAME_RECORDS_NUM;
        return true;
    }
    return false;
}

void HdiLayer::SelectHitchsInfo(std::string windowName, std::string& result)
{
    int sixtySixTimes = 0;
    int thirtyThreeTimes = 0;
    int sixteenTimes = 0;
    int64_t lastFlushTimestamp = 0;
    int64_t nowFlushTimestamp = 0;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        const uint32_t offset = count_;
        for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
            // Reverse output timestamp array
            uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
            auto windowsName = presentTimeRecords_[order].windowsName;
            auto iter = std::find(windowsName.begin(), windowsName.end(), windowName);
            if (iter != windowsName.end()) {
                nowFlushTimestamp = presentTimeRecords_[order].presentTime;
                if (lastFlushTimestamp != 0) {
                    float time = (nowFlushTimestamp - lastFlushTimestamp) / FPS_TO_MS;
                    if (time > SIXTY_SIX_INTERVAL_IN_MS) {
                        sixtySixTimes++;
                    } else if (time > THIRTY_THREE_INTERVAL_IN_MS) {
                        thirtyThreeTimes++;
                    } else if (time > SIXTEEN_INTERVAL_IN_MS) {
                        sixteenTimes++;
                    }
                }
            }
            lastFlushTimestamp = nowFlushTimestamp;
        }
    }
    result += "more than 66 ms       " + std::to_string(sixtySixTimes) + "\n";
    result += "more than 33 ms       " + std::to_string(thirtyThreeTimes) + "\n";
    result += "more than 16.67 ms    " + std::to_string(sixteenTimes) + "\n";
}

void HdiLayer::RecordMergedPresentTime(int64_t timestamp)
{
    std::unique_lock<std::mutex> lock(mutex_);
    mergedPresentTimeRecords_[mergedCount_] = timestamp;
    mergedCount_ = (mergedCount_ + 1) % FRAME_RECORDS_NUM;
}

void HdiLayer::MergeWithFramebufferFence(const sptr<SyncFence>& fbAcquireFence)
{
    if (currBufferInfo_ == nullptr || fbAcquireFence == nullptr) {
        return;
    }
    currBufferInfo_->releaseFence_ = Merge(currBufferInfo_->releaseFence_, fbAcquireFence);
}

void HdiLayer::MergeWithLayerFence(const sptr<SyncFence>& layerReleaseFence)
{
    if (currBufferInfo_ == nullptr || layerReleaseFence == nullptr) {
        return;
    }
    currBufferInfo_->releaseFence_ = Merge(currBufferInfo_->releaseFence_, layerReleaseFence);
}

void HdiLayer::UpdateCompositionType(GraphicCompositionType type)
{
    if (rsLayer_ == nullptr) {
        return;
    }

    rsLayer_->SetCompositionType(type);
}
/* backend get layer info end */

sptr<SyncFence> HdiLayer::Merge(const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2)
{
    return SyncFence::MergeFence("ReleaseFence", fence1, fence2);
}

void HdiLayer::CheckRet(int32_t ret, const char* func)
{
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGD("call hdi %{public}s failed, ret is %{public}d", func, ret);
    }
}

void HdiLayer::SavePrevRSLayer()
{
    if (prevRSLayer_ == nullptr) {
        prevRSLayer_ = std::make_shared<RSSurfaceLayer>();
    }
    prevRSLayer_->CopyLayerInfo(rsLayer_);
}

void HdiLayer::Dump(std::string& result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    const uint32_t offset = count_;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
        result += std::to_string(presentTimeRecords_[order].presentTime) + "\n";
    }
}

void HdiLayer::DumpByName(std::string windowName, std::string& result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    const uint32_t offset = count_;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
        auto windowsName = presentTimeRecords_[order].windowsName;
        auto iter = std::find(windowsName.begin(), windowsName.end(), windowName);
        if (iter != windowsName.end()) {
            result += std::to_string(presentTimeRecords_[order].presentTime) + "\n";
        }
    }
}

void HdiLayer::DumpMergedResult(std::string& result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    const uint32_t offset = mergedCount_;
    for (uint32_t i = 0; i < FRAME_RECORDS_NUM; i++) {
        uint32_t order = (offset + FRAME_RECORDS_NUM - i - 1) % FRAME_RECORDS_NUM;
        result += std::to_string(mergedPresentTimeRecords_[order]) + "\n";
    }
}

void HdiLayer::ClearDump()
{
    std::vector<std::string> windowName = {};
    FPSInfo defaultFPSInfo = {0, windowName};

    std::unique_lock<std::mutex> lock(mutex_);
    presentTimeRecords_.fill(defaultFPSInfo);
    mergedPresentTimeRecords_.fill(0);
}

int32_t HdiLayer::SetPerFrameParameters()
{
    const auto& supportedKeys = device_->GetSupportedLayerPerFrameParameterKey();
    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    for (const auto& key : supportedKeys) {
        if (key == GENERIC_METADATA_KEY_SDR_NIT) {
            ret = SetPerFrameParameterSdrNit();
            CheckRet(ret, "SetPerFrameParameterSdrNit");
        } else if (key == GENERIC_METADATA_KEY_BRIGHTNESS_NIT) {
            ret = SetPerFrameParameterDisplayNit();
            CheckRet(ret, "SetPerFrameParameterDisplayNit");
        } else if (key == GENERIC_METADATA_KEY_SDR_RATIO) {
            ret = SetPerFrameParameterBrightnessRatio();
            CheckRet(ret, "SetPerFrameParameterBrightnessRatio");
        } else if (key == GENERIC_METADATA_KEY_LAYER_LINEAR_MATRIX) {
            ret = SetPerFrameLayerLinearMatrix();
            CheckRet(ret, "SetLayerLinearMatrix");
        } else if (key == GENERIC_METADATA_KEY_SOURCE_CROP_TUNING) {
            ret = SetPerFrameLayerSourceTuning();
            CheckRet(ret, "SetLayerSourceTuning");
        }
    }

    if (rsLayer_->GetTunnelLayerId() && rsLayer_->GetTunnelLayerProperty()) {
        ret = SetTunnelLayerId();
        CheckRet(ret, "SetTunnelLayerId");
        ret = SetTunnelLayerProperty();
        CheckRet(ret, "SetTunnelLayerProperty");
    }
    return ret;
}

int32_t HdiLayer::SetPerFrameParameterSdrNit()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetSdrNit() == prevRSLayer_->GetSdrNit()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    std::vector<int8_t> valueBlob(sizeof(float));
    *reinterpret_cast<float*>(valueBlob.data()) = rsLayer_->GetSdrNit();
    return device_->SetLayerPerFrameParameterSmq(
        screenId_, layerId_, GENERIC_METADATA_KEY_SDR_NIT, valueBlob);
}

int32_t HdiLayer::SetPerFrameParameterDisplayNit()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetDisplayNit() == prevRSLayer_->GetDisplayNit()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }
    std::vector<int8_t> valueBlob(sizeof(float));
    *reinterpret_cast<float*>(valueBlob.data()) = rsLayer_->GetDisplayNit();
    return device_->SetLayerPerFrameParameterSmq(
        screenId_, layerId_, GENERIC_METADATA_KEY_BRIGHTNESS_NIT, valueBlob);
}

int32_t HdiLayer::SetPerFrameParameterBrightnessRatio()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetBrightnessRatio() == prevRSLayer_->GetBrightnessRatio()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }
    std::vector<int8_t> valueBlob(sizeof(float));
    *reinterpret_cast<float*>(valueBlob.data()) = rsLayer_->GetBrightnessRatio();
    return device_->SetLayerPerFrameParameterSmq(
        screenId_, layerId_, GENERIC_METADATA_KEY_SDR_RATIO, valueBlob);
}

int32_t HdiLayer::SetPerFrameLayerLinearMatrix()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetLayerLinearMatrix() == prevRSLayer_->GetLayerLinearMatrix()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    } else {
        if (rsLayer_->GetLayerLinearMatrix().empty() || rsLayer_->GetLayerLinearMatrix() == DEFAULT_MATRIX) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }

    std::vector<int8_t> valueBlob(MATRIX_SIZE * sizeof(float));
    if (rsLayer_->GetLayerLinearMatrix().size() != MATRIX_SIZE ||
        memcpy_s(valueBlob.data(), valueBlob.size(), rsLayer_->GetLayerLinearMatrix().data(),
        MATRIX_SIZE * sizeof(float)) != EOK) {
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }
    return device_->SetLayerPerFrameParameterSmq(
        screenId_, layerId_, GENERIC_METADATA_KEY_LAYER_LINEAR_MATRIX, valueBlob);
}

int32_t HdiLayer::SetPerFrameLayerSourceTuning()
{
    if (prevRSLayer_ != nullptr) {
        if (rsLayer_->GetLayerSourceTuning() == prevRSLayer_->GetLayerSourceTuning()) {
            return GRAPHIC_DISPLAY_SUCCESS;
        }
    }
    std::vector<int8_t> valueBlob(sizeof(int32_t));
    *reinterpret_cast<int32_t*>(valueBlob.data()) = rsLayer_->GetLayerSourceTuning();
    return device_->SetLayerPerFrameParameterSmq(
        screenId_, layerId_, GENERIC_METADATA_KEY_SOURCE_CROP_TUNING, valueBlob);
}

void HdiLayer::ClearBufferCache()
{
    if (bufferCache_.empty()) {
        return;
    }
    RS_TRACE_NAME_FMT("HdiOutput::ClearBufferCache, screenId=%u, layerId=%u, bufferCacheSize=%zu", screenId_, layerId_,
        bufferCache_.size());
    ResetBufferCache();
    if (device_ == nullptr) {
        HLOGE("device_ is nullptr.");
        return;
    }
    int32_t ret = device_->ClearLayerBuffer(screenId_, layerId_);
    CheckRet(ret, "ClearLayerBuffer");
}

void HdiLayer::ResetBufferCache()
{
    bufferCache_.clear();
    bufferCleared_ = true;
    if (rsLayer_ != nullptr && rsLayer_->GetBuffer() != nullptr) {
        currBuffer_ = rsLayer_->GetBuffer();
    }
}
} // namespace Rosen
} // namespace OHOS
