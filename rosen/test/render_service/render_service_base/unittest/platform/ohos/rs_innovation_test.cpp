/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/common/rs_innovation.h"
#include <dlfcn.h>
#include <parameters.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInnovationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInnovationTest::SetUpTestCase() {}
void RSInnovationTest::TearDownTestCase() {}
void RSInnovationTest::SetUp() {}
void RSInnovationTest::TearDown() {}

/**
 * @tc.name: CloseInnovationSoTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInnovationTest, CloseInnovationSoTest, TestSize.Level1)
{
    RSInnovation::OpenInnovationSo(); 
    RSInnovation::CloseInnovationSo();
    ASSERT_EQ(RSInnovation::innovationHandle, NULL);
    int value = 1;
    RSInnovation::innovationHandle = &value;
    RSInnovation::CloseInnovationSo();
    ASSERT_NE(RSInnovation::innovationHandle, NULL);
}

/**
 * @tc.name: GetParallelCompositionEnabledTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInnovationTest, GetParallelCompositionEnabledTest, TestSize.Level1)
{
    bool isUniRender = true;
    RSInnovation::GetParallelCompositionEnabled(isUniRender);
    isUniRender = false;
    RSInnovation::GetParallelCompositionEnabled(isUniRender);
    ASSERT_EQ(std::atoi((system::GetParameter("rosen.parallelcomposition.enabled", "0")).c_str()), 0);
}

/**
 * @tc.name: ResetParallelCompositionFuncTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInnovationTest, ResetParallelCompositionFuncTest, TestSize.Level1)
{
    RSInnovation::OpenInnovationSo();
    ASSERT_NE(RSInnovation::innovationHandle, NULL);
    RSInnovation::_s_parallelCompositionLoaded = true;
    RSInnovation::CloseInnovationSo();
}
} // namespace Rosen
} // namespace OHOS