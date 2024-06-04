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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PLAYER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PLAYER_H

#ifdef PLAYER_FRAMEWORK_ENABLE
#include "player.h"
#endif
#include "transaction/rs_interfaces.h"
#include "util.h"

namespace OHOS {
class BootPlayer {
public:
    virtual ~BootPlayer() {};

    virtual void Play() {};

    std::string GetResPath(const std::string& type)
    {
        if (IsFileExisted(resPath_)) {
            return FILE_PREFIX + resPath_;
        }
        return type == TYPE_VIDEO ? BOOT_VIDEO_PATH : BOOT_SOUND_PATH;
    }

    Rosen::ScreenId screenId_;
    std::string resPath_;
    bool isSoundEnabled_ = false;
    std::shared_ptr<Media::Player> mediaPlayer_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PLAYER_H
