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

#include "glfw_render_context.h"

#include <mutex>

#ifdef __APPLE__
#include <glad/gl.h>
#endif
#include <GLFW/glfw3.h>

#include "hilog/log.h"

namespace OHOS::Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001403, "GlfwRC" };
}

std::shared_ptr<GlfwRenderContext> GlfwRenderContext::GetGlobal()
{
    if (global_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard lock(mutex);
        if (global_ == nullptr) {
            global_ = std::make_shared<GlfwRenderContext>();
        }
    }

    return global_;
}

int GlfwRenderContext::Init()
{
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "Init");
    external_ = false;
    int flag = glfwInit();
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "Init end");
    return flag;
}

void GlfwRenderContext::InitFrom(void *glfwWindow)
{
    if (glfwWindow == nullptr) {
        ::OHOS::HiviewDFX::HiLog::Error(LABEL, "InitFrom glfwWindow is nullptr");
        return;
    }
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "InitFrom glfwWindow");

    external_ = true;

    // to be done: replace this with normal way, a no flutter way.
    // from third_party/flutter/engine/flutter/shell/platform/glfw/flutter_glfw.cc +39
    window_ = *reinterpret_cast<GLFWwindow **>(glfwWindow);

    glfwSetCharCallback(window_, nullptr);
    glfwSetCursorEnterCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetFramebufferSizeCallback(window_, nullptr);
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    glfwSetWindowRefreshCallback(window_, nullptr);
    glfwSetWindowUserPointer(window_, this);

    glfwGetWindowSize(window_, &width_, &height_);
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "glfwSetWindowSizeCallback %{public}d %{public}d", width_, height_);
    glfwSetWindowSizeCallback(window_, GlfwRenderContext::OnSizeChanged);
}

void GlfwRenderContext::Terminate()
{
    if (external_) {
        return;
    }

    glfwTerminate();
}

int GlfwRenderContext::CreateGlfwWindow(int32_t width, int32_t height, bool visible)
{
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "CreateGlfwWindow");
    isVisible_ = visible;
    if (external_) {
        return 0;
    }

    if (window_ != nullptr) {
        return 0;
    }

    auto flag = visible ? GLFW_TRUE : GLFW_FALSE;
    glfwWindowHint(GLFW_DECORATED, flag);
    glfwWindowHint(GLFW_VISIBLE, flag);
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "CreateGlfwWindow::glfwWindowHint end");
    window_ = glfwCreateWindow(width, height, "glfw window", nullptr, nullptr);
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "CreateGlfwWindow::glfwCreateWindow end");
    if (window_ == nullptr) {
        return 1;
    }

    glfwSetWindowUserPointer(window_, this);

#ifdef __APPLE__
    if (isVisible_) {
        glfwGetFramebufferSize(window_, &framebufferWidth_, &framebufferHeight_);
    }
#endif
    width_ = width;
    height_ = height;
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "glfwSetWindowSizeCallback %{public}d %{public}d", width, height);
    glfwSetWindowSizeCallback(window_, GlfwRenderContext::OnSizeChanged);

#ifdef __APPLE__
    if (isVisible_) {
        CreateTexture();
    }
#endif
    return 0;
}

void GlfwRenderContext::DestroyWindow()
{
    if (external_) {
        return;
    }

#ifdef __APPLE__
    glDeleteTextures(1, &textureId);
    if (renderingWindow_ != nullptr) {
        glfwDestroyWindow(renderingWindow_);
    }
#endif

    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
    }
}

int GlfwRenderContext::WindowShouldClose()
{
    return glfwWindowShouldClose(window_);
}

void GlfwRenderContext::WaitForEvents()
{
    glfwWaitEvents();
}

void GlfwRenderContext::PollEvents()
{
    glfwPollEvents();
#ifdef __APPLE__
    if (isVisible_) {
        DrawTexture();
    }
#endif
}

void GlfwRenderContext::GetWindowSize(int32_t &width, int32_t &height)
{
    glfwGetWindowSize(window_, &width, &height);
}

void GlfwRenderContext::SetWindowSize(int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
    glfwSetWindowSize(window_, width, height);
}

void GlfwRenderContext::SetWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(window_, title.c_str());
}

std::string GlfwRenderContext::GetClipboardData()
{
    if (!window_) {
        return std::string("");
    }
    const char* text = glfwGetClipboardString(window_);
    if (!text) {
        return std::string("");
    }
    return std::string(text);
}

void GlfwRenderContext::SetClipboardData(const std::string &data)
{
    glfwSetClipboardString(window_, data.c_str());
}

void GlfwRenderContext::MakeCurrent()
{
    glfwMakeContextCurrent(window_);
}

#ifdef __APPLE__
void GlfwRenderContext::MakeRenderingCurrent()
{
    if (renderingWindow_) {
        glfwMakeContextCurrent(renderingWindow_);
    }
}

bool GlfwRenderContext::CreateRenderingContext()
{
    if (!isVisible_) {
        return false;
    }
    if (renderingWindow_ != nullptr) {
        return true;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    renderingWindow_ = glfwCreateWindow(width_, height_, "glfw window", nullptr, nullptr);
    if (renderingWindow_ == nullptr) {
        return false;
    }
    return true;
}

void GlfwRenderContext::CreateTexture()
{
    if (!isVisible_) {
        return;
    }
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
    std::lock_guard<std::mutex> lock(renderingMutex);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferWidth_, framebufferHeight_, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GlfwRenderContext::CopySnapshot(void* addr)
{
    if (addr == nullptr || !isVisible_) {
        return;
    }
    std::lock_guard<std::mutex> lock(renderingMutex);
    glfwMakeContextCurrent(window_);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, framebufferWidth_, framebufferHeight_, GL_RGBA, GL_UNSIGNED_BYTE, addr);
    textureReady = true;
    glfwMakeContextCurrent(NULL);
}

void GlfwRenderContext::DrawTexture()
{
    std::lock_guard<std::mutex> lock(renderingMutex);
    if (!textureReady || !isVisible_) {
        return;
    }

    glfwMakeContextCurrent(window_);

    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, framebufferWidth_);

    glTexCoord2f(0, 0);
    glVertex2f(-1, 1);

    glTexCoord2f(1, 0);
    glVertex2f(1, 1);

    glTexCoord2f(1, 1);
    glVertex2f(1, -1);

    glTexCoord2f(0, 1);
    glVertex2f(-1, -1);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glfwSwapBuffers(window_);
    textureReady = false;
}
#endif

void GlfwRenderContext::SwapBuffers()
{
    glfwSwapBuffers(window_);
}

void GlfwRenderContext::GetFrameBufferSize(int32_t &width, int32_t &height)
{
    glfwGetFramebufferSize(window_, &width, &height);
}

void GlfwRenderContext::OnMouseButton(const OnMouseButtonFunc &onMouseBotton)
{
    onMouseBotton_ = onMouseBotton;
    glfwSetMouseButtonCallback(window_, GlfwRenderContext::OnMouseButton);
}

void GlfwRenderContext::OnCursorPos(const OnCursorPosFunc &onCursorPos)
{
    onCursorPos_ = onCursorPos;
    glfwSetCursorPosCallback(window_, GlfwRenderContext::OnCursorPos);
}

void GlfwRenderContext::OnKey(const OnKeyFunc &onKey)
{
    onKey_ = onKey;
    glfwSetKeyCallback(window_, GlfwRenderContext::OnKey);
}

void GlfwRenderContext::OnChar(const OnCharFunc &onChar)
{
    onChar_ = onChar;
    glfwSetCharCallback(window_, GlfwRenderContext::OnChar);
}

void GlfwRenderContext::OnSizeChanged(const OnSizeChangedFunc &onSizeChanged)
{
    onSizeChanged_ = onSizeChanged;
    glfwSetWindowSizeCallback(window_, GlfwRenderContext::OnSizeChanged);
}

void GlfwRenderContext::OnMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that && that->onMouseBotton_) {
        that->onMouseBotton_(button, action == GLFW_PRESS, mods);
    }
}

void GlfwRenderContext::OnCursorPos(GLFWwindow *window, double x, double y)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that && that->onCursorPos_) {
        that->onCursorPos_(x, y);
    }
}

void GlfwRenderContext::OnKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that && that->onKey_) {
        that->onKey_(key, scancode, action, mods);
    }
}

void GlfwRenderContext::OnChar(GLFWwindow *window, unsigned int codepoint)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that && that->onChar_) {
        that->onChar_(codepoint);
    }
}

void GlfwRenderContext::OnSizeChanged(GLFWwindow *window, int32_t width, int32_t height)
{
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "OnSizeChanged %{public}d %{public}d", width, height);

    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that->width_ != width || that->height_ != height) {
        glfwSetWindowSize(window, that->width_, that->height_);
    }
    if (that->onSizeChanged_) {
        that->onSizeChanged_(that->width_, that->height_);
    }
    ::OHOS::HiviewDFX::HiLog::Info(LABEL, "OnSizeChanged done %{public}d %{public}d", width, height);
}
} // namespace OHOS::Rosen
