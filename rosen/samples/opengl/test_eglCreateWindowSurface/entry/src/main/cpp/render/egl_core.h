/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef EGL_CORE_H
#define EGL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

namespace OHOS {
class EGLCore {
public:
    explicit EGLCore(){};
    ~EGLCore() {}
    bool EglContextInit(void *window, int width, int height);
    bool CreateEnvironment();
    void Draw();
    void Release();

private:
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint CreateProgram(const char *vertexShader, const char *fragShader);
    GLint PrepareDraw();
    static bool ExecuteDraw(GLint position, const GLfloat *color, const GLfloat rectangleVertices[],
        unsigned long vertSize);
    bool FinishDraw();

private:
    EGLNativeWindowType eglwindow_;
    EGLDisplay egldisplay_ = EGL_NO_DISPLAY;
    EGLConfig eglconfig_ = EGL_NO_CONFIG_KHR;
    EGLSurface eglsurface_ = EGL_NO_SURFACE;
    EGLContext eglcontext_ = EGL_NO_CONTEXT;
    GLuint program_ = 0;
    bool flag_ = false;
    int width_ = 0;
    int height_ = 0;
    GLfloat widthpercent_;
};
} // namespace OHOS
#endif // EGL_CORE_H
