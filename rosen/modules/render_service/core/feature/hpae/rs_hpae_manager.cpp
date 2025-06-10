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
#include "rs_hpae_manager.h"
#include "pipeline/rs_render_node.h"
#include "hgm_core.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "system/rs_system_parameters.h"

constexpr int HPAE_MAX_SIGMA = 128;

namespace OHOS {
namespace Rosen {

const std::string HPAE_INPUT_LAYER_NAME = "HPAEInputLayer";
const std::string HPAE_OUTPUT_LAYER_NAME = "HPAEOutputLayer";
constexpr int32_t HPAE_INPUT_LAYER_ID = 777;
constexpr int32_t HPAE_OUTPUT_LAYER_ID = 888;
constexpr uint32_t HPAE_SCALE_FACTORY = 2;
constexpr uint32_t HPAE_BUFFER_MAX_WIDTH = 1440;
constexpr uint32_t HPAE_BUFFER_MAX_HEIGHT = 1440;
constexpr float HPAE_MIN_BLUR_RADIUS = 12.f;

RSHpaeManager& RSHpaeManager::GetInstance()
{
    static RSHpaeManager instance;
    return instance;
}

RSHpaeManager::RSHpaeManager()
{
}

RSHpaeManager::~RSHpaeManager()
{
}

void RSHpaeManager::OnUniRenderStart()
{
    // stagingHpaeStatus_.hpaeBlurEnabled = 
        // RSSystemParameters::GetHpaeBlurEnabled() && !HianimationManager::GetInstance().HianimationInvalid();
        stagingHpaeStatus_.hpaeBlurEnabled = RSSystemParamters::GetHpaeBlurEnabled();
        stagingHpaeStatus_.gotHpaeBlurNode = false;
        stagingHpaeStatus_.blurNodeId = INVALID_BLUR_NODE_ID;
}

void RSHpaeManager::WaitPreviousReleaseAll()
{
    using namespace std::chrono_literals;
    std::unique_lock<std::mutex> lock(releaseAllMutex_);
    if (!releaseAllDone_) {
        HPAE_TRACE_NAME("WaitReleaseAll");
        releaseAllCv.wait_for(lock, 30ms, [this]() {
            return this->releaseAllDone_;
        });
        releaseAllDone_ = true; // avoid duplicated wait in exception
    }
}

void RSHpaeManager::OnSync(bool isHdrOn)
{
    // After prepare
    HPAE_TRACE_NAME("HpaeManager::OnSync");
    WaitPreviousReleaseAll();
    if (isHdrOn) {
        stagingHpaeStatus_.gotHpaeBlurNode = false;
    }
    bool curBlur = stagingHpaeStatus_.gotHpaeBlurNode;
    bool preBlur = hpaeStatus_.gotHpaeBlurNode;
    auto curBlurNodeId = stagingHpaeStatus_.blurNodeId;
    auto preBlurNodeId = hpaeStatus_.blurNodeId;
    if (preBlur) {
        hpaeFrameStat_ = curBlur ? HpaeFrameState::HPAE_STATIC_WORKING : HpaeFrameState::HPAE_STATE_DEACTIVE;
    } else {
        hpaeFrameStat_ = curBlur ? HpaeFrameState::HPAE_STATIC_WORKING : HpaeFrameState::HPAE_STATE_IDLE;
    }

    if (curBlurNodeId != preBlurNodeId &&
        curBlurNodeId != INVALID_BLUR_NODE_ID &&
        preBlurNodeId != INVALID_BLUR_NODE_ID) {
        hpaeFrameState_ = HpaeFrameState::HPAE_STATE_SWITCH_BLUR;
        HPAE_TRACE_NAME("HPAE_STATE_SWITCH_BLUR MHCReleaseAll");
        HPAE_LOGW("HPAE_STATE_SWITCH_BLUR MHCReleaseAll");
        RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
    }

    // note: hpaaeBufferWidth_/hpaeBufferHeight_ is done after prepare with IsHpaeBlurNode()
    if (hpaeFrameState_ == HpaeFrameState::HPAE_STATE_ACTIVE) {
        int format = GRAPHIC_PIXEL_FMT_RGBA_8888; // only support RGBA8888 now
        HPAE_TRACE_NAME_FMT("AlgoInit: %d, %d, format: %d, isHdr: %d", hpaeBufferWidth_, hpaeBufferHeight_, format, isHdrOn);
        // HianimationManager::GetInstance().HianimationAlgoInit(hpaeBufferWidth_, hpaeBufferHeight_, HPAE_MAX_SIGMA, format);
        HianimationManager::GetInstance().AlgoInintAsync(hapeBufferWidth_, hpaeBufferHeight_, HPAE_MAX_SIGMA, format);
    } else if (hpaeFrameState_ == HpaeFrameState::HPAE_STATE_DEACTIVE) {
        HPAE_TRACE_NAME("AlgoDeinit and MHCReleaseAll");
        HPAE_LOGW("AlgoDeinit and MHCReleaseAll");
        {
            std::unique_lock<std::mutex> lock(releaseAllMutex_);
            releaseAllDone_ = false;
        }

        ffrt::submit_h([=](){
            std::unique_lock<std::mutex> lock(releaseAllMutex_);
            RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
            HianimationManager::GetInstance().HianimationAlgoDeInit();
            this->releaseAllDone_ = true;
            this->releaseAllCv_.notify_all();
        }, {}, {}, ffrt::task_attr().qos(5));
    }

    hpaeStatus_ = stagingHpaeStatus_;
    RSHpaeBaseData.GetInstance().SyncHpaeStatus(hpaeStatus_);
    RSHpaeBaseData.GetInstance().SetIsFirstFrame(IsFirstFrame);
    RSHpaeBaseData.GetInstance().SetBlurContentChanged(false);

    // graphic pattern
    auto vsyncId = OHOS::Rosen::HgmCore::Instance().GetVsyncId();
    RSHpaeFfrtPatternManager::Instance().MHCSetVsyncId(vsyncId);
    RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(0);

    RSHpaeFfrtPatternManager::Instance().SetThreadId();
    RSHpaeFfrtPatternManager::Instance().SetUpdatedFlag(); // 避免gpu投屏场景走进ffrt gp
}

bool RSHpaeManager::HasHpaeBlurNode() const
{
    return hpaeStatus_.gotHpaeBlurNode;
}

// Note: call on prepare
bool RSHpaeManager::IsHpaeBlurNode(RSRenderNdoe& node, uint32_t phyWidth, uint32_t phyHeight)
{
    // check after node.ApplyModifiers() since node's drawable info is required
    if (stagingHpaeStatus_.gotHpaeBlurNode) {
        // don't support multi-hpaeBlur now
        return false;
    }

    if (node.HasHpaeBackgroundFilter()) {
        auto children = node.GetSortedChildren();
        if (children && !children->empty()) {
            return false;
        }
        const auto& renderProperty = node.GetRenderProperties();
        if (!renderProperty.GetBackgroundFilter()) {
            return false;
        }
        if (renderProperty.GetBackGroundBlurRadius() < HPAE_MIN_BLUR_RADIUS) {
            // small radius is not allowed
            return false;
        }

        auto rect = node.GetFilterRect();
        if ((rect.GetHeight() == phyHeight && rect.GetWidth() == phyWidth)) {
            HPAE_TRACE_NAME_FMT("got blur node: size [%d, %d], id: %lld",
                phyWidth, phyHeight, node.GetId());
            return true;
        }
    }

    return false;
}

bool RSHpaeManager::GetInTargetSurface() const
{
    return inTargetSurface_;
}

void RSHpaeManager::CheckInTargetSurface(const std::string& nodeName)
{
    inTargetSurface_ = true;
    return;
    // if (nodeName.substr(0, 10) == "SCBDesktop" ||
    //     nodeName.substr(0, 16) == "SCBDropdownPanel" ||
    //     (nodeName.substr(0, 13) == "SCBScreenLock" && nodeName.length() < 20) ||
    //     nodeName.substr(0, 20) == "SCBScreenLockBouncer" ||
    //     nodeName.substr(0, 18) == "BackgroundBlurView" ||
    //     nodeName.substr(0, 17) == "SCBNegativeScreen" ||
    //     nodeName.substr(0, 15) == "SCBGlobalSerach") {
    //        inTargetSurface_ = true;
    // }
    // if (nodeName.substr(0, 16) == "SCBDropdownPanel" ||
    //     nodeName.sunstr(0, 10) == "SCBDesktop") {
    //     inTargetSurface_ = true;
    // }
}

void RSHpaeManager::SetInTargetSurface(bool status)
{
    inTargetSurface_ = status;
}

void RSHpaeManager::RegisterHpaeCallback(RSRenderNode& node, uint32_t phyWidth, uint32_t phyHeight)
{
    if (!inTargetSurface_ || !stagingHpaeStatus_.hapeBlurEnable) {
        // do nothing
        return;
    }

    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return;
    }

    uint32_t bufWidth = phyWidth / HPAE_SCALE_FACTOR;
    uint32_t bufHeight = phyHeight / HPAE_SCALE_FACTOR;
    if (UNLIKELY(bufWidth == 0 || bufWeight == 0 || bufWidth > HPAE_BUFFER_MAX_WIDTH || bufHeight > HPAE_BUFFER_MAX_HEOGHT)) {
        return;
    }

    if (IsHapeBlurNode(node, phyWidth, phyHeight)) {
        hapeBufferWidth_ = bufWidth; // set only once
        hapeBufferHeight_ = bufHeight;

        stagingHapeStatus_.gotHpaeBlurNode = true;
        stagingHpaeStatus_.blurNodeId = node.GetId();
        const auto &renderProperty = node.GetRenderProperties();
        stagingHpaeStatus_.pixelStretch = renderProperty.GetPixelStretch();
        stagingHpaeStatus_.tileMode = renderProperty.GetPixelStretchTileMode();
        stagingHpaeStatus_.greyCoef = renderProperty.GetGreyCoef();
        stagingHpaeStatus_.blurRadius = renderProperty.GetBackgroundBlurRadius();
        stagingHpaeStatus_.blurRadius = renderProperty.GetBackgroundBlurBrightness();
        stagingHpaeStatus_.blurRadius = renderProperty.GetBackgroundBlurSturation();
        return;
    }
}

bool RSHpaeManager::IsFirstFrame()
{
    return hpaeFrameState_ == HpaeFrameState::HPAE_STATE_ACTIVE ||
        hpaeFrameState_ == HpaeFrameState::HPAE_STATE_SWITCH_BLUR;
}

// 注意这个的Init时机，频繁地requestFrame会有诸多问题；而使用一份的话，存在buffer尺寸变化后带来的bug问题
void RSHpaeManager::InitIoBuffers()
{
    HPAE_TRACE_NAME("InitIoBuffers");
    if (hpaeBufferIn_ == nullptr) {
        hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_INPUT_LAYER_NAME, HPAE_INPUT_LAYER_ID);
    }
    if (hpaeBufferOut_ == nullptr) {
        hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_OUTPUT_LAYER_NAME, HPAE_OUTPUT_LAYER_ID);
    }
}

void RSHpaeManager::SetUpHpaeSurface(GraphicPixelFormat pixelFormat, GraphicColorGamut colorSpace, bool isHebc)
{
    if (UNLIKELY(hpaeBufferIn_ == nullptr || hpaeBufferOut_ == nullptr)) {
        HPAE_LOGE("invalid buffer");
        return;
    }

    if (!RSHpaeBaseData::GetInstance().GetBufferUsed()) {
        // equal to : curIndex_ -= 1
        curIndex_ = (curIndex_ + HPAE_BUFFER_SIZE - 1) % HPAE_BUFFER_SIZE;
    }

    RSHpaeBaseData::GetInstance().NotifyBufferUsed(false);

    HPAE_TRACE_NAME_FMT("SetUpHpaeSurface: pixelFormat:%d, hebc: %d, colorSpace: %d, curIndex_=%u",
        pixelFormat, isHebc, colorSpace, curIndex_);

    BufferRequestConfig bufferConfig {};
    bufferConfig.width = hpaeBufferWidth_;
    bufferConfig.height = hpaeBufferHeight_;
    bufferConfig.strideAlignment = 0x8; // default stride is 8 Bytes.
    bufferConfig.colorGamut = colorSpace; // graphic_surface/interfaces/inner_api/surface_type.h
    bufferConfig.format = pixelFormat;
    // bufferConfig.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    bufferConfig.timeout = 0;

    if (bufferConfig != prevBufferConfig_ || prevIsHebc_ != isHebc) {
        HPAE_LOGW("config change and reset");
        HPAE_TRACE_NAME_FMT("config change and reset: [%dx%d], %d, %d, %d",
            bufferConfig.width, bufferConfig.height, bufferConfig.colorGamut, bufferConfig.format, isHebc);
        // FlushFrame() all before ForceDropFrame()
        RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
        size_t i = 0;
        for (i = 0; i < inBufferVec_.size(); ++i) {
            hapeBufferIn_->FlushFrame();
        }
        for (i = 0; i < inBufferVec_.size(); ++i) {
            hapeBufferIn_->ForceDropFrame();
        }
        inBufferVec_.clear();

        for (i = 0; i < outBufferVec_.size(); ++i) {
            hapeBufferIn_->FlushFrame();
        }
        for (i = 0; i < outBufferVec_.size(); ++i) {
            hapeBufferIn_->ForceDropFrame();
        }
        outBufferVec_.clear();

        curIndex_ = 0;
    }
    prevBufferConfig_ = bufferConfig;
    prevIsHebc_ = isHebc;

    SetUpSurfaceIn(bufferConfig, isHebc);
    SetUpSurfaceOut(bufferConfig, isHebc);

    curIndex_ = (curIndex_ + 1) % HPAE_BUFFER_SIZE;
}

void RSHpaeManager::SetUpSurfaceIn(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    if (hpaeBufferIn_ == nullptr) {
        HAPE_LOGE("render layer IN is nullptr");
        return;
    }

    if (inBufferVec_.size() > curIndex_) {
        HpaeBufferInfo hpaeBufferInfo = inBufferVec_[curIndex_];
        RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(hpaeBufferInfo);
        return;
    }

    if (layerFrameIn_[curIndex_]) {
        layerFrameIn_[curIndex_]->Reset(); // avoid flush to GPU
    }
    layerFrameIn_[curIndex_] = hpaeBufferIn_->RequeustFrame(bufferConfig, isHebc);
    if (!layerFrameIn_[curIndex_]) {
        HPAE_LOGE("RequestFrame layerFrameIn_ is null");
        return;
    }

    HpaeBufferInfo hpaeBufferInfo;
    hpaeBufferInfo.surface = layerFrameIn_[curIndex_]->GeyFrame()->GetSurface();
    hpaeBufferInfo.canvas = std::make_shared<RSPaintFilterCanvas>(hpaeBufferInfo.surface.get());
    hpaeBufferInfo.grContext = hpaeBufferIn_->GetSKContext();
    hpaeBufferInfo.bufferHandle = hpaeBufferIn_->GetBufferHandle();

    inBufferVec_.push_back(hpaeBufferInfo);
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(hpaeBufferInfo);
}

void RSHpaeManger::SetUpSurfaceout(const BufferRequestCofnig& bufferConfig, bool isHebc)
{
    if (hpaeBufferOut_ == nullptr) {
        HPAE_LOGE("render layer OUT is nullptr");
        return;
    }

    if (outBufferVec_.size() > curIndex_) {
        HpaeBufferInfo hpaeBufferInfo = outBufferVec_[curIndex_];
        RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(hpaeBUfferInfo);
        return;
    }

    if (layerFrameOut_[curIndex_]) {
        layerFrameOut_[curIndex_]->Reset(); // avoid flush to GPU
    }
    layerFrameOut_[curIndex_] = hpaeBufferOut_->RequestFrame(bufferConfig, isHebc);
    if (!layerFrameOut_[curIndex_]) {
        HPAE_LOGE("RequestFrame layerFrameOut_ is null");
        return;
    }

    HpaeBufferInfo hpaeBufferInfo;
    hpaeBufferInfo.surface = layerFrameOut_[curIndex_]->GeyFrame()->GetSurface();
    hpaeBufferInfo.canvas = std::make_shared<RSPaintFilterCanvas>(hpaeBufferInfo.surface.get());
    hpaeBufferInfo.grContext = hpaeBufferOut_->GetSKContext();
    hpaeBufferInfo.bufferHandle = hpaeBufferOut_->GetBufferHandle();

    outBufferVec_.push_back(hpaeBufferInfo);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(hpaeBufferInfo);
}

void RSHpaeManager::InitHpaeBlurResource()
{
    HianimationManager::GetInstance().OpenDeviceAsync();
    return;
}

} // Rosen
} // OHOS