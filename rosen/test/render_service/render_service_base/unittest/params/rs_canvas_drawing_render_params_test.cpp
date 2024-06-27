/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "params/rs_canvas_drawing_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSCanvasDrawingRenderParamsTest::SetUpTestCase() {}
void RSCanvasDrawingRenderParamsTest::TearDownTestCase() {}
void RSCanvasDrawingRenderParamsTest::SetUp() {}
void RSCanvasDrawingRenderParamsTest::TearDown() {}
void RSCanvasDrawingRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSCanvasDrawingRenderParams params(id);
    params.OnSync(target);
    EXPECT_FALSE(params.isNeedProcess_);
}

/**
 * @tc.name: OnSync002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, OnSync002, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSCanvasDrawingRenderParams>(id);
    auto targetDrawingRenderParam = static_cast<RSCanvasDrawingRenderParams*>(target.get());
    RSCanvasDrawingRenderParams params(id);
    params.isNeedProcess_ = true;
    params.OnSync(target);
    EXPECT_EQ(params.isNeedProcess_, targetDrawingRenderParam->isNeedProcess_);
}

} // namespace OHOS::Rosen