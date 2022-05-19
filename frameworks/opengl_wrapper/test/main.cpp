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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>

using namespace std;

void gl_func_test(void)
{
    glClearColor(0, 0, 0, 1);
}

int main(int argc, char **argv)
{
    cout << "egl wrapper api test start!" << endl;

    EGLContext eglContext_;
    //EGLSurface eglSurface_;
    EGLConfig config_;

    const char *str1 = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    std::string sExtensions(str1);
    cout << "EGL_EXTENSIONS:" << sExtensions << endl;

    EGLDisplay eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        cout << "eglGetDisplay error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        cout << "eglInitialize error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    cout << "eglInitialize, version major=" << major << ", minor=" << minor << endl;

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        cout << "eglBindAPI error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    unsigned int ret;
    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        cout << "eglChooseConfig error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    // for not current context test-1.
    glClearColor(0, 0, 0, 1);

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        cout << "eglCreateContext error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    if (!eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_)) {
        cout << "eglMakeCurrent error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    const GLubyte *str = glGetString(GL_VERSION);
    std::string sVersion((const char *)str);
    cout << "GL_VERSION:" << sVersion << endl;
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // for not current context test-2.
    std::thread ttt(gl_func_test);
    ttt.join();

    if (!eglDestroyContext(eglDisplay_, eglContext_)) {
        cout << "eglDestroyContext error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    if (!eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        cout << "eglMakeCurrent error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    if (!eglTerminate(eglDisplay_)) {
        cout << "eglTerminate error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    if (!eglReleaseThread()) {
        cout << "eglReleaseThread error!" << endl;
        cout << "error code="<< eglGetError() << endl;
        return 1;
    }

    // for not current context test-3.
    glClearColor(0, 0, 0, 1);

    cout << "egl wrapper api test end!" << endl;

    return 0;
}
