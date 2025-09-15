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

#include "animation/rs_animation_timing_protocol.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAnimationTimingProtocolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTimingProtocolTest::SetUpTestCase() {}
void RSAnimationTimingProtocolTest::TearDownTestCase() {}
void RSAnimationTimingProtocolTest::SetUp() {}
void RSAnimationTimingProtocolTest::TearDown() {}

/**
 * @tc.name: SetFinishCallbackTypeTest
 * @tc.desc: Verify the SetFinishCallbackType
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetFinishCallbackTypeTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest SetFinishCallbackTypeTest start";
    RSAnimationTimingProtocol protocol;
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    ASSERT_EQ(protocol.GetFinishCallbackType(), FinishCallbackType::TIME_SENSITIVE);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_INSENSITIVE);
    ASSERT_EQ(protocol.GetFinishCallbackType(), FinishCallbackType::TIME_INSENSITIVE);
    protocol.SetFinishCallbackType(FinishCallbackType::LOGICALLY);
    ASSERT_EQ(protocol.GetFinishCallbackType(), FinishCallbackType::LOGICALLY);
    GTEST_LOG_(INFO) << "RSAnimationTimingProtocolTest SetFinishCallbackTypeTest end";
}
} // namespace Rosen
} // namespace OHOS