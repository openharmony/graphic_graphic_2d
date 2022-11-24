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

#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertiesTest::SetUpTestCase() {}
void RSPropertiesTest::TearDownTestCase() {}
void RSPropertiesTest::SetUp() {}
void RSPropertiesTest::TearDown() {}

/**
 * @tc.name: SetSublayerTransform001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetSublayerTransform001, TestSize.Level1)
{
    RSProperties properties;
    Matrix3f sublayerTransform;
    properties.SetSublayerTransform(sublayerTransform);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetBackgroundShader001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetBackgroundShader(RSShader::CreateRSShader());
}

/**
 * @tc.name: SetShadowColor001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowColor001, TestSize.Level1)
{
    RSProperties properties;
    RSColor color;
    properties.SetShadowColor(color);
}

/**
 * @tc.name: SetShadowOffsetX001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowOffsetX001, TestSize.Level1)
{
    RSProperties properties;
    float offsetX = 0.1f;
    properties.SetShadowOffsetX(offsetX);
}

/**
 * @tc.name: SetShadowOffsetY001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, SetShadowOffsetY001, TestSize.Level1)
{
    RSProperties properties;
    float offsetY = 0.1f;
    properties.SetShadowOffsetX(offsetY);
}

/**
 * @tc.name: Dump001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesTest, Dump001, TestSize.Level1)
{
    RSProperties properties;
    properties.Dump();
}

} // namespace Rosen
} // namespace OHOS