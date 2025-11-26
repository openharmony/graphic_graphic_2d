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
#include "common/rs_optional_trace.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_log.h"
#include "system/rs_system_parameters.h"

#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"

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
    prevBufferConfig_ = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0x8, // default stride is 8 Bytes.
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = 0,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
}

RSHpaeManager::~RSHpaeManager()
{
    ReleaseIoBuffers();
}

void RSHpaeManager::OnUniRenderStart()
{
    // check Hianimation dependency and system parameters
    stagingHpaeStatus_.hpaeBlurEnabled =
        (!HianimationManager::GetInstance().HianimationInvalid()) && RSSystemParameters::GetHpaeBlurEnabled();

    stagingHpaeStatus_.gotHpaeBlurNode = false;
    stagingHpaeStatus_.blurNodeId = INVALID_BLUR_NODE_ID;
}

void RSHpaeManager::UpdateHpaeState()
{
    // check hpaeBufferAllocating_ before hpaeBufferNeedInit_
    bool bufferAllocating = hpaeBufferAllocating_.load(std::memory_order_acquire);
    bool releaseAllDone = releaseAllDone_.load(std::memory_order_acquire);
    if (bufferAllocating || !releaseAllDone) {
        HPAE_LOGD("HPAE::WAITING: %d, %d", bufferAllocating, releaseAllDone);
        RS_TRACE_NAME("HPAE::WAITING");
        stagingHpaeStatus_.gotHpaeBlurNode = false;
        hpaeFrameState_ = HpaeFrameState::WAITING;
        return;
    }

    if (stagingHpaeStatus_.gotHpaeBlurNode && hpaeBufferNeedInit_.load(std::memory_order_acquire)) {
        HPAE_LOGI("HPAE::ALLOC_BUF");
        RS_TRACE_NAME("HPAE::ALLOC_BUF");
        stagingHpaeStatus_.gotHpaeBlurNode = false;
        hpaeFrameState_ = HpaeFrameState::ALLOC_BUF;
        return;
    }

    bool curBlur = stagingHpaeStatus_.gotHpaeBlurNode;
    bool prevBlur = hpaeStatus_.gotHpaeBlurNode;
    auto curBlurNodeId = stagingHpaeStatus_.blurNodeId;
    auto preBlurNodeId = hpaeStatus_.blurNodeId;
    if (prevBlur) {
        hpaeFrameState_ = curBlur ? HpaeFrameState::WORKING : HpaeFrameState::DEACTIVE;
    } else {
        hpaeFrameState_ = curBlur ? HpaeFrameState::ACTIVE : HpaeFrameState::IDLE;
    }

    if (!curBlur && !hpaeBufferNeedInit_.load(std::memory_order_acquire)) {
        // no blur but has buffer
        hpaeFrameState_ = HpaeFrameState::DEACTIVE;
    }

    if (hpaeFrameState_ == HpaeFrameState::WORKING) {
        if (curBlurNodeId != preBlurNodeId &&
            curBlurNodeId != INVALID_BLUR_NODE_ID &&
            preBlurNodeId != INVALID_BLUR_NODE_ID) {
            hpaeFrameState_ = HpaeFrameState::SWITCH_BLUR;
        } else if (prevBufferConfig_.width != static_cast<int>(stagingHpaeStatus_.bufferWidth) ||
            prevBufferConfig_.height != static_cast<int>(stagingHpaeStatus_.bufferHeight)) {
            hpaeFrameState_ = HpaeFrameState::CHANGE_CONFIG;
        }
    }
}

void RSHpaeManager::OnActive()
{
    int format = GRAPHIC_PIXEL_FMT_RGBA_8888; // only support RGBA888 now
    RS_TRACE_NAME_FMT("HPAE::ACTIVE: %d, %d, format: %d",
        stagingHpaeStatus_.bufferWidth, stagingHpaeStatus_.bufferHeight, format);
    HPAE_LOGI("HPAE::ACTIVE");
    // can also use: HianimationAlgoInit
    HianimationManager::GetInstance().AlgoInitAsync(stagingHpaeStatus_.bufferWidth,
        stagingHpaeStatus_.bufferHeight, HPAE_MAX_SIGMA, format);
}

void RSHpaeManager::OnDeActive()
{
    RS_TRACE_NAME("HPAE::DEACTIVE");
    HPAE_LOGI("HPAE::DEACTIVE");
    releaseAllDone_.store(false, std::memory_order_release);
    ffrt::submit_h([=]() {
        RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
        HianimationManager::GetInstance().HianimationAlgoDeInit();
        this->ReleaseIoBuffers();
        RSUniRenderThread::Instance().PostSyncTask([this]() {
            RS_OPTIONAL_TRACE_BEGIN("HPAE::ResetBuffer");
            std::unique_lock<std::mutex> lock(this->releaseIoBuffersMutex_);
            this->hpaeBufferIn_.reset();
            this->hpaeBufferOut_.reset();
            RS_OPTIONAL_TRACE_END();
        });
        this->releaseAllDone_.store(true, std::memory_order_release);
        HPAE_LOGI("HPAE::DEACTIVE done");
        }, {}, {}, ffrt::task_attr().qos(5));
}

void RSHpaeManager::HandleHpaeStateChange()
{
    UpdateHpaeState();
    // note: bufferWidth/bufferHeight is done after prepare with IsHpaeBlurNode()
    switch (hpaeFrameState_) {
        case HpaeFrameState::ACTIVE: {
            OnActive();
            break;
        }
        case HpaeFrameState::DEACTIVE: {
            OnDeActive();
            break;
        }
        case HpaeFrameState::CHANGE_CONFIG: {
            RS_TRACE_NAME_FMT("HPAE::CHANGE_CONFIG: %d, %d",
                stagingHpaeStatus_.bufferWidth, stagingHpaeStatus_.bufferHeight);
            HPAE_LOGI("CHANGE_CONFIG: %d, %d",
                stagingHpaeStatus_.bufferWidth, stagingHpaeStatus_.bufferHeight);
            break;
        }
        case HpaeFrameState::SWITCH_BLUR: {
            RS_TRACE_NAME("HPAE::SWITCH_BLUR");
            HPAE_LOGI("SWITCH_BLUR");
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
    RS_OPTIONAL_TRACE_NAME("HpaeManager::OnSync");
    hpaeVsyncId_++;
    bool notHebc = RSAncoManager::Instance()->GetAncoHebcStatus() == AncoHebcStatus::NOT_USE_HEBC;
    if (isHdrOn || hpaeVsyncId_ == 0 || notHebc) {
        stagingHpaeStatus_.gotHpaeBlurNode = false;
    }

    HandleHpaeStateChange();

    hpaeStatus_ = stagingHpaeStatus_;
    RSHpaeBaseData::GetInstance().Reset();
    RSHpaeBaseData::GetInstance().SyncHpaeStatus(hpaeStatus_);
    RSHpaeBaseData::GetInstance().SetIsFirstFrame(IsFirstFrame());
    RSHpaeBaseData::GetInstance().SetBlurContentChanged(false);

    // graphic pattern
    RSHpaeFfrtPatternManager::Instance().MHCSetVsyncId(hpaeVsyncId_);
    RSHpaeFfrtPatternManager::Instance().MHCSetCurFrameId(0);

    RSHpaeFfrtPatternManager::Instance().SetThreadId();
    RSHpaeFfrtPatternManager::Instance().SetUpdatedFlag(); // avoid DP to HPAE/FFTS
}

bool RSHpaeManager::HasHpaeBlurNode() const
{
    return hpaeStatus_.gotHpaeBlurNode;
}

// Note: call on prepare
bool RSHpaeManager::IsHpaeBlurNode(RSRenderNode& node, uint32_t phyWidth, uint32_t phyHeight)
{
    // check after node.ApplyModifiers() since node's drawable info is required
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
        if ((rect.GetHeight() == static_cast<int>(phyHeight) && rect.GetWidth() == static_cast<int>(phyWidth))) {
            RS_OPTIONAL_TRACE_NAME_FMT("got blur node: size [%d, %d], id: %lld",
                phyWidth, phyHeight, node.GetId());
            return true;
        }
    }

    return false;
}
void RSHpaeManager::RegisterHpaeCallback(RSRenderNode& node, const std::shared_ptr<RSScreenRenderNode>& screenNode)
{
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return;
    }

    if (!stagingHpaeStatus_.hpaeBlurEnabled || stagingHpaeStatus_.gotHpaeBlurNode) {
        // don't support multi-hpaeBlur now
        return;
    }

    if (screenNode == nullptr) {
        return;
    }

    auto& screenInfo = screenNode->GetScreenInfo();
    uint32_t phyWidth = screenInfo.phyWidth;
    uint32_t phyHeight = screenInfo.phyHeight;

    uint32_t bufWidth = phyWidth / HPAE_SCALE_FACTOR;
    uint32_t bufHeight = phyHeight / HPAE_SCALE_FACTOR;
    if (UNLIKELY(bufWidth == 0 || bufHeight == 0 || bufWidth > HPAE_BUFFER_MAX_WIDTH ||
        bufHeight > HPAE_BUFFER_MAX_HEIGHT)) {
        return;
    }

    if (IsHpaeBlurNode(node, phyWidth, phyHeight)) {
        stagingHpaeStatus_.bufferWidth = bufWidth; // set only once
        stagingHpaeStatus_.bufferHeight = bufHeight;

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
void RSHpaeManager::InitIoBuffers()
{
    RS_OPTIONAL_TRACE_NAME("InitIoBuffers");
    std::unique_lock<std::mutex> lock(releaseIoBuffersMutex_);
    if (hpaeBufferIn_ == nullptr) {
        hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_INPUT_LAYER_NAME, HPAE_INPUT_LAYER_ID);
    }
    if (hpaeBufferOut_ == nullptr) {
        hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_OUTPUT_LAYER_NAME, HPAE_OUTPUT_LAYER_ID);
    }
    hpaeBufferNeedInit_.store(true, std::memory_order_release);
}

void RSHpaeManager::ReleaseIoBuffers()
{
    RS_OPTIONAL_TRACE_NAME("ReleaseIoBuffers");
    HPAE_LOGI("ReleaseIoBuffers +");
    std::unique_lock<std::mutex> lock(releaseIoBuffersMutex_);
    size_t i = 0;
    if (hpaeBufferIn_ != nullptr) {
        for (i = 0; i < inBufferVec_.size(); ++i) {
            hpaeBufferIn_->ForceDropFrame();
        }
        inBufferVec_.clear();
    }
    if (hpaeBufferOut_ != nullptr) {
        for (i = 0; i < outBufferVec_.size(); ++i) {
            hpaeBufferOut_->ForceDropFrame();
        }
        outBufferVec_.clear();
    }
    for (i = 0; i < HPAE_BUFFER_SIZE; ++i) {
        if (layerFrameIn_[i]) {
            layerFrameIn_[i]->Reset();
            layerFrameIn_[i] = nullptr;
        }
        if (layerFrameOut_[i]) {
            layerFrameOut_[i]->Reset();
            layerFrameOut_[i] = nullptr;
        }
    }
    curIndex_ = 0;
    hpaeBufferNeedInit_.store(true, std::memory_order_release);
    HPAE_LOGI("ReleaseIoBuffers -");
}

bool RSHpaeManager::UpdateBufferIfNeed(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    // compare the configs that may change
    bool algoNeedInit = bufferConfig.width != prevBufferConfig_.width ||
        bufferConfig.height != prevBufferConfig_.height ||
        bufferConfig.format != prevBufferConfig_.format;

    if (algoNeedInit ||
        bufferConfig.colorGamut != prevBufferConfig_.colorGamut ||
        prevIsHebc_ != isHebc) {
        HPAE_LOGI("config change and reset");
        RS_OPTIONAL_TRACE_NAME_FMT("config change and reset: [%dx%d], %d, %d, %d",
            bufferConfig.width, bufferConfig.height, bufferConfig.colorGamut, bufferConfig.format, isHebc);
        RSHpaeFfrtPatternManager::Instance().MHCReleaseAll();
        if (algoNeedInit) {
            HianimationManager::GetInstance().HianimationAlgoDeInit();
        }
        ReleaseIoBuffers();
        return true;
    }

    return false;
}

void RSHpaeManager::CheckHpaeBlur(bool isHdrOn, GraphicPixelFormat pixelFormat, GraphicColorGamut colorSpace,
    bool isHebc)
{
    if (isHebc && !isHdrOn && (HasHpaeBlurNode() || hpaeFrameState_ == HpaeFrameState::ALLOC_BUF)) {
        SetUpHpaeSurface(pixelFormat, colorSpace, isHebc);
    }
}

void RSHpaeManager::SetUpHpaeSurface(GraphicPixelFormat pixelFormat, GraphicColorGamut colorSpace, bool isHebc)
{
    RS_OPTIONAL_TRACE_NAME_FMT("SetUpHpaeSurface: pixelFormat:%d, hebc: %d, colorSpace: %d, curIndex_=%u",
        pixelFormat, isHebc, colorSpace, curIndex_);

    BufferRequestConfig bufferConfig {};
    bufferConfig.width = static_cast<int>(hpaeStatus_.bufferWidth);
    bufferConfig.height = static_cast<int>(hpaeStatus_.bufferHeight);
    bufferConfig.strideAlignment = 0x8; // default stride is 8 Bytes.
    bufferConfig.colorGamut = colorSpace;
    bufferConfig.format = pixelFormat;
    bufferConfig.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_GRAPHIC_2D_ACCEL |
        BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_ALLOC_NO_IPC;
    bufferConfig.timeout = 0;

    if (hpaeFrameState_ != HpaeFrameState::ALLOC_BUF && UpdateBufferIfNeed(bufferConfig, isHebc)) {
        hpaeStatus_.gotHpaeBlurNode = false; // notify no hpae blur for this frame
        hpaeFrameState_ = HpaeFrameState::ALLOC_BUF; // try alloc buffer again
    }

    if (hpaeFrameState_ == HpaeFrameState::ALLOC_BUF) {
        AllocBuffer(bufferConfig, isHebc);
        return;
    }

    if (!RSHpaeBaseData::GetInstance().GetBufferUsed()) {
        // equal to : curIndex_ -= 1
        curIndex_ = (curIndex_ + HPAE_BUFFER_SIZE - 1) % HPAE_BUFFER_SIZE;
    }

    RSHpaeBaseData::GetInstance().NotifyBufferUsed(false);

    SetUpSurfaceIn(bufferConfig, isHebc);
    SetUpSurfaceOut(bufferConfig, isHebc);

    curIndex_ = (curIndex_ + 1) % HPAE_BUFFER_SIZE;
}

void RSHpaeManager::AllocBuffer(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    prevBufferConfig_ = bufferConfig;
    prevIsHebc_ = isHebc;

    std::unique_lock<std::mutex> lock(releaseIoBuffersMutex_);
    if (hpaeBufferIn_ == nullptr) {
        hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_INPUT_LAYER_NAME, HPAE_INPUT_LAYER_ID);
    }
    if (hpaeBufferOut_ == nullptr) {
        hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>(HPAE_OUTPUT_LAYER_NAME, HPAE_OUTPUT_LAYER_ID);
    }
    hpaeBufferIn_->Init(bufferConfig, isHebc);
    hpaeBufferOut_->Init(bufferConfig, isHebc);
    hpaeBufferNeedInit_.store(false, std::memory_order_release);

    hpaeBufferAllocating_.store(true, std::memory_order_release);
    curIndex_ = 0;
    RSHpaeBaseData::GetInstance().SyncHpaeStatus(HpaeStatus()); // notify no hpae blur
    ffrt::submit_h([this, bufferConfig, isHebc]() {
        RS_TRACE_BEGIN("HpaePreAllocateBuffer");
        HPAE_LOGI("AllocBuf: +");
        std::unique_lock<std::mutex> lock(this->releaseIoBuffersMutex_);
        if (this->hpaeBufferIn_ && this->hpaeBufferOut_) {
            this->hpaeBufferIn_->PreAllocateBuffer(bufferConfig.width, bufferConfig.height, isHebc);
            this->hpaeBufferOut_->PreAllocateBuffer(bufferConfig.width, bufferConfig.height, isHebc);
        }
        this->hpaeBufferAllocating_.store(false, std::memory_order_release);
        HPAE_LOGI("AllocBuf: -");
        RS_TRACE_END();
    });
}

void RSHpaeManager::SetUpSurfaceIn(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    std::unique_lock<std::mutex> lock(releaseIoBuffersMutex_);
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
    layerFrameIn_[curIndex_] = hpaeBufferIn_->RequestFrame(bufferConfig.width, bufferConfig.height, isHebc);
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
    hpaeBufferIn_->FlushFrame();
}

void RSHpaeManager::SetUpSurfaceOut(const BufferRequestConfig& bufferConfig, bool isHebc)
{
    std::unique_lock<std::mutex> lock(releaseIoBuffersMutex_);
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
    layerFrameOut_[curIndex_] = hpaeBufferOut_->RequestFrame(bufferConfig.width, bufferConfig.height, isHebc);
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
    hpaeBufferOut_->FlushFrame();
}

void RSHpaeManager::InitHpaeBlurResource()
{
    HianimationManager::GetInstance().OpenDeviceAsync();
}

} // Rosen
} // OHOS

#endif // (ROSEN_OHOS)