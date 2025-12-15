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

#include "rs_render_composer_manager.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "rs_render_composer_agent.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "ComposerManager"
namespace OHOS {
namespace Rosen {
RSRenderComposerManager::RSRenderComposerManager(std::shared_ptr<AppExecFwk::EventHandler>& handler) :
    handler_(handler) {}

void RSRenderComposerManager::SetComposerToRenderConnection(ScreenId screenId, sptr<RSIComposerToRenderConnection> conn)
{
    if (conn == nullptr) {
        RS_LOGE("SetComposerToRenderConnection conn is nullptr, screenId:%{public}" PRIu64, screenId);
        return;
    }
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("SetComposerToRenderConnection not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    renderComposerAgent->SetComposerToRenderConnection(conn);
}

void RSRenderComposerManager::InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    rsVsyncManagerAgent_ = rsVsyncManagerAgent;
}

void RSRenderComposerManager::OnScreenConnected(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    if (output == nullptr || property == nullptr) {
        RS_LOGE("OnScreenConnected output or property is nullptr");
        return;
    }
    auto screenId = output->GetScreenId();
    RS_TRACE_NAME_FMT("RSRenderComposerManager::OnScreenConnected screenId %u", screenId);
    RS_LOGI("OnScreenConnected screenId:%{public}u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    bool isReuseComposer = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
        renderComposer = std::make_shared<RSRenderComposer>(output, property);
            renderComposer->InitRsVsyncManagerAgent(rsVsyncManagerAgent_);
            auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
            sptr<RSRenderToComposerConnection> composerConnection =
                sptr<RSRenderToComposerConnection>::MakeSptr("composer_conn", screenId, renderComposerAgent);
            rsComposerConnectionMap_.insert(std::pair(screenId, composerConnection));
            rsRenderComposerMap_.insert(std::pair(screenId, renderComposer));
        } else {
            isReuseComposer = true;
            renderComposer = iter->second;
        }
    }
    if (isReuseComposer) {
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
        renderComposerAgent->OnScreenConnected(output, property);
    }
}

void RSRenderComposerManager::OnScreenDisconnected(ScreenId screenId)
{
    RS_TRACE_NAME_FMT("RSRenderComposerManager::OnScreenDisconnected screenId %u", screenId);
    RS_LOGI("OnScreenDisconnected screenId:%{public}" PRIu64, screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("OnScreenDisconnected not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    renderComposerAgent->OnScreenDisconnected();
}

sptr<RSRenderToComposerConnection> RSRenderComposerManager::GetRSComposerConnection(ScreenId screenId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::GetRSComposerConnection screenId %u", screenId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = rsComposerConnectionMap_.find(screenId);
    if (iter == rsComposerConnectionMap_.end()) {
        RS_LOGE("GetRSComposerConnection not find screenId:%{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

void RSRenderComposerManager::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposerManager::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposerManager::PostDelayTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposerManager::SurfaceDump(std::string& dumpString)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::SurfaceDump");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->SurfaceDump(dumpString);
    }
}

void RSRenderComposerManager::GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::GetRefreshInfoToSP");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->GetRefreshInfoToSP(dumpString, nodeId);
        return;
    }
}

void RSRenderComposerManager::FpsDump(std::string& dumpString, std::string& layerName)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::FpsDump");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    dumpString += "\n-- The recently fps records info of screens:\n";
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->FpsDump(dumpString, layerName);
    }
}

void RSRenderComposerManager::ClearFpsDump(std::string& dumpString, std::string& layerName)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::ClearFpsDump");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    dumpString += "\n-- Clear fps records info of screens:\n";
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->ClearFpsDump(dumpString, layerName);
    }
}

void RSRenderComposerManager::HitchsDump(std::string& dumpString, std::string& layerArg)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::HitchsDump");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    dumpString += "\n-- The recently window hitchs records info of screens:\n";
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->HitchsDump(dumpString, layerArg);
    }
}

void RSRenderComposerManager::RefreshRateCounts(std::string& dumpString)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::RefreshRateCounts");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->RefreshRateCounts(dumpString);
    }
}

void RSRenderComposerManager::ClearRefreshRateCounts(std::string& dumpString)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::ClearRefreshRateCounts");
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    for (const auto& [id, rsRenderComposer] : rsRenderComposerMap) {
        if (rsRenderComposer == nullptr) {
            RS_LOGE("%{public}s: screen %{public}" PRIu64 " not found.", __func__, id);
            continue;
        }
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->ClearRefreshRateCounts(dumpString);
    }
}

void RSRenderComposerManager::SetScreenPowerOnChanged(ScreenId screenId, bool flag)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::SetScreenPowerOnChanged screenId %" PRIu64, screenId);
    std::shared_ptr<RSRenderComposer> composer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("SetScreenPowerOnChanged not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        composer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(composer);
    return renderComposerAgent->SetScreenPowerOnChanged(flag);
}
} // namespace Rosen
} // namespace OHOS