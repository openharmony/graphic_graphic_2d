/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "egl_wrapper_layer.h"

#include <dlfcn.h>

#include "egl_defs.h"
#include "../wrapper_log.h"

namespace OHOS {

EglWrapperLayer& EglWrapperLayer::GetInstance()
{
    static EglWrapperLayer layer;
    return layer;
}

EglWrapperLayer::~EglWrapperLayer()
{
    WLOGD("");
    if (dlhandle_) {
        dlclose(dlhandle_);
    }
}

bool EglWrapperLayer::Init(EglWrapperDispatchTable *table)
{
    WLOGD("");
    if (loaded_) {
        WLOGI("EglWrapperLayer is already loaded.");
        return true;
    }

    if (table == nullptr) {
        WLOGI("table is nullptr.");
        return false;
    }

    // Layer init proc TBD.
    
    loaded_ = true;
    return true;
}

} // namespace OHOS
