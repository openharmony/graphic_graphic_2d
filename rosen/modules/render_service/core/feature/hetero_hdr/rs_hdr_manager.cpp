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

#include "common/rs_optional_trace.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/hetero_hdr/rs_hetero_hdr_util.h"
#include "feature/hetero_hdr/rs_hdr_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "metadata_helper.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSHdrManager &RSHdrManager::Instance()
{
    static RSHdrManager instance;
    return instance;
}

RSHdrManager::RSHdrManager()
{
    isHeterogComputingHdrOn_ = RSSystemProperties::GetHeterogComputingHDREnabled();
    if (!RSHDRPatternManager::Instance().MHCDlOpen()) {
        RS_LOGE("MHCDlOpen() failed!");
        isHeterogComputingHdrOn_ = false;
        return;
    }
    if (!RSHDRPatternManager::Instance().MHCGraphPatternInit(GRAPH_NUM)) {
        RS_LOGE("MHCGraphPatternInit() failed!");
        isHeterogComputingHdrOn_ = false;
    }
}

RectI RSHdrManager::RectRound(RectI srcRect)
{
    int32_t srcWidth = srcRect.width_ & ~1;
    int32_t srcHeight = srcRect.height_ & ~1;
    int32_t srcTop = srcRect.top_ & ~1;
    int32_t srcLeft = srcRect.left_ & ~1;

    RectI dstRect = {srcLeft, srcTop, srcWidth, srcHeight};
    return dstRect;
}

bool RSHdrManager::EquRect(RectI a, RectI b)
{
    if ((a.top_ != b.top_) || (a.height_ != b.height_) || (a.left_ != b.left_) || (a.width_ != b.width_)) {
        return false;
    }
    return true;
}

void RSHdrManager::UpdateHdrNodes(RSSurfaceRenderNode &node, bool isCurrentFrameBufferConsumed)
{
    if (isHeterogComputingHdrOn_ && node.GetVideoHdrStatus() == HdrStatus::HDR_VIDEO) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.shared_from_this());
        pendingPostNodes_[node.GetId()] = surfaceNode;
        isCurrentFrameBufferConsumedMap_[node.GetId()] = isCurrentFrameBufferConsumed;
    }
}

void RSHdrManager::GetFixDstRectStatus(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable,
    bool isUifistR, RSSurfaceRenderParams *surfaceParams, RectI &finalDstRect, bool &isFixDstRect)
{
    if (surfaceParams == nullptr) {
        RS_LOGE("surfaceParams is nullptr");
        return;
    }

    auto srcRect = surfaceParams->GetLayerInfo().srcRect;
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    Drawing::Matrix matrix = surfaceParams->GetLayerInfo().matrix;

    float ratio = 1.0f;
    if (float(dstRect.h) * float(srcRect.w) != 0.0f) {
        ratio = float(srcRect.h) * float(dstRect.w) / (float(dstRect.h) * float(srcRect.w));
    }

    bool ratiojudge = (abs(ratio - 1.0) > 0.02);
    ScreenInfo curScreenInfo = CreateOrGetScreenManager()->QueryScreenInfo(GetScreenIDByDrawable(drawable));
    int realRotation = RSBaseRenderUtil::RotateEnumToInt(
        RSBaseRenderUtil::GetRotateTransform(surfaceParams->GetLayerInfo().transformType));

    Vector2f HpaeBufferSize = (realRotation == ROTATION_90 || realRotation == ROTATION_270)
                               ? Vector2f(curScreenInfo.height, curScreenInfo.width)
                               : Vector2f(curScreenInfo.width, curScreenInfo.height);
    Vector2f boundSize = surfaceParams->GetCacheSize();
    bool sizejudge = abs(matrix.Get(1)) > HDR_EPSILON || abs(matrix.Get(3)) > HDR_EPSILON;
    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();

    if (realRotation == ROTATION_90 || realRotation == ROTATION_270) {
        boundSize.y_ = HpaeBufferSize.y_;
        if (srcBuffer->GetSurfaceBufferHeight() > 0) {
            boundSize.x_ = round(boundSize.y_ * srcBuffer->GetSurfaceBufferWidth() /
                srcBuffer->GetSurfaceBufferHeight());
        }
        boundSize.x_ = (boundSize.x_ > HpaeBufferSize.x_) ? HpaeBufferSize.x_ : boundSize.x_;
        sizejudge = true;
    } else {
        boundSize.x_ = HpaeBufferSize.x_;
        if (srcBuffer->GetSurfaceBufferWidth() > 0) {
            boundSize.x_ = round(boundSize.y_ * srcBuffer->GetSurfaceBufferWidth() /
                srcBuffer->GetSurfaceBufferHeight());
        }
        boundSize.y_ = round(boundSize.x_ * srcBuffer->GetSurfaceBufferHeight() / srcBuffer->GetSurfaceBufferWidth());
        boundSize.y_ = (boundSize.y_ > HpaeBufferSize.y_) ? HpaeBufferSize.y_ : boundSize.y_;
    }

    finalDstRect.left_ = 0;
    finalDstRect.top_ = 0;
    finalDstRect.width_ = boundSize.x_;
    finalDstRect.height_ = boundSize.y_;
    isFixDstRect = isUifistR || ratiojudge || sizejudge;
    if (!isFixDstRect) {
        finalDstRect.width_ = dstRect.w;
        finalDstRect.height_ = dstRect.h;
    }
    finalDstRect = RectRound(finalDstRect);
}

bool RSHdrManager::PrepareHapeTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable, uint64_t curFrameId)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(nodeDrawable->GetRenderParams().get());
    // 1. Hpae Device is ready
    std::unique_lock<std::mutex> lock(HpaeMtx_);
    bool ret0 = cond_.wait_for(lock, std::chrono::milliseconds(HPAE_TIMEOUT), [
                                this] { return this->destroyedFlag_.load(); });
    if (ret0 == false) {
        RS_LOGE("Hpae device is not ready!");
        return false;
    }
    // 2. Request Graph
    if (!RSHDRPatternManager::Instance().MHCRequestEGraph(curFrameId)) {
        RS_LOGE("mhc_so MHCRequestEGraph failed. frameId:%{public}" PRIu64 " ", curFrameId);
        return false;
    }
    // 3. Set Current Frame Id
    bool ret1 = RSHDRPatternManager::Instance().MHCSetCurFrameId(curFrameId);
    if (ret1 == false) {
        RS_LOGE("MHCSetCurFrameId failed!");
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }

    // 4. Apply for ION memory
    bool ret2 = RSHDRPatternManager::Instance().TryConsumeBuffer(
        [nodeDrawable]() { (nodeDrawable->GetRsHdrBUfferLayer())->ConsumeAndUpdateBuffer(); });
    if (ret2 == false) {
        RS_LOGE("TryConsumeBuffer failed");
        RSHDRPatternManager::Instance().MHCReSetCurFrameId();
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }
    dstBuffer_ = (nodeDrawable->GetRsHdrBUfferLayer())->PrepareHDRDstBuffer(surfaceParams, GetScreenIDByDrawable(nodeDrawable));
    if (dstBuffer_ == nullptr) {
        RS_LOGE("dstBuffer is nullptr");
        RSHDRPatternManager::Instance().MHCReSetCurFrameId();
        RSHDRPatternManager::Instance().MHCReleaseEGraph(curFrameId);
        return false;
    }

    (nodeDrawable->GetRsHdrBUfferLayer())->FlushHDRDstBuffer(dstBuffer_);
    RSHDRPatternManager::Instance().SetBufferFlushed();

    return true;
}

bool RSHdrManager::GetCurFrameHeterogHandleCanBeUsed()
{
    return curFrameHeterogHandleCanBeUesed_;
}

void RSHdrManager::ProcessPendingNode(uint32_t nodeId, uint64_t curFrameId)
{
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(nodeId);
    if (!drawable) {
        curFrameHeterogHandleCanBeUesed_ = false;
        RS_LOGE("RSHdrManager::PostHdrSubTasks drawable is nullptr!");
        return;
    }

    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
    nodeDrawable->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);

    if (!CheckWindowOwnership(nodeId)) {
        curFrameHeterogHandleCanBeUesed_ = false;
        return;
    }

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
    if (!ValidateSurface(surfaceParams)) {
        curFrameHeterogHandleCanBeUesed_ = false;
        return;
    }

    if (!PrepareAndSubmitHdrTask(nodeDrawable, surfaceParams, nodeId, curFrameId)) {
        curFrameHeterogHandleCanBeUesed_ = false;
    }
}

bool RSHdrManager::CheckWindowOwnership(uint32_t nodeId)
{
    const auto& pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();
    if (ownedLeashWindowIdMap_.find(nodeId) == ownedLeashWindowIdMap_.end() &&
        ownedAppWindowIdMap_.find(nodeId) == ownedAppWindowIdMap_.end()) {
        RS_LOGE("Cannot find the owned Leash Window of the video surface!");
        return false;
    }

    if (ownedAppWindowIdMap_.find(nodeId) != ownedAppWindowIdMap_.end()) {
        auto ownedAppWindowDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(ownedAppWindowIdMap_[nodeId]));
        if (ownedAppWindowDrawable && IsHDRSurfaceNodeSkipped(ownedAppWindowDrawable)) {
            RS_LOGE("ownedAppWindowDrawable IsHDRSurfaceNodeSkipped!");
            return false;
        }
    }

    if (ownedLeashWindowIdMap_.find(nodeId) != ownedLeashWindowIdMap_.end()) {
        auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(ownedLeashWindowIdMap_[nodeId]);
        if (drawable == nullptr) {
            RS_LOGE("RSHdrManager::CheckWindowOwnership drawable is nullptr");
            return false;
        }

        auto ownedLeashWindowDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (ownedLeashWindowDrawable && IsHDRSurfaceNodeSkipped(ownedLeashWindowDrawable) &&
            pendingNodes.find(ownedLeashWindowIdMap_[nodeId]) == pendingNodes.end()) {
            RS_LOGE("DrawWithUIFirstCache and not rendered in UIFirst SubThread!");
            return false;
        }
    }
    return true;
}

bool RSHdrManager::ValidateSurface(RSSurfaceRenderParams* surfaceParams)
{
    if (surfaceParams == nullptr) {
        RS_LOGE("surfaceParams is nullptr");
        return false;
    }

    const auto& dstRect = surfaceParams->GetLayerInfo().dstRect;
    if (dstRect.w == 0 || dstRect.h == 0) {
        RS_LOGE("dstRect is invalid");
        return false;
    }
    return true;
}

bool RSHdrManager::PrepareAndSubmitHdrTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable,
    RSSurfaceRenderParams* surfaceParams,
    uint32_t nodeId,
    uint64_t curFrameId)
{
    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();
    auto srcRect = surfaceParams->GetLayerInfo().srcRect;
    const auto& pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();

    bool need2Handle = isCurrentFrameBufferConsumedMap_[nodeId] ||
                      (!nodeDrawable->GetCurHeterogComputingHdr());
    auto src = RectRound(RectI(srcRect.x, srcRect.y, srcRect.w, srcRect.h));

    GetFixDstRectStatus(nodeDrawable, (pendingNodes.find(ownedLeashWindowIdMap_[nodeId]) != pendingNodes.end()),
        surfaceParams, dst_, isFixedDstBuffer_);
    // isFixedDstBuffer is true when hpae and GPU are used separately for scaling
    if (isFixedDstBuffer_ || srcRect.w == 0 || srcRect.h == 0) {
        // this condition, crop and scale need to be handle by gpu, so the output of hpae shulde 
        // be full size and the input of the video should also be full size
        src = {0, 0, srcBuffer->GetSurfaceBufferWidth(), srcBuffer->GetSurfaceBufferHeight()};
    }

    RectI prevSrc = nodeSrcRect_[nodeId];
    if (!EquRect(src, prevSrc)) {
        need2Handle = true;
    }
    nodeSrcRect_[nodeId] = src;

    MdcRectT srcRect_mdc{src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom()};
    nodeDrawable->SetNodeDrawableNodeDstRect(isFixedDstBuffer_, dst_);

    if (!need2Handle) {
        return true;
    }
    if (!PrepareHapeTask(nodeDrawable, curFrameId)) {
        RS_LOGE("Hape hdr heterog PrepareHapeTask failed!");
        return false;
    }

    // 5. Submit HDR task
    RSHeteroHdrUtil::GetInstance().RequestHpaeChannel();
    destroyedFlag_.store(false);
    nodeDrawable->SetHpaeDstRect(dst_);

    RSHDRPatternManager::Instance().MHCSubmitHDRTask(
        curFrameId, MHC_PATTERN_TASK_HDR_HPAE,
        [surfaceParams, nodeId, srcRect_mdc, this] {
            uint32_t taskId = 0;
            this->buildHDRTaskStatus = BuildHDRTask(surfaceParams, srcRect_mdc, &taskId, &(this->taskPtr_));
            this->taskId_.store(taskId);
        },
        &taskPtr_,
        [this, curFrameId] {
            RSHeteroHdrUtil::GetInstance().DestroyHpaeHdrTask(this->taskId_.load());
            this->taskPtr_ = nullptr;
            destroyedFlag_.store(true);
            this->cond_.notify_one();
        });
    return true;
}

void RSHdrManager::PostHdrSubTasks()
{
    if (!isHeterogComputingHdrOn_) {
        return;
    }

    uint64_t curFrameId = OHOS::Rosen::HgmCore::Instance().GetVsyncId();
    RSHDRPatternManager::Instance().MHCSetVsyncId(curFrameId);
    RSHdrManager::Instance().FindParentLeashWindowNode();

    if (pendingPostNodes_.size() == 1 && curFrameId != 0) {
        curFrameHeterogHandleCanBeUesed_ = true;
        for (auto &item : pendingPostNodes_) {
            ProcessPendingNode(item.first, curFrameId);
            if (!curFrameHeterogHandleCanBeUesed_) {
                break;
            }
        }
    } else {
        curFrameHeterogHandleCanBeUesed_ = false;
        if (destroyedFlag_.load()) {
            RSHeteroHdrUtil::GetInstance().ReleaseHpaeHdrChannel();
        } else {
            RS_LOGI("AAE device is not ready or aae task doesnt exit ");
        }
    }

    pendingPostNodes_.clear();
    ownedLeashWindowIdMap_.clear();
    ownedAppWindowIdMap_.clear();
}

ScreenId RSHdrManager::GetScreenIDByDrawable(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable)
{
#ifdef RS_ENABLE_GPU
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
    if (!surfaceParams || !surfaceParams->GetAncestorScreenNode().lock()) {
        return INVALID_SCREEN_ID;
    }
    auto ancestor = surfaceParams->GetAncestorScreenNode().lock()->ReinterpretCastTo<RSScreenRenderNode>();
    if (!ancestor) {
        return INVALID_SCREEN_ID;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(ancestor->GetRenderParams().get());
    if (!screenParams) {
        return INVALID_SCREEN_ID;
    }
    return screenParams->GetScreenId();
#else
    return INVALID_SCREEN_ID;
#endif
}

void RSHdrManager::FindParentLeashWindowNode()
{
    if (pendingPostNodes_.size() != 1) {
        return;
    }
    for (auto &item : pendingPostNodes_) {
        windowSceneNode_ = nullptr;
        appWindowNode_ = nullptr;
        ReverseParentNode(item.second->GetParent().lock());
        if (windowSceneNode_ != nullptr) {
            ownedLeashWindowIdMap_[item.first] = windowSceneNode_->GetId();
        }
        if (appWindowNode_ != nullptr) {
            ownedAppWindowIdMap_[item.first] = appWindowNode_->GetId();
        }
    }
    return;
}

void RSHdrManager::ReverseParentNode(std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr) {
        return;
    }
    auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
    if (surfaceNode && surfaceNode->IsAppWindow()) {
        appWindowNode_ = surfaceNode;
    }
    if (surfaceNode && surfaceNode->IsLeashWindow()) {
        windowSceneNode_ = surfaceNode;
    }
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node);
    if (displayNode) {
        auto drawable = displayNode->GetRenderDrawable();
        if (drawable == nullptr) {
            return;
        }
        auto displayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(drawable);
        displayDrawable->CheckAndUpdateFilterCacheOcclusionFast();
        return;
    }
    ReverseParentNode(node->GetParent().lock());
}

int32_t RSHdrManager::BuildHDRTask(
    RSSurfaceRenderParams *surfaceParams, MdcRectT srcRect, uint32_t *taskId, void **taskPtr)
{
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    BufferHandle *dstHandle = dstBuffer_->GetBufferHandle();
    if (!dstHandle) {
        RS_LOGE("QM: dstHandle is not available");
        return -1;
    }
    MdcRectT HpaeDstRect = {dst_.GetLeft(), dst_.GetTop(), dst_.GetRight(), dst_.GetBottom()};

    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();
    constexpr float GAMMA2_2 = 2.2f;

    hapeTaskInfoT taskInfo;
    taskInfo.taskId = taskId;
    taskInfo.taskPtr = taskPtr;
    taskInfo.srcRect = srcRect;
    taskInfo.dstRect = HpaeDstRect;
    taskInfo.srcHandle = srcBuffer->GetBufferHandle();
    taskInfo.dstHandle = dstHandle;
    taskInfo.displaySDRNits = surfaceParams->GetSdrNit();
    taskInfo.displayHDRNits =
        (surfaceParams->GetDisplayNit()) / std::pow(surfaceParams->GetBrightnessRatio(), GAMMA2_2);

    return RSHeteroHdrUtil::GetInstance().BuildHpaeHdrTask(taskInfo);
}

bool RSHdrManager::IsHDRSurfaceNodeSkipped(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> surfaceDrawable)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceDrawable->GetRenderParams().get());
    if (RSUniRenderUtil::CheckRenderSkipIfScreenOff(true, GetScreenIDByDrawable(surfaceDrawable))) {
        return true;
    }
    if (!surfaceDrawable->ShouldPaint()) {
        return true;
    }
    auto &uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw uniParam is nullptr");
        return true;
    }

    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return true;
    }
    if (surfaceParams->GetOccludedByFilterCache()) {
        RS_TRACE_NAME("RSSurfaceRenderNodeDrawable::FilterCache Skip");
        return true;
    }
    auto enableType = surfaceParams->GetUifirstNodeEnableParam();
    auto cacheState = surfaceDrawable->GetRsSubThreadCache().GetCacheSurfaceProcessedStatus();
    if ((!RSUniRenderThread::GetCaptureParam().isSnapshot_ && enableType == MultiThreadCacheType::NONE &&
            cacheState != CacheProcessStatus::WAITING && cacheState != CacheProcessStatus::DOING) ||
        (RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
            !surfaceDrawable->GetRsSubThreadCache().HasCachedTexture())) {
        return false;
    }
    return true;
}
}  // namespace Rosen
}  // namespace OHOS
