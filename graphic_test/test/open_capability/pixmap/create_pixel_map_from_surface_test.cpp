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

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_canvas_node.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms

class RSCreatePixelMapFromSurfaceIdTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({0, 0, size.x_ / 2.0f, size.y_ / 2.0f});
    }
    void AfterEach() override {}

    RSSurfaceNode::SharedPtr CreateStandardTestSurface(int& outWidth, int& outHeight)
    {
        auto size = GetScreenSize();
        int surfaceWidth = static_cast<int>(size.x_ / 2.0f);
        int surfaceHeight = static_cast<int>(size.y_ / 2.0f);
        outWidth = surfaceWidth;
        outHeight = surfaceHeight;

        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = "PixelMapCaptureSurface";
        auto surfaceNode = RSSurfaceNode::Create(config, RSSurfaceNodeType::APP_WINDOW_NODE,
            true, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        surfaceNode->SetBounds({0, 0, surfaceWidth, surfaceHeight});
        surfaceNode->SetFrame({0, 0, surfaceWidth, surfaceHeight});
        surfaceNode->SetBackgroundColor(SK_ColorBLUE);

        auto child = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        child->SetBounds({0, 0, surfaceWidth / 2, surfaceHeight / 2});
        child->SetFrame({0, 0, surfaceWidth / 2, surfaceHeight / 2});
        child->SetBackgroundColor(SK_ColorRED);
        surfaceNode->AddChild(child, -1);

        return surfaceNode;
    }
};

/*
 * @tc.name: GRAPHIC_TEST_CREATE_PIXELMAP_FROM_SURFACE_ID_FALSE
 * @tc.desc: Test CreatePixelMapFromSurfaceId with applyTransform = false
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCreatePixelMapFromSurfaceIdTest, CONTENT_DISPLAY_TEST,
               GRAPHIC_TEST_CREATE_PIXELMAP_FROM_SURFACE_ID_FALSE)
{
    int surfaceWidth = 0;
    int surfaceHeight = 0;
    auto surfaceNode = CreateStandardTestSurface(surfaceWidth, surfaceHeight);
    ASSERT_NE(surfaceNode, nullptr);

    GetRootNode()->SetTestSurface(surfaceNode);
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);

    SurfaceId surfaceId = surfaceNode->GetSurface()->GetUniqueId();
    ASSERT_NE(surfaceId, 0U);

    OHOS::Rect rect = {0, 0, surfaceWidth, surfaceHeight};
    auto pixelMap = RSInterfaces::GetInstance().CreatePixelMapFromSurfaceId(surfaceId, rect, false);

    RegisterNode(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_CREATE_PIXELMAP_FROM_SURFACE_ID_TRUE
 * @tc.desc: Test CreatePixelMapFromSurfaceId with applyTransform = true
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCreatePixelMapFromSurfaceIdTest, CONTENT_DISPLAY_TEST,
               GRAPHIC_TEST_CREATE_PIXELMAP_FROM_SURFACE_ID_TRUE)
{
    int surfaceWidth = 0;
    int surfaceHeight = 0;
    auto surfaceNode = CreateStandardTestSurface(surfaceWidth, surfaceHeight);
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetScale(1.2f);
    surfaceNode->SetRotation(10.0f);

    GetRootNode()->SetTestSurface(surfaceNode);
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);

    SurfaceId surfaceId = surfaceNode->GetSurface()->GetUniqueId();
    ASSERT_NE(surfaceId, 0U);

    OHOS::Rect rect = {0, 0, surfaceWidth, surfaceHeight};
    auto pixelMap = RSInterfaces::GetInstance().CreatePixelMapFromSurfaceId(surfaceId, rect, true);

    RegisterNode(surfaceNode);
}

} // namespace OHOS::Rosen