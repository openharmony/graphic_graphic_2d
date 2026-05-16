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
#include "behind_window_filter_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BehindWindowFilterParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BehindWindowFilterParamParseTest::SetUpTestCase() {}
void BehindWindowFilterParamParseTest::TearDownTestCase() {}
void BehindWindowFilterParamParseTest::SetUp()
{
    BehindWindowFilterParam::SetBehindWindowFilterEnable(true);
}
void BehindWindowFilterParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParamTest001
 * @tc.desc: Verify ParseFeatureParam with empty node returns PARSE_GET_CHILD_FAIL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest001, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;

    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}

/**
 * @tc.name: ParseFeatureParamTest002
 * @tc.desc: Verify ParseFeatureParam enables BehindWindowBlur when xml value is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest002, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BehindWindowBlur"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}

/**
 * @tc.name: ParseFeatureParamTest003
 * @tc.desc: Verify ParseFeatureParam disables BehindWindowBlur when xml value is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest003, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BehindWindowBlur"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), false);
}

/**
 * @tc.name: ParseFeatureParamTest004
 * @tc.desc: Verify ParseFeatureParam does not change state when name is not matched
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest004, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("UnknownFeature"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}
/**
 * @tc.name: ParseFeatureParamTest005
 * @tc.desc: Verify ParseFeatureParam does not change state when xmlParamType is PARSE_XML_FEATURE_SINGLEPARAM
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest005, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSingleParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("UnknownSingleParam"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("100"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}

/**
 * @tc.name: ParseFeatureParamTest006
 * @tc.desc: Verify ParseFeatureParam does not change state when xmlParamType is neither SWITCH nor SINGLEPARAM
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BehindWindowFilterParamParseTest, ParseFeatureParamTest006, TestSize.Level1)
{
    BehindWindowFilterParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BehindWindowFilterConfig"] = std::make_shared<BehindWindowFilterParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureMultiParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("UnknownMultiParam"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BehindWindowFilterParam::IsBehindWindowFilterEnable(), true);
}
} // namespace OHOS::Rosen
