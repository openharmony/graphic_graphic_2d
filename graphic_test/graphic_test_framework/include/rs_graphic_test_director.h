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

#ifndef RS_GRAPHIC_TEST_DIRECTOR_H
#define RS_GRAPHIC_TEST_DIRECTOR_H

#include "common/rs_rect.h"
#include "pixel_map.h"
#include "rs_graphic_rootnode.h"
#include "rs_graphic_test_profiler_thread.h"
#include "ui/rs_display_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
class VSyncWaiter;
class RSGraphicTestDirector {
public:
    ~RSGraphicTestDirector();

    void Run();
    void FlushMessage();
    std::shared_ptr<Media::PixelMap> TakeScreenCaptureAndWait(int ms, bool isScreenShot = false);
    void ResetImagePath();

    std::shared_ptr<RSGraphicRootNode> GetRootNode() const;
    Vector2f GetScreenSize() const;
    void SetSurfaceBounds(const Vector4f& bounds);
    void SetScreenSurfaceBounds(const Vector4f& bounds);
    void SetSurfaceColor(const RSColor& color);
    void SetScreenSize(float width, float height);
    bool IsSingleTest();
    void SetSingleTest(bool IsSingleTest);
    void SetProfilerTest(bool isProfilerTest);

    void StartRunUIAnimation();
    bool HasUIRunningAnimation();
    void FlushAnimation(int64_t time);
    void RequestNextVSync();
    void OnVSync(int64_t time);

    void SendProfilerCommand(const std::string command, int outTime = 0);

    static RSGraphicTestDirector& Instance();

private:
    ScreenId screenId_ = 0;
    RectF screenBounds_;
    bool isSingleTest_ = false;
    bool isProfilerTest_ = false;
    std::shared_ptr<RSGraphicRootNode> rootNode_;
    std::shared_ptr<RSUIDirector> rsUiDirector_;

    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_;
    std::shared_ptr<VSyncWaiter> vsyncWaiter_;

    std::shared_ptr<RSGraphicTestProfilerThread> profilerThread_;

    friend class RSGraphicTest;
    friend class TestDefManager;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_TEST_DIRECTOR_H
