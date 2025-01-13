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

#include "graphic_ccm_feature_param_manager.h"

using namespace testing;
using namespace testing::ext;
std::set<std::string> featureSet = {"HdrConfig", "DrmConfig", "DvsyncConfig", "HwcConfig"};
namespace OHOS {
namespace Rosen {

class GraphicCcmFeatureParamManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GraphicCcmFeatureParamManagerTest::SetUpTestCase() {}
void GraphicCcmFeatureParamManagerTest::TearDownTestCase() {}
void GraphicCcmFeatureParamManagerTest::SetUp() {}
void GraphicCcmFeatureParamManagerTest::TearDown() {}

/**
 * @tc.name: Init
 * @tc.desc: Verify the GraphicCcmFeatureParamManager Init function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GraphicCcmFeatureParamManagerTest, Init, Function | SmallTest | Level1)
{
    GraphicCcmFeatureParamManager::GetInstance()->Init();
    auto parseMap = GraphicCcmFeatureParamManager::GetInstance()->featureParseMap;
    auto paramMap = GraphicCcmFeatureParamManager::GetInstance()->featureParamMap;
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
 * @tc.require:
 */
HWTEST_F(GraphicCcmFeatureParamManagerTest, FeatureParamParseEntry, Function | SmallTest | Level1)
{
    GraphicCcmFeatureParamManager ccmManager;
    ccmManager.featureParser_ = nullptr;
    ccmManager.FeatureParamParseEntry();
    ASSERT_NE(ccmManager.featureParser_, nullptr);
}

/**
 * @tc.name: GetFeatureParam
 * @tc.desc: Verify the result of GetFeatureParam function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GraphicCcmFeatureParamManagerTest, GetFeatureParam, Function | SmallTest | Level1)
{
    GraphicCcmFeatureParamManager::GetInstance()->Init();
    GraphicCcmFeatureParamManager::GetInstance()->FeatureParamParseEntry();
    std::shared_ptr<FeatureParam> featureParam = nullptr;
    for (const auto& feature : featureSet) {
        featureParam = GraphicCcmFeatureParamManager::GetInstance()->GetFeatureParam(feature);
        ASSERT_NE(featureParam, nullptr);
    }
}
} // namespace Rosen
} // namespace OHOS