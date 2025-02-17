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

#ifndef TOUCH_SCREEN_H
#define TOUCH_SCREEN_H

#include <dlfcn.h>

#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace Rosen {
class TouchScreen final {
public:
    using TsSetFeatureConfig = int32_t (*)(int32_t, const char *);
    using TsSetAftConfig = int32_t (*)(const char *);

    DISALLOW_COPY_AND_MOVE(TouchScreen);
    
    void InitTouchScreen();

    bool IsSetFeatureConfigHandleValid() const
    {
        return setFeatureConfigHandle_ != nullptr;
    }

    int32_t SetFeatureConfig(int32_t feature, const char *config)
    {
        return setFeatureConfigHandle_(feature, config);
    }

    bool IsSetAftConfigHandleValid() const
    {
        return setAftConfigHandle_ != nullptr;
    }

    int32_t SetAftConfig(const char *config)
    {
        return setAftConfigHandle_(config);
    }

private:
    DECLARE_DELAYED_SINGLETON(TouchScreen);

    void* touchScreenHandle_ = nullptr;
    TsSetFeatureConfig setFeatureConfigHandle_ = nullptr;
    TsSetAftConfig setAftConfigHandle_ = nullptr;
};

#define TOUCH_SCREEN ::OHOS::DelayedSingleton<TouchScreen>::GetInstance()
} // namespace Rosen
} // namespace OHOS
#endif // TOUCH_SCREEN_H