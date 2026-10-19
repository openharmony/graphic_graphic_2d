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

#include "occlusion_culling_param.h"
#include "occlusion_culling_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class OcclusionCullingParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void OcclusionCullingParamTest::SetUpTestCase() {}
void OcclusionCullingParamTest::TearDownTestCase() {}
void OcclusionCullingParamTest::SetUp()
{
    OcclusionCullingParam::SetDynamicLayerSkipEnable(true);
    OcclusionCullingParam::SetVirtualSelfDrawOptEnable(true);
}
void OcclusionCullingParamTest::TearDown() {}

/**
 * @tc.name: SetStencilPixelOcclusionCullingEnable
 * @tc.desc: Verify the SetStencilPixelOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require: #IBPXUM
 */
HWTEST_F(OcclusionCullingParamTest, SetStencilPixelOcclusionCullingEnable, Function | SmallTest | Level1)
{
    OcclusionCullingParam::SetStencilPixelOcclusionCullingEnable(true);
    ASSERT_TRUE(OcclusionCullingParam::IsStencilPixelOcclusionCullingEnable());
}

/**
 * @tc.name: IsStencilPixelOcclusionCullingEnable
 * @tc.desc: Verify the result of IsStencilPixelOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require: #IBPXUM
 */
HWTEST_F(OcclusionCullingParamTest, IsStencilPixelOcclusionCullingEnable, Function | SmallTest | Level1)
{
    OcclusionCullingParam::SetStencilPixelOcclusionCullingEnable(false);
    ASSERT_FALSE(OcclusionCullingParam::IsStencilPixelOcclusionCullingEnable());
}

/**
 * @tc.name: SetIntraAppControlsLevelOcclusionCullingEnable
 * @tc.desc: Verify the result of SetIntraAppControlsLevelOcclusionCullingEnable function
 * @tc.type: FUNC
 * @tc.require: #IBPXUM
 */
HWTEST_F(OcclusionCullingParamTest, SetIntraAppControlsLevelOcclusionCullingEnable, Function | SmallTest | Level1)
{
    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(true);
    ASSERT_TRUE(OcclusionCullingParam::IsIntraAppControlsLevelOcclusionCullingEnable());
    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(false);
    ASSERT_FALSE(OcclusionCullingParam::IsIntraAppControlsLevelOcclusionCullingEnable());
}

/**
 * @tc.name: DynamicLayerSkipEnableTest001
 * @tc.desc: Verify SetDynamicLayerSkipEnable can toggle the dynamic layer skip switch
 * @tc.type: FUNC
 * @tc.require: issue25950
 */
HWTEST_F(OcclusionCullingParamTest, DynamicLayerSkipEnableTest001, Function | SmallTest | Level1)
{
    EXPECT_EQ(OcclusionCullingParam::IsDynamicLayerSkipEnable(), true);
    OcclusionCullingParam::SetDynamicLayerSkipEnable(false);
    EXPECT_EQ(OcclusionCullingParam::IsDynamicLayerSkipEnable(), false);
    OcclusionCullingParam::SetDynamicLayerSkipEnable(true);
    EXPECT_EQ(OcclusionCullingParam::IsDynamicLayerSkipEnable(), true);
}

/**
 * @tc.name: VirtualSelfDrawOptEnableTest001
 * @tc.desc: Verify SetVirtualSelfDrawOptEnable can toggle the virtual self-draw opt switch
 * @tc.type: FUNC
 * @tc.require: issue25950
 */
HWTEST_F(OcclusionCullingParamTest, VirtualSelfDrawOptEnableTest001, Function | SmallTest | Level1)
{
    EXPECT_EQ(OcclusionCullingParam::IsVirtualSelfDrawOptEnable(), true);
    OcclusionCullingParam::SetVirtualSelfDrawOptEnable(false);
    EXPECT_EQ(OcclusionCullingParam::IsVirtualSelfDrawOptEnable(), false);
    OcclusionCullingParam::SetVirtualSelfDrawOptEnable(true);
    EXPECT_EQ(OcclusionCullingParam::IsVirtualSelfDrawOptEnable(), true);
}

/**
 * @tc.name: ParseDynamicLayerSkipTest001
 * @tc.desc: Verify OcclusionCullingParamParse parses the DynamicLayerSkip switch without
 *           affecting the VirtualSelfDrawOpt switch
 * @tc.type: FUNC
 * @tc.require: issue25950
 */
HWTEST_F(OcclusionCullingParamTest, ParseDynamicLayerSkipTest001, Function | SmallTest | Level1)
{
    OcclusionCullingParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["OcclusionCullingConfig"] = std::make_shared<OcclusionCullingParam>();
    xmlNode node{};
    xmlNode childNode{};
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode{};
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("DynamicLayerSkip"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(OcclusionCullingParam::IsDynamicLayerSkipEnable(), false);
    // virtual self-draw opt switch untouched
    EXPECT_EQ(OcclusionCullingParam::IsVirtualSelfDrawOptEnable(), true);
}

/**
 * @tc.name: ParseVirtualSelfDrawOptTest001
 * @tc.desc: Verify OcclusionCullingParamParse parses the VirtualSelfDrawOpt switch without
 *           affecting the DynamicLayerSkip switch
 * @tc.type: FUNC
 * @tc.require: issue25950
 */
HWTEST_F(OcclusionCullingParamTest, ParseVirtualSelfDrawOptTest001, Function | SmallTest | Level1)
{
    OcclusionCullingParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["OcclusionCullingConfig"] = std::make_shared<OcclusionCullingParam>();
    xmlNode node{};
    xmlNode childNode{};
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode{};
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("VirtualSelfDrawOpt"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(OcclusionCullingParam::IsVirtualSelfDrawOptEnable(), false);
    // dynamic layer skip switch untouched
    EXPECT_EQ(OcclusionCullingParam::IsDynamicLayerSkipEnable(), true);
}
} // namespace Rosen
} // namespace OHOS