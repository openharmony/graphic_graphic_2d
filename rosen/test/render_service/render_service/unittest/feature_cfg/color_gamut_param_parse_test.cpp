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
#include "color_gamut_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ColorGamutParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ColorGamutParamParseTest::SetUpTestCase() {}
void ColorGamutParamParseTest::TearDownTestCase() {}
void ColorGamutParamParseTest::SetUp()
{
    ColorGamutParam::SetForceSRGBOutput(false);
}
void ColorGamutParamParseTest::TearDown() {}

HWTEST_F(ColorGamutParamParseTest, ParseFeatureParamTest001, TestSize.Level1)
{
    ColorGamutParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["ColorGamutConfig"] = std::make_shared<ColorGamutParam>();
    xmlNode node;

    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(ColorGamutParam::IsForceSRGBOutputEnabled(), false);
}

HWTEST_F(ColorGamutParamParseTest, ParseFeatureParamTest002, TestSize.Level1)
{
    ColorGamutParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["ColorGamutConfig"] = std::make_shared<ColorGamutParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("ForceSRGBOutput"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(ColorGamutParam::IsForceSRGBOutputEnabled(), true);
}

HWTEST_F(ColorGamutParamParseTest, ParseFeatureParamTest003, TestSize.Level1)
{
    ColorGamutParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["ColorGamutConfig"] = std::make_shared<ColorGamutParam>();
    xmlNode node;
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;

    ColorGamutParam::SetForceSRGBOutput(true);
    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("ForceSRGBOutput"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(ColorGamutParam::IsForceSRGBOutputEnabled(), false);
}
} // namespace OHOS::Rosen
