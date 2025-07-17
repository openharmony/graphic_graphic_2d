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

#if defined(ROSEN_OHOS)
#include "ffrt_inner.h"
#endif
#include "hgm_core.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_log.h"
#include "system/rs_system_parameters.h"

#include "pipeline/rs_render_node.h"

constexpr int HPAE_MAX_SIGMA = 128;

namespace OHOS {
namespace Rosen {

const std::string HPAE_INPUT_LAYER_NAME = "HPAEInputLayer";
const std::string HPAE_OUTPUT_LAYER_NAME = "HPAEOutputLayer";
constexpr int32_t HPAE_INPUT_LAYER_ID = 777;
constexpr int32_t HPAE_OUTPUT_LAYER_ID = 888;
constexpr uint32_t HPAE_SCALE_FACTOR = 2;
constexpr uint32_t HPAE_BUFFER_MAX_WIDTH = 1440;
constexpr uint32_t HPAE_BUFFER_MAX_HEIGHT = 4096;
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
    // check Hianimation dependency and system parameters
#if defined(ROSEN_OHOS)
    stagingHpaeStatus_.hpaeBlurEnabled =
        (!HianimationManager::GetInstance().HianimationInvalid()) && RSSystemParameters::GetHpaeBlurEnabled();
#endif
    stagingHpaeStatus_.gotHpaeBlurNode = false;
    stagingHpaeStatus_.blurNodeId = INVALID_BLUR_NODE_ID;
}

void RSHpaeManager::WaitPreviousReleaseAll()
{
    using namespace std::chrono_literals;
    std::unique_lock<std::mutex> lock(releaseAllMutex_);
    if (!releaseAllDone_) {
        HPAE_TRACE_NAME("WaitReleaseAll");
        releaseAllCv_.wait_for(lock, 30ms, [this]() {
            return this->releaseAllDone_;
        });
        releaseAllDone_ = true; // avoid duplicated wait in exception
    }
}

void RSHpaeManager::UpdateHpaeState()
{
    bool curBlur = stagingHpaeStatus_.gotHpaeBlurNode;
    bool prevBlur = hpaeStatus_.gotHpaeBlurNode;
    auto curBlurNodeId = stagingHpaeStatus_.blurNodeId;
    auto preBlurNodeId = hpaeStatus_.blurNodeId;
    if (prevBlur) {
        hpaeFrameState_ = curBlur ? HpaeFrameState::WORKING : HpaeFrameState::DEACTIVE;
    } else {
        hpaeFrameState_ = curBlur ? HpaeFrameState::ACTIVE : HpaeFrameState::IDLE;
    }

    if (hpaeFrameState_ == HpaeFrameState::WORKING) {
        if (curBlurNodeId != preBlurNodeId &&
            curBlurNodeId != INVALID_BLUR_NODE_ID &&
            preBlurNodeId != INVALID_BLUR_NODE_ID) {
            hpaeFrameState_ = HpaeFrameState::SWITCH_BLUR;
        } else if (prevBufferConfig_.width != (int32_t)hpaeBufferWidth_ ||
            prevBufferConfig_.height != hpaeBufferHeight_) {
            hpaeFrameState_ = HpaeFrameState::CHANGE_CONFIG;
        }
    }
}
#if defined(ROSEN_OHOS)
constexpr int HPAE_USE_HPAE_QOS_LEVEL = 5;
#endif
void RSHpaeManager::HandleHpaeStateChange()
{
    UpdateHpaeState();
    int format = GRAPHIC_PIXEL_FMT_RGBA_8888; // only support RGBA888 now
    // note: hpaeBufferWidth_/hpaeBufferHeight_ is done after prepare with IsHpaeBlurNode()
    switch (hpaeFrameState_) {
        case HpaeFrameState::ACTIVE: {
            HPAE_TRACE_NAME_FMT("AlgoInit: %d, %d, format: %d", hpaeBufferWidth_, hpaeBufferHeight_, format);
            // can also use: HianimationAlgoInit
            HianimationManager::GetInstance().AlgoInitAsync(
                hpaeBufferWidth_, hpaeBufferHeight_, HPAE_MAX_SIGMA, format);
            break;
        }
        case HpaeFrameState::DEACTIVE: {
            HPAE_TRACE_NAME("AlgoDeinit and MHCReleaseAll");
            HPAE_LOGW("AlgoDeinit and MHCReleaseAll");
#if defined(ROSEN_OHOS)
            {
                std::unique_lock<std::mutex> lock(releaseAllMutex_);
                releaseAllDone_ = false;
            }

            ffrt::submit_h([=]() {
                std::unique_lock<std::mutex> lock(releaseAllMutex_);
                RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
                HianimationManager::GetInstance().HianimationAlgoDeInit();
                this->releaseAllDone_ = true;
                this->releaseAllCv_.notify_all();
                }, {}, {}, ffrt::task_attr().qos(HPAE_USE_HPAE_QOS_LEVEL));
#else
            RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
            HianimationManager::GetInstance().HianimationAlgoDeInit();

#endif
            break;
        }
        case HpaeFrameState::CHANGE_CONFIG: {
            HPAE_TRACE_NAME_FMT("CHANGE_CONFIG: %d, %d", hpaeBufferWidth_, hpaeBufferHeight_);
            HPAE_LOGW("CHANGE_CONFIG: %d, %d", hpaeBufferWidth_, hpaeBufferHeight_);
            break;
        }
        case HpaeFrameState::SWITCH_BLUR: {
            HPAE_TRACE_NAME("SWITCH_BLUR");
            HPAE_LOGW("SWITCH_BLUR");
            RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
            break;
        }
        default:
            break;
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

    HandleHpaeStateChange();

    hpaeStatus_ = stagingHpaeStatus_;
    RSHpaeBaseData::GetInstance().SyncHpaeStatus(hpaeStatus_);
    RSHpaeBaseData::GetInstance().SetIsFirstFrame(IsFirstFrame());
    RSHpaeBaseData::GetInstance().SetBlurContentChanged(false);

    // graphic pattern
    auto vsyncId = OHOS::Rosen::HgmCore::Instance().GetVsyncId();
    RSHpaeFfrtPatternManager::Instance().MHCSetVsyncId(vsyncId);
    RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(0);

    RSHpaeFfrtPatternManager::Instance().SetThreadId();
    RSHpaeFfrtPatternManager::Instance().SetUpdatedFlag();
}

bool RSHpaeManager::HasHpaeBlurNode() const
{
    return hpaeStatus_.gotHpaeBlurNode;
}

// Note: call on prepare
bool RSHpaeManager::IsHpaeBlurNode(RSRenderNode& node, uint32_t phyWidth, uint32_t phyHeight)
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
        if (renderProperty.GetBackgroundBlurRadius() < HPAE_MIN_BLUR_RADIUS ||
            renderProperty.GetBackgroundBlurRadius() > HPAE_MAX_SIGMA * HPAE_SCALE_FACTOR) {
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

void RSHpaeManager::RegisterHpaeCallback(RSRenderNode& node, uint32_t phyWidth, uint32_t phyHeight)
{
    if (!stagingHpaeStatus_.hpaeBlurEnabled) {
        // do nothing
        return;
    }

    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return;
    }

    uint32_t bufWidth = phyWidth / HPAE_SCALE_FACTOR;
    uint32_t bufHeight = phyHeight / HPAE_SCALE_FACTOR;
    if (UNLIKELY(bufWidth == 0 || bufHeight == 0 || bufWidth > HPAE_BUFFER_MAX_WIDTH ||
        bufHeight > HPAE_BUFFER_MAX_HEIGHT)) {
        return;
    }

    if (IsHpaeBlurNode(node, phyWidth, phyHeight)) {
        hpaeBufferWidth_ = bufWidth; // set only once
        hpaeBufferHeight_ = bufHeight;

        stagingHpaeStatus_.gotHpaeBlurNode = true;
        stagingHpaeStatus_.blurNodeId = node.GetId();
        const auto &renderProperty = node.GetRenderProperties();
        stagingHpaeStatus_.tileMode = renderProperty.GetPixelStretchTileMode();
        stagingHpaeStatus_.greyCoef = renderProperty.GetGreyCoef();
        stagingHpaeStatus_.blurRadius = renderProperty.GetBackgroundBlurRadius();
        stagingHpaeStatus_.brightness = renderProperty.GetBackgroundBlurBrightness();
        stagingHpaeStatus_.saturation = renderProperty.GetBackgroundBlurSaturation();
        if (node.CanFuzePixelStretch()) {
            stagingHpaeStatus_.pixelStretch = renderProperty.GetPixelStretch();
            node.ResetPixelStretchSlot();
        } else {
            stagingHpaeStatus_.pixelStretch = Vector4f(0.f, 0.f, 0.f, 0.f);
        }
        return;
    }
}

bool RSHpaeManager::IsFirstFrame()
{
    return hpaeFrameState_ == HpaeFrameState::ACTIVE ||
        hpaeFrameState_ == HpaeFrameState::SWITCH_BLUR ||
        hpaeFrameState_ == HpaeFrameState::CHANGE_CONFIG;
}

// init only once
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

void RSHpaeManager::UpdateBufferIfNeed(const BufferRequestConfig &bufferConfig, bool isHebc)
{
    // compare the configs that may change
    if (bufferConfig.width != prevBufferConfig_.width ||
        bufferConfig.height != prevBufferConfig_.height ||
        bufferConfig.colorGamut != prevBufferConfig_.colorGamut ||
        bufferConfig.format != prevBufferConfig_.format ||
        prevIsHebc_ != isHebc) {
        HPAE_LOGW("config change and reset");
        HPAE_TRACE_NAME_FMT("config change and reset: [%dx%d], %d, %d, %d",
            bufferConfig.width, bufferConfig.height, bufferConfig.colorGamut, bufferConfig.format, isHebc);
        // FlushFrame() all before ForceDropFrame()
        RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
        HianimationManager::GetInstance().HianimationAlgoDeInit();
        HianimationManager::GetInstance().AlgoInitAsync(hpaeBufferWidth_, hpaeBufferHeight_,
            HPAE_MAX_SIGMA, bufferConfig.format);
        RSHpaeBaseData::GetInstance().SetIsFirstFrame(true);
        size_t i = 0;
        for (i = 0; i < inBufferVec_.size(); ++i) {
            hpaeBufferIn_->FlushFrame();
        }
        for (i = 0; i < inBufferVec_.size(); ++i) {
            hpaeBufferIn_->ForceDropFrame();
        }
        inBufferVec_.clear();

        for (i = 0; i < outBufferVec_.size(); ++i) {
            hpaeBufferOut_->FlushFrame();
        }
        for (i = 0; i < outBufferVec_.size(); ++i) {
            hpaeBufferOut_->ForceDropFrame();
        }
        outBufferVec_.clear();

        curIndex_ = 0;
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
    bufferConfig.colorGamut = colorSpace;
    bufferConfig.format = pixelFormat;
    bufferConfig.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_GRAPHIC_2D_ACCEL |
        BUFFER_USAGE_MEM_DMA;
    bufferConfig.timeout = 0;

    UpdateBufferIfNeed(bufferConfig, isHebc);

    prevBufferConfig_ = bufferConfig;
    prevIsHebc_ = isHebc;

    SetUpSurfaceIn(bufferConfig, isHebc);
    SetUpSurfaceOut(bufferConfig, isHebc);

    curIndex_ = (curIndex_ + 1) % HPAE_BUFFER_SIZE;
}

void RSHpaeManager::SetUpSurfaceIn(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    if (hpaeBufferIn_ == nullptr) {
        HPAE_LOGE("render layer IN is nullptr");
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
    layerFrameIn_[curIndex_] = hpaeBufferIn_->RequestFrame(bufferConfig, isHebc);
    if (!layerFrameIn_[curIndex_]) {
        HPAE_LOGE("RequestFrame layerFrameIn_ is null");
        return;
    }

    HpaeBufferInfo hpaeBufferInfo;
    hpaeBufferInfo.surface = layerFrameIn_[curIndex_]->GetFrame()->GetSurface();
    hpaeBufferInfo.canvas = std::make_shared<RSPaintFilterCanvas>(hpaeBufferInfo.surface.get());
    hpaeBufferInfo.grContext = hpaeBufferIn_->GetSkContext();
    hpaeBufferInfo.bufferHandle = hpaeBufferIn_->GetBufferHandle();

    inBufferVec_.push_back(hpaeBufferInfo);
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(hpaeBufferInfo);
}

void RSHpaeManager::SetUpSurfaceOut(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    if (hpaeBufferOut_ == nullptr) {
        HPAE_LOGE("render layer OUT is nullptr");
        return;
    }

    if (outBufferVec_.size() > curIndex_) {
        HpaeBufferInfo hpaeBufferInfo = outBufferVec_[curIndex_];
        RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(hpaeBufferInfo);
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
    hpaeBufferInfo.surface = layerFrameOut_[curIndex_]->GetFrame()->GetSurface();
    hpaeBufferInfo.canvas = std::make_shared<RSPaintFilterCanvas>(hpaeBufferInfo.surface.get());
    hpaeBufferInfo.grContext = hpaeBufferOut_->GetSkContext();
    hpaeBufferInfo.bufferHandle = hpaeBufferOut_->GetBufferHandle();

    outBufferVec_.push_back(hpaeBufferInfo);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(hpaeBufferInfo);
}

void RSHpaeManager::InitHpaeBlurResource()
{
    HianimationManager::GetInstance().OpenDeviceAsync();
}

} // Rosen
} // OHOS