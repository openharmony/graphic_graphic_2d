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

#include "jank_detector/rs_jank_detector.h"

using namespace testing::ext;

namespace OHOS::Rosen {
class RSJankDetectorTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSJankDetectorTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSJankDetector> jankDetector = nullptr;
};

void RSJankDetectorTest::SetUpTestCase()
{
    jankDetector = std::make_shared<RSJankDetector>();
}

void RSJankDetectorTest::TearDownTestCase()
{
    jankDetector = nullptr;
}

void RSJankDetectorTest::SetUp() {}
void RSJankDetectorTest::TearDown() {}

/**
 * @tc.name: SetRefreshPeriodTest
 * @tc.desc: Verify the SetRefreshPeriod of RSJankDetectorTest
 * @tc.type: FUNC
 */
HWTEST_F(RSJankDetectorTest, SetRefreshPeriodTest, Function | SmallTest | Level1)
{
    // start SetRefreshPeriodTest test
    uint64_t refreshPeriod = 16666667;
    jankDetector->SetRefreshPeriod(refreshPeriod);
    EXPECT_EQ(jankDetector->GetRefreshPeriod(), refreshPeriod);
}

/**
 * @tc.name: GetRefreshPeriodTest
 * @tc.desc: Verify the GetRefreshPeriod of RSJankDetectorTest
 * @tc.type: FUNC
 */
HWTEST_F(RSJankDetectorTest, GetRefreshPeriodTest, Function | SmallTest | Level1)
{
    // start GetRefreshPeriodTest test
    uint64_t refreshPeriod = jankDetector->GetRefreshPeriod();
    EXPECT_EQ(16666667, refreshPeriod);
}

/**
 * @tc.name: GetSysTimeNsTest
 * @tc.desc: Verify the GetSysTimeNs of RSJankDetectorTest
 * @tc.type: FUNC
 */
HWTEST_F(RSJankDetectorTest, GetSysTimeNsTest, Function | SmallTest | Level1)
{
    // start GetSysTimeNsTest test
    uint64_t endTimeStamp = jankDetector->GetSysTimeNs();;
    EXPECT_FALSE(endTimeStamp == 0);
}
} // namespace OHOS::Rosen