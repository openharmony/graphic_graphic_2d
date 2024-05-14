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

#include "command/rs_frame_rate_linker_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSFrameRateLinkerCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameRateLinkerCommandTest::SetUpTestCase() {}
void RSFrameRateLinkerCommandTest::TearDownTestCase() {}
void RSFrameRateLinkerCommandTest::SetUp() {}
void RSFrameRateLinkerCommandTest::TearDown() {}

/**
 * @tc.name: Destroy001
 * @tc.desc: test results of Destroy
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSFrameRateLinkerCommandTest, Destroy001, TestSize.Level1)
{
    RSContext context;
    FrameRateLinkerId linkerId = 0;
    auto& linkerMap = context.GetMutableFrameRateLinkerMap();
    RSFrameRateLinkerCommandHelper::Destroy(context, linkerId);
    EXPECT_EQ(linkerMap.GetFrameRateLinker(linkerId), nullptr);

    FrameRateLinkerId linkerId1 = 1;
    auto linkerPtr = std::make_shared<RSRenderFrameRateLinker>(linkerId1);
    linkerMap.RegisterFrameRateLinker(linkerPtr);
    EXPECT_NE(linkerMap.GetFrameRateLinker(linkerId1), nullptr);
    RSFrameRateLinkerCommandHelper::Destroy(context, linkerId1);
    EXPECT_EQ(linkerMap.GetFrameRateLinker(linkerId1), nullptr);
}

/**
 * @tc.name: UpdateRange001
 * @tc.desc: test results of UpdateRange
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSFrameRateLinkerCommandTest, UpdateRange001, TestSize.Level1)
{
    RSContext context;
    FrameRateLinkerId linkerId = 1;
    FrameRateRange range = { 0, 0, 0 };
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkerId, range, false);

    std::shared_ptr<RSRenderFrameRateLinker> linkerPtr = std::make_shared<RSRenderFrameRateLinker>(linkerId);
    context.GetMutableFrameRateLinkerMap().RegisterFrameRateLinker(linkerPtr);
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkerId, range, false);
}

/**
 * @tc.name: UpdateRange002
 * @tc.desc: test results of UpdateRange
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSFrameRateLinkerCommandTest, UpdateRange002, TestSize.Level1)
{
    RSContext context;
    FrameRateLinkerId linkerId = 1;
    FrameRateRange range = { 0, 0, 0 };
    auto& linkerMap = context.GetMutableFrameRateLinkerMap();
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkerId, range, true);

    std::shared_ptr<RSRenderFrameRateLinker> linkerPtr = std::make_shared<RSRenderFrameRateLinker>(linkerId);
    context.GetMutableFrameRateLinkerMap().RegisterFrameRateLinker(linkerPtr);
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkerId, range, true);
    EXPECT_NE(linkerMap.GetFrameRateLinker(linkerId), nullptr);
}
} // namespace Rosen
} // namespace OHOS
