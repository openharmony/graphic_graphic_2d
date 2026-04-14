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

#include "transaction/rs_render_interface.h"

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
    RSRenderInterface::GetInstance().RegisterCanvasCallback(nullptr);
    RSRenderInterface::GetInstance().RegisterCanvasCallback(nullptr);
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
    auto ret = RSRenderInterface::GetInstance().SubmitCanvasPreAllocatedBuffer(1, nullptr, 1);
    ASSERT_NE(ret, 0);
    ret = RSRenderInterface::GetInstance().SubmitCanvasPreAllocatedBuffer(1, nullptr, 1);
    ASSERT_NE(ret, 0);
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
    int32_t ret = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
    RSSystemProperties::isUniRenderEnabled_ = false;
    ret = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
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
    int32_t ret = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
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
    int32_t ret = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
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
    int32_t ret1 = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth1, maxHeight1);

    uint32_t maxWidth2 = 0;
    uint32_t maxHeight2 = 0;
    int32_t ret2 = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth2, maxHeight2);
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
    int32_t ret = RSRenderInterface::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_GE(ret, -1);
}
#endif
} // namespace OHOS::Rosen
