/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "include/offscreen_render/rs_offscreen_render_thread.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSOffscreenRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSOffscreenRenderThreadTest::SetUpTestCase() {}
void RSOffscreenRenderThreadTest::TearDownTestCase() {}
void RSOffscreenRenderThreadTest::SetUp() {}
void RSOffscreenRenderThreadTest::TearDown() {}
void RSOffscreenRenderThreadTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: Stop001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, Stop001, TestSize.Level1)
{
    auto task = RSOffscreenRenderThreadTest::DisplayTestInfo;
    RSOffscreenRenderThread::Instance().PostTask(task);
    std::function<void()> taskTwo = []() {};
    RSOffscreenRenderThread::Instance().InSertCaptureTask(1, taskTwo);
    ASSERT_NE(RSOffscreenRenderThread::Instance().GetCaptureTask(1), nullptr);
}

/**
 * @tc.name: InsertTask
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, InsertTask, TestSize.Level1)
{
    ASSERT_TRUE(RSOffscreenRenderThread::Instance().GetCaptureTask(0) == nullptr);
}

/**
 * @tc.name: GetCaptureTask
 * @tc.desc: Test PostTask and GetCaptureTask
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSOffscreenRenderThreadTest, GetCaptureTask, TestSize.Level1)
{
    auto task = RSOffscreenRenderThreadTest::DisplayTestInfo;
    RSOffscreenRenderThread::Instance().handler_.reset();
    RSOffscreenRenderThread::Instance().PostTask(task);
    std::function<void()> taskTwo = []() {};
    RSOffscreenRenderThread::Instance().InSertCaptureTask(1, taskTwo);
    ASSERT_NE(RSOffscreenRenderThread::Instance().GetCaptureTask(1), nullptr);
    ASSERT_EQ(RSOffscreenRenderThread::Instance().GetCaptureTask(2), nullptr);
    RSOffscreenRenderThread::Instance().GetRenderContext();
}

/**
 * @tc.name: GetRenderContextTest001
 * @tc.desc: GetRenderContext and CleanGrResource test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSOffscreenRenderThreadTest, GetRenderContextTest001, TestSize.Level1)
{
    std::shared_ptr<RSOffscreenRenderThread> thread = std::make_shared<RSOffscreenRenderThread>();
    EXPECT_NE(thread, nullptr);

    thread->renderContext_ = nullptr;
    thread->GetRenderContext();
    thread->CleanGrResource();

    thread->renderContext_ = std::make_shared<RenderContext>();
    EXPECT_NE(thread->renderContext_, nullptr);
    EXPECT_NE(thread->GetRenderContext(), nullptr);

    thread->renderContext_->drGPUContext_ = nullptr;
    thread->CleanGrResource();

    std::shared_ptr<Drawing::GPUContext> drGPUContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(drGPUContext, nullptr);
    thread->renderContext_->drGPUContext_ = drGPUContext;
    thread->CleanGrResource();
}
} // namespace OHOS::Rosen