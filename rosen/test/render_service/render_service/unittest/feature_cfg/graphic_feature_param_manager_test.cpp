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

#include "graphic_feature_param_manager.h"

using namespace testing;
using namespace testing::ext;
std::set<std::string> featureSet = {"ColorGamutConfig", "DrmConfig", "HwcConfig"};
namespace OHOS {
namespace Rosen {

class GraphicFeatureParamManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GraphicFeatureParamManagerTest::SetUpTestCase() {}
void GraphicFeatureParamManagerTest::TearDownTestCase() {}
void GraphicFeatureParamManagerTest::SetUp() {}
void GraphicFeatureParamManagerTest::TearDown() {}

/**
 * @tc.name: Init
 * @tc.desc: Verify the GraphicFeatureParamManager Init function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(GraphicFeatureParamManagerTest, Init, Function | SmallTest | Level1)
{
    GraphicFeatureParamManager::GetInstance().Init();
    auto parseMap = GraphicFeatureParamManager::GetInstance().featureParseMap_;
    auto paramMap = GraphicFeatureParamManager::GetInstance().featureParamMap_;
    for (const auto& feature : featureSet) {
        // Check if featureParseMap is initialized correctly
        EXPECT_NE(parseMap.count(feature), 0);
        // Check if featureParamMap is initialized correctly
        EXPECT_NE(paramMap.count(feature), 0);
    }
}

/**
 * @tc.name: FeatureParamParseEntry
 * @tc.desc: Verify the FeatureParamParseEntry function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(GraphicFeatureParamManagerTest, FeatureParamParseEntry, Function | SmallTest | Level1)
{
    GraphicFeatureParamManager featureManager;
    featureManager.featureParser_ = nullptr;
    featureManager.FeatureParamParseEntry();
    ASSERT_NE(featureManager.featureParser_, nullptr);
}

/**
 * @tc.name: GetFeatureParam
 * @tc.desc: Verify the result of GetFeatureParam function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(GraphicFeatureParamManagerTest, GetFeatureParam, Function | SmallTest | Level1)
{
    GraphicFeatureParamManager::GetInstance().Init();
    GraphicFeatureParamManager::GetInstance().FeatureParamParseEntry();
    std::shared_ptr<FeatureParam> featureParam = nullptr;
    for (const auto& feature : featureSet) {
        featureParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(feature);
        ASSERT_NE(featureParam, nullptr);
    }
}
} // namespace Rosen
} // namespace OHOS