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

#include "gtest/gtest.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "rs_multi_display_feature_param_parse.h"
#include "rs_multi_display_feature_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
static xmlDocPtr StringToXmlDoc(const std::string& xmlContent)
{
    return xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
}

class RSMultiDisplayFeatureParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMultiDisplayFeatureParamParseTest::SetUpTestCase() {}
void RSMultiDisplayFeatureParamParseTest::TearDownTestCase() {}
void RSMultiDisplayFeatureParamParseTest::SetUp() {}
void RSMultiDisplayFeatureParamParseTest::TearDown() {}

/**
 * @tc.name: ParseSplitScreenFeature_Enabled
 * @tc.desc: test ParseSplitScreenFeature with switch enabled and valid params
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseSplitScreenFeature_Enabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <split_screen_feature name="switch" value="true">
            <params mainDisplayId="0" subDisplayId="1"/>
        </split_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 1);
    auto it = result.params.find(0);
    EXPECT_NE(it, result.params.end());
    EXPECT_EQ(it->second.mainDisplayId, 0);
    EXPECT_EQ(it->second.subDisplayId, 1);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseSplitScreenFeature_Disabled
 * @tc.desc: test ParseSplitScreenFeature with switch disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseSplitScreenFeature_Disabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <split_screen_feature name="switch" value="false">
            <params mainDisplayId="0" subDisplayId="1"/>
        </split_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*root);

    EXPECT_EQ(result.enabled, false);
    EXPECT_EQ(result.params.size(), 1);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseSplitScreenFeature_InvalidParams
 * @tc.desc: test ParseSplitScreenFeature with non-numeric params (should skip)
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseSplitScreenFeature_InvalidParams, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <split_screen_feature name="switch" value="true">
            <params mainDisplayId="abc" subDisplayId="xyz"/>
        </split_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 0);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseSplitScreenFeature_MultipleParams
 * @tc.desc: test ParseSplitScreenFeature with multiple params entries
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseSplitScreenFeature_MultipleParams, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <split_screen_feature name="switch" value="true">
            <params mainDisplayId="0" subDisplayId="1"/>
            <params mainDisplayId="2" subDisplayId="3"/>
        </split_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 2);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseSplitScreenFeature_NoSwitchName
 * @tc.desc: test ParseSplitScreenFeature when name attribute is not "switch"
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseSplitScreenFeature_NoSwitchName, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <split_screen_feature name="other" value="true">
            <params mainDisplayId="0" subDisplayId="1"/>
        </split_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseSplitScreenFeature(*root);

    EXPECT_EQ(result.enabled, false);
    EXPECT_EQ(result.params.size(), 1);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInterpolationFeature_Enabled
 * @tc.desc: test ParseInterpolationFeature with switch enabled and valid params
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseInterpolationFeature_Enabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <interp_screen_feature name="switch" value="true">
            <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
        </interp_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 1);
    auto it = result.params.find(1);
    EXPECT_NE(it, result.params.end());
    EXPECT_EQ(it->second.displayId, 1);
    EXPECT_EQ(it->second.realWidth, 1280);
    EXPECT_EQ(it->second.realHeight, 720);
    EXPECT_EQ(it->second.paramA, 2);
    EXPECT_EQ(it->second.paramN, 3);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInterpolationFeature_Disabled
 * @tc.desc: test ParseInterpolationFeature with switch disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseInterpolationFeature_Disabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <interp_screen_feature name="switch" value="false">
            <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
        </interp_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(*root);

    EXPECT_EQ(result.enabled, false);
    EXPECT_EQ(result.params.size(), 1);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInterpolationFeature_InvalidParams
 * @tc.desc: test ParseInterpolationFeature with non-numeric params (should skip)
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseInterpolationFeature_InvalidParams, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <interp_screen_feature name="switch" value="true">
            <params displayId="abc" realWidth="xyz" realHeight="def" paramA="2" paramN="3"/>
        </interp_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 0);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseInterpolationFeature_MultipleParams
 * @tc.desc: test ParseInterpolationFeature with multiple params entries
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseInterpolationFeature_MultipleParams, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <interp_screen_feature name="switch" value="true">
            <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
            <params displayId="2" realWidth="1920" realHeight="1080" paramA="4" paramN="5"/>
        </interp_screen_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseInterpolationFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 2);
    EXPECT_NE(result.params.find(1), result.params.end());
    EXPECT_NE(result.params.find(2), result.params.end());

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseCrossDomainFeature_Enabled
 * @tc.desc: test ParseCrossDomainFeature with switch enabled and valid params
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseCrossDomainFeature_Enabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <cross_domain_feature name="switch" value="true">
            <params displayId="0"/>
            <params displayId="1"/>
        </cross_domain_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 2);
    EXPECT_NE(result.params.find(0), result.params.end());
    EXPECT_NE(result.params.find(1), result.params.end());

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseCrossDomainFeature_Disabled
 * @tc.desc: test ParseCrossDomainFeature with switch disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseCrossDomainFeature_Disabled, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <cross_domain_feature name="switch" value="false">
            <params displayId="0"/>
        </cross_domain_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(*root);

    EXPECT_EQ(result.enabled, false);
    EXPECT_EQ(result.params.size(), 1);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseCrossDomainFeature_InvalidParams
 * @tc.desc: test ParseCrossDomainFeature with non-numeric params (should skip)
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseCrossDomainFeature_InvalidParams, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <cross_domain_feature name="switch" value="true">
            <params displayId="abc"/>
        </cross_domain_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(*root);

    EXPECT_EQ(result.enabled, true);
    EXPECT_EQ(result.params.size(), 0);

    xmlFreeDoc(doc);
}

/**
 * @tc.name: ParseCrossDomainFeature_NoSwitchName
 * @tc.desc: test ParseCrossDomainFeature when name attribute is not "switch"
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiDisplayFeatureParamParseTest, ParseCrossDomainFeature_NoSwitchName, TestSize.Level1)
{
    std::string xmlContent = (R"(<?xml version="1.0" encoding="UTF-8"?>
        <cross_domain_feature name="other" value="true">
            <params displayId="0"/>
        </cross_domain_feature>)");
    xmlDocPtr doc = StringToXmlDoc(xmlContent);
    ASSERT_NE(doc, nullptr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    auto result = RSMultiDisplayFeatureParamParse::ParseCrossDomainFeature(*root);

    EXPECT_EQ(result.enabled, false);
    EXPECT_EQ(result.params.size(), 1);

    xmlFreeDoc(doc);
}
} // namespace OHOS::Rosen