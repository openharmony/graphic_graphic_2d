/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "display_manager.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"
#include "rs_trace.h"
#include "rs_parameter_parse.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <filesystem>

namespace OHOS {
namespace Rosen {
namespace {
constexpr float TOP_LEVEL_Z = 1000000.0f;
constexpr uint32_t SCREEN_COLOR = 0;
constexpr int64_t WAIT_ANIMATION_SYNC_TIME_OUT = 100;

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        {
            std::unique_lock lock(mutex_);
            ready_ = true;
            pixelMap_ = pixelMap;
        }
        cv_.notify_all();

        if (pixelMap == nullptr) {
            LOGE("RSGraphicTestDirector pixelMap == nullptr");
        }
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}

    void Wait(int ms)
    {
        std::unique_lock lock(mutex_);
        cv_.wait_for(lock, std::chrono::milliseconds(ms), [&] { return ready_; });
    }

    std::mutex mutex_;
    std::condition_variable cv_;
    bool ready_ = false;
    std::shared_ptr<Media::PixelMap> pixelMap_;
};
}

class VSyncWaiter {
public:
    explicit VSyncWaiter(std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler)
    {
        frameCallback_ = {
            .userData_ = this,
            .callback_ = [this](int64_t timestamp, void*) { this->OnVSync(timestamp); },
        };
        vsyncReceiver_ = RSInterfaces::GetInstance().CreateVSyncReceiver("RSGraphicTest", handler);
        vsyncReceiver_->Init();
    }

    void OnVSync(int64_t timestamp)
    {
        {
            std::unique_lock lock(mutex_);
            ready_ = true;
            time_ = time_ + ANIMATION_VSYNC_TIME_MS * UNIT_MS_TO_NS;
        }
        RS_TRACE_NAME_FMT("VSyncWaiter Send OnVSync time is %llu", time_);
        cv_.notify_all();
    }

    void RequestNextVSync()
    {
        if (vsyncReceiver_) {
            vsyncReceiver_->RequestNextVSync(frameCallback_);
        }
        {
            std::unique_lock lock(mutex_);
            ready_ = false;
            cv_.wait_for(lock, std::chrono::milliseconds(WAIT_ANIMATION_SYNC_TIME_OUT), [&] { return ready_; });
        }
        RSGraphicTestDirector::Instance().OnVSync(time_);
    }

    void InitStartTime()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        time_ = ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool ready_ = false;
    int64_t time_;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> vsyncReceiver_;
    Rosen::VSyncReceiver::FrameCallback frameCallback_;
};

RSGraphicTestDirector& RSGraphicTestDirector::Instance()
{
    static RSGraphicTestDirector instance;
    return instance;
}

RSGraphicTestDirector::~RSGraphicTestDirector()
{
    rootNode_->screenSurfaceNode_->RemoveFromTree();
    rsUiDirector_->SendMessages();
    runner_->Stop();
    sleep(1);
}

void RSGraphicTestDirector::Run()
{
    rsUiDirector_ = RSUIDirector::Create();
    rsUiDirector_->Init();

    rsUiDirector_->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) {
        if (task) {
            task();
        }
    });

    runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
    vsyncWaiter_ = std::make_shared<VSyncWaiter>(handler_);
    runner_->Run();

    profilerThread_ = std::make_shared<RSGraphicTestProfilerThread>();
    profilerThread_->Start();

    screenId_ = RSInterfaces::GetInstance().GetDefaultScreenId();

    auto defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    Vector4f defaultScreenBounds = {0, 0, defaultDisplay->GetWidth(), defaultDisplay->GetHeight()};
    screenBounds_ = defaultScreenBounds;

    rootNode_ = std::make_shared<RSGraphicRootNode>();
    rootNode_->screenSurfaceNode_->SetBounds(defaultScreenBounds);
    rootNode_->screenSurfaceNode_->SetFrame(defaultScreenBounds);
    rootNode_->screenSurfaceNode_->SetBackgroundColor(SCREEN_COLOR);
    rootNode_->screenSurfaceNode_->AttachToDisplay(screenId_);
    rootNode_->screenSurfaceNode_->SetPositionZ(TOP_LEVEL_Z);

    rsUiDirector_->SetRSSurfaceNode(rootNode_->screenSurfaceNode_);
    rsUiDirector_->SendMessages();
    sleep(1);

    ResetImagePath();
}

void RSGraphicTestDirector::FlushMessage()
{
    RS_TRACE_NAME("RSGraphicTestDirector::FlushMessage");
    rsUiDirector_->SendMessages();
}

std::shared_ptr<Media::PixelMap> RSGraphicTestDirector::TakeScreenCaptureAndWait(int ms, bool isScreenShot)
{
    RS_TRACE_NAME("RSGraphicTestDirector::TakeScreenCaptureAndWait");

    if (isScreenShot) {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        return pixelMap;
    }

    auto callback = std::make_shared<TestSurfaceCaptureCallback>();
    if (!RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(rootNode_->screenSurfaceNode_, callback)) {
        return nullptr;
    }

    callback->Wait(ms);
    return callback->pixelMap_;
}

void RSGraphicTestDirector::ResetImagePath()
{
    namespace fs = std::filesystem;
    const auto& path = RSParameterParse::Instance().imageSavePath;
    if (!fs::exists(path)) {
        if (!fs::create_directories(path)) {
            LOGE("RSGraphicTestDirector create dir failed %{public}s", path.c_str());
        }
    } else {
        if (!fs::is_directory(path) || !fs::is_empty(path)) {
            LOGE("RSGraphicTestDirector path is not dir or not empty %{public}s", path.c_str());
        }
    }
}

std::shared_ptr<RSGraphicRootNode> RSGraphicTestDirector::GetRootNode() const
{
    return rootNode_;
}

Vector2f RSGraphicTestDirector::GetScreenSize() const
{
    return {screenBounds_.GetWidth(), screenBounds_.GetHeight()};
}

void RSGraphicTestDirector::SetScreenSize(float width, float height)
{
    screenBounds_ = {0, 0, width, height};
}

void RSGraphicTestDirector::SetSingleTest(bool isSingleTest)
{
    isSingleTest_ = isSingleTest;
}

bool RSGraphicTestDirector::IsSingleTest()
{
    return isSingleTest_;
}

void RSGraphicTestDirector::SetSurfaceBounds(const Vector4f& bounds)
{
    if (rootNode_->testSurfaceNode_) {
        rootNode_->testSurfaceNode_->SetBounds(bounds);
    }
}

void RSGraphicTestDirector::SetScreenSurfaceBounds(const Vector4f& bounds)
{
    if (rootNode_->screenSurfaceNode_) {
        rootNode_->screenSurfaceNode_->SetBounds(bounds);
        rootNode_->screenSurfaceNode_->SetFrame(bounds);
    }
}

void RSGraphicTestDirector::SetSurfaceColor(const RSColor& color)
{
    if (rootNode_->testSurfaceNode_) {
        rootNode_->testSurfaceNode_->SetBackgroundColor(color.AsRgbaInt());
    }
}

void RSGraphicTestDirector::StartRunUIAnimation()
{
    if (HasUIRunningAnimation()) {
        if (vsyncWaiter_) {
            vsyncWaiter_->InitStartTime();
        }
        // request first frame
        RequestNextVSync();
    }
}

bool RSGraphicTestDirector::HasUIRunningAnimation()
{
    return rsUiDirector_->HasUIRunningAnimation();
}

void RSGraphicTestDirector::FlushAnimation(int64_t time)
{
    RS_TRACE_NAME_FMT("RSGraphicTestDirector FlushAnimation time is %llu", time);
    rsUiDirector_->FlushAnimation(time);
    rsUiDirector_->FlushModifier();
}

void RSGraphicTestDirector::RequestNextVSync()
{
    if (vsyncWaiter_) {
        vsyncWaiter_->RequestNextVSync();
    }
}

void RSGraphicTestDirector::OnVSync(int64_t time)
{
    FlushAnimation(time);
    //also have animation request next frame
    if (HasUIRunningAnimation()) {
        RequestNextVSync();
    }
}

void RSGraphicTestDirector::SendProfilerCommand(const std::string command, int outTime)
{
    if (profilerThread_) {
        profilerThread_->SendCommand(command, outTime);
    }
}
} // namespace Rosen
} // namespace OHOS
