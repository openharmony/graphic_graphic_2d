/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>
#include "platform/common/rs_event_detector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEventTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSEventTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSBaseEventDetector> detector = nullptr;
};

void RSEventTest::SetUpTestCase() {}
void RSEventTest::TearDownTestCase() {}
void RSEventTest::SetUp() {}
void RSEventTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Verify the main function of detector
 * @tc.type: FUNC
 * @tc.require: issueI5GKGC
 */
HWTEST_F(RSEventTest, CreateAndDestroy001, Function | SmallTest | Level2)
{
    // The best way to create RSDetector.
    auto d = RSBaseEventDetector::CreateRSTimeOutDetector(2000, "RS_EVENT_TEST");
    EXPECT_NE(d.get(), nullptr);
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc: Verify the main function of detector
 * @tc.type: FUNC
 * @tc.require: issueI5GKGC
 */
HWTEST_F(RSEventTest, CreateAndDestroy002, Function | SmallTest | Level2)
{
    // The best way to create RSDetector.
    auto d = RSBaseEventDetector::CreateRSTimeOutDetector(2000, "RS_EVENT_TEST");
    // Verify string id
    EXPECT_EQ(d->GetStringId(), "RS_EVENT_TEST");
}
} // namespace OHOS::Rosen
