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
#endif
} // namespace OHOS::Rosen
