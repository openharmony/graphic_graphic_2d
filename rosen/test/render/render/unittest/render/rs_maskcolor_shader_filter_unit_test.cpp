/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>

#include "inner_event.h"
#include "gtest/gtest.h"

#include "render/rs_maskcolor_shader_filter.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMaskColorShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskColorShaderFilterUnitTest::SetUpTestCase() {}
void RSMaskColorShaderFilterUnitTest::TearDownTestCase() {}
void RSMaskColorShaderFilterUnitTest::SetUp() {}
void RSMaskColorShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestCreateMaskColorShaderFilter
 * @tc.desc: Verify function MaskColorShaderFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaskColorShaderFilterUnitTest, TestCreateMaskColorShaderFilter, TestSize.Level1)
{
    auto colorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(0, RSColor());
    ASSERT_TRUE(colorShaderFilter != nullptr);
}
} // namespace Rosen
} // namespace OHOS