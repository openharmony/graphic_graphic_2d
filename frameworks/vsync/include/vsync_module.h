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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_H

#include <refbase.h>

#include "vsync_type.h"

namespace OHOS {
class VsyncModule : public RefBase {
public:
    static sptr<VsyncModule> GetInstance();

    virtual VsyncError Start() = 0;
    virtual VsyncError Stop() = 0;
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_H
