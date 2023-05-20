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

#include "recording/shader_effect_cmd_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ShaderEffectCmdListTest::SetUpTestCase() {}
void ShaderEffectCmdListTest::TearDownTestCase() {}
void ShaderEffectCmdListTest::SetUp() {}
void ShaderEffectCmdListTest::TearDown() {}

/**
 * @tc.name: CreateFromData001
 * @tc.desc: test for creating ShaderEffectCmdList.
 * @tc.type: FUNC
 * @tc.require: I76XYC
 */
HWTEST_F(ShaderEffectCmdListTest, CreateFromData001, TestSize.Level1)
{
    CmdListData data;
    auto newShaderEffectCmdList = ShaderEffectCmdList::CreateFromData(data);
    EXPECT_TRUE(newShaderEffectCmdList != nullptr);
}

/**
 * @tc.name: CreateFromData002
 * @tc.desc: test for creating ShaderEffectCmdList.
 * @tc.type: FUNC
 * @tc.require: I76XYC
 */
HWTEST_F(ShaderEffectCmdListTest, CreateFromData002, TestSize.Level1)
{
    CmdListData data;
    LargeObjectData largeObjectData;
    auto newShaderEffectCmdList = ShaderEffectCmdList::CreateFromData(data, largeObjectData);
    EXPECT_TRUE(newShaderEffectCmdList != nullptr);
}

/**
 * @tc.name: AddLargeObject001
 * @tc.desc: test for adding largeObjectData to ShaderEffectCmdList.
 * @tc.type: FUNC
 * @tc.require: I76XYC
 */
HWTEST_F(ShaderEffectCmdListTest, AddLargeObject001, TestSize.Level1)
{
    CmdListData data;
    LargeObjectData largeObjectData;
    auto newShaderEffectCmdList = ShaderEffectCmdList::CreateFromData(data);
    EXPECT_TRUE(newShaderEffectCmdList != nullptr);
    EXPECT_TRUE(newShaderEffectCmdList->AddLargeObject(largeObjectData) == 0);
}

/**
 * @tc.name: GetLargeObjectData001
 * @tc.desc: test for geting largeObjectData.
 * @tc.type: FUNC
 * @tc.require: I76XYC
 */
HWTEST_F(ShaderEffectCmdListTest, GetLargeObjectData001, TestSize.Level1)
{
    CmdListData data;
    LargeObjectData largeObjectData;
    auto newShaderEffectCmdList = ShaderEffectCmdList::CreateFromData(data, largeObjectData);
    EXPECT_TRUE(newShaderEffectCmdList != nullptr);
    auto ret = newShaderEffectCmdList->GetLargeObjectData();
    EXPECT_EQ(ret, largeObjectData);
}

/**
 * @tc.name: Playback001
 * @tc.desc: test for ShaderEffectCmdList Playback function
 * @tc.type: FUNC
 * @tc.require: I76XYC
 */
HWTEST_F(ShaderEffectCmdListTest, Playback001, TestSize.Level1)
{
    CmdListData data;
    LargeObjectData largeObjectData;
    auto newShaderEffectCmdList = ShaderEffectCmdList::CreateFromData(data, largeObjectData);
    EXPECT_TRUE(newShaderEffectCmdList != nullptr);
    auto ret = newShaderEffectCmdList->Playback();
    EXPECT_TRUE(ret == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
