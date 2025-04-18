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

#ifndef RS_GRAPHIC_TEST_H
#define RS_GRAPHIC_TEST_H

#include "common/rs_color.h"
#include "common/rs_rect.h"
#include "rs_graphic_rootnode.h"
#include "rs_graphic_test_ext.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
class RSGraphicRootNode;
class RSGraphicTest : public testing::Test {
public:
    std::shared_ptr<RSGraphicRootNode> GetRootNode() const;
    Vector2f GetScreenSize() const;
    void SetSurfaceBounds(const Vector4f& bounds);
    void SetScreenSurfaceBounds(const Vector4f& bounds);
    void SetSurfaceColor(const RSColor& color);
    void RegisterNode(std::shared_ptr<RSNode> node);
    void StartUIAnimation();
    void SetScreenSize(float width, float height);

    // overrides gtest functions
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override final;
    void TearDown() override final;

    // user functions
    virtual void BeforeEach() {};
    virtual void AfterEach() {};

    void AddFileRenderNodeTreeToNode(std::shared_ptr<RSNode> node, const std::string& filePath);
    void PlaybackRecover(const std::string& filePath, float pauseTimeStamp);
    void PlaybackStop();
private:
    std::string GetImageSavePath(const std::string path);
    bool IsSingleTest();
    UIPoint GetScreenCapacity(const std::string testCase);
    UIPoint GetPos(int id, int cl);
    bool WaitOtherTest();
    void TestCaseCapture(bool isScreenshot);

    bool shouldRunTest_ = true;
    Vector4f surfaceBounds_;
    std::vector<std::shared_ptr<RSNode>> nodes_;
    static uint32_t imageWriteId_;
    std::string imageSavePath_ = "";
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_TEST_H
