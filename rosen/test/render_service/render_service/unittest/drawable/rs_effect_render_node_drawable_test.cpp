/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "drawable/rs_effect_render_node_drawable.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSEffectRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectRenderNodeDrawableTest::SetUpTestCase() {}
void RSEffectRenderNodeDrawableTest::TearDownTestCase() {}
void RSEffectRenderNodeDrawableTest::SetUp() {}
void RSEffectRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateEffectRenderNodeDrawableTest
 * @tc.desc: Test If EffectRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST(RSEffectRenderNodeDrawableTest, CreateEffectRenderNodeDrawable, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    auto effectNode = std::make_shared<RSEffectRenderNode>(id, rsContext->weak_from_this());
    auto drawable = RSEffectRenderNodeDrawable::OnGenerate(effectNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST(RSEffectRenderNodeDrawableTest, OnCapture001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());

    CaptureParam params;
    drawable->renderParams_ = nullptr;
    params.isMirror_ = true;
    params.rootIdInWhiteList_ = INVALID_NODEID;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    RSUniRenderThread::SetCaptureParam(params);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
}
}
