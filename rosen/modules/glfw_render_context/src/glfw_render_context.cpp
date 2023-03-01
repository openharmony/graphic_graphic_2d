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
    return glfwInit();
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
}

void GlfwRenderContext::Terminate()
{
    if (external_) {
        return;
    }

    glfwTerminate();
}

int GlfwRenderContext::CreateWindow(int32_t width, int32_t height, bool visible)
{
    if (external_) {
        return 0;
    }

    if (window_ != nullptr) {
        return 0;
    }

    auto flag = visible ? GLFW_TRUE : GLFW_FALSE;
    glfwWindowHint(GLFW_DECORATED, flag);
    glfwWindowHint(GLFW_VISIBLE, flag);
    window_ = glfwCreateWindow(width, height, "glfw window", nullptr, nullptr);
    if (window_ == nullptr) {
        return 1;
    }

    glfwSetWindowUserPointer(window_, this);
    return 0;
}

void GlfwRenderContext::DestroyWindow()
{
    if (external_) {
        return;
    }

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
}

void GlfwRenderContext::GetWindowSize(int32_t &width, int32_t &height)
{
    glfwGetWindowSize(window_, &width, &height);
}

void GlfwRenderContext::SetWindowSize(int32_t width, int32_t height)
{
    glfwSetWindowSize(window_, width, height);
}

void GlfwRenderContext::SetWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(window_, title.c_str());
}

std::string GlfwRenderContext::GetClipboardData()
{
    return glfwGetClipboardString(window_);
}

void GlfwRenderContext::SetClipboardData(const std::string &data)
{
    glfwSetClipboardString(window_, data.c_str());
}

void GlfwRenderContext::MakeCurrent()
{
    glfwMakeContextCurrent(window_);
}

void GlfwRenderContext::SwapBuffers()
{
    glfwSwapBuffers(window_);
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

void GlfwRenderContext::OnMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that->onMouseBotton_) {
        that->onMouseBotton_(button, action == GLFW_PRESS, mods);
    }
}

void GlfwRenderContext::OnCursorPos(GLFWwindow *window, double x, double y)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that->onCursorPos_) {
        that->onCursorPos_(x, y);
    }
}

void GlfwRenderContext::OnKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that->onKey_) {
        that->onKey_(key, scancode, action, mods);
    }
}

void GlfwRenderContext::OnChar(GLFWwindow *window, unsigned int codepoint)
{
    const auto &that = reinterpret_cast<GlfwRenderContext *>(glfwGetWindowUserPointer(window));
    if (that->onChar_) {
        that->onChar_(codepoint);
    }
}
} // namespace OHOS::Rosen
