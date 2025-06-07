/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef RS_SINGLE_FRAME_COMPOSER_H
#define RS_SINGLE_FRAME_COMPOSER_H

#include <list>
#include <map>
#include <mutex>
#include <thread>
#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_modifier.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSingleFrameComposer {
public:
    RSSingleFrameComposer() = default;
    ~RSSingleFrameComposer() = default;

    bool SingleFrameModifierAddToList(RSModifierType type, std::list<std::shared_ptr<RSRenderModifier>>& modifierList);
    bool SingleFrameIsNeedSkip(bool needSkip, const std::shared_ptr<RSRenderModifier>& modifier);
    void SingleFrameAddModifier(const std::shared_ptr<RSRenderModifier>& modifier);
    static void SetSingleFrameFlag(const std::thread::id ipcThreadId);
    static bool IsShouldSingleFrameComposer();
    static void AddOrRemoveAppPidToMap(bool isNodeSingleFrameComposer, pid_t pid);
    static bool IsShouldProcessByIpcThread(pid_t pid);

private:
    bool FindSingleFrameModifier(const std::list<std::shared_ptr<RSRenderModifier>>& modifierList);
    void EraseSingleFrameModifier(std::list<std::shared_ptr<RSRenderModifier>>& modifierList);
    void SingleFrameModifierAdd(std::list<std::shared_ptr<RSRenderModifier>>& singleFrameModifierList,
        std::list<std::shared_ptr<RSRenderModifier>>& modifierList);

    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> singleFrameDrawCmdModifiers_;
    mutable std::mutex singleFrameDrawMutex_;
    static std::map<std::thread::id, uint64_t> ipcThreadIdMap_;
    static std::mutex ipcThreadIdMapMutex_;
    static std::map<pid_t, uint64_t> appPidMap_;
    static std::mutex appPidMapMutex_;
};
}
}

#endif