/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <string>

#include "pipeline/rs_occlusion_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSOcclusionConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionConfigTest::SetUpTestCase() {}
void RSOcclusionConfigTest::TearDownTestCase() {}
void RSOcclusionConfigTest::SetUp() {}
void RSOcclusionConfigTest::TearDown() {}

/**
 * @tc.name: IsAlphaWindow001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionConfigTest, IsAlphaWindow001, TestSize.Level1)
{
    std::string win = "test";
    RSOcclusionConfig::GetInstance().IsAlphaWindow(win);

    std::string winTwo = "StatusBar";
    ASSERT_TRUE(RSOcclusionConfig::GetInstance().IsAlphaWindow(winTwo));
}

} // namespace Rosen
} // namespace OHOS
