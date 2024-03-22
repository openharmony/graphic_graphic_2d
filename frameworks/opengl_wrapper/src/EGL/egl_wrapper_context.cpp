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
#include "egl_wrapper_context.h"

#include "wrapper_log.h"
namespace OHOS {
EglWrapperContext::EglWrapperContext(EglWrapperDisplay *disp, EGLContext context, EGLint version)
    : EglWrapperObject(disp), context_(context), read_(nullptr), draw_(nullptr), version_(version)
{
    WLOGD("");
}

EglWrapperContext::~EglWrapperContext()
{
    WLOGD("");
    context_ = nullptr;
    draw_ = nullptr;
    read_ = nullptr;
}

void EglWrapperContext::SetCurrentSurface(EGLSurface draw, EGLSurface read)
{
    WLOGD("");
    draw_ = draw;
    read_ = read;
}

EglWrapperContext *EglWrapperContext::GetWrapperContext(EGLContext ctx)
{
    WLOGD("");
    return reinterpret_cast<EglWrapperContext *>(ctx);
}

EGLSurface EglWrapperContext::GetCurrentSurface(EGLint type) const
{
    WLOGD("");
    switch (type) {
        case EGL_READ: {
            return read_;
        }
        case EGL_DRAW: {
            return draw_;
        }
        default: {
            WLOGE("type error!");
            return EGL_NO_SURFACE;
        }
    }
}
} // namespace OHOS
