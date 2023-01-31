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
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_base_render_engine.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSBaseRenderEngine> renderEngine_;
};
std::shared_ptr<RSBaseRenderEngine> RSBaseRenderEngineTest::renderEngine_ = RSMainThread::Instance()->GetRenderEngine();

void RSBaseRenderEngineTest::SetUpTestCase() {}

void RSBaseRenderEngineTest::TearDownTestCase()
{
    renderEngine_ = nullptr;
}

void RSBaseRenderEngineTest::SetUp() {}
void RSBaseRenderEngineTest::TearDown() {}

/**
 * @tc.name: TestRSBaseRenderEngine001
 * @tc.desc: NeedForceCPU test.
 * @tc.type: FUNC
 * @tc.require: issueI61BE3
 */
HWTEST_F(RSBaseRenderEngineTest, TestRSBaseRenderEngine001, TestSize.Level1)
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
HWTEST_F(RSBaseRenderEngineTest, TestRSBaseRenderEngine002, TestSize.Level1)
{
    std::vector<LayerInfoPtr> layerInfo;
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_DISABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_DISABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_NORMAL_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::DALTONIZATION_NORMAL_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE);
    renderEngine_->SetColorFilterMode(ColorFilterMode::COLOR_FILTER_END);
    renderEngine_->SetColorFilterMode(static_cast<ColorFilterMode>(-1));
    renderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_DISABLE_MODE);
}
} // namespace OHOS::Rosen
