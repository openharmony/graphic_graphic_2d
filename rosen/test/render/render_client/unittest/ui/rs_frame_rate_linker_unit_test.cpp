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

#include "gtest/gtest.h"

#include "feature/hyper_graphic_manager/rs_frame_rate_linker.h"
#include "animation/rs_transition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsFrameRateLinkrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameRateLinkrTest::SetUpTestCase() {}
void RsFrameRateLinkrTest::TearDownTestCase() {}
void RsFrameRateLinkrTest::SetUp() {}
void RsFrameRateLinkrTest::TearDown() {}

/**
 * @tc.name: Create
 * @tc.desc: Test Create
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, Create, TestSize.Level1)
{
    auto frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    EXPECT_GT(frameRateLinker->GetId(), 0);
}

/**
 * @tc.name: UpdateFrameRateRange01
 * @tc.desc: Test UpdateFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, UpdateFrameRateRange01, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initialRange = {30, 144, 60};
    FrameRateRange newRange = {60, 144, 120};
    frameRateLinker->UpdateFrameRateRange(initialRange, -1);
    frameRateLinker->UpdateFrameRateRange({30, 144, 60}, -1);
    frameRateLinker->UpdateFrameRateRange(newRange, -1);
}

/**
 * @tc.name: UpdateFrameRateRange02
 * @tc.desc: Test UpdateFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, UpdateFrameRateRange02, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initial = {0, 0, 0};
    frameRateLinker->UpdateFrameRateRange(initial, -1);

    frameRateLinker->UpdateFrameRateRange(initial, 0);

    FrameRateRange initialRange = {30, 144, 60};
    frameRateLinker->UpdateFrameRateRange(initialRange, -1);

    frameRateLinker->UpdateFrameRateRange(initialRange, 0);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    frameRateLinker->UpdateFrameRateRange(initialRange, 0);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: UpdateFrameRateRangeImme01
 * @tc.desc: Test UpdateFrameRateRangeImme
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, UpdateFrameRateRangeImme01, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initialRange = {30, 144, 60};
    FrameRateRange newRange = {60, 144, 120};
    frameRateLinker->UpdateFrameRateRangeImme(initialRange, 0);
    frameRateLinker->UpdateFrameRateRangeImme({30, 144, 60}, 0);
    frameRateLinker->UpdateFrameRateRangeImme(newRange, 0);
}

/**
 * @tc.name: UpdateFrameRateRangeImme02
 * @tc.desc: Test UpdateFrameRateRangeImme
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, UpdateFrameRateRangeImme02, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange initial = {0, 0, 0};
    frameRateLinker->UpdateFrameRateRangeImme(initial, -1);

    frameRateLinker->UpdateFrameRateRangeImme(initial, 0);

    FrameRateRange initialRange = {30, 144, 60};
    frameRateLinker->UpdateFrameRateRangeImme(initialRange, -1);
}

/**
 * @tc.name: SetEnable
 * @tc.desc: Test SetEnable
 * @tc.type: FUNC
 */
HWTEST_F(RsFrameRateLinkrTest, SetEnable, TestSize.Level1)
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
HWTEST_F(RsFrameRateLinkrTest, GenerateId, TestSize.Level1)
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
HWTEST_F(RsFrameRateLinkrTest, IsUniRenderEnabled, TestSize.Level1)
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
HWTEST_F(RsFrameRateLinkrTest, InitUniRenderEnabled, TestSize.Level1)
{
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->InitUniRenderEnabled();
    ASSERT_NE(frameRateLinker, nullptr);
}
} // namespace OHOS::Rosen
