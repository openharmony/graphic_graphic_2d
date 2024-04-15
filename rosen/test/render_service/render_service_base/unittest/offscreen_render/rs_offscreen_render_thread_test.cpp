/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
} // namespace OHOS::Rosen