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
 * @tc.name: UpdateRange
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameRateLinkerCommandTest, UpdateRange, TestSize.Level1)
{
    RSContext context;
    FrameRateLinkerId linkId = 1;
    FrameRateRange range = { 0, 0, 0 };
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkId, range);

    std::shared_ptr<RSRenderFrameRateLinker> linkerPtr = std::make_shared<RSRenderFrameRateLinker>(linkId);
    context.GetMutableFrameRateLinkerMap().RegisterFrameRateLinker(linkerPtr);
    RSFrameRateLinkerCommandHelper::UpdateRange(context, linkId, range);
}

} // namespace Rosen
} // namespace OHOS
