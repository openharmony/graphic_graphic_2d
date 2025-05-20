/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "native_window.h"
#include "ui/rs_surface_node.h"
#include "utils/canvas_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr static int32_t WIDTH = 720;
constexpr static int32_t HEIGHT = 1280;

class CanvasUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    OHNativeWindow *window_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
};

void CanvasUtilsTest::SetUpTestCase() {}
void CanvasUtilsTest::TearDownTestCase() {}

void CanvasUtilsTest::SetUp()
{
    RSSurfaceNodeConfig config;
    surfaceNode = RSSurfaceNode::Create(config);
    EXPECT_NE(surfaceNode, nullptr);
    sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
    window_ = CreateNativeWindowFromSurface(&surf);
    EXPECT_NE(window_, nullptr);
    NativeWindowHandleOpt(window_, SET_BUFFER_GEOMETRY, WIDTH, HEIGHT);
}

void CanvasUtilsTest::TearDown() {}

/*
 * @tc.name: LockAndUnlockCanvas001
 * @tc.desc: test for Lock and Unlock canvas.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(CanvasUtilsTest, LockAndUnlockCanvas, TestSize.Level1)
{
    Canvas* canvas = CanvasUtils::CreateLockCanvas(nullptr);
    EXPECT_EQ(canvas, nullptr);
    bool ret = CanvasUtils::UnlockCanvas(nullptr, window_);
    EXPECT_EQ(ret, false);
    ret = CanvasUtils::UnlockCanvas(nullptr, nullptr);
    EXPECT_EQ(ret, false);
    Canvas* canvas1 = CanvasUtils::CreateLockCanvas(window_);
    EXPECT_NE(canvas1, nullptr);
    ret = CanvasUtils::UnlockCanvas(canvas1, window_);
    EXPECT_EQ(ret, true);
    delete canvas1;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS