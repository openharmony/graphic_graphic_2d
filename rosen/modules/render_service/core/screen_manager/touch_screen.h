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
    DISALLOW_COPY_AND_MOVE(TouchScreen);

#ifdef TP_FEATURE_ENABLE
    int32_t SetFeatureConfig(int32_t feature, const char *config);
    int32_t SetAftConfig(const char *config);
#endif

private:
    DECLARE_DELAYED_SINGLETON(TouchScreen);
};

#define TOUCH_SCREEN ::OHOS::DelayedSingleton<TouchScreen>::GetInstance()
} // namespace Rosen
} // namespace OHOS
#endif // TOUCH_SCREEN_H