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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_EGL_CONSUMER_SURFACE_H
#define FRAMEWORKS_SURFACE_INCLUDE_EGL_CONSUMER_SURFACE_H

#include <string>

#include "consumer_surface.h"

namespace OHOS {
class EglConsumerSurface : public ConsumerSurface {
public:
    EglConsumerSurface(const std::string &name, bool isShared = false);
    virtual ~EglConsumerSurface();
    GSError Init();

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
        int64_t &timestamp, Rect &damage) override;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_EGL_CONSUMER_SURFACE_H
