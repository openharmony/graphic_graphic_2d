/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_main_thread.h"

#include <securec.h>
#include "rs_trace.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_occlusion_config.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/drawing/rs_vsync_client.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"
#include "accessibility_config.h"
#include "rs_qos_thread.h"

using namespace OHOS::AccessibilityConfig;
namespace OHOS {
namespace Rosen {
namespace {
bool Compare(const std::unique_ptr<RSTransactionData>& data1, const std::unique_ptr<RSTransactionData>& data2)
{
    if (!data1 || !data2) {
        RS_LOGW("Compare RSTransactionData: nullptr!");
        return true;
    }
    return data1->GetIndex() < data2->GetIndex();
}

void InsertToEnd(std::vector<std::unique_ptr<RSTransactionData>>& source,
    std::vector<std::unique_ptr<RSTransactionData>>& target)
{
    target.insert(target.end(), std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
    source.clear();
}
}

class ColorCorrectionObserver : public AccessibilityConfigObserver {
public:
    ColorCorrectionObserver() = default;
    virtual void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
    {
        RS_LOGD("ColorCorrectionObserver OnConfigChanged");
        ColorFilterMode mode = ColorFilterMode::COLOR_FILTER_END;
        if (id == CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
            switch (value.daltonizationColorFilter) {
                case Protanomaly:
                    mode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
                    break;
                case Deuteranomaly:
                    mode = ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE;
                    break;
                case Tritanomaly:
                    mode = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
                    break;
                case Normal:
                    mode = ColorFilterMode::DALTONIZATION_NORMAL_MODE;
                    break;
            }
        } else {
            mode = value.invertColor ? ColorFilterMode::INVERT_COLOR_ENABLE_MODE :
                                        ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
        }
        RSMainThread::Instance()->GetRenderEngine()->SetColorFilterMode(mode);
    }
};

RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    RSAnimationFraction::Init();
    return &instance;
}

RSMainThread::RSMainThread() : mainThreadId_(std::this_thread::get_id()) {}

RSMainThread::~RSMainThread() noexcept
{
    RemoveRSEventDetector();
    RSInnovation::CloseInnovationSo();
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        RS_LOGD("RsDebug mainLoop start");
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition");
        ConsumeAndUpdateAllNodes();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        Animate(timestamp_);
        Render();
        ReleaseAllNodesBuffer();
        SendCommands();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        SetRSEventDetectorLoopFinishTag();
        rsEventManager_.UpdateParam();
        RS_LOGD("RsDebug mainLoop end");
    };

    if (isUniRender_) {
        unmarshalBarrierTask_ = [this]() {
            auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
            MergeToEffectiveTransactionDataMap(cachedTransactionData);
            {
                std::lock_guard<std::mutex> lock(unmarshalMutex_);
                ++unmarshalFinishedCount_;
            }
            unmarshalTaskCond_.notify_all();
        };
        RSUnmarshalThread::Instance().Start();
    }

    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    InitRSEventDetector();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs");
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, handler_);
    receiver_->Init();
    renderEngine_ = std::make_shared<RSRenderEngine>();
#ifdef RS_ENABLE_GL
    if (renderEngine_) {
        int cacheLimitsTimes = 2; // double skia Resource Cache Limits
        auto grContext = renderEngine_->GetRenderContext()->GetGrContext();
        int maxResources = 0;
        size_t maxResourcesSize = 0;
        grContext->getResourceCacheLimits(&maxResources, &maxResourcesSize);
        grContext->setResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes * maxResourcesSize);
    }
#endif
    RSInnovation::OpenInnovationSo();
    Occlusion::Region::InitDynamicLibraryFunction();

    correctionObserver_ = std::make_shared<ColorCorrectionObserver>();
    auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    config.InitializeContext();
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER, correctionObserver_);
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_INVERT_COLOR, correctionObserver_);
}

void RSMainThread::RsEventParamDump(std::string& dumpString)
{
    rsEventManager_.DumpAllEventParam(dumpString);
}

void RSMainThread::RemoveRSEventDetector()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.RemoveEvent(rsCompositionTimeoutDetector_->GetStringId());
    }
}

void RSMainThread::InitRSEventDetector()
{
    // default Threshold value of Timeout Event: 2000ms
    rsCompositionTimeoutDetector_ = RSBaseEventDetector::CreateRSTimeOutDetector(2000, "RS_COMPOSITION_TIMEOUT");
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.AddEvent(rsCompositionTimeoutDetector_, 60000); // report Internal 1min:60s：60000ms
        RS_LOGD("InitRSEventDetector  finish");
    }
}

void RSMainThread::SetRSEventDetectorLoopStartTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsCompositionTimeoutDetector_->SetLoopStartTag();
    }
}

void RSMainThread::SetRSEventDetectorLoopFinishTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsCompositionTimeoutDetector_->SetLoopFinishTag();
    }
}

void RSMainThread::Start()
{
    if (runner_) {
        runner_->Run();
    }
}

void RSMainThread::ProcessCommand()
{
    if (!isUniRender_) { // divided render for all
        ProcessCommandForDividedRender();
        return;
    }
    CheckBufferAvailableIfNeed();
    // dynamic switch
    if (useUniVisitor_) {
        ProcessCommandForDividedRender();
        ProcessCommandForUniRender();
    } else {
        ProcessCommandForUniRender();
        ProcessCommandForDividedRender();
    }
    CheckUpdateSurfaceNodeIfNeed();
}

void RSMainThread::ProcessCommandForUniRender()
{
    TransactionDataMap transactionDataEffective;
    std::string transactionFlags;
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);

        for (auto& elem: effectiveTransactionDataIndexMap_) {
            auto& transactionVec = elem.second.second;
            std::sort(transactionVec.begin(), transactionVec.end(), Compare);
        }

        for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
            auto pid = rsTransactionElem.first;
            auto& lastIndex = rsTransactionElem.second.first;
            auto& transactionVec = rsTransactionElem.second.second;
            auto iter = transactionVec.begin();
            for (; iter != transactionVec.end(); ++iter) {
                if ((*iter) == nullptr) {
                    continue;
                }
                auto curIndex = (*iter)->GetIndex();
                if (curIndex == lastIndex + 1) {
                    ++lastIndex;
                    transactionFlags += ", [" + std::to_string(pid) + ", " + std::to_string(curIndex) + "]";
                } else {
                    RS_LOGE("RSMainThread::ProcessCommandForUniRender wait curIndex:%llu, lastIndex:%llu, pid:%d",
                        curIndex, lastIndex, pid);
                    break;
                }
            }
            transactionDataEffective[pid].insert(transactionDataEffective[pid].end(),
                std::make_move_iterator(transactionVec.begin()), std::make_move_iterator(iter));
            transactionVec.erase(transactionVec.begin(), iter);
        }
    }
    RS_TRACE_NAME("RSMainThread::ProcessCommandUni" + transactionFlags);
    for (auto& rsTransactionElem: transactionDataEffective) {
        for (auto& rsTransaction: rsTransactionElem.second) {
            if (rsTransaction) {
                rsTransaction->Process(context_);
            }
        }
    }
}

void RSMainThread::ProcessCommandForDividedRender()
{
    const auto& nodeMap = context_.GetNodeMap();
    RS_TRACE_BEGIN("RSMainThread::ProcessCommand");
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        if (!pendingEffectiveCommands_.empty()) {
            effectiveCommands_.insert(effectiveCommands_.end(),
                std::make_move_iterator(pendingEffectiveCommands_.begin()),
                std::make_move_iterator(pendingEffectiveCommands_.end()));
            pendingEffectiveCommands_.clear();
        }
        if (!waitingBufferAvailable_ && !followVisitorCommands_.empty()) {
            effectiveCommands_.insert(effectiveCommands_.end(),
                std::make_move_iterator(followVisitorCommands_.begin()),
                std::make_move_iterator(followVisitorCommands_.end()));
            followVisitorCommands_.clear();
        }
        for (auto& [surfaceNodeId, commandMap] : cachedCommands_) {
            auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
            auto bufferTimestamp = bufferTimestamps_.find(surfaceNodeId);
            std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>::iterator effectIter;

            if (!node || !node->IsOnTheTree() || bufferTimestamp == bufferTimestamps_.end() || useUniVisitor_) {
                // If node has been destructed or is not on the tree or has no valid buffer,
                // for all command cached in commandMap should be executed immediately
                effectIter = commandMap.end();
            } else {
                uint64_t timestamp = bufferTimestamp->second;
                effectIter = commandMap.upper_bound(timestamp);
            }

            for (auto it = commandMap.begin(); it != effectIter; it++) {
                effectiveCommands_.insert(effectiveCommands_.end(), std::make_move_iterator(it->second.begin()),
                    std::make_move_iterator(it->second.end()));
            }
            commandMap.erase(commandMap.begin(), effectIter);

            for (auto it = commandMap.begin(); it != commandMap.end(); it++) {
                RS_LOGD("RSMainThread::ProcessCommand CacheCommand NodeId = %" PRIu64 ", timestamp = %" PRIu64
                        ", commandSize = %zu",
                    surfaceNodeId, it->first, it->second.size());
            }
        }
    }
    for (auto& command : effectiveCommands_) {
        if (command) {
            command->Process(context_);
        }
    }
    effectiveCommands_.clear();
    RS_TRACE_END();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    RS_TRACE_NAME("RSMainThread::ConsumeAndUpdateAllNodes");
    bool needRequestNextVsync = false;
    bufferTimestamps_.clear();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraversalNodes([this, &needRequestNextVsync](const std::shared_ptr<RSBaseRenderNode>& node) mutable {
        if (node == nullptr) {
            return;
        }

        if (node->IsInstanceOf<RSSurfaceRenderNode>()) {
            RSSurfaceRenderNode& surfaceNode = *(RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node));
            auto& surfaceHandler = static_cast<RSSurfaceHandler&>(surfaceNode);
            surfaceHandler.ResetCurrentFrameBufferConsumed();
            if (RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler)) {
                this->bufferTimestamps_[surfaceNode.GetId()] = static_cast<uint64_t>(surfaceNode.GetTimestamp());
            }

            // still have buffer(s) to consume.
            if (surfaceHandler.GetAvailableBufferCount() > 0) {
                needRequestNextVsync = true;
            }

            // check first frame callback in uniRender case
            if (!IfUseUniVisitor() || !surfaceNode.IsAppWindow()) {
                return;
            }
            for (auto& child : surfaceNode.GetSortedChildren()) {
                if (child != nullptr && child->IsInstanceOf<RSRootRenderNode>()) {
                    auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
                    rootNode->ApplyModifiers();
                    const auto& property = rootNode->GetRenderProperties();
                    if (property.GetFrameWidth() > 0 && property.GetFrameWidth() > 0 &&
                        rootNode->GetEnableRender()) {
                        surfaceNode.NotifyUIBufferAvailable();
                    }
                }
            }
            surfaceNode.ResetSortedChildren();
        }
    });

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
}

void RSMainThread::ReleaseAllNodesBuffer()
{
    RS_TRACE_NAME("RSMainThread::ReleaseAllNodesBuffer");
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraversalNodes([this](const std::shared_ptr<RSBaseRenderNode>& node) mutable {
        if (node == nullptr) {
            return;
        }

        if (node->IsInstanceOf<RSSurfaceRenderNode>()) {
            RSSurfaceRenderNode& surfaceNode = *(RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node));
            auto& surfaceHandler = static_cast<RSSurfaceHandler&>(surfaceNode);
            (void)RSBaseRenderUtil::ReleaseBuffer(surfaceHandler);
        }
    });
}

void RSMainThread::WaitUtilUniRenderFinished()
{
    std::unique_lock<std::mutex> lock(uniRenderMutex_);
    if (uniRenderFinished_) {
        return;
    }
    uniRenderCond_.wait(lock, [this]() { return uniRenderFinished_; });
    uniRenderFinished_ = false;
}

void RSMainThread::WaitUntilUnmarshallingTaskFinished()
{
    if (!isUniRender_) {
        return;
    }
    RS_TRACE_NAME("RSMainThread::WaitUntilUnmarshallingTaskFinished");
    std::unique_lock<std::mutex> lock(unmarshalMutex_);
    unmarshalTaskCond_.wait(lock, [this]() { return unmarshalFinishedCount_ > 0; });
    --unmarshalFinishedCount_;
}

void RSMainThread::MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap)
{
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    for (auto& elem : cachedTransactionDataMap) {
        auto pid = elem.first;
        if (effectiveTransactionDataIndexMap_.count(pid) == 0) {
            RS_LOGE("RSMainThread::MergeToEffectiveTransactionDataMap pid:%d not valid, skip it", pid);
            continue;
        }
        InsertToEnd(elem.second, effectiveTransactionDataIndexMap_[pid].second);
    }
    cachedTransactionDataMap.clear();
}

void RSMainThread::NotifyUniRenderFinish()
{
    if (std::this_thread::get_id() != Id()) {
        std::lock_guard<std::mutex> lock(uniRenderMutex_);
        uniRenderFinished_ = true;
        uniRenderCond_.notify_one();
    } else {
        uniRenderFinished_ = true;
    }
}

bool RSMainThread::IfUseUniVisitor() const
{
    return (useUniVisitor_ && !waitingUpdateSurfaceNode_) || (!useUniVisitor_ && waitingBufferAvailable_);
}

void RSMainThread::CheckBufferAvailableIfNeed()
{
    if (!waitingBufferAvailable_) {
        return;
    }
    const auto& nodeMap = GetContext().GetNodeMap();
    bool allBufferAvailable = true;
    for (auto& [id, node] : nodeMap.renderNodeMap_) {
        if (node == nullptr || !node->IsInstanceOf<RSSurfaceRenderNode>() || !node->IsOnTheTree()) {
            continue;
        }
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        if (!surfaceNode->IsAppWindow()) {
            continue;
        }
        if (surfaceNode->GetBuffer() == nullptr) {
            allBufferAvailable = false;
            break;
        }
    }
    waitingBufferAvailable_ = !allBufferAvailable;
    if (!waitingBufferAvailable_ && renderModeChangeCallback_) {
        renderModeChangeCallback_->OnRenderModeChanged(false);
    }
}

void RSMainThread::CheckUpdateSurfaceNodeIfNeed()
{
    if (!waitingUpdateSurfaceNode_) {
        return;
    }
    const auto& nodeMap = GetContext().GetNodeMap();
    bool allSurfaceNodeUpdated = true;
    for (auto& [id, node] : nodeMap.renderNodeMap_) {
        if (!allSurfaceNodeUpdated) {
            break;
        }
        if (node == nullptr || !node->IsInstanceOf<RSSurfaceRenderNode>() || !node->IsOnTheTree()) {
            continue;
        }
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        if (!surfaceNode->IsAppWindow()) {
            continue;
        }
        for (auto& child : node->GetSortedChildren()) {
            if (child != nullptr && child->IsInstanceOf<RSSurfaceRenderNode>() && child->IsOnTheTree()) {
                allSurfaceNodeUpdated = false;
                break;
            }
        }
        node->ResetSortedChildren();
    }
    waitingUpdateSurfaceNode_ = !allSurfaceNodeUpdated;
    if (!waitingBufferAvailable_) {
        for (auto& elem : applicationAgentMap_) {
            if (elem.second != nullptr) {
                elem.second->NotifyClearBufferCache();
            }
        }
        if (renderModeChangeCallback_) {
            renderModeChangeCallback_->OnRenderModeChanged(true);
        }
    }
}

void RSMainThread::Render()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        RS_LOGE("RSMainThread::Render GetGlobalRootRenderNode fail");
        return;
    }

    RS_LOGD("RSMainThread::Render isUni:%d", IfUseUniVisitor());
    std::shared_ptr<RSNodeVisitor> visitor;
    if (IfUseUniVisitor()) {
        auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
        uniVisitor->SetAnimateState(doAnimate_);
        visitor = uniVisitor;
    } else {
        bool doParallelComposition = false;
        if (RSInnovation::GetParallelCompositionEnabled()) {
            doParallelComposition = DoParallelComposition(rootNode);
        }
        if (doParallelComposition) {
            renderEngine_->ShrinkCachesIfNeeded();
            return;
        }
        auto rsVisitor = std::make_shared<RSRenderServiceVisitor>();
        rsVisitor->SetAnimateState(doAnimate_);
        visitor = rsVisitor;
    }

    rootNode->Prepare(visitor);
    CalcOcclusion();
    rootNode->Process(visitor);

    renderEngine_->ShrinkCachesIfNeeded();
}

void RSMainThread::CalcOcclusion()
{
    if (doAnimate_ && !useUniVisitor_) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> node = context_.GetGlobalRootRenderNode();
    if (node == nullptr) {
        RS_LOGE("RSMainThread::CalcOcclusion GetGlobalRootRenderNode fail");
        return;
    }
    RSInnovation::UpdateOcclusionCullingSoEnabled();
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    node->CollectSurface(node, curAllSurfaces, isUniRender_);
    // 1. Judge whether it is dirty
    // Surface cnt changed or surface DstRectChanged or surface ZorderChanged
    bool winDirty = lastSurfaceCnt_ != curAllSurfaces.size();
    lastSurfaceCnt_ = curAllSurfaces.size();
    if (!winDirty) {
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (surface == nullptr) {
                continue;
            }
            if (surface->GetRenderProperties().GetZorderChanged() || surface->GetDstRectChanged() ||
                surface->GetAbilityBgAlphaChanged()) {
                winDirty = true;
            }
            surface->GetMutableRenderProperties().CleanZorderChanged();
            surface->CleanDstRectChanged();
            surface->CleanAbilityBgAlphaChanged();
        }
    }
    if (!winDirty) {
        return;
    }

    RS_TRACE_NAME("RSMainThread::CalcOcclusion");
    // 2. Calc occlusion
    Occlusion::Region curRegion;
    VisibleData curVisVec;
    std::map<uint32_t, bool> pidVisMap;
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surface == nullptr || surface->GetDstRect().IsEmpty()) {
            continue;
        }
        Occlusion::Rect rect { surface->GetDstRect().left_, surface->GetDstRect().top_,
            surface->GetDstRect().GetRight(), surface->GetDstRect().GetBottom() };
        Occlusion::Region curSurface { rect };
        // Current surface subtract current region, if result region is empty that means it's covered
        Occlusion::Region subResult = curSurface.Sub(curRegion);
        // Set result to SurfaceRenderNode and its children
        surface->setQosCal(qosPidCal_);
        surface->SetVisibleRegionRecursive(subResult, curVisVec, pidVisMap);
        // Current region need to merge current surface for next calculation(ignore alpha surface)
        const uint8_t opacity = 255;
        if (isUniRender_) {
            if (surface->GetAbilityBgAlpha() == opacity &&
                ROSEN_EQ(surface->GetRenderProperties().GetAlpha(), 1.0f)) {
                curRegion = curSurface.Or(curRegion);
            }
        } else {
            bool diff = surface->GetDstRect().width_ > surface->GetBuffer()->GetWidth() ||
                        surface->GetDstRect().height_ > surface->GetBuffer()->GetHeight();
            if (surface->GetAbilityBgAlpha() == opacity &&
                ROSEN_EQ(surface->GetRenderProperties().GetAlpha(), 1.0f) && !diff) {
                curRegion = curSurface.Or(curRegion);
            }
        }
    }

    // 3. Callback to QOS
    CallbackToQOS(pidVisMap);

    // 4. Callback to WMS
    CallbackToWMS(curVisVec);
}

bool RSMainThread::CheckQosVisChanged(std::map<uint32_t, bool>& pidVisMap)
{
    bool isVisibleChanged  = pidVisMap.size() != lastPidVisMap_.size();
    if (!isVisibleChanged) {
        auto iterCur = pidVisMap.begin();
        auto iterLast = lastPidVisMap_.begin();
        for (; iterCur != pidVisMap.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first ||
                iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                break;
            }
        }
    }

    lastPidVisMap_.clear();
    lastPidVisMap_.insert(pidVisMap.begin(), pidVisMap.end());
    return isVisibleChanged;
}

void RSMainThread::CallbackToQOS(std::map<uint32_t, bool>& pidVisMap)
{
    if (!RSInnovation::UpdateQosVsyncEnabled()) {
        qosPidCal_ = false;
        RSQosThread::GetInstance()->ResetQosPid(pidVisMap);
        return;
    }
    qosPidCal_ = true;
    if (!CheckQosVisChanged(pidVisMap)) {
        return;
    }

    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

void RSMainThread::CallbackToWMS(VisibleData& curVisVec)
{
    // if visible surfaces changed callback to WMS：
    // 1. curVisVec size changed
    // 2. curVisVec content changed
    bool visibleChanged = curVisVec.size() != lastVisVec_.size();
    std::sort(curVisVec.begin(), curVisVec.end());
    if (!visibleChanged) {
        for (uint32_t i = 0; i < curVisVec.size(); i++) {
            if (curVisVec[i] != lastVisVec_[i]) {
                visibleChanged = true;
                break;
            }
        }
    }
    if (visibleChanged) {
        for (auto& listener : occlusionListeners_) {
            listener->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(curVisVec));
        }
    }
    lastVisVec_.clear();
    std::copy(curVisVec.begin(), curVisVec.end(), std::back_inserter(lastVisVec_));
}

void RSMainThread::RequestNextVSync()
{
    RS_TRACE_FUNC();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](uint64_t timestamp, void* data) { OnVsync(timestamp, data); },
    };
    if (receiver_ != nullptr) {
        receiver_->RequestNextVSync(fcb);
    }
}

void RSMainThread::OnVsync(uint64_t timestamp, void* data)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::OnVsync");
    timestamp_ = timestamp;
    if (isUniRender_) {
        MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
        RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
    }
    mainLoop_();
    if (handler_) {
        auto screenManager_ = CreateOrGetScreenManager();
        if (screenManager_ != nullptr) {
            PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        }
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();

    if (context_.animatingNodeList_.empty()) {
        doAnimate_ = false;
        return;
    }

    RS_LOGD("RSMainThread::Animate start, processing %d animating nodes", context_.animatingNodeList_.size());

    doAnimate_ = true;
    // iterate and animate all animating nodes, remove if animation finished
    std::__libcpp_erase_if_container(context_.animatingNodeList_, [timestamp](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            RS_LOGD("RSMainThread::Animate removing expired animating node");
            return true;
        }
        bool animationFinished = !node->Animate(timestamp);
        if (animationFinished) {
            RS_LOGD("RSMainThread::Animate removing finished animating node %" PRIu64, node->GetId());
        }
        return animationFinished;
    });

    RS_LOGD("RSMainThread::Animate end, %d animating nodes remains", context_.animatingNodeList_.size());

    RequestNextVSync();
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData) {
        return;
    }
    if (rsTransactionData->GetUniRender()) {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
    } else {
        ClassifyRSTransactionData(rsTransactionData);
    }
    RequestNextVSync();
}

void RSMainThread::ClassifyRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    const auto& nodeMap = context_.GetNodeMap();
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        std::unique_ptr<RSTransactionData> transactionData(std::move(rsTransactionData));
        auto timestamp = transactionData->GetTimestamp();
        RS_LOGD("RSMainThread::RecvRSTransactionData timestamp = %" PRIu64, timestamp);
        for (auto& [nodeId, followType, command] : transactionData->GetPayload()) {
            if (nodeId == 0 || followType == FollowType::NONE) {
                pendingEffectiveCommands_.emplace_back(std::move(command));
                continue;
            }
            auto node = nodeMap.GetRenderNode(nodeId);
            if (waitingBufferAvailable_ && node && followType == FollowType::FOLLOW_VISITOR) {
                followVisitorCommands_.emplace_back(std::move(command));
                continue;
            }
            if (node && followType == FollowType::FOLLOW_TO_PARENT) {
                auto parentNode = node->GetParent().lock();
                if (parentNode) {
                    nodeId = parentNode->GetId();
                } else {
                    pendingEffectiveCommands_.emplace_back(std::move(command));
                    continue;
                }
            }
            cachedCommands_[nodeId][timestamp].emplace_back(std::move(command));
        }
    }
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSMainThread::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    applicationAgentMap_.emplace(pid, app);
}

void RSMainThread::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    std::__libcpp_erase_if_container(applicationAgentMap_, [&app](auto& iter) { return iter.second == app; });
}

void RSMainThread::NotifyRenderModeChanged(bool useUniVisitor)
{
    if (RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_DYNAMIC_SWITCH) {
        return;
    }
    if (useUniVisitor == useUniVisitor_) {
        RS_LOGI("RSMainThread::NotifyRenderModeChanged useUniVisitor_:%d, not changed", useUniVisitor_.load());
        return;
    }
    PostTask([useUniVisitor = useUniVisitor, this]() {
        if (waitingBufferAvailable_ || waitingUpdateSurfaceNode_) {
            RS_LOGE("RSMainThread::NotifyRenderModeChanged last update mode not finished, switch again");
        }
        useUniVisitor_ = useUniVisitor;
        waitingBufferAvailable_ = !useUniVisitor;
        waitingUpdateSurfaceNode_ = useUniVisitor;
        for (auto& elem : applicationAgentMap_) {
            if (elem.second != nullptr) {
                elem.second->OnRenderModeChanged(!useUniVisitor);
            }
        }
    });
}

bool RSMainThread::QueryIfUseUniVisitor() const
{
    RS_LOGI("RSMainThread::QueryIfUseUniVisitor useUniVisitor_:%d", useUniVisitor_.load());
    return useUniVisitor_;
}

void RSMainThread::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    occlusionListeners_.emplace_back(callback);
}

void RSMainThread::UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    auto iter = std::find(occlusionListeners_.begin(), occlusionListeners_.end(), callback);
    if (iter != occlusionListeners_.end()) {
        occlusionListeners_.erase(iter);
    }
}

void RSMainThread::CleanOcclusionListener()
{
    occlusionListeners_.clear();
}

void RSMainThread::SetRenderModeChangeCallback(sptr<RSIRenderModeChangeCallback> callback)
{
    renderModeChangeCallback_ = callback;
}

void RSMainThread::SendCommands()
{
    RS_TRACE_FUNC();
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, RSTransactionData>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    PostTask([this, transactionMapPtr]() {
        for (auto& transactionIter : *transactionMapPtr) {
            auto pid = transactionIter.first;
            auto appIter = applicationAgentMap_.find(pid);
            if (appIter == applicationAgentMap_.end()) {
                RS_LOGW(
                    "RSMainThread::SendCommand no application agent registered as pid %d, this will cause memory leak!",
                    pid);
                continue;
            }
            auto& app = appIter->second;
            auto transactionPtr = std::make_shared<RSTransactionData>(std::move(transactionIter.second));
            app->OnTransaction(transactionPtr);
        }
    });
}

void RSMainThread::RenderServiceTreeDump(std::string& dumpString)
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        dumpString.append("rootNode is null\n");
        return;
    }
    rootNode->DumpTree(0, dumpString);
}

bool RSMainThread::DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    using CreateParallelSyncSignalFunc = void* (*)(uint32_t);
    using SignalCountDownFunc = void (*)(void*);
    using SignalAwaitFunc = void (*)(void*);
    using AssignTaskFunc = void (*)(std::function<void()>);
    using RemoveStoppedThreadsFunc = void (*)();

    auto CreateParallelSyncSignal = (CreateParallelSyncSignalFunc)RSInnovation::_s_createParallelSyncSignal;
    auto SignalCountDown = (SignalCountDownFunc)RSInnovation::_s_signalCountDown;
    auto SignalAwait = (SignalAwaitFunc)RSInnovation::_s_signalAwait;
    auto AssignTask = (AssignTaskFunc)RSInnovation::_s_assignTask;
    auto RemoveStoppedThreads = (RemoveStoppedThreadsFunc)RSInnovation::_s_removeStoppedThreads;

    void* syncSignal = (*CreateParallelSyncSignal)(rootNode->GetChildrenCount());
    if (!syncSignal) {
        return false;
    }

    (*RemoveStoppedThreads)();
    std::shared_ptr<OHOS::Rosen::RSNodeVisitor> visitor = std::make_shared<RSRenderServiceVisitor>();
    rootNode->Prepare(visitor);
    CalcOcclusion();
    auto children = rootNode->GetSortedChildren();
    bool animate_ = doAnimate_;
    for (auto it = children.rbegin(); it != children.rend(); it++) {
        auto child = *it;
        auto task = [&syncSignal, SignalCountDown, child, animate_]() {
            std::shared_ptr<RSNodeVisitor> visitor;
            auto rsVisitor = std::make_shared<RSRenderServiceVisitor>(true);
            rsVisitor->SetAnimateState(animate_);
            visitor = rsVisitor;
            child->Process(visitor);
            (*SignalCountDown)(syncSignal);
        };
        if (*it == *children.begin()) {
            task();
        } else {
            (*AssignTask)(task);
        }
    }
    (*SignalAwait)(syncSignal);
    ResetSortedChildren(rootNode);
    return true;
}

void RSMainThread::ResetSortedChildren(std::shared_ptr<RSBaseRenderNode> node)
{
    for (auto& child : node->GetSortedChildren()) {
        ResetSortedChildren(child);
    }
    node->ResetSortedChildren();
}

void RSMainThread::ClearTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0 &&
        !effectiveTransactionDataIndexMap_[remotePid].second.empty()) {
        RS_LOGD("RSMainThread::ClearTransactionDataPidInfo process:%d destroyed, skip commands", remotePid);
    }
    effectiveTransactionDataIndexMap_.erase(remotePid);
}

void RSMainThread::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0) {
        RS_LOGW("RSMainThread::AddTransactionDataPidInfo remotePid:%d already exists", remotePid);
    }
    effectiveTransactionDataIndexMap_[remotePid].first = 0;
}
} // namespace Rosen
} // namespace OHOS
