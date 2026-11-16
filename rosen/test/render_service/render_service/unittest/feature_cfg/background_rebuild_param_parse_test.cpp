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
#include "background_rebuild_param_parse.h"
#include "pipeline/rs_background_rebuild_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BackgroundRebuildParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BackgroundRebuildParamParseTest::SetUpTestCase() {}
void BackgroundRebuildParamParseTest::TearDownTestCase() {}
void BackgroundRebuildParamParseTest::SetUp() {}
void BackgroundRebuildParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest001
 * @tc.desc: Verify ParseFeatureParam with invalid param map type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseFeatureParamTest001, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    FeatureParamMapType invalidParamMapType;
    xmlNode node;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    auto res = paramParse.ParseFeatureParam(invalidParamMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: ParseFeatureParamTest002
 * @tc.desc: Verify ParseFeatureParam with featureParam
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseFeatureParamTest002, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BackgroundRebuildConfig"] = std::make_shared<BackgroundRebuildParam>();

    xmlNode node;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: ParseFeatureParamTest003
 * @tc.desc: Verify ParseFeatureParam with childNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseFeatureParamTest003, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BackgroundRebuildConfig"] = std::make_shared<BackgroundRebuildParam>();

    xmlNode node;
    node.xmlChildrenNode = nullptr;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;

    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: ParseFeatureParamTest004
 * @tc.desc: Verify ParseFeatureParam with nextNode and true value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseFeatureParamTest004, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BackgroundRebuildConfig"] = std::make_shared<BackgroundRebuildParam>();

    xmlNode node;
    node.xmlChildrenNode = nullptr;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    childNode.next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BackgroundRebuildEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), true);
}

/**
 * @tc.name: ParseFeatureParamTest005
 * @tc.desc: Verify ParseFeatureParam with nextNode and false value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseFeatureParamTest005, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BackgroundRebuildConfig"] = std::make_shared<BackgroundRebuildParam>();

    xmlNode node;
    node.xmlChildrenNode = nullptr;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    childNode.next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BackgroundRebuildEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: ParseBackgroundRebuildInternalUnsupportedName
 * @tc.desc: Verify Parse BackgroundRebuild Internal Unsupported Name
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseBackgroundRebuildInternalUnsupportedName, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSwitch";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("UnsupportedName"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("true"));

    auto res = paramParse.ParseBackgroundRebuildInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: ParseBackgroundRebuildInternalUnsupportedXmlType
 * @tc.desc: Verify Parse BackgroundRebuild Internal Unsupported Xml Type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundRebuildParamParseTest, ParseBackgroundRebuildInternalUnsupportedXmlType, TestSize.Level1)
{
    BackgroundRebuildParamParse paramParse;
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);

    xmlNode node;
    node.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSingleParam";
    node.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&node, (const xmlChar*)("name"), (const xmlChar*)("BackgroundRebuildEnabled"));
    xmlSetProp(&node, (const xmlChar*)("value"), (const xmlChar*)("true"));

    auto res = paramParse.ParseBackgroundRebuildInternal(node);
    EXPECT_EQ(res, ParseErrCode::PARSE_ERROR);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}
} // namespace OHOS::Rosen
