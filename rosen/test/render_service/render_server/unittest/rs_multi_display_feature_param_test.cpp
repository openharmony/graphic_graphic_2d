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

#include "rs_multi_display_feature_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string INVALID_CONFIG = "/sys_prod/etc/window/resources/invalid_config.xml";
}

class RSMultiDisplayFeatureParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMultiDisplayFeatureParamTest::SetUpTestCase() {}
void RSMultiDisplayFeatureParamTest::TearDownTestCase() {}
void RSMultiDisplayFeatureParamTest::SetUp() {}
void RSMultiDisplayFeatureParamTest::TearDown() {}

static xmlDocPtr StringToXmlDoc(const std::string& xmlContent)
{
    xmlDocPtr docPtr = xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
    return docPtr;
}

/**
 * @tc.name: TestLoad
 * @tc.desc: test RSMultiDisplayFeatureParam.Load
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestLoad, TestSize.Level1)
{
    EXPECT_EQ(RSMultiDisplayFeatureParam::Load(INVALID_CONFIG), false);
}

/**
 * @tc.name: TestParse
 * @tc.desc: test RSMultiDisplayFeatureParam.Parse
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestParse, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};

    std::string noRootElementXmlContent = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?><WrongRoot></WrongRoot>)");
    xmlDocPtr noRootElementXmlDocPtr = StringToXmlDoc(noRootElementXmlContent);
    EXPECT_NE(noRootElementXmlDocPtr, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*noRootElementXmlDocPtr), false);

    std::string xmlContent = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="true">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
                <interp_screen_feature name="switch" value="true">
                    <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
                </interp_screen_feature>
                <cross_domain_feature name="switch" value="true">
                    <params displayId="0"/>
                </cross_domain_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr = StringToXmlDoc(xmlContent);
    EXPECT_NE(xmlDocPtr, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr), true);

    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsInterpolationFeatureEnable(), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsCrossDomainFeatureEnable(), true);

    auto splitParams1 = RSMultiDisplayFeatureParam::GetSplitScreenParams(0);
    EXPECT_EQ(splitParams1.has_value(), true);
    EXPECT_EQ(splitParams1.value().mainDisplayId, 0);
    EXPECT_EQ(splitParams1.value().subDisplayId, 1);

    auto splitParams2 = RSMultiDisplayFeatureParam::GetSplitScreenParams(1);
    EXPECT_EQ(splitParams2.has_value(), false);

    auto interpolationParams1 = RSMultiDisplayFeatureParam::GetInterpolationParams(0);
    EXPECT_EQ(interpolationParams1.has_value(), false);

    auto interpolationParams2 = RSMultiDisplayFeatureParam::GetInterpolationParams(1);
    EXPECT_EQ(interpolationParams2.has_value(), true);
    EXPECT_EQ(interpolationParams2.value().displayId, 1);
    EXPECT_EQ(interpolationParams2.value().realWidth, 1280);
    EXPECT_EQ(interpolationParams2.value().realHeight, 720);
    EXPECT_EQ(interpolationParams2.value().paramA, 2);
    EXPECT_EQ(interpolationParams2.value().paramN, 3);

    EXPECT_EQ(RSMultiDisplayFeatureParam::IsScreenInCrossDomain(0), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsScreenInCrossDomain(1), false);
}

/**
 * @tc.name: TestIsSplitScreenFeatureEnable
 * @tc.desc: test RSMultiDisplayFeatureParam.IsSplitScreenFeatureEnable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestIsSplitScreenFeatureEnable, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent1 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="false">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr1 = StringToXmlDoc(xmlContent1);
    EXPECT_NE(xmlDocPtr1, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr1), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), false);

    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent2 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="true">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr2 = StringToXmlDoc(xmlContent2);
    EXPECT_NE(xmlDocPtr2, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr2), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), true);
}

/**
 * @tc.name: TestIsInterpolationFeatureEnable
 * @tc.desc: test RSMultiDisplayFeatureParam.IsInterpolationFeatureEnable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestIsInterpolationFeatureEnable, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent1 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="false">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
                <interp_screen_feature name="switch" value="true">
                    <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
                </interp_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr1 = StringToXmlDoc(xmlContent1);
    EXPECT_NE(xmlDocPtr1, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr1), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), false);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsInterpolationFeatureEnable(), false);

    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent2 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="true">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
                <interp_screen_feature name="switch" value="true">
                    <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
                </interp_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr2 = StringToXmlDoc(xmlContent2);
    EXPECT_NE(xmlDocPtr2, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr2), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsInterpolationFeatureEnable(), true);
}

/**
 * @tc.name: TestIsCrossDomainFeatureEnable
 * @tc.desc: test RSMultiDisplayFeatureParam.IsCrossDomainFeatureEnable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestIsCrossDomainFeatureEnable, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent1 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <cross_domain_feature name="switch" value="false">
                    <params displayId="0"/>
                </cross_domain_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr1 = StringToXmlDoc(xmlContent1);
    EXPECT_NE(xmlDocPtr1, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr1), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsCrossDomainFeatureEnable(), false);

    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent2 = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <cross_domain_feature name="switch" value="true">
                    <params displayId="0"/>
                </cross_domain_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr2 = StringToXmlDoc(xmlContent2);
    EXPECT_NE(xmlDocPtr2, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr2), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsCrossDomainFeatureEnable(), true);
}

/**
 * @tc.name: TestGetSplitScreenParams
 * @tc.desc: test RSMultiDisplayFeatureParam.GetSplitScreenParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestGetSplitScreenParams, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="true">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr = StringToXmlDoc(xmlContent);
    EXPECT_NE(xmlDocPtr, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsSplitScreenFeatureEnable(), true);
    auto splitParams = RSMultiDisplayFeatureParam::GetSplitScreenParams(0);
    EXPECT_EQ(splitParams.has_value(), true);
    EXPECT_EQ(splitParams.value().mainDisplayId, 0);
    EXPECT_EQ(splitParams.value().subDisplayId, 1);
}

/**
 * @tc.name: TestGetInterpolationParams
 * @tc.desc: test RSMultiDisplayFeatureParam.GetInterpolationParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestGetInterpolationParams, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <split_screen_feature name="switch" value="true">
                    <params mainDisplayId="0" subDisplayId="1"/>
                </split_screen_feature>
                <interp_screen_feature name="switch" value="true">
                    <params displayId="1" realWidth="1280" realHeight="720" paramA="2" paramN="3"/>
                </interp_screen_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr = StringToXmlDoc(xmlContent);
    EXPECT_NE(xmlDocPtr, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr), true);
    auto interpolationParams = RSMultiDisplayFeatureParam::GetInterpolationParams(1);
    EXPECT_EQ(interpolationParams.has_value(), true);
    EXPECT_EQ(interpolationParams.value().displayId, 1);
    EXPECT_EQ(interpolationParams.value().realWidth, 1280);
    EXPECT_EQ(interpolationParams.value().realHeight, 720);
    EXPECT_EQ(interpolationParams.value().paramA, 2);
    EXPECT_EQ(interpolationParams.value().paramN, 3);
}

/**
 * @tc.name: TestIsScreenInCrossDomain
 * @tc.desc: test RSMultiDisplayFeatureParam.IsScreenInCrossDomain
  * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayFeatureParamTest, TestIsScreenInCrossDomain, TestSize.Level1)
{
    RSMultiDisplayFeatureParam::splitFeature_ = {};
    RSMultiDisplayFeatureParam::interpolationFeature_ = {};
    RSMultiDisplayFeatureParam::crossDomainFeature_ = {};
    std::string xmlContent = (
        R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        <Configs>
            <multi_display_features>
                <cross_domain_feature name="switch" value="true">
                    <params displayId="0"/>
                </cross_domain_feature>
            </multi_display_features>
        </Configs>)"
    );
    xmlDocPtr xmlDocPtr = StringToXmlDoc(xmlContent);
    EXPECT_NE(xmlDocPtr, nullptr);
    EXPECT_EQ(RSMultiDisplayFeatureParam::Parse(*xmlDocPtr), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsScreenInCrossDomain(0), true);
    EXPECT_EQ(RSMultiDisplayFeatureParam::IsScreenInCrossDomain(1), false);
}
} // namespace OHOS::Rosen