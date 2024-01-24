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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_CONTEXT_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_CONTEXT_H

#include <EGL/egl.h>
#include "egl_wrapper_object.h"
#include "egl_defs.h"
namespace OHOS {
class EglWrapperDisplay;

class EglWrapperContext : public EglWrapperObject {
public:
    EglWrapperContext(EglWrapperDisplay *disp, EGLContext context,
        EGLint version = EglWrapperDispatchTable::GLESV1_INDEX);
    void SetCurrentSurface(EGLSurface draw, EGLSurface read);
    EGLSurface GetCurrentSurface(EGLint type) const;
    static EglWrapperContext *GetWrapperContext(EGLContext ctx);
    inline EGLContext GetEglContext() const
    {
        return context_;
    };
    inline EGLint GetEglVersion() const
    {
        return version_;
    };
protected:
    ~EglWrapperContext() override;
private:
    EGLContext context_;
    EGLSurface read_;
    EGLSurface draw_;
    EGLint version_;
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_CONTEXT_H
