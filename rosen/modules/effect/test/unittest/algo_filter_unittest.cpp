/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "algo_filter_unittest.h"
#include "filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: GetFilterTypeTest001
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, GetFilterTypeTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest GetFilterTypeTest001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto gaussian = filterFactory.GetFilter("GaussianBlur");
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    auto type = gaussian->GetFilterType();
    EXPECT_EQ(type, FILTER_TYPE::ALGOFILTER);
}

/**
 * @tc.name: GetFilterTypeTest002
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, GetFilterTypeTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest GetFilterTypeTest002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto horizon = filterFactory.GetFilter("HorizontalBlur");
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    auto type = horizon->GetFilterType();
    EXPECT_EQ(type, FILTER_TYPE::ALGOFILTER);
}

/**
 * @tc.name: GetFilterTypeTest003
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, GetFilterTypeTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest GetFilterTypeTest003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto brightness = filterFactory.GetFilter("Brightness");
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    auto type = brightness->GetFilterType();
    EXPECT_EQ(type, FILTER_TYPE::ALGOFILTER);
}

/**
 * @tc.name: SetValue001
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto brightness = filterFactory.GetFilter("Brightness");
    bool testResult = brightness != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sBright = std::make_shared<float>(0.5f);
    std::weak_ptr<void> vBright = sBright;
    brightness->SetValue("brightness", vBright.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto saturation = filterFactory.GetFilter("Saturation");
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sSaturation = std::make_shared<float>(0.5f);
    std::weak_ptr<void> vSaturation = sSaturation;
    saturation->SetValue("saturation", vSaturation.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(AlgoFilterUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AlgoFilterUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto contrast = filterFactory.GetFilter("Contrast");
    bool testResult = contrast != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sContrast = std::make_shared<float>(0.6f);
    std::weak_ptr<void> vContrast = sContrast;
    contrast->SetValue("contrast", vContrast.lock(), 1);
}
} // namespace Rosen
} // namespace OHOS
