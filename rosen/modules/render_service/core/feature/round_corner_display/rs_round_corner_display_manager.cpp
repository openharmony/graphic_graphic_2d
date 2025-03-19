/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_round_corner_display_manager.h"
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "rs_message_bus.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RoundCornerDisplayManager::RoundCornerDisplayManager()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Created \n", __func__);
}

RoundCornerDisplayManager::~RoundCornerDisplayManager()
{
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] Destroy \n", __func__);
}

bool RoundCornerDisplayManager::CheckExist(NodeId id)
{
    auto it = rcdMap_.find(id);
    if (rcdMap_.end() == it) {
        return false;
    }
    return true;
}

void RoundCornerDisplayManager::RegisterRcdMsg()
{
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RS_LOGD("RSSubThreadManager::RegisterRcdMsg");
        if (!isRcdMessageRegisted_) {
            auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
            auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
            msgBus.RegisterTopic<NodeId, uint32_t, uint32_t, uint32_t, uint32_t>(
                TOPIC_RCD_DISPLAY_SIZE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateDisplayParameter);
            msgBus.RegisterTopic<NodeId, ScreenRotation>(
                TOPIC_RCD_DISPLAY_ROTATION, &rcdInstance,
                &RoundCornerDisplayManager::UpdateOrientationStatus);
            msgBus.RegisterTopic<NodeId, int>(
                TOPIC_RCD_DISPLAY_NOTCH, &rcdInstance,
                &RoundCornerDisplayManager::UpdateNotchStatus);
            msgBus.RegisterTopic<NodeId, bool>(
                TOPIC_RCD_DISPLAY_HWRESOURCE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateHardwareResourcePrepared);
            isRcdMessageRegisted_ = true;
            RS_LOGD("RSSubThreadManager::RegisterRcdMsg Registed rcd renderservice end");
            return;
        }
        RS_LOGD("RSSubThreadManager::RegisterRcdMsg Registed rcd renderservice already.");
    }
}

void RoundCornerDisplayManager::AddLayer(const std::string& name, NodeId id,
    RoundCornerDisplayManager::RCDLayerType type)
{
    std::lock_guard<std::mutex> lock(rcdLayerMapMut_);
    rcdlayerMap_[name] = {id, type};
    RS_LOGI_IF(DEBUG_PIPELINE, "[%{public}s] rendertargetNodeId:%{public}" PRIu64 " with rcd layer name %{public}s \n",
        __func__, id, name.c_str());
}

std::pair<NodeId, RoundCornerDisplayManager::RCDLayerType> RoundCornerDisplayManager::GetLayerPair(
    const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(rcdLayerMapMut_);
    auto it = rcdlayerMap_.find(layerName);
    if (rcdlayerMap_.end() == it) {
        return {0, RoundCornerDisplayManager::RCDLayerType::INVALID};
    }
    return rcdlayerMap_.at(layerName);
}

bool RoundCornerDisplayManager::CheckLayerIsRCD(const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(rcdLayerMapMut_);
    auto it = rcdlayerMap_.find(layerName);
    if (rcdlayerMap_.end() == it) {
        return false;
    }
    return true;
}

void RoundCornerDisplayManager::AddRoundCornerDisplay(NodeId id)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (CheckExist(id)) {
        RS_LOGD_IF(DEBUG_PIPELINE,
            "[%{public}s] nodeId:%{public}" PRIu64 " rcd module already exist, size:%{public}zd \n",
            __func__, id, rcdMap_.size());
        return;
    }
    auto rcd = std::make_shared<RoundCornerDisplay>(id);
    rcdMap_[id] = rcd;
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module create failed \n", __func__, id);
        return;
    }
    rcdMap_[id]->InitOnce();
    RS_LOGI_IF(DEBUG_PIPELINE, "[%{public}s] size:%{public}zd rcd module after added for screen \n", __func__,
        rcdMap_.size());
}

void RoundCornerDisplayManager::RemoveRCDResource(NodeId id)
{
    {
        std::lock_guard<std::mutex> lock(rcdMapMut_);
        RemoveRoundCornerDisplay(id);
    }
    {
        std::lock_guard<std::mutex> lock(rcdLayerMapMut_);
        RemoveRCDLayerInfo(id);
    }
}

void RoundCornerDisplayManager::RemoveRoundCornerDisplay(NodeId id)
{
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    rcdMap_.erase(id);
    RS_LOGI_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module removed for screen \n", __func__,
        id);
}

void RoundCornerDisplayManager::RemoveRCDLayerInfo(NodeId id)
{
    const auto& it = std::find_if(rcdlayerMap_.begin(), rcdlayerMap_.end(),
        [id](const std::pair<std::string, std::pair<NodeId, RCDLayerType>>& p) {
            return p.second.first == id;
        });
    if (rcdlayerMap_.end() == it) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd layer not exist \n", __func__, id);
        return;
    }
    rcdlayerMap_.erase(it->first);
    RS_LOGI_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd layer removed for screen \n", __func__,
        id);
}

void RoundCornerDisplayManager::UpdateDisplayParameter(NodeId id, uint32_t left, uint32_t top,
    uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->UpdateDisplayParameter(left, top, width, height);
}

void RoundCornerDisplayManager::UpdateNotchStatus(NodeId id, int status)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->UpdateNotchStatus(status);
}

void RoundCornerDisplayManager::UpdateOrientationStatus(NodeId id, ScreenRotation orientation)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->UpdateOrientationStatus(orientation);
}

void RoundCornerDisplayManager::UpdateHardwareResourcePrepared(NodeId id, bool prepared)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->UpdateHardwareResourcePrepared(prepared);
}

void RoundCornerDisplayManager::RefreshFlagAndUpdateResource(NodeId id)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->RefreshFlagAndUpdateResource();
}

void RoundCornerDisplayManager::DrawRoundCorner(const RoundCornerDisplayManager::RCDLayerInfoVec& layerInfos,
    RSPaintFilterCanvas* canvas)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    for (const auto& layerInfo : layerInfos) {
        if (layerInfo.second == RoundCornerDisplayManager::RCDLayerType::TOP) {
            DrawTopRoundCorner(layerInfo.first, canvas);
        }
        if (layerInfo.second == RoundCornerDisplayManager::RCDLayerType::BOTTOM) {
            DrawBottomRoundCorner(layerInfo.first, canvas);
        }
    }
}

void RoundCornerDisplayManager::DrawTopRoundCorner(NodeId id, RSPaintFilterCanvas* canvas)
{
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->DrawTopRoundCorner(canvas);
}

void RoundCornerDisplayManager::DrawBottomRoundCorner(NodeId id, RSPaintFilterCanvas* canvas)
{
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return;
    }
    rcdMap_[id]->DrawBottomRoundCorner(canvas);
}

bool RoundCornerDisplayManager::HandleRoundCornerDirtyRect(NodeId id, RectI &dirtyRect, const RCDLayerType type)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return false;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return false;
    }
    bool isDirty = false;
    if (type == RCDLayerType::TOP) {
        isDirty = rcdMap_[id]->HandleTopRcdDirty(dirtyRect) || isDirty;
    }
    if (type == RCDLayerType::BOTTOM) {
        isDirty = rcdMap_[id]->HandleBottomRcdDirty(dirtyRect) || isDirty;
    }
    return isDirty;
}

void RoundCornerDisplayManager::RunHardwareTask(NodeId id, const std::function<void()>& task)
{
    {
        std::lock_guard<std::mutex> lock(rcdMapMut_);
        if (!CheckExist(id)) {
            RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
            return;
        }
        if (rcdMap_[id] == nullptr) {
            RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
            RemoveRoundCornerDisplay(id);
            return;
        }
    }
    rcdMap_[id]->RunHardwareTask(task);
}

rs_rcd::RoundCornerHardware RoundCornerDisplayManager::GetHardwareInfo(NodeId id)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    rs_rcd::RoundCornerHardware rcdHardwareInfo{};
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return rcdHardwareInfo;
    }
    if (rcdMap_.at(id) == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        return rcdHardwareInfo;
    }
    return rcdMap_.at(id)->GetHardwareInfo();
}

rs_rcd::RoundCornerHardware RoundCornerDisplayManager::PrepareHardwareInfo(NodeId id)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    rs_rcd::RoundCornerHardware rcdHardwareInfo{};
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return rcdHardwareInfo;
    }
    if (rcdMap_.at(id) == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        return rcdHardwareInfo;
    }
    return rcdMap_.at(id)->PrepareHardwareInfo();
}

bool RoundCornerDisplayManager::GetRcdEnable() const
{
    auto &rcdCfg = RSSingleton<rs_rcd::RCDConfig>::GetInstance();
    // assume support rcd before rcd cfg loaded
    bool rcdSupport = !rcdCfg.IsDataLoaded();
    if (!rcdSupport) {
        auto lcdModel = rcdCfg.GetLcdModel(rs_rcd::ATTR_DEFAULT);
        if (lcdModel != nullptr) {
            rcdSupport = lcdModel->surfaceConfig.topSurface.support && lcdModel->surfaceConfig.bottomSurface.support;
        }
    }
    return RSSystemProperties::GetRSScreenRoundCornerEnable() && rcdSupport;
}

bool RoundCornerDisplayManager::IsNotchNeedUpdate(NodeId id, bool notchStatus)
{
    std::lock_guard<std::mutex> lock(rcdMapMut_);
    if (!CheckExist(id)) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module not exist \n", __func__, id);
        return false;
    }
    if (rcdMap_[id] == nullptr) {
        RS_LOGE_IF(DEBUG_PIPELINE, "[%{public}s] nodeId:%{public}" PRIu64 " rcd module is null \n", __func__, id);
        RemoveRoundCornerDisplay(id);
        return false;
    }
    return rcdMap_[id]->IsNotchNeedUpdate(notchStatus);
}
} // namespace Rosen
} // namespace OHOS