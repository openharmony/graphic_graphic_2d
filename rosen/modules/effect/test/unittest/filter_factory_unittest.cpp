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

#include "filter_factory_unittest.h"
#include "filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: GetFilter001
 * @tc.desc: Get the corresponding Filter according to the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterFactoryUnittest, GetFilter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterFactoryUnittest GetFilter001 start";
    /**
     * @tc.steps: step1. Create a FilterFactory instance
     */

    FilterFactory filterFactory;
    /**
     * @tc.steps: step2. Call GetFilter to get a Filter pointer
     */
    auto input = filterFactory.GetFilter("Input");
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
}

/**
 * @tc.name: GetFilter002
 * @tc.desc: Get the corresponding Filter according to the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterFactoryUnittest, GetFilter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterFactoryUnittest GetFilter002 start";
    /**
     * @tc.steps: step1. Create a FilterFactory instance
     */

    FilterFactory filterFactory;
    /**
     * @tc.steps: step2. Call GetFilter to get a Filter pointer
     */
    auto output = filterFactory.GetFilter("Output");
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
}

/**
 * @tc.name: GetFilter003
 * @tc.desc: Get the corresponding Filter according to the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterFactoryUnittest, GetFilter003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterFactoryUnittest GetFilter003 start";
    /**
     * @tc.steps: step1. Create a FilterFactory instance
     */

    FilterFactory filterFactory;
    /**
     * @tc.steps: step2. Call GetFilter to get a Filter pointer
     */
    auto gaussianBlur = filterFactory.GetFilter("GaussianBlur");
    bool testResult = gaussianBlur != nullptr;
    EXPECT_TRUE(testResult);
}
} // namespace Rosen
} // namespace OHOS
