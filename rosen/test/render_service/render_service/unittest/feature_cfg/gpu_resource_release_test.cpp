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

#include <gtest/gtest.h>
#include <test_header.h>

#include "gpu_resource_release_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DeeplyRelGpuResParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DeeplyRelGpuResParamTest::SetUpTestCase() {}
void DeeplyRelGpuResParamTest::TearDownTestCase() {}
void DeeplyRelGpuResParamTest::SetUp() {}
void DeeplyRelGpuResParamTest::TearDown() {}

/**
 * @tc.name: SetDeeplyRelGpuResEnable
 * @tc.desc: Verify the SetDeeplyRelGpuResEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DeeplyRelGpuResParamTest, SetDeeplyRelGpuResEnable, Function | SmallTest | Level1)
{
    DeeplyRelGpuResParam deeplyRelGpuResParam;
    deeplyRelGpuResParam.SetDeeplyRelGpuResEnable(true);
    EXPECT_EQ(deeplyRelGpuResParam.isDeeplyRelGpuResEnable_, true);
    deeplyRelGpuResParam.SetDeeplyRelGpuResEnable(false);
    EXPECT_EQ(deeplyRelGpuResParam.isDeeplyRelGpuResEnable_, false);
}

/**
 * @tc.name: IsDeeplyRelGpuResEnable
 * @tc.desc: Verify the result of IsDeeplyRelGpuResEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DeeplyRelGpuResParamTest, IsDeeplyRelGpuResEnable, Function | SmallTest | Level1)
{
    DeeplyRelGpuResParam deeplyRelGpuResParam;
    deeplyRelGpuResParam.isDeeplyRelGpuResEnable_ = true;
    EXPECT_TRUE(deeplyRelGpuResParam.IsDeeplyRelGpuResEnable());
    deeplyRelGpuResParam.isDeeplyRelGpuResEnable_ = false;
    EXPECT_FALSE(deeplyRelGpuResParam.IsDeeplyRelGpuResEnable());
}

} // namespace Rosen
} // namespace OHOS