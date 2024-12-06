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

#include "ipc_callbacks/rs_rt_refresh_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRTCallbackRefreshTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRTCallbackRefreshTest::SetUpTestCase() {}
void RSRTCallbackRefreshTest::TearDownTestCase() {}
void RSRTCallbackRefreshTest::SetUp() {}
void RSRTCallbackRefreshTest::TearDown() {}

/**
 * @tc.name: ExecuteRefresh
 * @tc.desc: Verify function ExecuteRefresh
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */

HWTEST_F(RSRTCallbackRefreshTest, RefreshWorkingSimple, TestSize.Level1)
{
    int a = 1, b = 2, sum = 0;
    const int expected = 3;
    std::function<void(void)> callback = [&]() { sum += a + b; };
    RSRTRefreshCallback::Instance().SetRefresh(callback);
    RSRTRefreshCallback::Instance().ExecuteRefresh();
    EXPECT_TRUE(RSRTRefreshCallback::Instance().callback_ != nullptr);
    EXPECT_EQ(expected, sum);
}

HWTEST_F(RSRTCallbackRefreshTest, SetRefresh, TestSize.Level1)
{
    RSRTRefreshCallback::Instance().ExecuteRefresh();
    EXPECT_TRUE(RSRTRefreshCallback::Instance().callback_ == nullptr);
    std::function<void(void)> callback = []() {};
    RSRTRefreshCallback::Instance().SetRefresh(callback);
    RSRTRefreshCallback::Instance().ExecuteRefresh();
    EXPECT_TRUE(RSRTRefreshCallback::Instance().callback_ != nullptr);
}

} // namespace OHOS::Rosen
