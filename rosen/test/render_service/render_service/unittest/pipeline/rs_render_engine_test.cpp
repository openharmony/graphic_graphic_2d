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

#include "gtest/gtest.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    RSRenderEngine renderEngine_;
};

void RSRenderEngineTest::SetUpTestCase() {}
void RSRenderEngineTest::TearDownTestCase() {}
void RSRenderEngineTest::SetUp() {}
void RSRenderEngineTest::TearDown() {}

/**
 * @tc.name: SetColorFilterMode001
 * @tc.desc:SetColorFilterMode INVERT_COLOR_ENABLE_MODE
 * @tc.type:FUNC
 * @tc.require:issueI5NJLC
 */
HWTEST_F(RSRenderEngineTest, SetColorFilterMode001, TestSize.Level1)
{
    ColorFilterMode mode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    renderEngine_.SetColorFilterMode(mode);
    ASSERT_EQ(renderEngine_.GetColorFilterMode(), mode);

    renderEngine_.SetColorFilterMode(ColorFilterMode::COLOR_FILTER_END);
}

/**
 * @tc.name: SetColorFilterMode002
 * @tc.desc:SetColorFilterMode DALTONIZATION
 * @tc.type:FUNC
 * @tc.require:issueI5NM41
 */
HWTEST_F(RSRenderEngineTest, SetColorFilterMode002, TestSize.Level1)
{
    ColorFilterMode mode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
    renderEngine_.SetColorFilterMode(mode);
    ASSERT_EQ(renderEngine_.GetColorFilterMode(), mode);

    renderEngine_.SetColorFilterMode(ColorFilterMode::COLOR_FILTER_END);
}
} // namespace OHOS::Rosen
