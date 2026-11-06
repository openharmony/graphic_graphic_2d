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
#include <test_header.h>

#include "ddgr_config_param.h"
#include "graphic_feature_param_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DDGRConfigParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DDGRConfigParamTest::SetUpTestCase() {}
void DDGRConfigParamTest::TearDownTestCase() {}
void DDGRConfigParamTest::SetUp() {}
void DDGRConfigParamTest::TearDown() {}

/**
 * @tc.name: SetParam
 * @tc.desc: Verify the SetParam function inserts a new param
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamTest, SetParam, Function | SmallTest | Level1)
{
    DDGRConfigParam::SetParam("ddgr_param_test_key1", "value1");
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_param_test_key1"), 1);
    ASSERT_EQ(params["ddgr_param_test_key1"], "value1");
}

/**
 * @tc.name: SetParamTwice
 * @tc.desc: Verify the SetParam function overwrites the same key
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamTest, SetParamTwice, Function | SmallTest | Level1)
{
    DDGRConfigParam::SetParam("ddgr_param_test_key2", "value1");
    DDGRConfigParam::SetParam("ddgr_param_test_key2", "value2");
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_param_test_key2"), 1);
    ASSERT_EQ(params["ddgr_param_test_key2"], "value2");
}

/**
 * @tc.name: SetParamEmptyValue
 * @tc.desc: Verify the SetParam function keeps an empty value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamTest, SetParamEmptyValue, Function | SmallTest | Level1)
{
    DDGRConfigParam::SetParam("ddgr_param_test_key3", "");
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_param_test_key3"), 1);
    ASSERT_EQ(params["ddgr_param_test_key3"], "");
}

/**
 * @tc.name: GetParams
 * @tc.desc: Verify the GetParams function returns all params that were set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamTest, GetParams, Function | SmallTest | Level1)
{
    DDGRConfigParam::SetParam("ddgr_param_test_key4", "value4");
    DDGRConfigParam::SetParam("ddgr_param_test_key5", "value5");
    auto params = DDGRConfigParam::GetParams();
    ASSERT_EQ(params.count("ddgr_param_test_key4"), 1);
    ASSERT_EQ(params["ddgr_param_test_key4"], "value4");
    ASSERT_EQ(params.count("ddgr_param_test_key5"), 1);
    ASSERT_EQ(params["ddgr_param_test_key5"], "value5");

    // GetParams returns a copy, modifying it must not affect the stored params
    params["ddgr_param_test_key4"] = "modified";
    params.erase("ddgr_param_test_key5");
    auto storedParams = DDGRConfigParam::GetParams();
    ASSERT_EQ(storedParams["ddgr_param_test_key4"], "value4");
    ASSERT_EQ(storedParams.count("ddgr_param_test_key5"), 1);
}

/**
 * @tc.name: FeatureConfigRegistered
 * @tc.desc: Verify DDGRConfig is registered in GraphicFeatureParamManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DDGRConfigParamTest, FeatureConfigRegistered, Function | SmallTest | Level1)
{
    ASSERT_EQ(FEATURE_CONFIGS[FeatureModule::DDGR_CONFIG], "DDGRConfig");
    GraphicFeatureParamManager::GetInstance().Init();
    auto parseMap = GraphicFeatureParamManager::GetInstance().featureParseMap_;
    auto paramMap = GraphicFeatureParamManager::GetInstance().featureParamMap_;
    ASSERT_EQ(parseMap.count("DDGRConfig"), 1);
    ASSERT_EQ(paramMap.count("DDGRConfig"), 1);
    ASSERT_NE(GraphicFeatureParamManager::GetInstance().GetFeatureParam("DDGRConfig"), nullptr);
}
} // namespace Rosen
} // namespace OHOS
