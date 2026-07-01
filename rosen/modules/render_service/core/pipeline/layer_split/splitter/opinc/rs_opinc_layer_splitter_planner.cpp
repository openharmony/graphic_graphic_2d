/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_opinc_layer_splitter_planner.h"

#include "system/rs_system_parameters.h"

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {

namespace {

constexpr uint32_t BUFFER_WIDTH_EXPAND = 2;
constexpr float BUFFER_HEIGHT_RATIO = 1.4f;
constexpr float BUFFER_TOP_EXPAND_RATIO = 0.2f;
constexpr int BUFFER_LEFT_INITIAL = -1;
} // namespace

RSOpincLayerSplitterPlanner::RSOpincLayerSplitterPlanner()
{
    requestController_ = std::make_shared<RequestController>();
}

RSOpincLayerSplitterPlanner::~RSOpincLayerSplitterPlanner()
{
    ClearAllChildrenLayerObjects();
}

void RSOpincLayerSplitterPlanner::Reset()
{
    if (splitSurface_ == nullptr) {
        return;
    }

    if (!opIncNodes_.empty()) {
        std::swap(opIncNodes_, lastOpIncNodes_);
    }
    opIncNodes_.clear();
    lastOpIncNodeIds_.clear();
    for (const auto& pair : lastOpIncNodes_) {
        lastOpIncNodeIds_.insert(pair.first);
    }
    currentOffset_ = Vector2f();
    isUpdateBuffer_ = false;
    visitedNodeId_.clear();
    canDoDirectComposition_ = false;

    if (planStatus_ != PlanStatus::ON || UNLIKELY(requestController_ == nullptr)) {
        return;
    }
    requestController_->IncStayOnCount();
    auto surfaceHandler = splitSurface_->splitSurfaceBuffer_->GetSurfaceHandler();
    if (!surfaceHandler) {
        return;
    }
    surfaceHandler->SetCurrentFrameBufferConsumed();
}

bool RSOpincLayerSplitterPlanner::CheckDoDirectCompositionWithSplitLayer(
    std::shared_ptr<TransactionDataMap> transactionDataEffective)
{
    if (planStatus_ != PlanStatus::ON) {
        return false;
    }

    auto checkTransactionFunc = [this](const auto& rsTransaction) -> bool {
        for (auto& [nodeId, followType, command] : rsTransaction->GetPayload()) {
            if (command == nullptr) {
                continue;
            }
            if (!CheckOpIncNodeFromCommand(command->GetNodeId())) {
                return false;
            }
        }
        return true;
    };

    for (auto& rsTransactionElem : *transactionDataEffective) {
        for (auto& rsTransaction : rsTransactionElem.second) {
            if (!rsTransaction) {
                continue;
            }
            if (!checkTransactionFunc(rsTransaction)) {
                return false;
            }
        }
    }
    canDoDirectComposition_ = CheckCanDoDirectComposition();
    return canDoDirectComposition_;
}

bool RSOpincLayerSplitterPlanner::CheckOpIncNodeFromCommand(NodeId nodeId)
{
    if (visitedNodeId_.find(nodeId) != visitedNodeId_.end()) {
        return true;
    }

    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    if (node == nullptr || !node->IsOnTheTree()) {
        return true;
    }

    auto parent = node->GetParent().lock();
    if (!parent || !opIncParentNode_ || parent->GetId() != opIncParentNode_->GetId()) {
        LAYER_SPLIT_LOGD("%{public}s parent->GetId() != opIncParentNode_->GetId() nodeId=%{public}" PRIu64,
            __func__, nodeId);
        return false;
    }

    if (lastOpIncNodeIds_.find(nodeId) == lastOpIncNodeIds_.end()) {
        LAYER_SPLIT_LOGD("%{public}s lastOpIncNodeIds_.find() == end() nodeId=%{public}" PRIu64, __func__, nodeId);
        return false;
    }

    auto dirtyTypes = node->GetDirtyTypes();
    if (dirtyTypes.count() > size_t(dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS))) +
                                 size_t(dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::FRAME)))) {
        LAYER_SPLIT_LOGD("%{public}s RSModifierType Dirty", __func__);
        return false;
    }

    LAYER_SPLIT_LOGD("%{public}s nodeId=%{public}" PRIu64, __func__, nodeId);
    visitedNodeId_.insert(nodeId);
    return true;
}

std::pair<bool, Vector4f> RSOpincLayerSplitterPlanner::GetBoundsFromModifier(const std::shared_ptr<RSRenderNode>& node)
{
    Vector4f result;
    if (node == nullptr) {
        return { false, result };
    }

    auto boundsModifiers = node->GetModifiersNG(ModifierNG::RSModifierType::BOUNDS);
    for (auto& modifier : boundsModifiers) {
        if (modifier == nullptr) {
            continue;
        }
        auto prop = modifier->GetProperty(ModifierNG::RSPropertyType::BOUNDS);
        if (prop == nullptr) {
            continue;
        }
        auto boundsProp = std::static_pointer_cast<RSRenderProperty<Vector4f>>(prop);
        Vector4f bounds = boundsProp->GetRef();
        return { true, bounds };
    }
    return { false, result };
}

bool RSOpincLayerSplitterPlanner::CheckCanDoDirectComposition()
{
    if (visitedNodeId_.empty() || visitedNodeId_.size() != lastOpIncNodes_.size()) {
        LAYER_SPLIT_LOGD("%{public}s visitedNodeId_.size() != lastOpIncNodes_.size()", __func__);
        return false;
    }

    bool isFirst = true;
    for (auto& [nodeId, position] : lastOpIncNodes_) {
        auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        auto [isSucc, bounds] = GetBoundsFromModifier(node);
        if (!isSucc) {
            LAYER_SPLIT_LOGD("%{public}s !isSucc", __func__);
            return false;
        }
        if (ROSEN_NE(bounds.z_, position.z_) || ROSEN_NE(bounds.w_, position.w_)) {
            return false;
        }
        if (isFirst) {
            currentOffset_.x_ = bounds.x_ - position.x_;
            currentOffset_.y_ = bounds.y_ - position.y_;
            isFirst = false;
        } else {
            auto nodeOffset = Vector2f(bounds.x_ - position.x_, bounds.y_ - position.y_);
            if (!Vector2fNearEqual(currentOffset_, nodeOffset)) {
                LAYER_SPLIT_LOGD("%{public}s !eq(currentOffset_, nodeOffset)", __func__);
                return false;
            }
        }
        opIncNodes_.emplace_back(nodeId, bounds);
    }

    bool isBufferBoundsOut = UpdateBufferBounds();
    if (isBufferBoundsOut) {
        return false;
    }

    LAYER_SPLIT_LOGD("%{public}s DoComposition", __func__);
    return true;
}

void RSOpincLayerSplitterPlanner::MoveSplitSurfaceNode()
{
    if (splitSurface_ == nullptr || splitSurface_->splitSurfaceNode_ == nullptr) {
        return;
    }

    if (splitSurface_->IsBufferConsumed()) {
        auto parent = splitSurface_->splitSurfaceNode_->GetParent().lock();
        if (!parent) {
            return;
        }

        auto childrenPtr = parent->GetSortedChildren();
        if (!childrenPtr) {
            return;
        }

        auto children = *childrenPtr;
        if (!children.empty() && children.front() != splitSurface_->splitSurfaceNode_) {
            parent->MoveChild(splitSurface_->splitSurfaceNode_, 0);
        }
    }
}

void RSOpincLayerSplitterPlanner::InitSplitSurface(const ScreenInfo& screenInfo)
{
    if (splitSurface_ == nullptr) {
        screenWidth_ = screenInfo.width;
        screenHeight_ = screenInfo.height;
        splitSurface_ = std::make_shared<SplitSurface>(screenWidth_, screenHeight_);
    }
}

bool RSOpincLayerSplitterPlanner::CollectOpIncNodes()
{
    opIncNodes_.clear();
    bool isOverlap = false;
    std::vector<RectI> nonOpIncNodeRects;
    itemBounds_.Clear();
    // 是否需要对 opIncParentNode_加入空指针判断
    auto& children = *(opIncParentNode_->GetSortedChildren());
    for (auto& child : children) {
        if (!child || child == splitSurface_->splitSurfaceNode_) {
            continue;
        }
        auto& geoPtr = child->GetRenderProperties().GetBoundsGeometry();
        if (!geoPtr) {
            continue;
        }
        auto rect = geoPtr->GetAbsRect();
        itemBounds_ = itemBounds_.JoinRect(rect);
        rect = rect.JoinRect(geoPtr->MapAbsRect(child->GetChildrenRect().ConvertTo<float>()));
        if (child->GetOpincRootCache().OpincGetRootFlag()) {
            auto boundsPosition = child->GetRenderProperties().GetBoundsPosition();
            auto boundsSize = child->GetRenderProperties().GetBoundsSize();
            opIncNodes_.emplace_back(child->GetId(),
                Vector4f(boundsPosition[0], boundsPosition[1], boundsSize[0], boundsSize[1]));
            if (!isOverlap && CheckRectOverlap(rect, nonOpIncNodeRects)) {
                isOverlap = true;
                LAYER_SPLIT_LOGD("%{public}s isOverLap nodeId=%{public}" PRIu64, __func__, child->GetId());
            }
        } else {
            nonOpIncNodeRects.emplace_back(rect);
        }
    }
    return isOverlap;
}

bool RSOpincLayerSplitterPlanner::CheckRectOverlap(const RectI& rect, const std::vector<RectI>& nonOpIncNodeRects) const
{
    for (auto& nonOpIncNodeRect : nonOpIncNodeRects) {
        if (rect.GetLeft() < nonOpIncNodeRect.GetRight() && nonOpIncNodeRect.GetLeft() < rect.GetRight() &&
            rect.GetTop() < nonOpIncNodeRect.GetBottom() && nonOpIncNodeRect.GetTop() < rect.GetBottom()) {
            return true;
        }
    }
    return false;
}

bool RSOpincLayerSplitterPlanner::IsOpIncNodesChanged()
{
    if (lastOpIncNodes_.empty()) {
        LAYER_SPLIT_LOGD("%{public}s lastOpIncNodes_.empty()", __func__);
        return true;
    }

    size_t opIncNodesSize = opIncNodes_.size();
    if (opIncNodesSize != lastOpIncNodes_.size()) {
        LAYER_SPLIT_LOGD("%{public}s opIncNodesSize != lastOpIncNodes_.size()", __func__);
        return true;
    }

    currentOffset_[0] = opIncNodes_[0].second[0] - lastOpIncNodes_[0].second[0];
    currentOffset_[1] = opIncNodes_[0].second[1] - lastOpIncNodes_[0].second[1];
    for (size_t i = 0; i < opIncNodesSize; ++i) {
        if (opIncNodes_[i].first != lastOpIncNodes_[i].first) {
            LAYER_SPLIT_LOGD("%{public}s opIncNodes_[i]->GetId() != lastOpIncNodes_[i]->GetId()", __func__);
            return true;
        }
        auto nodeOffset = Vector2f(opIncNodes_[i].second[0] - lastOpIncNodes_[i].second[0],
            opIncNodes_[i].second[1] - lastOpIncNodes_[i].second[1]);
        LAYER_SPLIT_LOGD("%{public}s currentOffset_=[%f, %f], nodeOffset=[%f, %f]", __func__, currentOffset_[0],
            currentOffset_[1], nodeOffset[0], nodeOffset[1]);
        if (!Vector2fNearEqual(currentOffset_, nodeOffset)) {
            LAYER_SPLIT_LOGD("%{public}s no common offset", __func__);
            return true;
        }
    }
    return false;
}

void RSOpincLayerSplitterPlanner::SetSrcAndDstRect()
{
    if (splitSurface_ == nullptr || splitSurface_->splitSurfaceNode_ == nullptr) {
        return;
    }

    splitSurface_->splitSurfaceNode_->SetSrcRect(RectI(srcRect_.GetLeft(), srcRect_.GetTop(),
        srcRect_.GetWidth(), srcRect_.GetHeight()));
    splitSurface_->splitSurfaceNode_->SetDstRect(RectI(dstRect_.GetLeft(), dstRect_.GetTop(),
        dstRect_.GetWidth(), dstRect_.GetHeight()));

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(
        splitSurface_->splitSurfaceNode_->GetStagingRenderParams().get());
    if (!surfaceParams) {
        return;
    }
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.srcRect = { srcRect_.left_, srcRect_.top_, srcRect_.width_, srcRect_.height_ };
    layerInfo.dstRect = { dstRect_.left_, dstRect_.top_, dstRect_.width_, dstRect_.height_ };
    auto geoPtr = splitSurface_->splitSurfaceNode_->GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return;
    }
    auto matrix = geoPtr->GetAbsMatrix();
    float scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
    float scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
    if (ROSEN_EQ(scaleX, 0.0f) || ROSEN_EQ(scaleY, 0.0f)) {
        return;
    }
    layerInfo.boundRect = { 0, 0, dstRect_.width_ / scaleX, dstRect_.height_ / scaleY };
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->SetSplitLayerTag(true);
    LAYER_SPLIT_LOGD("%{public}s srcRect_=[%d, %d, %d, %d]", __func__, layerInfo.srcRect.x, layerInfo.srcRect.y,
        layerInfo.srcRect.w, layerInfo.srcRect.h);

    auto surfacHandler = splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler();
    if (!surfacHandler) {
        return;
    }
    sptr<SurfaceBuffer> buffer = surfacHandler->GetBuffer();
    if (buffer) {
        buffer->SetCropMetadata({srcRect_.left_, srcRect_.top_, srcRect_.width_, srcRect_.height_});
    }
}

bool RSOpincLayerSplitterPlanner::UpdateBufferBounds()
{
    auto geoPtr = opIncParentNode_->GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return true;
    }
    auto absRect = geoPtr->GetAbsRect();
    dstRect_ = RectF(absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight());
    if (planStatus_ == PlanStatus::OFF) {
        bufferBounds_ = RectI(BUFFER_LEFT_INITIAL, static_cast<int>(screenHeight_) * -BUFFER_TOP_EXPAND_RATIO,
            static_cast<int>(screenWidth_) + BUFFER_WIDTH_EXPAND,
            static_cast<int>(screenHeight_ * BUFFER_HEIGHT_RATIO));
        if (bufferBounds_.GetBottom() > itemBounds_.GetBottom()) {
            bufferBounds_.Move(0, itemBounds_.GetBottom() - bufferBounds_.GetBottom());
        }
        if (bufferBounds_.GetTop() < itemBounds_.GetTop()) {
            bufferBounds_.Move(0, itemBounds_.GetTop() - bufferBounds_.GetTop());
        }
        srcRect_ = RectF(dstRect_.GetLeft() - bufferBounds_.GetLeft(), dstRect_.GetTop() - bufferBounds_.GetTop(),
            dstRect_.GetWidth(), dstRect_.GetHeight());
    } else if (planStatus_ == PlanStatus::PREPARE || planStatus_ == PlanStatus::ON) {
        if (!isUpdateBuffer_) {
            auto geoMatrix = opIncParentNode_->GetRenderProperties().GetBoundsGeometry();
            if (!geoMatrix) {
                return true;
            }
            auto matrix = geoMatrix->GetAbsMatrix();
            float scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
            float scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
            LAYER_SPLIT_LOGD("%{public}s scale=[%f, %f]", __func__, scaleX, scaleY);
            srcRect_.Move(std::round(-currentOffset_[0] * scaleX), std::round(-currentOffset_[1] * scaleY));
            if (currentOffset_ != Vector2f(0, 0)) {
                isUpdateBuffer_ = true;
            }
        }
    }
    LAYER_SPLIT_LOGD("%{public}s srcRect_=%s, dstRect_=%s, bufferBounds=%s", __func__, srcRect_.ToString().c_str(),
        dstRect_.ToString().c_str(), bufferBounds_.ToString().c_str());

    if (srcRect_.GetTop() < 0 || srcRect_.GetBottom() > splitSurface_->bufferHeight_ || srcRect_.GetLeft() < 0 ||
        srcRect_.GetRight() > splitSurface_->bufferWidth_) {
        LAYER_SPLIT_LOGD("%{public}s srcRect_ outside", __func__);
        return true;
    }
    SetSrcAndDstRect();
    return false;
}

void RSOpincLayerSplitterPlanner::CheckNeedLeave()
{
    if (splitSurface_ == nullptr || opIncParentNode_ == nullptr) {
        LAYER_SPLIT_LOGD("%{public}s splitSurface_ or opIncParentNode_ is nullptr", __func__);
        needLeave_ = true;
        return;
    }

    auto instanceRootNode = opIncParentNode_->GetInstanceRootNode();
    if (instanceRootNode) {
        auto windowSceneNode =
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(instanceRootNode->GetParent().lock());
        if (windowSceneNode && windowSceneNode->GetLastFrameUifirstCacheType() != MultiThreadCacheType::NONE) {
            LAYER_SPLIT_LOGD("%{public}s UIFirst enabled", __func__);
            needLeave_ = true;
            return;
        }
    }

    auto geoPtr = opIncParentNode_->GetRenderProperties().GetBoundsGeometry();
    if (geoPtr) {
        auto matrix = geoPtr->GetAbsMatrix();
        float skewX = matrix.Get(Drawing::Matrix::SKEW_X);
        float skewY = matrix.Get(Drawing::Matrix::SKEW_Y);
        float persp0 = matrix.Get(Drawing::Matrix::PERSP_0);
        float persp1 = matrix.Get(Drawing::Matrix::PERSP_1);
        float persp2 = matrix.Get(Drawing::Matrix::PERSP_2);
        if (ROSEN_NE(skewX, 0.0f) || ROSEN_NE(skewY, 0.0f) || ROSEN_NE(persp0, 0.0f) || ROSEN_NE(persp1, 0.0f) ||
            ROSEN_NE(persp2, 1.0f)) {
            needLeave_ = true;
            return;
        }
    }

    bool isOverlap = CollectOpIncNodes();
    if (isOverlap) {
        needLeave_ = true;
        return;
    }
    bool isOpIncNodesChanged = IsOpIncNodesChanged();
    if (isOpIncNodesChanged) {
        needLeave_ = true;
        return;
    }
    bool isBufferBoundsOut = UpdateBufferBounds();
    if (isBufferBoundsOut) {
        needLeave_ = true;
        return;
    }
    needLeave_ = false;
}

void RSOpincLayerSplitterPlanner::SetColorSpaceInfo()
{
    if (!opIncParentNode_ || !splitSurface_ || !splitSurface_->splitSurfaceNode_) {
        return;
    }
    GraphicColorGamut parentNodeColorSpace = opIncParentNode_->GetNodeColorSpace();
    if (splitSurface_->GetColorSpace() != parentNodeColorSpace) {
        return;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    static const std::map<GraphicColorGamut, CM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT601, CM_BT601_EBU_FULL },
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT709, CM_BT709_FULL },
        { GRAPHIC_COLOR_GAMUT_SRGB, CM_SRGB_FULL },
        { GRAPHIC_COLOR_GAMUT_ADOBE_RGB, CM_ADOBERGB_FULL },
        { GRAPHIC_COLOR_GAMUT_DISPLAY_P3, CM_P3_FULL },
    };
    CM_ColorSpaceType colorSpaceType = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.at(parentNodeColorSpace);
    CM_ColorSpaceInfo colorSpaceInfo;
    MetadataHelper::ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    // request的色域和meta的色域要一致
    splitSurface_->SetColorSpace(parentNodeColorSpace);
    auto rssurfacHandler = splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler();
    if (!rssurfacHandler) {
        return;
    }
    sptr<SurfaceBuffer> buffer = rssurfacHandler->GetBuffer();
    GSError ret = MetadataHelper::SetColorSpaceInfo(buffer, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        LAYER_SPLIT_LOGD("etMetadata SetColorSpaceInfo failed %{public}d", ret);
    }
}

void RSOpincLayerSplitterPlanner::CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (splitSurface_ == nullptr || splitSurface_->splitSurfaceNode_ == nullptr) {
        return;
    }
    if (hwcNode == splitSurface_->splitSurfaceNode_) {
        needLeave_ = true;
        return;
    }
}

void RSOpincLayerSplitterPlanner::UpdateSplitPlan()
{
    if (splitSurface_ == nullptr) {
        return;
    }

    bool isHardwareEnabled = false;
    if (splitSurface_->IsBufferConsumed()) {
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(
            splitSurface_->splitSurfaceNode_->GetRenderParams().get());
        if (surfaceParams && surfaceParams->GetHardwareEnabled()) {
            isHardwareEnabled = true;
        }
    }

    switch (planStatus_) {
        case PlanStatus::OFF:
            if (!needLeave_ && splitSurface_->IsBufferConsumed()) {
                planStatus_ = PlanStatus::PREPARE;
            }
            break;
        case PlanStatus::PREPARE:
            if (needLeave_) {
                planStatus_ = PlanStatus::LEAVE;
            } else {
                planStatus_ = PlanStatus::ON;
            }
            break;
        case PlanStatus::ON:
            if (needLeave_) {
                planStatus_ = PlanStatus::LEAVE;
            }
            break;
        case PlanStatus::LEAVE:
            planStatus_ = PlanStatus::OFF;
        default:
            return;
    }

    requestController_->Update(needLeave_, planStatus_, canDoDirectComposition_, isHardwareEnabled);

    LAYER_SPLIT_LOGD("%{public}s planStatus_=%d, needLeave=%d", __func__, static_cast<int>(planStatus_), needLeave_);
    RS_TRACE_NAME_FMT("%s planStatus_=%d, needLeave=%d", __func__, static_cast<int>(planStatus_), needLeave_);
    ProcessPlanStatusAction();
}

void RSOpincLayerSplitterPlanner::ProcessPlanStatusAction()
{
    if (planStatus_ == PlanStatus::OFF) {
        if (!needLeave_) {
            splitSurface_->Init(opIncParentNode_);
            auto parent = splitSurface_->splitSurfaceNode_->GetParent().lock();
            if (parent == nullptr) {
                opIncParentNode_->AddChild(splitSurface_->splitSurfaceNode_, 0);
            }
            SetColorSpaceInfo();
        }

        if (requestController_->IsLongTermOff() && !splitSurface_->CheckParentNodeOnTheTree()) {
            UnregisterSplitSurfaceNode();
        }
    }
    if (needLeave_ && splitSurface_->IsBufferConsumed()) {
        splitSurface_->SetBufferNull();
    }
}

void RSOpincLayerSplitterPlanner::UpdateScreenDirtyRegion(std::shared_ptr<RSDirtyRegionManager> curScreenDirtyManager)
{
    if (planStatus_ == PlanStatus::LEAVE) {
        if (curScreenDirtyManager != nullptr) {
            curScreenDirtyManager->MergeDirtyRect(RectI(dstRect_.GetLeft(), dstRect_.GetTop(),
                dstRect_.GetWidth(), dstRect_.GetHeight()));
        }
    }
}

SurfaceStatus RSOpincLayerSplitterPlanner::GetSurfaceStatus()
{
    if (splitSurface_) {
        return splitSurface_->surfaceStatus_;
    }
    return SurfaceStatus::UNKNOWN;
}

void RSOpincLayerSplitterPlanner::UnregisterSplitSurfaceNode()
{
    if (splitSurface_) {
        splitSurface_->UnregisterSplitSurfaceNode();
    }
    if (LIKELY(requestController_)) {
        requestController_->Reset();
    }
}

void RSOpincLayerSplitterPlanner::ClearAllChildrenLayerObjects()
{
    auto clearFunc = [](const std::shared_ptr<RSRenderNode>& node) {
        auto drawable = node->GetRenderDrawable();
        if (drawable) {
            drawable->SetLayerSplitterProcessor(nullptr);
        }
    };

    for (const auto& node : currFrameChildSet_) clearFunc(node);
    for (const auto& node : lastFrameChildSet_) clearFunc(node);
    if (splitSurface_ && splitSurface_->splitSurfaceDrawable_) {
        splitSurface_->splitSurfaceDrawable_->SetLayerSplitterProcessor(nullptr);
    }
}

void RSOpincLayerSplitterPlanner::Sync(std::shared_ptr<RSLayerSplitterProcessor> processor_)
{
    if (splitSurface_ && splitSurface_->splitSurfaceDrawable_) {
        splitSurface_->splitSurfaceDrawable_->SetLayerSplitterProcessor(processor_);
    }
    for (const auto& node : currFrameChildSet_) {
        if (lastFrameChildSet_.count(node) == 0) {
            if (auto drawable = node->GetRenderDrawable()) {
                drawable->SetLayerSplitterProcessor(processor_);
            }
        }
    }
    for (const auto& node : lastFrameChildSet_) {
        if (currFrameChildSet_.count(node) == 0) {
            if (auto drawable = node->GetRenderDrawable()) {
                drawable->SetLayerSplitterProcessor(nullptr);
            }
        }
    }
}

void RSOpincLayerSplitterPlanner::UpdateChildren(const std::shared_ptr<RSRenderNode>& parent)
{
    if (!parent) {
        return;
    }
    std::unordered_set<std::shared_ptr<RSRenderNode>> childSet;
    const auto& children = parent->GetSortedChildren();
    if (!children) {
        return;
    }
    for (const auto& child : *children) {
        if (child && child->GetOpincRootCache().OpincGetRootFlag()) {
            childSet.insert(child);
        }
    }
    lastFrameChildSet_ = std::move(currFrameChildSet_);
    currFrameChildSet_ = std::move(childSet);
}

std::string RSOpincLayerSplitterPlanner::GetDfxString()
{
    std::ostringstream oss;
    oss << ", Cur[" << currFrameChildSet_.size() << "]:";
    for (const auto& node : currFrameChildSet_) {
        oss << " " << node->GetId();
    }

    oss << ", Last[" << lastFrameChildSet_.size() << "]:";
    for (const auto& node : lastFrameChildSet_) {
        oss << " " << node->GetId();
    }
    return oss.str();
}

void RSOpincLayerSplitterPlanner::SetOpIncParentNode(const std::shared_ptr<RSRenderNode>& parentNode)
{
    opIncParentNode_ = parentNode;
}

} // namespace OHOS::Rosen