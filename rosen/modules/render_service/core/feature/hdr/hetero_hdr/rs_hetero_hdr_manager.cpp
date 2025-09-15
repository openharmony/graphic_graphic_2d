/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_hetero_hdr_manager.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_util.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "metadata_helper.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float DEGAMMA = 1.0f / 2.2f;
constexpr float GAMMA2_2 = 2.2f;
constexpr int GRAPH_NUM = 3;
constexpr int MAX_RELEASE_FRAME_NUM = 5;
constexpr float RATIO_CHANGE_TH = 0.02f;
}

RSHeteroHDRManager& RSHeteroHDRManager::Instance()
{
    static RSHeteroHDRManager instance;
    return instance;
}

RSHeteroHDRManager::RSHeteroHDRManager()
{
    isHeteroComputingHdrOn_ = RSSystemProperties::GetHeterogeneousHDREnabled();
    if (!isHeteroComputingHdrOn_) {
        return;
    }
    if (!RSHDRPatternManager::Instance().MHCDlOpen()) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager MHCDlOpen failed");
        isHeteroComputingHdrOn_ = false;
        return;
    }
    if (!RSHeteroHDRHpae::GetInstance().IsHpaeAvailable()) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager Hpae device is unavailable");
        isHeteroComputingHdrOn_ = false;
        return;
    }
    if (!RSHDRPatternManager::Instance().MHCGraphPatternInit(GRAPH_NUM)) {
        RSHDRPatternManager::Instance().MHCDlClose();
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager MHCGraphPatternInit failed");
        isHeteroComputingHdrOn_ = false;
    }
}

int32_t RSHeteroHDRManager::RoundDownToEven(int32_t value)
{
    int32_t result = (value % 2 == 0) ? value : value - 1;
    return (value > 0) ? result : 0;
}

RectI RSHeteroHDRManager::RectRound(RectI srcRect)
{
    int32_t srcWidth = RoundDownToEven(srcRect.width_);
    int32_t srcHeight = RoundDownToEven(srcRect.height_);
    int32_t srcTop = RoundDownToEven(srcRect.top_);
    int32_t srcLeft = RoundDownToEven(srcRect.left_);

    RectI dstRect = { srcLeft, srcTop, srcWidth, srcHeight };
    return dstRect;
}

void RSHeteroHDRManager::UpdateHDRNodes(RSSurfaceRenderNode& node, bool isCurrentFrameBufferConsumed)
{
    if (!isHeteroComputingHdrOn_) {
        return;
    }
    HdrStatus hdrStatus = node.GetVideoHdrStatus();
    if (hdrStatus != HdrStatus::HDR_VIDEO && hdrStatus != HdrStatus::AI_HDR_VIDEO_GAINMAP) {
        return;
    }
    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.shared_from_this());

    pendingPostNodes_.push_back(surfaceNode);
    isCurrentFrameBufferConsumedMap_[node.GetId()] = isCurrentFrameBufferConsumed;
}

void RSHeteroHDRManager::GetFixedDstRectStatus(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& drawable,
    bool isUiFirstMode, RSSurfaceRenderParams* surfaceParams)
{
    isFixedDstBuffer_ = false;
    if (surfaceParams == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GetFixedDstRectStatus surfaceParams is nullptr");
        return;
    }
    // The precondition has already determined that the surfaceParams->GetBuffer() is not nullptr(ValidateSurface)
    auto bufferWidth = surfaceParams->GetBuffer()->GetSurfaceBufferWidth();
    auto bufferHeight = surfaceParams->GetBuffer()->GetSurfaceBufferHeight();
    // The precondition has already checked that this node is in nodeHdrStatusMap
    if (curHandleStatus_ == HdrStatus::AI_HDR_VIDEO_GAINMAP) {
        isFixedDstBuffer_ = true;
        dst_ = { 0, 0, bufferWidth, bufferHeight };
        return;
    }
    auto srcRect = surfaceParams->GetLayerInfo().srcRect;
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    Drawing::Matrix matrix = surfaceParams->GetLayerInfo().matrix;

    float ratio = float(srcRect.h) * float(dstRect.w) / (float(dstRect.h) * float(srcRect.w));

    bool ratioJudge = !ROSEN_EQ<float>(ratio, 1.0, RATIO_CHANGE_TH);
    ScreenInfo curScreenInfo = CreateOrGetScreenManager()->QueryScreenInfo(GetScreenIDByDrawable(drawable));
    auto transform = RSBaseRenderUtil::GetRotateTransform(surfaceParams->GetLayerInfo().transformType);
    bool isVertical =
        (transform == GraphicTransformType::GRAPHIC_ROTATE_90 || transform == GraphicTransformType::GRAPHIC_ROTATE_270);

    Vector2f hpaeBufferSize = (isVertical) ? Vector2f(curScreenInfo.height, curScreenInfo.width)
                                           : Vector2f(curScreenInfo.width, curScreenInfo.height);
    Vector2f boundSize = surfaceParams->GetCacheSize();
    bool sizeJudge = !(ROSEN_EQ(matrix.Get(Drawing::Matrix::Index::SKEW_X), 0.0f) &&
        ROSEN_EQ(matrix.Get(Drawing::Matrix::Index::SKEW_Y), 0.0f));

    if (bufferHeight == 0 or bufferWidth == 0) {
        return;
    }

    if (isVertical) {
        boundSize.y_ = hpaeBufferSize.y_;
        boundSize.x_ = round(boundSize.y_ * bufferWidth / bufferHeight);
        boundSize.x_ = (boundSize.x_ > hpaeBufferSize.x_) ? hpaeBufferSize.x_ : boundSize.x_;
        sizeJudge = true;
    } else {
        boundSize.x_ = hpaeBufferSize.x_;
        boundSize.y_ = round(boundSize.x_ * bufferHeight / bufferWidth);
        boundSize.y_ = (boundSize.y_ > hpaeBufferSize.y_) ? hpaeBufferSize.y_ : boundSize.y_;
    }
    dst_.left_, dst_.top_ = 0, 0;
    dst_.width_, dst_.height_ = boundSize.x_, boundSize.y_;
    isFixedDstBuffer_ = isUiFirstMode || ratioJudge || sizeJudge ||
        transform == GraphicTransformType::GRAPHIC_ROTATE_180;
    if (!isFixedDstBuffer_) {
        dst_.width_ = dstRect.w;
        dst_.height_ = dstRect.h;
    }
    dst_ = RectRound(dst_);
}

bool RSHeteroHDRManager::PrepareHpaeTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& nodeDrawable, RSSurfaceRenderParams* surfaceParams,
    uint64_t curFrameId)
{
    // 1. Hpae Device is ready
    if (!destroyedFlag_.load()) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask Hpae device is not ready");
        return false;
    }
    if (!RSHeteroHDRHpae::GetInstance().CheckHpaeAccessible(curHandleStatus_)) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask CheckHpaeAccessible failed");
        return false;
    }
    // 2. Request ExecutionGraph
    if (!MHCRequestEGraph(curFrameId)) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask MHCRequestEGraph failed. frameId:%{public}" PRIu64 " ",
            curFrameId);
        return false;
    }
    // 3. Set Current Frame Id
    if (!MHCSetCurFrameId(curFrameId)) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask MHCSetCurFrameId failed");
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }
    // 4. Apply for ION memory
    if (!TryConsumeBuffer([this]() { this->rsHeteroHDRBufferLayer_.ConsumeAndUpdateBuffer(); })) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask TryConsumeBuffer failed");
        RSHDRPatternManager::Instance().MHCResetCurFrameId();
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }

    dstBuffer_ = rsHeteroHDRBufferLayer_.PrepareHDRDstBuffer(surfaceParams, GetScreenIDByDrawable(nodeDrawable));
    if (dstBuffer_ == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask prepare dstBuffer is nullptr");
        RSHDRPatternManager::Instance().MHCResetCurFrameId();
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }
    RS_LOGD("[hdrHetero]:RSHeteroHDRManager PrepareHpaeTask dstBuffer seq = %{public}" PRIu32 ".",
        dstBuffer_->GetSeqNum());
    rsHeteroHDRBufferLayer_.FlushHDRDstBuffer(dstBuffer_);
    RSHDRPatternManager::Instance().SetBufferFlushed();

    return true;
}

bool RSHeteroHDRManager::ProcessPendingNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, uint64_t curFrameId)
{
    curNodeId_ = surfaceNode->GetId();
    curHandleStatus_ = surfaceNode->GetVideoHdrStatus();
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(curNodeId_);
    if (!drawable) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager ProcessPendingNode drawable is nullptr");
        return false;
    }
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
    if (!nodeDrawable) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager ProcessPendingNode nodeDrawable is nullptr");
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
    if (!RSHeteroHDRUtil::ValidateSurface(surfaceParams)) {
        return false;
    }
    if (surfaceParams->GetHardwareEnabled()) {
        RS_LOGD("[hdrHetero]:RSHeteroHDRManager ProcessPendingNode Hardware is enable");
        ClearBufferCache();
        return false;
    }
    // isHdrOn_ will be updated in function GetScreenIDByDrawable
    ScreenId screenId = GetScreenIDByDrawable(nodeDrawable);
    if (!isHdrOn_ || surfaceParams->GetColorFollow()) {
        RS_LOGD("[hdrHetero]:RSHeteroHDRManager ProcessPendingNode isHdrOn is false or GetColorFollow is true");
        ClearBufferCache();
        return false;
    }
    if (!CheckWindowOwnership(curNodeId_)) {
        return false;
    }

    if (!PrepareAndSubmitHDRTask(nodeDrawable, surfaceParams, curNodeId_, curFrameId)) {
        return false;
    } else {
        framesNoApplyCnt_ = 0;
    }
    return true;
}

bool RSHeteroHDRManager::CheckWindowOwnership(NodeId nodeId)
{
    auto ownedLeashWindowIdIter = ownedLeashWindowIdMap_.find(nodeId);
    auto ownedAppWindowIdIter = ownedAppWindowIdMap_.find(nodeId);
    if (ownedLeashWindowIdIter == ownedLeashWindowIdMap_.end() && ownedAppWindowIdIter == ownedAppWindowIdMap_.end()) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager CheckWindowOwnership Cannot find the owned Leash Window");
        return false;
    }
    if (ownedAppWindowIdIter != ownedAppWindowIdMap_.end()) {
        auto ownedAppWindowDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(ownedAppWindowIdIter->second));
        if (IsHDRSurfaceNodeSkipped(ownedAppWindowDrawable)) {
            RS_LOGD("[hdrHetero]:RSHeteroHDRManager CheckWindowOwnership "
                "ownedAppWindowDrawable IsHDRSurfaceNodeSkipped");
            return false;
        }
    }

    if (ownedLeashWindowIdIter != ownedLeashWindowIdMap_.end()) {
        auto ownedLeashWindowDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(ownedLeashWindowIdIter->second));
        const auto& pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();
        if (IsHDRSurfaceNodeSkipped(ownedLeashWindowDrawable) && !pendingNodes.count(nodeId)) {
            RS_LOGD("[hdrHetero]:RSHeteroHDRManager CheckWindowOwnership this frame draw with UiFirst cache and "
                "not rendered in UIFirst SubThread");
            return false;
        }
    }
    return true;
}

void RSHeteroHDRManager::ClearBufferCache()
{
    if (framesNoApplyCnt_ > MAX_RELEASE_FRAME_NUM) {
        dstBuffer_ = nullptr;
        rsHeteroHDRBufferLayer_.CleanCache();
        framesNoApplyCnt_ = 0;
        nodeSrcRectMap_.clear();
    } else {
        framesNoApplyCnt_++;
    }
}

bool RSHeteroHDRManager::PrepareAndSubmitHDRTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& nodeDrawable,
    RSSurfaceRenderParams* surfaceParams, NodeId nodeId, uint64_t curFrameId)
{
    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();
    const auto& srcRect = surfaceParams->GetLayerInfo().srcRect;
    const auto& pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();
    auto iterConsumed = isCurrentFrameBufferConsumedMap_.find(nodeId);
    bool need2Handle = (iterConsumed != isCurrentFrameBufferConsumedMap_.end() && iterConsumed->second) ||
        (!preFrameHeteroHandleCanBeUsed_);

    /* Whether nodeId is in the map has been determined in function CheckWindowOwnership. */
    bool isUiFirstMode = pendingNodes.count(ownedLeashWindowIdMap_[nodeId]);
    GetFixedDstRectStatus(nodeDrawable, isUiFirstMode, surfaceParams);
    RectI src = {};
    /*
    * isFixedDstBuffer_ is true when hpae and GPU are used separately for scaling.
    * The precondition has already determined that the width and height of srcBuffer are not zero
    */
    if (isFixedDstBuffer_) {
        // this condition, crop and scale need to be handle by gpu
        src = { 0, 0, srcBuffer->GetSurfaceBufferWidth(), srcBuffer->GetSurfaceBufferHeight() };
    } else {
        src = RectRound(RectI(srcRect.x, srcRect.y, srcRect.w, srcRect.h));
    }

    RectI prevSrc = nodeSrcRectMap_[nodeId];
    if (src != prevSrc) {
        need2Handle = true;
        nodeSrcRectMap_[nodeId] = src;
    }
    MDCRectT MDCSrcRect{ src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom() };
    if (!need2Handle) {
        return true;
    }
    if (!PrepareHpaeTask(nodeDrawable, surfaceParams, curFrameId)) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager PrepareAndSubmitHDRTask PrepareHpaeTask failed!");
        return false;
    }
    hpaeDstRect_ = { dst_.GetLeft(), dst_.GetTop(), dst_.GetRight(), dst_.GetBottom() };

    destroyedFlag_.store(false);

    bool submitRet = RSHDRPatternManager::Instance().MHCSubmitHDRTask(
        curFrameId, MHC_PATTERN_TASK_HDR_HPAE,
        [surfaceParams, nodeId, MDCSrcRect, this] {
            uint32_t taskId = 0;
            this->buildHdrTaskStatus_ =
                BuildHDRTask(surfaceParams, MDCSrcRect, &taskId, &(this->taskPtr_), this->curHandleStatus_);
            this->taskId_.store(taskId);
        },
        &taskPtr_,
        [this, curFrameId] {
            RS_TRACE_NAME("[hdrHetero]:RSHeteroHDRManager PrepareAndSubmitHDRTask afterFunc finished");
            RSHeteroHDRHpae::GetInstance().DestroyHpaeHDRTask(this->taskId_.load());
            this->taskPtr_ = nullptr;
            destroyedFlag_.store(true);
        });
    return submitRet;
}

void RSHeteroHDRManager::PostHDRSubTasks()
{
    if (!isHeteroComputingHdrOn_) {
        return;
    }
    curNodeId_ = 0;
    uint64_t curFrameId = OHOS::Rosen::HgmCore::Instance().GetVsyncId();
    RSHDRPatternManager::Instance().MHCSetVsyncId(curFrameId);
    FindParentLeashWindowNode();

    if (pendingPostNodes_.size() == 1 && curFrameId != 0) {
        curFrameHeteroHandleCanBeUsed_ = ProcessPendingNode(pendingPostNodes_.front(), curFrameId);
    } else {
        curFrameHeteroHandleCanBeUsed_ = false;
        if (destroyedFlag_.load()) {
            RSHeteroHDRHpae::GetInstance().ReleaseHpaeHDRChannel();
        } else {
            RS_LOGD("[hdrHetero]:RSHeteroHDRManager PostHDRSubTasks Hpae device is not ready or the task doesn't exit");
        }
        ClearBufferCache();
    }
    preFrameHeteroHandleCanBeUsed_ = curFrameHeteroHandleCanBeUsed_;
    pendingPostNodes_.clear();
    ownedLeashWindowIdMap_.clear();
    ownedAppWindowIdMap_.clear();
    isCurrentFrameBufferConsumedMap_.clear();
}

ScreenId RSHeteroHDRManager::GetScreenIDByDrawable(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& drawable)
{
    isHdrOn_ = false;
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GetScreenIDByDrawable surfaceParams is nullptr");
        return INVALID_SCREEN_ID;
    }
    auto screenNodeptr = surfaceParams->GetAncestorScreenNode().lock();
    if (!screenNodeptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GetScreenIDByDrawable screenNodeptr is nullptr");
        return INVALID_SCREEN_ID;
    }
    auto ancestor = screenNodeptr->ReinterpretCastTo<RSScreenRenderNode>();
    if (!ancestor) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GetScreenIDByDrawable ancestor is nullptr");
        return INVALID_SCREEN_ID;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(ancestor->GetRenderParams().get());
    if (!screenParams) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GetScreenIDByDrawable screenParams is nullptr");
        return INVALID_SCREEN_ID;
    }
    isHdrOn_ = screenParams->GetHDRPresent();
    return screenParams->GetScreenId();
#else
    return INVALID_SCREEN_ID;
#endif
}

void RSHeteroHDRManager::FindParentLeashWindowNode()
{
    if (pendingPostNodes_.size() != 1) {
        return;
    }
    auto node = pendingPostNodes_.front();
    std::shared_ptr<RSSurfaceRenderNode> windowSceneNode = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> appWindowNode = nullptr;
    auto instanceRootNode = node->GetInstanceRootNode();
    if (!instanceRootNode) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager FindParentLeashWindowNode get instanceRoot Node failed");
        return;
    }
    auto surfaceNode = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode && surfaceNode->IsAppWindow()) {
        appWindowNode = surfaceNode;
        auto parentSurfaceNode =
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(appWindowNode->GetParent().lock());
        if (parentSurfaceNode && parentSurfaceNode->IsLeashWindow()) {
            windowSceneNode = parentSurfaceNode;
        }
    }
    NodeId nodeId = node->GetId();
    if (windowSceneNode) {
        ownedLeashWindowIdMap_[nodeId] = windowSceneNode->GetId();
    }
    if (appWindowNode) {
        ownedAppWindowIdMap_[nodeId] = appWindowNode->GetId();
    }
    RS_LOGD("[hdrHetero]:RSHeteroHDRManager FindParentLeashWindowNode LeashWindowNode:%{public}" PRIu64 ", "
        "AppWindowNode:%{public}" PRIu64, ownedLeashWindowIdMap_[nodeId], ownedAppWindowIdMap_[nodeId]);
}

bool RSHeteroHDRManager::ReleaseBuffer()
{
    return rsHeteroHDRBufferLayer_.ReleaseBuffer();
}

int32_t RSHeteroHDRManager::BuildHDRTask(
    RSSurfaceRenderParams* surfaceParams, MDCRectT srcRect, uint32_t* taskId, void** taskPtr, HdrStatus curHandleStatus)
{
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    BufferHandle* dstHandle = dstBuffer_->GetBufferHandle();

    if (!dstHandle) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager BuildHDRTask dstHandle is not available");
        return -1;
    }
    MDCRectT hpaeDstRect = { dst_.GetLeft(), dst_.GetTop(), dst_.GetRight(), dst_.GetBottom() };

    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();

    HpaeTaskInfoT taskInfo;
    taskInfo.taskId = taskId;
    taskInfo.taskPtr = taskPtr;
    taskInfo.srcRect = srcRect;
    taskInfo.dstRect = hpaeDstRect;
    taskInfo.srcHandle = srcBuffer->GetBufferHandle();
    taskInfo.dstHandle = dstHandle;
    taskInfo.curHandleStatus = curHandleStatus;
    taskInfo.displaySdrNit = surfaceParams->GetSdrNit();
    taskInfo.displayHdrNit =
        (surfaceParams->GetDisplayNit()) / std::pow(surfaceParams->GetBrightnessRatio(), GAMMA2_2);

    return RSHeteroHDRHpae::GetInstance().BuildHpaeHDRTask(taskInfo);
}

bool RSHeteroHDRManager::IsHDRSurfaceNodeSkipped(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable)
{
    if (surfaceDrawable == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager IsHDRSurfaceNodeSkipped surfaceDrawable is nullptr");
        return true;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager IsHDRSurfaceNodeSkipped surfaceParams is nullptr");
        return true;
    }
    if (surfaceParams->GetOccludedByFilterCache()) {
        RS_TRACE_NAME("[hdrHetero]:RSHeteroHDRManager IsHDRSurfaceNodeSkipped FilterCache Skip");
        return true;
    }
    if (RSUniRenderUtil::CheckRenderSkipIfScreenOff(true, GetScreenIDByDrawable(surfaceDrawable))) {
        return true;
    }
    if (!surfaceDrawable->ShouldPaint()) {
        return true;
    }
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager IsHDRSurfaceNodeSkipped uniParam is nullptr");
        return true;
    }

    auto enableType = surfaceParams->GetUifirstNodeEnableParam();
    auto cacheStatus = surfaceDrawable->GetRsSubThreadCache().GetCacheSurfaceProcessedStatus();
    bool noSkip = (!RSUniRenderThread::GetCaptureParam().isSnapshot_ && enableType == MultiThreadCacheType::NONE &&
                    cacheStatus != CacheProcessStatus::WAITING && cacheStatus != CacheProcessStatus::DOING) ||
                    (RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
                    !surfaceDrawable->GetRsSubThreadCache().HasCachedTexture());
    if (noSkip) {
        return false;
    }
    return true;
}

bool RSHeteroHDRManager::UpdateHDRHeteroParams(RSPaintFilterCanvas& canvas,
    const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& drawableParams)
{
    if (!isHeteroComputingHdrOn_) {
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable.GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager UpdateHDRHeteroParams surfaceParams is nullptr");
        return false;
    }
    NodeId nodeId = surfaceParams->GetId();

    bool skipDraw = surfaceParams->GetHardwareEnabled() || !GetCurFrameHeteroHandleCanBeUsed(nodeId) ||
                    !(GetCurHandleStatus() == HdrStatus::HDR_VIDEO ||
                    GetCurHandleStatus() == HdrStatus::AI_HDR_VIDEO_GAINMAP) || drawableParams.useCPU ||
                    canvas.IsOnMultipleScreen() || RSUniRenderThread::IsInCaptureProcess();
    if (!skipDraw) {
        std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandler = GetHDRSurfaceHandler();
        if (hdrSurfaceHandler == nullptr) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRManager UpdateHDRHeteroParams hdrSurfaceHandler is nullptr");
            return false;
        }
        rsHeteroHDRBufferLayer_.ReleaseBuffer();
        bool invalidBuffer =
            !RSBaseRenderUtil::ConsumeAndUpdateBuffer(*hdrSurfaceHandler, CONSUME_DIRECTLY, false, 0) ||
            !hdrSurfaceHandler->GetBuffer();
        if (invalidBuffer) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRManager UpdateHDRHeteroParams ConsumeAndUpdateBuffer or GetBuffer failed");
            return false;
        }
        RSHDRPatternManager::Instance().SetThreadId(canvas);

        ProcessParamsUpdate(canvas, surfaceDrawable, drawableParams);
        RS_LOGD("[hdrHetero]:RSHeteroHDRManager UpdateHDRHeteroParams done");
        return true;
    }
    return false;
}

void RSHeteroHDRManager::ProcessParamsUpdate(RSPaintFilterCanvas& canvas,
    const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& drawableParams)
{
    std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandler = GetHDRSurfaceHandler();
    if (hdrSurfaceHandler == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager ProcessParamsUpdate hdrSurfaceHandler is nullptr");
        return;
    }
    HdrStatus curHandleStatus = GetCurHandleStatus();
    uint32_t hdrHeteroType;
    switch (curHandleStatus) {
        case HdrStatus::AI_HDR_VIDEO_GAINMAP:
            hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO | RSHeteroHDRUtilConst::HDR_HETERO_AIHDR;
            break;
        case HdrStatus::HDR_VIDEO:
            hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO | RSHeteroHDRUtilConst::HDR_HETERO_HDR;
            break;
        default:
            hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_NO;
            break;
    }
    RSHeteroHDRUtil::GenDrawHDRBufferParams(surfaceDrawable, hpaeDstRect_, isFixedDstBuffer_, drawableParams);
    drawableParams.buffer = hdrSurfaceHandler->GetBuffer();
    drawableParams.acquireFence = hdrSurfaceHandler->GetAcquireFence();
    drawableParams.hdrHeteroType = hdrHeteroType;
    drawableParams.threadIndex = static_cast<uint32_t>(gettid());
}

std::shared_ptr<Drawing::ShaderEffect> RSHeteroHDRManager::MakeHDRHeadroomShader(float hdrRatio,
    std::shared_ptr<Drawing::ShaderEffect>& imageShader)
{
    static constexpr char prog[] = R"(
        uniform half hdrRatio;
        uniform shader imageShader;
        const mat3 MAT_RGB_Bt2020tosRGB = mat3(1.660491, -0.124550, -0.018151,
            -0.587641, 1.132900, -0.100579,
            -0.072850, -0.008349, 1.118730);

        half4 main(float2 coord)
        {
            vec4 color = imageShader.eval(coord);
            vec3 linearColor = sign(color.rgb) * pow(abs(color.rgb), vec3(2.2f));
            linearColor = MAT_RGB_Bt2020tosRGB * linearColor;
            vec3 hdr = sign(linearColor) * pow(abs(linearColor), vec3(1.0f / 2.2f));
            hdr = hdr * hdrRatio;
            return vec4(hdr, 1.0);
        }
    )";
    static std::shared_ptr<Drawing::RuntimeEffect> hdrHeadroomShaderEffect;

    if (hdrHeadroomShaderEffect == nullptr) {
        hdrHeadroomShaderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
        if (hdrHeadroomShaderEffect == nullptr) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRManager MakeHDRHeadroomShader CreateForShader failed");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = { imageShader };
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&hdrRatio, sizeof(hdrRatio));
    return hdrHeadroomShaderEffect->MakeShader(data, children, childCount, nullptr, false);
}

std::shared_ptr<Drawing::ShaderEffect> RSHeteroHDRManager::MakeAIHDRGainmapHeadroomShader(float hdrRatio,
    std::shared_ptr<Drawing::ShaderEffect>& imageShader)
{
    static constexpr char prog[] = R"(
        uniform half hdrRatio;
        uniform shader imageShader;
        half4 main(float2 coord)
        {
            vec4 color = imageShader.eval(coord);
            vec4 hdr = color * hdrRatio;
            return vec4(hdr.rgb, 1.0);
        }
    )";
    static std::shared_ptr<Drawing::RuntimeEffect> AIHDRHeadroomShaderEffect;
    if (AIHDRHeadroomShaderEffect == nullptr) {
        AIHDRHeadroomShaderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
        if (AIHDRHeadroomShaderEffect == nullptr) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRManager MakeAIHDRGainmapHeadroomShader CreateForShader failed");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = { imageShader };
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&hdrRatio, sizeof(hdrRatio));
    return AIHDRHeadroomShaderEffect->MakeShader(data, children, childCount, nullptr, false);
}

void RSHeteroHDRManager::GenerateHDRHeteroShader(
    BufferDrawParam& param, std::shared_ptr<Drawing::ShaderEffect>& imageShader)
{
    float hdrRatio = 0.0f;
    std::shared_ptr<Drawing::ShaderEffect> shader = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (ROSEN_NE(param.sdrNits, 0.0f)) {
        hdrRatio = std::pow((param.tmoNits / param.sdrNits), DEGAMMA);
    }
    if (param.hdrHeteroType & RSHeteroHDRUtilConst::HDR_HETERO_AIHDR) {
        shader = MakeAIHDRGainmapHeadroomShader(hdrRatio, imageShader);
    } else if (param.hdrHeteroType & RSHeteroHDRUtilConst::HDR_HETERO_HDR) {
        shader = MakeHDRHeadroomShader(hdrRatio, imageShader);
    }
#endif // USE_VIDEO_PROCESSING_ENGINE
    if (shader == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRManager GenerateHDRHeteroShader shader is nullptr");
    } else {
        param.paint.SetShaderEffect(shader);
    }
}
} // namespace Rosen
} // namespace OHOS
