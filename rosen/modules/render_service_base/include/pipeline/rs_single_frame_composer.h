/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <mutex>
#include <list>
#include <thread>
#include <map>
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
    static void SetSingleFrameFlag(const std::thread::id ipcThreadId)
    {
        ipcThreadId_ = ipcThreadId;
    }
    static bool IsShouldSingleFrameComposer()
    {
        return ipcThreadId_ == std::this_thread::get_id();
    }

private:
    bool FindSingleFrameModifier(std::list<std::shared_ptr<RSRenderModifier>>& modifierList);
    void EraseSingleFrameModifier(std::list<std::shared_ptr<RSRenderModifier>>& modifierList);
    void SingleFrameModifierAdd(std::list<std::shared_ptr<RSRenderModifier>>& singleFrameModifierList,
        std::list<std::shared_ptr<RSRenderModifier>>& modifierList);

    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> singleFrameDrawCmdModifiers_;
    mutable std::mutex singleFrameDrawMutex_;
    static std::thread::id ipcThreadId_;
};
}
}

#endif