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

#include <memory>

#include <gtest/gtest.h>
#include <test_header.h>

#include "hwc_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace {
const xmlChar NODE_NAME_SINGLE_PARAM[] = "FeatureSingleParam";

const xmlChar ATTRIBUTE_NAME[] = "name";
const xmlChar ATTRIBUTE_VALUE[] = "value";

const xmlChar TV_PLAYER_BUNDLE_NAME_KEY[] = "TvPlayerBundleName";
const xmlChar TV_PLAYER_BUNDLE_NAME_VALUE[] = "com.example.tvplayer";

const xmlChar COLLABORATION_BUNDLE_NAME_KEY[] = "CollaborationBundleName";
const xmlChar COLLABORATION_BUNDLE_NAME_VALUE[] = "com.huawei.hmos.devicecollaboration";

const xmlChar ATTRIBUTE_OTHERS[] = "Others";

xmlAttribute CreateXmlAttribute(const xmlChar* key, const xmlChar* value, xmlAttributePtr next)
{
    xmlAttribute attribute;
    attribute.type = XML_ATTRIBUTE_DECL;
    attribute.name = key;
    attribute.defaultValue = value;
    attribute.next = reinterpret_cast<xmlNodePtr>(next);
    return attribute;
}
}

namespace OHOS {
namespace Rosen {
class HwcParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HwcParamParseTest::SetUpTestCase() {}
void HwcParamParseTest::TearDownTestCase() {}
void HwcParamParseTest::SetUp() {}
void HwcParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureMultiParamForApp001
 * @tc.desc: Verify the ParseFeatureMultiParamForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamParseTest, ParseFeatureMultiParamForApp001, Function | SmallTest | Level1)
{
    HWCParamParse hwcParamParse;
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    std::string name;
    int32_t result = hwcParamParse.ParseFeatureMultiParamForApp(node, name);
    EXPECT_EQ(result, PARSE_GET_CHILD_FAIL);
}

/**
 * @tc.name: TestParseHwcInternal001
 * @tc.desc: Verify the ParseHwcInternal function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamParseTest, TestParseHwcInternal001, TestSize.Level1)
{
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    node.name = NODE_NAME_SINGLE_PARAM;
    node.type = XML_ELEMENT_NODE;

    xmlAttribute attrVal = CreateXmlAttribute(ATTRIBUTE_VALUE, TV_PLAYER_BUNDLE_NAME_VALUE, nullptr);
    xmlAttribute attrName = CreateXmlAttribute(ATTRIBUTE_NAME, TV_PLAYER_BUNDLE_NAME_KEY, &attrVal);
    node.properties = reinterpret_cast<xmlAttrPtr>(&attrName);

    FeatureParamMapType featureParam;
    featureParam["HwcConfig"] = std::make_shared<HWCParam>();
    HWCParamParse hwcParamParse;
    int32_t ret = hwcParamParse.ParseHwcInternal(featureParam, node);
    EXPECT_EQ(ret, PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: TestParseHwcInternal002
 * @tc.desc: Verify the ParseHwcInternal function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamParseTest, TestParseHwcInternal002, TestSize.Level1)
{
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    node.name = NODE_NAME_SINGLE_PARAM;
    node.type = XML_ELEMENT_NODE;

    xmlAttribute attrVal = CreateXmlAttribute(ATTRIBUTE_VALUE, TV_PLAYER_BUNDLE_NAME_VALUE, nullptr);
    xmlAttribute attrName = CreateXmlAttribute(ATTRIBUTE_NAME, ATTRIBUTE_OTHERS, &attrVal);
    node.properties = reinterpret_cast<xmlAttrPtr>(&attrName);

    FeatureParamMapType featureParam;
    featureParam["HwcConfig"] = std::make_shared<HWCParam>();
    HWCParamParse hwcParamParse;
    int32_t ret = hwcParamParse.ParseHwcInternal(featureParam, node);
    EXPECT_EQ(ret, PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: TestParseHwcInternal003
 * @tc.desc: Verify the ParseHwcInternal function
 * @tc.type: FUNC
 * @tc.require: issuesICKNNB
 */
HWTEST_F(HwcParamParseTest, TestParseHwcInternal003, TestSize.Level1)
{
    FeatureParamMapType featureParam;
    featureParam["HwcConfig"] = std::make_shared<HWCParam>();

    xmlNode node;
    node.xmlChildrenNode = nullptr;
    node.name = NODE_NAME_SINGLE_PARAM;
    node.type = XML_ELEMENT_NODE;
    xmlAttribute attrVal = CreateXmlAttribute(ATTRIBUTE_VALUE, COLLABORATION_BUNDLE_NAME_VALUE, nullptr);
    xmlAttribute attrName = CreateXmlAttribute(ATTRIBUTE_NAME, COLLABORATION_BUNDLE_NAME_KEY, &attrVal);
    node.properties = reinterpret_cast<xmlAttrPtr>(&attrName);

    HWCParamParse hwcParamParse;
    int32_t ret = hwcParamParse.ParseHwcInternal(featureParam, node);
    EXPECT_EQ(ret, PARSE_EXEC_SUCCESS);
}
}
}