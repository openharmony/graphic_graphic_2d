/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_frame_rate_linker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderFrameRateLinkerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderFrameRateLinkerTest::SetUpTestCase() {}
void RSRenderFrameRateLinkerTest::TearDownTestCase() {}
void RSRenderFrameRateLinkerTest::SetUp() {}
void RSRenderFrameRateLinkerTest::TearDown() {}

/**
 * @tc.name: SetExpectedRange
 * @tc.desc: Test SetExpectedRange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerTest, SetExpectedRange, TestSize.Level1)
{
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    ASSERT_NE(frameRateLinker, nullptr);
    FrameRateRange range(60, 144, 120);
    frameRateLinker->SetExpectedRange(range);
    EXPECT_EQ(frameRateLinker->GetExpectedRange(), range);
}

/**
 * @tc.name: SetFrameRate
 * @tc.desc: Test SetFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerTest, SetFrameRate, TestSize.Level1)
{
    FrameRateLinkerId id = 1;
    std::shared_ptr<RSRenderFrameRateLinker> frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id);
    ASSERT_NE(frameRateLinker, nullptr);
    EXPECT_EQ(frameRateLinker->GetId(), id);
    uint32_t rate = 120;
    frameRateLinker->SetFrameRate(rate);
    EXPECT_EQ(frameRateLinker->GetFrameRate(), rate);
}

/**
 * @tc.name: GenerateId
 * @tc.desc: Test GenerateId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerTest, GenerateId, TestSize.Level1)
{
    FrameRateLinkerId id = 1;
    std::shared_ptr<RSRenderFrameRateLinker> frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id);
    auto id1 = frameRateLinker->GenerateId();
    auto id2 = frameRateLinker->GenerateId();
    EXPECT_LT(id1, id2);
}
} // namespace OHOS::Rosen