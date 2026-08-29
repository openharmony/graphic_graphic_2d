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

#include <gtest/gtest.h>
#include <libxml/tree.h>

#include "tunnel_layer_param.h"
#include "tunnel_layer_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class TunnelLayerParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TunnelLayerParamParseTest::SetUpTestCase() {}
void TunnelLayerParamParseTest::TearDownTestCase() {}
void TunnelLayerParamParseTest::SetUp() {}
void TunnelLayerParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParam_NoChildren_ReturnsGetChildFail
 * @tc.desc: Verify ParseFeatureParam when xmlChildrenNode == nullptr (if-branch [1] true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseFeatureParam_NoChildren_ReturnsGetChildFail, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;
    FeatureParamMapType featureMap;
    xmlNode node;
    node.xmlChildrenNode = nullptr;

    auto res = paramParse.ParseFeatureParam(featureMap, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: ParseFeatureParam_WithChildren_ReturnsSuccess
 * @tc.desc: Verify ParseFeatureParam when xmlChildrenNode != nullptr (if-branch [1] false,
 *           if-branch [3] false since ParseTunnelLayerInternal always returns SUCCESS)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseFeatureParam_WithChildren_ReturnsSuccess, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;
    FeatureParamMapType featureMap;

    xmlNode node;
    node.xmlChildrenNode = nullptr;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string childName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(childName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("NewTunnelEnabled"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    node.xmlChildrenNode = &childNode;

    auto res = paramParse.ParseFeatureParam(featureMap, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), true);

    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: ParseFeatureParam_NonElementChild_Skipped
 * @tc.desc: Verify ParseFeatureParam when child type != XML_ELEMENT_NODE (if-branch [2] true, skipped)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseFeatureParam_NonElementChild_Skipped, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;
    FeatureParamMapType featureMap;

    xmlNode node;
    node.xmlChildrenNode = nullptr;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;

    TunnelLayerParam::SetNewTunnelEnabled(false);

    auto res = paramParse.ParseFeatureParam(featureMap, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), false);

    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: ParseFeatureParam_ElementChild_Processed
 * @tc.desc: Verify ParseFeatureParam when child type == XML_ELEMENT_NODE (if-branch [2] false, processed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseFeatureParam_ElementChild_Processed, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;
    FeatureParamMapType featureMap;

    xmlNode node;
    node.xmlChildrenNode = nullptr;

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string childName = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(childName.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("NewTunnelEnabled"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    node.xmlChildrenNode = &childNode;

    TunnelLayerParam::SetNewTunnelEnabled(true);

    auto res = paramParse.ParseFeatureParam(featureMap, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), false);

    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: ParseTunnelLayerInternal_FeatureSwitch_Processed
 * @tc.desc: Verify ParseTunnelLayerInternal when xmlParamType == PARSE_XML_FEATURE_SWITCH (if-branch [4] true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseTunnelLayerInternal_FeatureSwitch_Processed, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("NewTunnelEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));

    TunnelLayerParam::SetNewTunnelEnabled(true);

    auto res = paramParse.ParseTunnelLayerInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), false);

    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: ParseTunnelLayerInternal_NonSwitch_Skipped
 * @tc.desc: Verify ParseTunnelLayerInternal when xmlParamType != PARSE_XML_FEATURE_SWITCH (if-branch [4] false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseTunnelLayerInternal_NonSwitch_Skipped, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSingleParam";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("NewTunnelEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));

    TunnelLayerParam::SetNewTunnelEnabled(true);

    auto res = paramParse.ParseTunnelLayerInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), true);
}

/**
 * @tc.name: ParseTunnelLayerInternal_MatchingName_SetsEnabled
 * @tc.desc: Verify ParseTunnelLayerInternal when name == "NewTunnelEnabled" (if-branch [5] true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseTunnelLayerInternal_MatchingName_SetsEnabled, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("NewTunnelEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));

    TunnelLayerParam::SetNewTunnelEnabled(true);

    auto res = paramParse.ParseTunnelLayerInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), false);

    TunnelLayerParam::SetNewTunnelEnabled(true);
}

/**
 * @tc.name: ParseTunnelLayerInternal_NonMatchingName_NoChange
 * @tc.desc: Verify ParseTunnelLayerInternal when name != "NewTunnelEnabled" (if-branch [5] false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TunnelLayerParamParseTest, ParseTunnelLayerInternal_NonMatchingName_NoChange, TestSize.Level1)
{
    TunnelLayerParamParse paramParse;

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("UnknownParam"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("false"));

    TunnelLayerParam::SetNewTunnelEnabled(true);

    auto res = paramParse.ParseTunnelLayerInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(TunnelLayerParam::IsNewTunnelEnabled(), true);
}
} // namespace OHOS::Rosen
