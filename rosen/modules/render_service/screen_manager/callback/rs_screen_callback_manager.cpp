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

#include "rs_screen_callback_manager.h"
#include <cstddef>

namespace OHOS {
namespace Rosen {
void RSScreenCallbackManager::SetCoreListener(const sptr<RSIScreenManagerListener>& listener)
{
    coreListener_ = listener;
}

sptr<RSIScreenManagerListener> RSScreenCallbackManager::GetCoreListener() const
{
    return coreListener_;
}

void RSScreenCallbackManager::AddAgentListener(const sptr<RSIScreenManagerAgentListener>& listener)
{
    std::lock_guard<std::mutex> lock(agentMtx_);
    agentListeners_.push_back(listener);
}

void RSScreenCallbackManager::RemoveAgentListener(const sptr<RSIScreenManagerAgentListener>& listener)
{
    std::lock_guard<std::mutex> lock(agentMtx_);
    auto iter = std::find(agentListeners_.cbegin(), agentListeners_.cend(), listener);
    if (iter != agentListeners_.cend()) {
        agentListeners_.erase(iter);
    }
}

void RSScreenCallbackManager::NotifyScreenPresenceChanged(const ScreenPresenceEvent& event)
{
    if (event.connected) {
        sptr<IRemoteObject> clientToRenderConnection = nullptr;
        if (coreListener_) {
            clientToRenderConnection = coreListener_->
                OnScreenConnected(event.id, {event.output, event.reason}, event.property);
        } else {
            RS_LOGI("%{public}s: coreListener is nullptr", __func__);
        }
        NotifyScreenConnectedToAgentListeners(event.id, event.reason, clientToRenderConnection);
        {
            std::lock_guard<std::mutex> lock(clientToRenderMtx_);
            auto iter = clientToRenderConns_.find(event.id);
            if (iter != clientToRenderConns_.end()) {
                iter->second = clientToRenderConnection;
            } else {
                clientToRenderConns_[event.id] = clientToRenderConnection;
            }
        }
    } else {
        if (coreListener_) {
            coreListener_->OnScreenDisconnected(event.id);
        } else {
            RS_LOGI("%{public}s: coreListener is nullptr", __func__);
        }
        NotifyScreenDisconnectedToAgentListeners(event.id, event.reason);
        {
            std::lock_guard<std::mutex> lock(clientToRenderMtx_);
            auto iter = clientToRenderConns_.find(event.id);
            if (iter != clientToRenderConns_.end()) {
                clientToRenderConns_.erase(iter);
            }
        }
    }
}

void RSScreenCallbackManager::NotifyScreenPropertyUpdated(ScreenId id, const sptr<RSScreenProperty>& property)
{
    if (coreListener_) {
        coreListener_->OnScreenPropertyChanged(id, property);
    } else {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
    }
}

void RSScreenCallbackManager::NotifyScreenRefresh(ScreenId id)
{
    if (coreListener_) {
        coreListener_->OnScreenRefresh(id);
    } else {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
    }
}

void RSScreenCallbackManager::NotifyHwcDead(ScreenId id)
{
    if (coreListener_) {
        coreListener_->OnScreenDisconnected(id);
    } else {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
    }
}

void RSScreenCallbackManager::NotifyHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (coreListener_) {
        coreListener_->OnHwcEvent(deviceId, eventId, eventData);
    } else {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
    }
}

void RSScreenCallbackManager::NotifyVBlankIdle(ScreenId id, uint64_t ns)
{
    if (coreListener_) {
        coreListener_->OnVBlankIdle(id, ns);
    } else {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
    }
}

void RSScreenCallbackManager::NotifyVirtualScreenPresenceChanged(ScreenId id, bool connected,
    ScreenId associatedScreenId, sptr<RSScreenProperty> property)
{
    if (!coreListener_) {
        RS_LOGI("%{public}s: coreListener is nullptr", __func__);
        return;
    }
    if (connected) {
        coreListener_->OnVirtualScreenConnected(id, associatedScreenId, property);
    } else {
        coreListener_->OnVirtualScreenDisconnected(id);
    }
}

void RSScreenCallbackManager::NotifyActiveScreenIdChanged(ScreenId activeScreenId)
{
    if (!coreListener_) {
        RS_LOGE("%{public}s: coreListener is nullptr", __func__);
        return;
    }
    coreListener_->OnActiveScreenIdChanged(activeScreenId);
}

void RSScreenCallbackManager::NotifyScreenBacklightChanged(ScreenId id, uint32_t level)
{
    if (!coreListener_) {
        RS_LOGE("%{public}s: coreListener is nullptr", __func__);
        return;
    }
    coreListener_->OnScreenBacklightChanged(id, level);
}

sptr<IRemoteObject> RSScreenCallbackManager::GetClientToRenderConnection(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(clientToRenderMtx_);
    auto iter = clientToRenderConns_.find(id);
    if (iter != clientToRenderConns_.end()) {
        return iter->second;
    } else {
        return nullptr;
    }
}

void RSScreenCallbackManager::NotifyScreenConnectedToAgentListeners(ScreenId id,
    ScreenChangeReason reason, sptr<IRemoteObject> clientToRenderConnection)
{
    std::vector<sptr<RSIScreenManagerAgentListener>> agentListeners;
    {
        std::lock_guard<std::mutex> lock(agentMtx_);
        agentListeners = agentListeners_;
    }
    RS_LOGI("%{public}s: screen %{public}" PRIu64 "is connected, reason:%{public}u,conn:%{public}s, size:%{public}zu",
        __func__, id, static_cast<uint8_t>(reason), clientToRenderConnection ? "not nullptr" : "nullptr",
        agentListeners.size());
    for (const auto& agentListener : agentListeners) {
        if (!agentListener) {
            continue;
        }
        agentListener->OnScreenConnected(id, reason, clientToRenderConnection);
    }
}


void RSScreenCallbackManager::NotifyScreenDisconnectedToAgentListeners(ScreenId id, ScreenChangeReason reason)
{
    std::vector<sptr<RSIScreenManagerAgentListener>> agentListeners;
    {
        std::lock_guard<std::mutex> lock(agentMtx_);
        agentListeners = agentListeners_;
    }
    RS_LOGI("%{public}s: screen %{public}" PRIu64 "is disconnected, reason:%{public}u, size:%{public}zu",
        __func__, id, static_cast<uint8_t>(reason), agentListeners.size());
    for (const auto& agentListener : agentListeners) {
        if (!agentListener) {
            continue;
        }
        agentListener->OnScreenDisconnected(id, reason);
    }
}
} // namespace Rosen
} // namespace OHOS
