/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_parallel_render_ext.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelRenderExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelRenderExtTest::SetUpTestCase() {}
void RSParallelRenderExtTest::TearDownTestCase() {}
void RSParallelRenderExtTest::SetUp() {}
void RSParallelRenderExtTest::TearDown() {}

/**
 * @tc.name: OpenParallelRenderExt
 * @tc.desc: Test RSParallelRenderExtTest.OpenParallelRenderExt
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, OpenParallelRenderExt, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::OpenParallelRenderExt();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetParallelRenderEnable
 * @tc.desc: Test RSParallelRenderExtTest.GetParallelRenderEnable
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetParallelRenderEnable, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetParallelRenderEnable();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetInitParallelRenderLoadBalancingFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetInitParallelRenderLoadBalancingFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetInitParallelRenderLoadBalancingFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetInitParallelRenderLoadBalancingFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetSetSubRenderThreadNumFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetSetSubRenderThreadNumFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetSetSubRenderThreadNumFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetSetSubRenderThreadNumFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetAddRenderLoadFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetAddRenderLoadFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetAddRenderLoadFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetAddRenderLoadFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetUpdateLoadCostFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetUpdateLoadCostFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetUpdateLoadCostFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetUpdateLoadCostFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetLoadBalancingFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetLoadBalancingFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetLoadBalancingFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetLoadBalancingFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetClearRenderLoadFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetClearRenderLoadFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetClearRenderLoadFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetClearRenderLoadFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetFreeParallelRenderLoadBalancing
 * @tc.desc: Test RSParallelRenderExtTest.GetFreeParallelRenderLoadBalancing
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetFreeParallelRenderLoadBalancing, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetFreeParallelRenderLoadBalancing();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: GetSetCoreLevelFunc
 * @tc.desc: Test RSParallelRenderExtTest.GetSetCoreLevelFunc
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, GetSetCoreLevelFunc, TestSize.Level1)
{
    auto isSuccessful = RSParallelRenderExt::GetSetCoreLevelFunc();
    ASSERT_EQ(isSuccessful, true);
}

/**
 * @tc.name: CloseParallelRenderExtTest
 * @tc.desc: Test RSParallelRenderExtTest.CloseParallelRenderExtTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, CloseParallelRenderExtTest, TestSize.Level1)
{
    RSParallelRenderExt::CloseParallelRenderExt();
}

/**
 * @tc.name: FreeFuncHandle
 * @tc.desc: Test RSParallelRenderExtTest.FreeFuncHandle
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelRenderExtTest, FreeFuncHandle, TestSize.Level1)
{
    RSParallelRenderExt::FreeFuncHandle();
}

} // namespace OHOS::Rosen