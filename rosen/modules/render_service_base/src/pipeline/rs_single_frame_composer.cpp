/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_single_frame_composer.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
std::map<std::thread::id, uint64_t> RSSingleFrameComposer::ipcThreadIdMap_;
std::mutex RSSingleFrameComposer::ipcThreadIdMapMutex_;
std::map<pid_t, uint64_t> RSSingleFrameComposer::appPidMap_;
std::mutex RSSingleFrameComposer::appPidMapMutex_;
bool RSSingleFrameComposer::FindSingleFrameModifier(const std::list<std::shared_ptr<RSRenderModifier>>& modifierList)
{
    for (auto iter = modifierList.begin(); iter != modifierList.end(); ++iter) {
        if ((*iter)->GetSingleFrameModifier()) {
            return true;
        }
    }
    return false;
}

void RSSingleFrameComposer::EraseSingleFrameModifier(std::list<std::shared_ptr<RSRenderModifier>>& modifierList)
{
    for (auto iter = modifierList.begin(); iter != modifierList.end();) {
        if ((*iter)->GetSingleFrameModifier()) {
            iter = modifierList.erase(iter);
        } else {
            ++iter;
        }
    }
}

void RSSingleFrameComposer::SingleFrameModifierAdd(
    std::list<std::shared_ptr<RSRenderModifier>>& singleFrameModifierList,
    std::list<std::shared_ptr<RSRenderModifier>>& modifierList)
{
    for (auto iter = singleFrameModifierList.begin(); iter != singleFrameModifierList.end(); ++iter) {
        RS_TRACE_NAME("Add SingleFrame DrawCmdModifier ID " + std::to_string((*iter)->GetDrawCmdListId()));
        modifierList.emplace_back(*iter);
    }
}

bool RSSingleFrameComposer::SingleFrameModifierAddToList(RSModifierType type,
    std::list<std::shared_ptr<RSRenderModifier>>& modifierList)
{
    bool needSkip = false;
    EraseSingleFrameModifier(modifierList);
    {
        std::lock_guard<std::mutex> lock(singleFrameDrawMutex_);
        auto iter = singleFrameDrawCmdModifiers_.find(type);
        if (iter != singleFrameDrawCmdModifiers_.end() && !iter->second.empty()) {
            SingleFrameModifierAdd(iter->second, modifierList);
            singleFrameDrawCmdModifiers_.erase(type);
        }
    }
    if (modifierList.size() > 1 && FindSingleFrameModifier(modifierList)) {
        needSkip = true;
    }

    return needSkip;
}

void RSSingleFrameComposer::SetSingleFrameFlag(const std::thread::id ipcThreadId)
{
    std::lock_guard<std::mutex> lock(ipcThreadIdMapMutex_);
    if (ipcThreadIdMap_.find(ipcThreadId) == ipcThreadIdMap_.end()) {
        ipcThreadIdMap_[ipcThreadId] = 1;
    } else {
        ipcThreadIdMap_[ipcThreadId]++;
    }
}

bool RSSingleFrameComposer::IsShouldSingleFrameComposer()
{
    std::lock_guard<std::mutex> lock(ipcThreadIdMapMutex_);
    std::thread::id ipcThreadId = std::this_thread::get_id();
    if (ipcThreadIdMap_.find(ipcThreadId) == ipcThreadIdMap_.end()) {
        return false;
    } else {
        ipcThreadIdMap_[ipcThreadId]--;
        if (ipcThreadIdMap_[ipcThreadId] == 0) {
            ipcThreadIdMap_.erase(ipcThreadId);
        }
        return true;
    }
}

bool RSSingleFrameComposer::SingleFrameIsNeedSkip(bool needSkip, const std::shared_ptr<RSRenderModifier>& modifier)
{
    return needSkip && !modifier->GetSingleFrameModifier();
}

void RSSingleFrameComposer::SingleFrameAddModifier(const std::shared_ptr<RSRenderModifier>& modifier)
{
    if (modifier->GetType() >= RSModifierType::CUSTOM) {
        modifier->SetSingleFrameModifier(true);
        RS_TRACE_NAME("Add modifier DrawCmdListId " + std::to_string(modifier->GetDrawCmdListId()));
        {
            std::lock_guard<std::mutex> lock(singleFrameDrawMutex_);
            singleFrameDrawCmdModifiers_.clear();
            singleFrameDrawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
        }
    }
}

void RSSingleFrameComposer::AddOrRemoveAppPidToMap(bool isNodeSingleFrameComposer, pid_t pid)
{
    std::lock_guard<std::mutex> lock(appPidMapMutex_);
    if (isNodeSingleFrameComposer) {
        appPidMap_[pid] = 1;
    } else {
        appPidMap_.erase(pid);
    }
}

bool RSSingleFrameComposer::IsShouldProcessByIpcThread(pid_t pid)
{
    std::lock_guard<std::mutex> lock(appPidMapMutex_);
    if ((appPidMap_.find(pid) != appPidMap_.end() && appPidMap_[pid] != 0) ||
        RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()) {
        return true;
    }
    return false;
}
}
}