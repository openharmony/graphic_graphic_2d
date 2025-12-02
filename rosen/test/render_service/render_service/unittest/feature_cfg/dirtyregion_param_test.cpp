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

#include "dirtyregion_param.h"
#include "dirtyregion_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr int DEFAULT_ALIGN_BITS = 128;

class DirtyRegionParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DirtyRegionParamTest::SetUpTestCase() {}
void DirtyRegionParamTest::TearDownTestCase() {}
void DirtyRegionParamTest::SetUp() {}
void DirtyRegionParamTest::TearDown() {}

/**
 * @tc.name: ParamSettingAndGettingTest
 * @tc.desc: Verify the each of those params can be correctly set and gotten.
 * @tc.type: FUNC
 * @tc.require: issueIBOSX3
 */
HWTEST_F(DirtyRegionParamTest, ParamSettingAndGettingTest, Function | SmallTest | Level1)
{
    DirtyRegionParam::SetDirtyRegionEnable(true);
    ASSERT_TRUE(DirtyRegionParam::IsDirtyRegionEnable());
    DirtyRegionParam::SetDirtyRegionEnable(false);
    ASSERT_FALSE(DirtyRegionParam::IsDirtyRegionEnable());

    DirtyRegionParam::SetAdvancedDirtyRegionEnable(true);
    ASSERT_TRUE(DirtyRegionParam::IsAdvancedDirtyRegionEnable());
    DirtyRegionParam::SetAdvancedDirtyRegionEnable(false);
    ASSERT_FALSE(DirtyRegionParam::IsAdvancedDirtyRegionEnable());

    DirtyRegionParam::SetComposeDirtyRegionEnableInPartialDisplay(true);
    ASSERT_TRUE(DirtyRegionParam::IsComposeDirtyRegionEnableInPartialDisplay());
    DirtyRegionParam::SetComposeDirtyRegionEnableInPartialDisplay(false);
    ASSERT_FALSE(DirtyRegionParam::IsComposeDirtyRegionEnableInPartialDisplay());

    DirtyRegionParam::SetTileBasedAlignEnable(true);
    ASSERT_TRUE(DirtyRegionParam::IsTileBasedAlignEnable());
    DirtyRegionParam::SetTileBasedAlignEnable(false);
    ASSERT_FALSE(DirtyRegionParam::IsTileBasedAlignEnable());

    DirtyRegionParam::SetTileBasedAlignBits(DEFAULT_ALIGN_BITS);
    ASSERT_EQ(DirtyRegionParam::GetTileBasedAlignBits(), DEFAULT_ALIGN_BITS);

    DirtyRegionParam::SetAnimationOcclusionEnable(true);
    ASSERT_TRUE(DirtyRegionParam::IsAnimationOcclusionEnable());
    DirtyRegionParam::SetAnimationOcclusionEnable(false);
    ASSERT_FALSE(DirtyRegionParam::IsAnimationOcclusionEnable());
}

/**
 * @tc.name: ParseFeatureParam
 * @tc.desc: Verify the ParseFeatureParam function
 * @tc.type: FUNC
 * @tc.require: issue20843
 */
HWTEST_F(DirtyRegionParamTest, ParseFeatureParamTest001, Function | SmallTest | Level1)
{
    DirtyRegionParamParse paramParse;
    xmlNode node;
    std::string name = "FeatureSwitch";
    node.type = xmlElementType::XML_ELEMENT_NODE;
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    auto res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("DirtyRegionEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("AdvancedDirtyRegionEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("ComposeDirtyRegionEnableInPartialDisplay"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("TileBasedAlignEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("AnimationOcclusionEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("Error"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);

    name = "FeatureSingleParam";
    node.type = xmlElementType::XML_ELEMENT_NODE;
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);

    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("TileBasedAlignBits"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("128"));
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "FeatureMultiParam";
    node.type = xmlElementType::XML_ELEMENT_NODE;
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    res = paramParse.ParseDirtyRegionInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);
}
} // namespace Rosen
} // namespace OHOS