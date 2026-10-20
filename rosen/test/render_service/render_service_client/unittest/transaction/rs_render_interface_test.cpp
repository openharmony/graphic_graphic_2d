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

#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderInterfaceTest::SetUpTestCase() {}
void RSRenderInterfaceTest::TearDownTestCase() {}
void RSRenderInterfaceTest::SetUp() {}
void RSRenderInterfaceTest::TearDown() {}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RegisterCanvasCallbackTest
 * @tc.desc: test results of RegisterCanvasCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterfaceTest, RegisterCanvasCallbackTest, TestSize.Level1)
{
    RSSystemProperties::isUniRenderEnabled_ = true;
    RSInterfaces::GetInstance().RegisterCanvasCallback(nullptr);
    RSInterfaces::GetInstance().RegisterCanvasCallback(nullptr);
    ASSERT_TRUE(RSSystemProperties::GetUniRenderEnabled());
}

/**
 * @tc.name: SubmitCanvasPreAllocatedBufferTest
 * @tc.desc: test results of SubmitCanvasPreAllocatedBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, SubmitCanvasPreAllocatedBufferTest, TestSize.Level1)
{
    RSSystemProperties::isUniRenderEnabled_ = true;
    auto ret = RSInterfaces::GetInstance().SubmitCanvasPreAllocatedBuffer(1, nullptr, 1);
    ASSERT_EQ(ret, 0);
    ret = RSInterfaces::GetInstance().SubmitCanvasPreAllocatedBuffer(1, nullptr, 1);
}

/**
 * @tc.name: GetMaxGpuBufferSize001
 * @tc.desc: test GetMaxGpuBufferSize with zero initial values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetMaxGpuBufferSize001, TestSize.Level1)
{
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    int32_t ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
    RSSystemProperties::isUniRenderEnabled_ = false;
    ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    RSSystemProperties::isUniRenderEnabled_ = true;
}

/**
 * @tc.name: GetMaxGpuBufferSize002
 * @tc.desc: test GetMaxGpuBufferSize with preset values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetMaxGpuBufferSize002, TestSize.Level1)
{
    uint32_t maxWidth = 2048;
    uint32_t maxHeight = 2048;
    int32_t ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
}

/**
 * @tc.name: GetMaxGpuBufferSize003
 * @tc.desc: test GetMaxGpuBufferSize with UINT32_MAX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetMaxGpuBufferSize003, TestSize.Level1)
{
    uint32_t maxWidth = UINT32_MAX;
    uint32_t maxHeight = UINT32_MAX;
    int32_t ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
}

/**
 * @tc.name: GetMaxGpuBufferSize004
 * @tc.desc: test GetMaxGpuBufferSize multiple calls consistency
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetMaxGpuBufferSize004, TestSize.Level1)
{
    uint32_t maxWidth1 = 0;
    uint32_t maxHeight1 = 0;
    int32_t ret1 = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth1, maxHeight1);

    uint32_t maxWidth2 = 0;
    uint32_t maxHeight2 = 0;
    int32_t ret2 = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth2, maxHeight2);
    EXPECT_EQ(ret1, ret2);
}

/**
 * @tc.name: GetMaxGpuBufferSize005
 * @tc.desc: test GetMaxGpuBufferSize return value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetMaxGpuBufferSize005, TestSize.Level1)
{
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    int32_t ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
}

/**
 * @tc.name: TakeUICaptureInRangeWithConfigInactiveNodeTest
 * @tc.desc: Test TakeUICaptureInRangeWithConfig with inactive node to trigger FlushCanvasDrawingNodeBuffers
 * @tc.type: FUNC
 * @tc.require: issuesIBZ6NM
 */
HWTEST_F(RSRenderInterfaceTest, TakeUICaptureInRangeWithConfigInactiveNodeTest, TestSize.Level1)
{
    class TestSurfaceCapture : public SurfaceCaptureCallback {
    public:
        TestSurfaceCapture() {}
        ~TestSurfaceCapture() {}
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override {}
        void OnSurfaceCaptureHDR(
            std::shared_ptr<Media::PixelMap> pixelMap, std::shared_ptr<Media::PixelMap> pixelMapHDR) override
        {}
    };
    auto callback = std::make_shared<TestSurfaceCapture>();
    auto canvasNodeBegin = RSCanvasNode::Create(false, true, nullptr);
    auto canvasNodeEnd = RSCanvasNode::Create(false, true, nullptr);
    bool backupProperty = RSSystemProperties::isUniRenderEnabled_;
    RSSystemProperties::isUniRenderEnabled_ = true;
    canvasNodeBegin->nodeState_ = RSNodeState::INACTIVE;
    RSSurfaceCaptureConfig captureConfig;
    auto res = RSInterfaces::GetInstance().TakeUICaptureInRangeWithConfig(
        canvasNodeBegin, canvasNodeEnd, false, callback, captureConfig);
    RSSystemProperties::isUniRenderEnabled_ = backupProperty;
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(res, false);
#else
    EXPECT_EQ(res, true);
#endif
}
#endif
/**
 * @tc.name: TakeSurfaceCaptureForUIWithoutUniTest001
 * @tc.desc: test TakeSurfaceCaptureForUIWithoutUni posts async offscreen task and returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, TakeSurfaceCaptureForUIWithoutUniTest001, TestSize.Level1)
{
    class TestCaptureCb : public SurfaceCaptureCallback {
    public:
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
        void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
            std::shared_ptr<Media::PixelMap> pixelmapHDR) override {}
    };
    auto renderInterface = std::make_shared<RSRenderInterface>();
    ASSERT_NE(renderInterface, nullptr);
    auto callback = std::make_shared<TestCaptureCb>();
    bool result = renderInterface->TakeSurfaceCaptureForUIWithoutUni(1, callback, 1.0f, 1.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: GetPixelmapTest001
 * @tc.desc: test GetPixelmap returns false when renderPipelineClient_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetPixelmapTest001, TestSize.Level1)
{
    auto renderInterface = std::make_shared<RSRenderInterface>();
    ASSERT_NE(renderInterface, nullptr);
    renderInterface->renderPipelineClient_ = nullptr;
    Drawing::Rect rect(0.f, 0.f, 0.f, 0.f);
    bool result = renderInterface->GetPixelmap(1, nullptr, &rect, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetPixelmapTest002
 * @tc.desc: test GetPixelmap returns false when rect pointer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetPixelmapTest002, TestSize.Level1)
{
    auto renderInterface = std::make_shared<RSRenderInterface>();
    ASSERT_NE(renderInterface, nullptr);
    bool result = renderInterface->GetPixelmap(1, nullptr, nullptr, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetPixelmapTest003
 * @tc.desc: test GetPixelmap delegates to pipeline client when client and rect are valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderInterfaceTest, GetPixelmapTest003, TestSize.Level1)
{
    auto renderInterface = std::make_shared<RSRenderInterface>();
    ASSERT_NE(renderInterface, nullptr);
    Drawing::Rect rect(0.f, 0.f, 0.f, 0.f);
    bool result = renderInterface->GetPixelmap(1, nullptr, &rect, nullptr);
    EXPECT_FALSE(result);
}
} // namespace OHOS::Rosen
