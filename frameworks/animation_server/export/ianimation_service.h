/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_ANIMATION_SERVER_EXPORT_IANIMATION_SERVICE_H
#define FRAMEWORKS_ANIMATION_SERVER_EXPORT_IANIMATION_SERVICE_H

#include <iremote_broker.h>
#include <graphic_common.h>

namespace OHOS {
class IAnimationService : public IRemoteBroker {
public:
    static GSError Init(bool reinit = false);
    static sptr<IAnimationService> Get();

    virtual GSError StartRotationAnimation(int32_t did, int32_t degree) = 0;
    virtual GSError SplitModeCreateBackground() = 0;
    virtual GSError SplitModeCreateMiddleLine() = 0;
    virtual GSError CreateLaunchPage(const std::string &filename) = 0;
    virtual GSError CancelLaunchPage() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"IAnimationService");

protected:
    enum {
        START_ROTATION_ANIMATION = 0,
        SPLIT_MODE_CREATE_BACKGOUND = 1,
        SPLIT_MODE_CREATE_MIDDLE_LINE = 2,
        CREATE_LAUNCH_PAGE = 3,
        DESTROY_LAUNCH_PAGE = 4,
    };

private:
    static inline sptr<IAnimationService> animationService = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_EXPORT_IANIMATION_SERVICE_H
