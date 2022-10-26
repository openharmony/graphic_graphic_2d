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

#include "gtest/gtest.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "wm/window.h"

namespace OHOS {
namespace Rosen {

class RSAnimationBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    static sptr<Window> window;
    static std::shared_ptr<RSCanvasNode> canvasNode;
    static std::shared_ptr<RSUIDirector> rsUiDirector;
    static std::shared_ptr<RSSurfaceNode> animationSurfaceNode;
    static std::shared_ptr<RSNode> rootNode;
    static void InitAnimationWindow();
    static void DestoryAnimationWindow();
    static void InitNode(int width, int height);
    void ResetAnimationCanvasNode();
    void RemoveAnimationCanvasNode();
    void NotifyStartAnimation();
};
} // namespace Rosen
} // namespace OHOS