/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_SHARED_CONTEXT_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_SHARED_CONTEXT_H

#include <memory>
#include <thread>

#include "common/rs_macros.h"
#include "image/gpu_context.h"

typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLDisplay;

namespace OHOS::Rosen {

class RSB_EXPORT RSSharedContext final : public std::enable_shared_from_this<RSSharedContext> {
public:
    static std::shared_ptr<RSSharedContext> MakeSharedGLContext(EGLContext context);

    ~RSSharedContext();
    void MakeCurrent();
    std::shared_ptr<Drawing::GPUContext> MakeDrContext();

private:
    RSSharedContext(EGLDisplay display, EGLContext context, EGLSurface surface)
        : display_(display), context_(context), surface_(surface) {}

    EGLDisplay display_ = nullptr;
    EGLContext context_ = nullptr;
    EGLSurface surface_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_SHARED_CONTEXT_H
