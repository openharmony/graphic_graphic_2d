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
RSRenderComposerManager& RSRenderComposerManager::GetInstance()
{
    static RSRenderComposerManager instance;
    return instance;
}

void RSRenderComposerManager::OnScreenConnected(const std::shared_ptr<HdiOutput>& output)
{
    if (output == nullptr) {
        RS_LOGE("OnScreenConnected output is nullptr");
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
            renderComposer = std::make_shared<RSRenderComposer>(output);
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
        renderComposerAgent->OnScreenConnected(output);
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

std::shared_ptr<RSRenderComposerClient> RSRenderComposerManager::CreateRSRenderComposerClient(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto clientIter = rsComposerClientMap_.find(screenId);
    if (clientIter != rsComposerClientMap_.end()) {
        return clientIter->second;
    }
    auto connectionIter = rsComposerConnectionMap_.find(screenId);
    if (connectionIter == rsComposerConnectionMap_.end()) {
        RS_LOGE("GetRSComposerConnection not find screenId:%{public}" PRIu64, screenId);
        return nullptr;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::CreateRSRenderComposerClient screenId %u", screenId);
    auto composerClient = RSRenderComposerClient::Create(connectionIter->second);
    rsComposerClientMap_.insert(std::pair(screenId, composerClient));
    return composerClient;
}

void RSRenderComposerManager::PreAllocateProtectedBuffer(ScreenId screenId, sptr<SurfaceBuffer> buffer)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::PreAllocateProtectedBuffer screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("PreAllocateProtectedBuffer not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        renderComposer = iter->second;
    }

    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    renderComposerAgent->PreAllocateProtectedBuffer(buffer);
}

uint32_t RSRenderComposerManager::GetUnExecuteTaskNum(ScreenId screenId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::GetUnExecuteTaskNum screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("GetUnExecuteTaskNum not find screenId:%{public}" PRIu64, screenId);
            return 0;
        }
        renderComposer = iter->second;
    }

    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    return renderComposerAgent->GetUnExecuteTaskNum();
}

int32_t RSRenderComposerManager::GetAccumulatedBufferCount(ScreenId screenId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::GetAccumulatedBufferCount screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("GetAccumulatedBufferCount not find screenId:%{public}" PRIu64, screenId);
            return 0;
        }
        renderComposer = iter->second;
    }

    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    return renderComposerAgent->GetAccumulatedBufferCount();
}

void RSRenderComposerManager::PostTask(ScreenId screenId, const std::function<void()>& task)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::PostTask screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("PostTask not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    renderComposerAgent->PostTask(task);
}

GSError RSRenderComposerManager::ClearFrameBuffers(ScreenId screenId, bool isNeedResetContext)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::ClearFrameBuffers screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("ClearFrameBuffers not find screenId:%{public}" PRIu64, screenId);
            return GSERROR_INVALID_ARGUMENTS;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    return renderComposerAgent->ClearFrameBuffers(isNeedResetContext);
}

void RSRenderComposerManager::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::OnScreenVBlankIdleCallback screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("OnScreenVBlankIdleCallback not find screenId:%{public}" PRIu64, screenId);
            return;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    renderComposerAgent->OnScreenVBlankIdleCallback(timestamp);
}

void RSRenderComposerManager::ForEachScreen(
    const std::function<void(ScreenId, std::shared_ptr<RSRenderComposer>)>& func)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsRenderComposerMap = rsRenderComposerMap_;
    }
    for (const auto& [screenId, composer] : rsRenderComposerMap) {
        if (composer != nullptr && func) {
            func(screenId, composer);
        }
    }
}

void RSRenderComposerManager::PostTaskToAllScreens(const std::function<void()>& task)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::PostTaskToAllScreens");
    ForEachScreen([&task](ScreenId screenId, std::shared_ptr<RSRenderComposer> composer) {
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(composer);
        renderComposerAgent->PostTask(task);
    });
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

sptr<SyncFence> RSRenderComposerManager::GetReleaseFence(ScreenId screenId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::GetReleaseFence screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> renderComposer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("GetReleaseFence not find screenId:%{public}" PRIu64, screenId);
            return nullptr;
        }
        renderComposer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    return renderComposerAgent->GetReleaseFence();
}

bool RSRenderComposerManager::WaitComposerTaskExecute(ScreenId screenId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::WaitComposerTaskExecute screenId %u", screenId);
    std::shared_ptr<RSRenderComposer> composer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = rsRenderComposerMap_.find(screenId);
        if (iter == rsRenderComposerMap_.end()) {
            RS_LOGE("WaitComposerTaskExecute not find screenId:%{public}" PRIu64, screenId);
            return false;
        }
        composer = iter->second;
    }
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(composer);
    return renderComposerAgent->WaitComposerTaskExecute();
}

void RSRenderComposerManager::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::CleanLayerBufferBySurfaceId surfaceId %lu", surfaceId);
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
        renderComposerAgent->CleanLayerBufferBySurfaceId(surfaceId);
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

void RSRenderComposerManager::DumpCurrentFrameLayers()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::DumpCurrentFrameLayers");
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
        renderComposerAgent->DumpCurrentFrameLayers();
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

void RSRenderComposerManager::DumpVkImageInfo(std::string& dumpString)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::DumpVkImageInfo");
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
        dumpString += "\n-- DumpVkImageInfo the screen id : " + std::to_string(id) + "\n";
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
        renderComposerAgent->DumpVkImageInfo(dumpString);
    }
}

void RSRenderComposerManager::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderComposerManager::ClearRedrawGPUCompositionCache");
    ForEachScreen([&bufferIds](ScreenId screenId, std::shared_ptr<RSRenderComposer> composer) {
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(composer);
        renderComposerAgent->ClearRedrawGPUCompositionCache(bufferIds);
    });
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