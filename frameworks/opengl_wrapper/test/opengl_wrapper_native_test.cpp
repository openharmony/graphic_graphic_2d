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

#include <iostream>
#include <surface.h>
#include <sys/time.h>

#include <hilog/log.h>
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"
#include "window.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define LOGD(fmt, ...)                                                                  \
    ::OHOS::HiviewDFX::HiLog::Debug(                                                    \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0, "opengl_wrapper_native_test" },    \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOGI(fmt, ...)                                                                  \
    ::OHOS::HiviewDFX::HiLog::Info(                                                     \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0, "opengl_wrapper_native_test" },    \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOGW(fmt, ...)                                                                  \
    ::OHOS::HiviewDFX::HiLog::Warn(                                                     \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0, "opengl_wrapper_native_test" },    \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOGE(fmt, ...)                                                                  \
    ::OHOS::HiviewDFX::HiLog::Error(                                                    \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0, "opengl_wrapper_native_test" },    \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

using namespace OHOS;
using namespace Rosen;
using namespace std;

using TestFunc = std::function<void(uint32_t, uint32_t)>;
static std::vector<TestFunc> testFuncVec;
constexpr static int32_t WIDTH = 720;
constexpr static int32_t HEIGHT = 1280;
constexpr static int32_t LOOP_COUNT = 5000;
constexpr static int32_t FRAME_UPDATE_TIME_INTERVAL = 16000;
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 3;
static EGLDisplay gEglDisplay = EGL_NO_DISPLAY;
static EGLContext gEglContext = EGL_NO_CONTEXT;
static EGLSurface gEglSurface = EGL_NO_SURFACE;
static EGLConfig gConfig;
static struct NativeWindow *gWindow = nullptr;
static GLuint gProgram;
static GLuint gPos;
static GLuint gColor;
static GLuint gOffsetUniform;

static const char *gFragShaderText =
    "precision mediump float;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_FragColor = v_color + 0.1;\n"
    "}\n";

static const char *gVertShaderText =
    "uniform float offset;\n"
    "attribute vec4 pos;\n"
    "attribute vec4 color;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_Position = pos + vec4(offset, offset, 0.1, 0.1);\n"
    "  v_color = color;\n"
    "}\n";

static GLuint CreateShader(const char *source, GLenum shaderType)
{
    GLuint shader;
    GLint status;

    shader = glCreateShader(shaderType);
    if (shader == 0) {
        LOGE("glCreateShader error!");
        return 0;
    }

    glShaderSource(shader, 1, (const char **) &source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        LOGE("glGetShaderiv status error!");
        return 0;
    }

    return shader;
}

static GLuint CreateAndLinkProgram(GLuint vert, GLuint frag)
{
    GLint status;
    GLuint program = glCreateProgram();

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        LOGE("glGetProgramiv status error!");
        return 0;
    }

    return program;
}

static bool SetUpGl()
{
    GLuint vert = CreateShader(gVertShaderText, GL_VERTEX_SHADER);
    GLuint frag = CreateShader(gFragShaderText, GL_FRAGMENT_SHADER);

    gProgram = CreateAndLinkProgram(vert, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    gPos = glGetAttribLocation(gProgram, "pos");
    gColor = glGetAttribLocation(gProgram, "color");

    glUseProgram(gProgram);

    gOffsetUniform = glGetUniformLocation(gProgram, "offset");

    return gProgram != 0;
}

static void render()
{
    /* Complete a movement iteration in 5000 ms. */
    static const uint64_t iterationMs = 5000;
    static const GLfloat verts[4][2] = {
        { -0.5, -0.5 },
        { -0.5,  0.5 },
        {  0.5, -0.5 },
        {  0.5,  0.5 }
    };
    static const GLfloat colors[4][3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 1, 1, 0 }
    };
    GLfloat offset;
    struct timeval tv;
    uint64_t timeMs;

    gettimeofday(&tv, nullptr);
    const int timeConversionRate = 1000;
    timeMs = tv.tv_sec * timeConversionRate + tv.tv_usec / timeConversionRate;

    /* Split timeMs in repeating windows of [0, iterationMs) and map them
     * to offsets in the [-0.5, 0.5) range. */
    const float range = 0.5;
    offset = (timeMs % iterationMs) / (float) iterationMs - range;

    glUniform1f(gOffsetUniform, offset);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    const int32_t vertsCnt = 2;
    glVertexAttribPointer(gPos, vertsCnt, GL_FLOAT, GL_FALSE, 0, verts);
    const int32_t colorsCnt = 3;
    glVertexAttribPointer(gColor, colorsCnt, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(gPos);
    glEnableVertexAttribArray(gColor);

    const int32_t pointsCnt = 4;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, pointsCnt);

    glDisableVertexAttribArray(gPos);
    glDisableVertexAttribArray(gColor);
}

static void TestDraw(uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDraw");

    LOGI("width=%{public}d, height=%{public}d\n", width, height);

    EGLint w, h;
    eglQuerySurface(gEglDisplay, gEglSurface, EGL_WIDTH, &w);
    eglQuerySurface(gEglDisplay, gEglSurface, EGL_HEIGHT, &h);

    LOGI("w=%{public}d, h=%{public}d\n", w, h);

    glViewport(0, 0, w, h);

    for (int i = 0; i < LOOP_COUNT; i++) {
        render();
        eglSwapBuffers(gEglDisplay, gEglSurface);
        usleep(FRAME_UPDATE_TIME_INTERVAL);
    }

    LOGI("+++++++ TestDraw");
}

static void InitEglContext()
{
    LOGI("Creating EGLContext!!!");
    gEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (gEglDisplay == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    EGLint major, minor;
    if (eglInitialize(gEglDisplay, &major, &minor) == EGL_FALSE) {
        LOGE("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGE("Failed to bind OpenGL ES API");
        return;
    }

    unsigned int ret;
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };

    ret = eglChooseConfig(gEglDisplay, configAttribs, &gConfig, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    gEglContext = eglCreateContext(gEglDisplay, gConfig, EGL_NO_CONTEXT, contextAttribs);
    if (gEglContext == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }

    eglMakeCurrent(gEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, gEglContext);

    EGLint winAttribs[] = { EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE };
    gEglSurface = eglCreateWindowSurface(gEglDisplay, gConfig, gWindow, winAttribs);
    if (gEglSurface == EGL_NO_SURFACE) {
        LOGW("Failed to create eglsurface!!! %{public}x", eglGetError());
        return;
    }

    LOGW("Create EGL context successfully, version %{public}d.%{public}d", major, minor);

    eglMakeCurrent(gEglDisplay, gEglSurface, gEglSurface, gEglContext);

    SetUpGl();
}

static void Deinit()
{
    if (gEglDisplay == EGL_NO_DISPLAY) {
        return;
    }
    eglDestroySurface(gEglDisplay, gEglSurface);
    eglDestroyContext(gEglDisplay, gEglContext);
    eglMakeCurrent(gEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(gEglDisplay);
    eglReleaseThread();

    gEglDisplay = EGL_NO_DISPLAY;
    gEglContext = EGL_NO_CONTEXT;
    gEglSurface = EGL_NO_SURFACE;
}

static void DrawSurface(int32_t width, int32_t height, size_t index)
{
    InitEglContext();
    testFuncVec[index](WIDTH, HEIGHT);
    Deinit();
}

static std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    sptr<Surface> surf = surfaceNode->GetSurface();
    gWindow = CreateNativeWindowFromSurface(&surf);

    NativeWindowHandleOpt(gWindow, SET_USAGE, BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
    NativeWindowHandleOpt(gWindow, SET_BUFFER_GEOMETRY, WIDTH, HEIGHT);
    NativeWindowHandleOpt(gWindow, SET_COLOR_GAMUT, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    return surfaceNode;
}

int main()
{
    testFuncVec.push_back(TestDraw);
    auto surfaceNode = CreateSurface();
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    for (size_t i = 0; i < testFuncVec.size(); i++) {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            continue;
        }
        // sleep 2s
        sleep(2);
        displayNode->AddChild(surfaceNode, -1);
        surfaceNode->SetBounds(0, 0, WIDTH, HEIGHT);
        transactionProxy->FlushImplicitTransaction();

        DrawSurface(WIDTH, HEIGHT, i);

        // sleep 8s
        sleep(8);
        displayNode->RemoveChild(surfaceNode);
        transactionProxy->FlushImplicitTransaction();
    }

    return 0;
}
