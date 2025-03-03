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
    std::shared_ptr<Media::PixelMap> TakeScreenCaptureAndWait(int ms);
    void ResetImagePath();

    std::shared_ptr<RSGraphicRootNode> GetRootNode() const;
    Vector2f GetScreenSize() const;
    void SetSurfaceBounds(const Vector4f& bounds);
    void SetScreenSurfaceBounds(const Vector4f& bounds);
    void SetSurfaceColor(const RSColor& color);

    void StartRunUIAnimation();
    bool HasUIRunningAnimation();
    void FlushAnimation(int64_t time);
    void RequestNextVSync();
    void OnVSync(int64_t time);

    static RSGraphicTestDirector& Instance();

private:
    ScreenId screenId_ = 0;
    RectF screenBounds_;
    std::shared_ptr<RSGraphicRootNode> rootNode_;
    std::shared_ptr<RSUIDirector> rsUiDirector_;

    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_;
    std::shared_ptr<VSyncWaiter> vsyncWaiter_;

    friend class RSGraphicTest;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_TEST_DIRECTOR_H
