/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "transaction/rs_render_service_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "ui/rs_surface_node.h"
#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSClientTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderServiceClient>();
}
void RSClientTest::TearDownTestCase() {}
void RSClientTest::SetUp() {}
void RSClientTest::TearDown() {}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, CreateNode_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
    }
};

/**
 * @tc.name: TakeSurfaceCapture_Test
 * @tc.desc: Test capture twice with same id
 * @tc.type:FUNC
 * @tc.require: I6X9V1
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::shared_ptr<TestSurfaceCaptureCallback> cb = std::make_shared<TestSurfaceCaptureCallback>();
    bool ret = rsClient->TakeSurfaceCapture(123, cb, 1.0f, 1.0f); // test a notfound number: 123
    ASSERT_EQ(ret, true);
    ret = rsClient->TakeSurfaceCapture(123, cb, 1.0f, 1.0f); // test number: 123 twice
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->TakeSurfaceCapture(123, nullptr, 1.0f, 1.0f); // NodeId: 123
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture01, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->TakeSurfaceCapture(123, nullptr, 1.0f, 1.0f, SurfaceCaptureType::DEFAULT_CAPTURE, true);
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id True
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id True
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_True, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(123, cb, true); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id False
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id False
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(123, cb, false); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test nullptr
 * @tc.desc: RegisterBufferAvailableListener Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->RegisterBufferAvailableListener(123, nullptr, false); // NodeId: 123
}

/**
 * @tc.name: UnregisterBufferAvailableListener Test a notfound id
 * @tc.desc: UnregisterBufferAvailableListener Test a notfound id
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, UnregisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    bool ret = rsClient->UnregisterBufferAvailableListener(123); // test a notfound number: 123
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: RegisterApplicationAgent Test nullptr
 * @tc.desc: RegisterApplicationAgent Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterApplicationAgent_Nullptr, TestSize.Level1)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    ASSERT_NE(renderService, nullptr);
    renderService->RegisterApplicationAgent(123, nullptr); // pid: 123
}

/**
 * @tc.name: CreateVirtualScreen Test
 * @tc.desc: CreateVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSClientTest, CreateVirtualScreen, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    std::vector<NodeId> filteredAppVector = {1};
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1, filteredAppVector);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(true);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(false);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(true);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(false);
}
} // namespace Rosen
} // namespace OHOS