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

#include "ui/rs_frame_rate_linker.h"
#include "animation/rs_transition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFrameRateLinkerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameRateLinkerTest::SetUpTestCase() {}
void RSFrameRateLinkerTest::TearDownTestCase() {}
void RSFrameRateLinkerTest::SetUp() {}
void RSFrameRateLinkerTest::TearDown() {}

/**
 * @tc.name: Create
 * @tc.desc: Test Create
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, Create, TestSize.Level1)
{
    auto frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    EXPECT_EQ(frameRateLinker.use_count(), 1);
    EXPECT_GT(frameRateLinker->GetId(), 0);
}

/**
 * @tc.name: UpdateFrameRateRange001
 * @tc.desc: Test UpdateFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, UpdateFrameRateRange001, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initialRange = {30, 144, 60};
    FrameRateRange newRange = {60, 144, 120};
    frameRateLinker->UpdateFrameRateRange(initialRange, false);
    frameRateLinker->UpdateFrameRateRange({30, 144, 60}, false);
    frameRateLinker->UpdateFrameRateRange(newRange, false);
}

/**
 * @tc.name: UpdateFrameRateRange002
 * @tc.desc: Test UpdateFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, UpdateFrameRateRange002, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initial = {0, 0, 0};
    frameRateLinker->UpdateFrameRateRange(initial, true);
    EXPECT_TRUE(frameRateLinker->currAnimationStatus_);

    frameRateLinker->UpdateFrameRateRange(initial, false);
    EXPECT_TRUE(!frameRateLinker->currAnimationStatus_);

    FrameRateRange initialRange = {30, 144, 60};
    frameRateLinker->UpdateFrameRateRange(initialRange, true);
    EXPECT_TRUE(frameRateLinker->currAnimationStatus_);

    frameRateLinker->UpdateFrameRateRange(initialRange, false);
    EXPECT_TRUE(!frameRateLinker->currAnimationStatus_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    frameRateLinker->UpdateFrameRateRange(initialRange, false);
    EXPECT_EQ(RSTransactionProxy::instance_, nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: UpdateFrameRateRangeImme001
 * @tc.desc: Test UpdateFrameRateRangeImme
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, UpdateFrameRateRangeImme001, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initialRange = {30, 144, 60};
    FrameRateRange newRange = {60, 144, 120};
    frameRateLinker->UpdateFrameRateRangeImme(initialRange, false);
    frameRateLinker->UpdateFrameRateRangeImme({30, 144, 60}, false);
    frameRateLinker->UpdateFrameRateRangeImme(newRange, false);
}

/**
 * @tc.name: UpdateFrameRateRangeImme002
 * @tc.desc: Test UpdateFrameRateRangeImme
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, UpdateFrameRateRangeImme002, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initial = {0, 0, 0};
    frameRateLinker->UpdateFrameRateRangeImme(initial, true);
    EXPECT_TRUE(frameRateLinker->currAnimationStatus_);

    frameRateLinker->UpdateFrameRateRangeImme(initial, false);
    EXPECT_TRUE(!frameRateLinker->currAnimationStatus_);

    FrameRateRange initialRange = {30, 144, 60};
    frameRateLinker->UpdateFrameRateRangeImme(initialRange, true);
    EXPECT_TRUE(frameRateLinker->currAnimationStatus_);

    frameRateLinker->UpdateFrameRateRangeImme(initialRange, false);
    EXPECT_TRUE(!frameRateLinker->currAnimationStatus_);
}

/**
 * @tc.name: SetEnable
 * @tc.desc: Test SetEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerTest, SetEnable, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinker->SetEnable(true);
    EXPECT_TRUE(frameRateLinker->IsEnable());
    frameRateLinker->SetEnable(false);
    EXPECT_FALSE(frameRateLinker->IsEnable());
}

/**
 * @tc.name: GenerateId
 * @tc.desc: test results of GenerateId
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRateLinkerTest, GenerateId, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    FrameRateLinkerId res = frameRateLinker->GenerateId();
    EXPECT_TRUE(res != 0);
}

/**
 * @tc.name: IsUniRenderEnabled
 * @tc.desc: test results of IsUniRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRateLinkerTest, IsUniRenderEnabled, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    bool res = frameRateLinker->IsUniRenderEnabled();
    EXPECT_TRUE(res != true);
}

/**
 * @tc.name: InitUniRenderEnabled
 * @tc.desc: test results of InitUniRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSFrameRateLinkerTest, InitUniRenderEnabled, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->InitUniRenderEnabled();
}
} // namespace OHOS::Rosen
