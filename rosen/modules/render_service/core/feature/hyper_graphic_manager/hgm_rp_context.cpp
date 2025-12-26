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

#include "feature/hyper_graphic_manager/hgm_rp_context.h"

#include "hgm_frame_rate_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"
#include "rp_hgm_xml_parser.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* HGM_CONFIG_PATH = "/sys_prod/etc/graphic/hgm_policy_config.xml";
}

HgmRPContext::HgmRPContext(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection)
    : renderToServiceConnection_(renderToServiceConnection)
{
    hgmRPEnergy_ = std::make_shared<HgmRPEnergy>();
    convertFrameRateFunc_ = [this](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t {
        return rpFrameRatePolicy_.GetExpectedFrameRate(unit, velocity, area, length);
    };
}

int32_t HgmRPContext::InitHgmConfig(std::unordered_map<std::string, std::string>& sourceTuningConfig,
    std::unordered_map<std::string, std::string>& solidLayerConfig, std::vector<std::string>& appBufferList)
{
    auto parser = std::make_unique<RPHgmXMLParser>();
    if (parser->LoadConfiguration(HGM_CONFIG_PATH) != EXEC_SUCCESS) {
        HGM_LOGW("HgmRPContext failed to load hgm xml configuration file");
        return XML_FILE_LOAD_FAIL;
    }
    sourceTuningConfig = parser->GetSourceTuningConfig();
    solidLayerConfig = parser->GetSolidLayerConfig();
    appBufferList = parser->GetAppBufferList();

    return EXEC_SUCCESS;
}

void HgmRPContext::NotifyRpHgmFrameRate(uint64_t vsyncId, const std::shared_ptr<RSContext>& rsContext,
    const std::unordered_map<NodeId, int>& vRateMap, bool isNeedRefreshVRate, PipelineParam& pipelineParam)
{
    int changed = 0;
    if (bool enable = RSSystemParameters::GetShowRefreshRateEnabled(&changed); changed != 0) {
        RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enable, 1);
    }

    HandleGameNode(rsContext->GetNodeMap());
    sptr<HgmProcessToServiceInfo> info = sptr<HgmProcessToServiceInfo>::MakeSptr();
    info->isGameNodeOnTree = isGameNodeOnTree_;
    info->rsCurrRange = rsCurrRange_;
    rsCurrRange_.Reset();
    info->surfaceData = std::move(surfaceData_);
    surfaceData_.clear();
    info->frameRateLinkerDestroyIds = std::move(rsContext->GetMutableFrameRateLinkerDestroyIds());
    info->frameRateLinkerUpdateInfoMap = std::move(rsContext->GetMutableFrameRateLinkerUpdateInfoMap());
    rsContext->ClearFrameRateLinker();
    info->uiFrameworkDirtyNodeNameMap = rsContext->GetUIFrameworkDirtyNodeNameMap();
    info->energyCommonData = hgmRPEnergy_->GetEnergyCommonData();
    info->vRateMap = vRateMap;
    info->isNeedRefreshVRate = isNeedRefreshVRate;
    auto serviceToProcessInfo =
        renderToServiceConnection_->NotifyRpHgmFrameRate(pipelineParam.frameTimestamp, vsyncId, info);
    SetServiceToProcessInfo(serviceToProcessInfo,
        &pipelineParam.pendingScreenRefreshRate, &pipelineParam.pendingConstraintRelativeTime);
    hgmRPEnergy_->ClearEnergyCommonData();
}

void HgmRPContext::AddScreenId(ScreenId screenId)
{
    RS_LOGI("%{public}s: screenId[%{public}" PRIu64 "]", __func__, screenId);
    screenIds_.insert(screenId);
}

void HgmRPContext::RemoveScreenId(ScreenId screenId)
{
    RS_LOGI("%{public}s: screenId[%{public}" PRIu64 "]", __func__, screenId);
    screenIds_.erase(screenId);
}

void HgmRPContext::HandleGameNode(const RSRenderNodeMap& nodeMap)
{
    if (isAdaptive_ != SupportASStatus::SUPPORT_AS) {
        isGameNodeOnTree_ = false;
        return;
    }
    bool isGameSelfNodeOnTree = false;
    bool isOtherSelfNodeOnTree = false;
    nodeMap.TraverseSurfaceNodes(
        [this, &isGameSelfNodeOnTree, &isOtherSelfNodeOnTree, &nodeMap]
        (const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        if (surfaceNode->IsOnTheTree() &&
            surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
            auto appNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
                nodeMap.GetRenderNode(surfaceNode->GetInstanceRootNodeId()));
            if (gameNodeName_ == surfaceNode->GetName()) {
                isGameSelfNodeOnTree = true;
            } else if (!appNode || !appNode->GetVisibleRegion().IsEmpty()) {
                isOtherSelfNodeOnTree = true;
            }
        }
    });
    RS_TRACE_NAME_FMT("HgmRPContext::HandleGameNode, game node on tree: %d, other node no tree: %d",
        isGameSelfNodeOnTree, isOtherSelfNodeOnTree);
    isGameNodeOnTree_ = isGameSelfNodeOnTree && !isOtherSelfNodeOnTree;
}

void HgmRPContext::SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> serviceToProcessInfo,
    uint32_t *pendingScreenRefreshRate, uint64_t *pendingConstraintRelativeTime)
{
    if (serviceToProcessInfo == nullptr) {
        HGM_LOGW("HgmRPContext serviceToProcessInfo is null");
        return;
    }

    RS_TRACE_NAME_FMT("HgmRPContext::SetServiceToProcessInfo %d, %s",
        serviceToProcessInfo->pendingScreenRefreshRate, serviceToProcessInfo->hgmDataChangeTypes.to_string().c_str());

    *pendingScreenRefreshRate = serviceToProcessInfo->pendingScreenRefreshRate;
    *pendingConstraintRelativeTime = serviceToProcessInfo->pendingConstraintRelativeTime;

    if (serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::ADAPTIVE_VSYNC)) {
        isAdaptive_ = serviceToProcessInfo->isAdaptive;
        gameNodeName_ = serviceToProcessInfo->gameNodeName;
    }

    if (serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::HGM_CONFIG_DATA)) {
        ltpoEnabled_ = serviceToProcessInfo->ltpoEnabled;
        isDelayMode_ = serviceToProcessInfo->isDelayMode;
        pipelineOffsetPulseNum_ = serviceToProcessInfo->pipelineOffsetPulseNum;
        rpFrameRatePolicy_.HgmConfigUpdateCallback(serviceToProcessInfo->rpHgmConfigData);
        hgmRPEnergy_->HgmConfigUpdateCallback(serviceToProcessInfo->rpHgmConfigData);
    }
    hgmRPEnergy_->SetTouchState(serviceToProcessInfo->isPowerIdle);
}
} // namespace Rosen
} // namespace OHOS