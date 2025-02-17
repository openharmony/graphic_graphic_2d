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
#include "limit_number.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsBaseRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSBaseRenderEngine> renderEngine_;
};
std::shared_ptr<RSBaseRenderEngine> RsBaseRenderEngineTest::renderEngine_ = RSMainThread::Instance()->GetRenderEngine();

void RsBaseRenderEngineTest::SetUpTestCase() {}

void RsBaseRenderEngineTest::TearDownTestCase()
{
    renderEngine_ = nullptr;
}

void RsBaseRenderEngineTest::SetUp() {}
void RsBaseRenderEngineTest::TearDown() {}

/**
 * @tc.name: TestRSBaseRenderEngine001
 * @tc.desc: NeedForceCPU test.
 * @tc.type: FUNC
 * @tc.require: issueI61BE3
 */
HWTEST_F(RsBaseRenderEngineTest, TestRSBaseRenderEngine01, TestSize.Level1)
{
    std::vector<LayerInfoPtr> layerInfo;
    renderEngine_->NeedForceCPU(layerInfo);
}

/**
 * @tc.name: TestRSBaseRenderEngine002
 * @tc.desc: SetColorFilterMode test.
 * @tc.type: FUNC
 * @tc.require: issueI61BE3
 */
HWTEST_F(RsBaseRenderEngineTest, TestRSBaseRenderEngine02, TestSize.Level1)
{
    ColorFilterMode defaultMode = renderEngine_->GetColorFilterMode();

    // enable invert mode
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_NORMAL_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::INVERT_COLOR_ENABLE_MODE);

    // disable invert mode
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_DISABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_NORMAL_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);

    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE);

    // these mode cannot be set directly
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);
    renderEngine_->SetColorFilterMode(ColorFilterMode::COLOR_FILTER_END);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);
    renderEngine_->SetColorFilterMode(static_cast<ColorFilterMode>(-1));
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::COLOR_FILTER_END);

    // enable invert mode
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_NORMAL_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::INVERT_COLOR_ENABLE_MODE);

    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE);
    ASSERT_EQ(renderEngine_->GetColorFilterMode(), ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE);

    // recover default mode
    renderEngine_->SetColorFilterMode(defaultMode);
}
} // namespace OHOS::Rosen
