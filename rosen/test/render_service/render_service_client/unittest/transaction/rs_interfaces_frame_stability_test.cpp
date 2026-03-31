/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t DEFAULT_ID = 100;
constexpr FrameStabilityTarget DEFAULT_TARGET = { .id = DEFAULT_ID, .type = FrameStabilityTargetType::SCREEN };
class RSInterfacesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInterfacesTest::SetUpTestCase() {}
void RSInterfacesTest::TearDownTestCase() {}
void RSInterfacesTest::SetUp() {}
void RSInterfacesTest::TearDown() {}
/*
 * @tc.name: RegisterFrameStabilityDetection001
 * @tc.desc: Test RegisterFrameStabilityDetection with valid screenId
 * @tc.type: FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSInterfacesTest, RegisterFrameStabilityDetection001, Function | SmallTest | Level2)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    FrameStabilityConfig config = {
        .stableDuration = 1000,
        .changePercent = 0.1f
    };
    FrameStabilityCallback callback = [](bool isStable) {};
    int32_t ret = instance.RegisterFrameStabilityDetection(DEFAULT_TARGET, config, callback);
    EXPECT_EQ(ret, 0);
}

/*
 * @tc.name: UnregisterFrameStabilityDetection001
 * @tc.desc: Test UnregisterFrameStabilityDetection with valid screenId
 * @tc.type: FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSInterfacesTest, UnregisterFrameStabilityDetection001, Function | SmallTest | Level2)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    int32_t ret = instance.UnregisterFrameStabilityDetection(DEFAULT_TARGET);
    EXPECT_EQ(ret, 0);
}

/*
 * @tc.name: StartFrameStabilityCollection001
 * @tc.desc: Test StartFrameStabilityCollection with valid screenId
 * @tc.type: FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSInterfacesTest, StartFrameStabilityCollection001, Function | SmallTest | Level2)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    FrameStabilityConfig config = {
        .stableDuration = 1000,
        .changePercent = 0.1f
    };
    int32_t ret = instance.StartFrameStabilityCollection(DEFAULT_TARGET, config);
    EXPECT_EQ(ret, 0);
}

/*
 * @tc.name: GetFrameStabilityResult001
 * @tc.desc: Test GetFrameStabilityResult with valid screenId
 * @tc.type: FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSInterfacesTest, GetFrameStabilityResult001, Function | SmallTest | Level2)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    bool result = false;
    int32_t ret = instance.GetFrameStabilityResult(DEFAULT_TARGET, result);
    EXPECT_EQ(ret, 0);
}
} // namespace OHOS::Rosen