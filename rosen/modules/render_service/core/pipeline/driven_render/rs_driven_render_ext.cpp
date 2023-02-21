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

#include "rs_driven_render_ext.h"
#include <dlfcn.h>

namespace OHOS {
namespace Rosen {
RSDrivenRenderExt& RSDrivenRenderExt::Instance()
{
    static RSDrivenRenderExt instance;
    return instance;
}

RSDrivenRenderExt::~RSDrivenRenderExt()
{
    CloseDrivenRenderExt();
}

void RSDrivenRenderExt::OpenDrivenRenderExt()
{
    drivenRenderExtHandle_ = dlopen("libdriven_render.z.so", RTLD_NOW);
    if (!drivenRenderExtHandle_) {
        RS_LOGW("RSDrivenRenderExt: dlopen libdriven_render.z.so failed!");
        return;
    }
    drivenRenderExtLoaded_ = LoadDrivenRenderFunc();
}

bool RSDrivenRenderExt::LoadDrivenRenderFunc()
{
    createDrivenRenderManagerFunc_ = dlsym(drivenRenderExtHandle_, "CreateDrivenRenderManagerExt");
    return createDrivenRenderManagerFunc_ != nullptr;
}

void RSDrivenRenderExt::CloseDrivenRenderExt()
{
    if (drivenRenderExtHandle_) {
        dlclose(drivenRenderExtHandle_);
    }
    drivenRenderExtLoaded_ = false;
    drivenRenderExtHandle_ = nullptr;
    createDrivenRenderManagerFunc_ = nullptr;
}

RSDrivenRenderManager* RSDrivenRenderExt::CreateDrivenRenderManager()
{
    if (!drivenRenderExtLoaded_) {
        return nullptr;
    }
    auto createDrivenRenderManager = reinterpret_cast<RSDrivenRenderManager* (*)()>(createDrivenRenderManagerFunc_);
    return createDrivenRenderManager();
}
} // namespace Rosen
} // namespace OHOS
