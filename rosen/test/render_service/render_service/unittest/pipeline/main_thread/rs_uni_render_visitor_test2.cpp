/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#if defined(RS_ENABLE_UNI_RENDER)
#include <memory>

#include "common/rs_common_hook.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/mock/mock_matrix.h"
#include "pipeline/rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "consumer_surface.h"
#include "draw/color.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "monitor/self_drawing_node_monitor.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_union_render_node.h"
#include "pipeline/mock/mock_rs_luminance_control.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen.h"
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
}

namespace OHOS::Rosen {
class RSChildrenDrawableAdapter : public RSDrawable {
public:
    RSChildrenDrawableAdapter() = default;
    ~RSChildrenDrawableAdapter() override = default;
    bool OnUpdate(const RSRenderNode& content) override { return true; }
    void OnSync() override {}
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override {}

private:
    bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node) { return true; }
    friend class RSRenderNode;
};

class RSUniRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
    static sptr<RSScreenManager> screenManager_;
    static std::shared_ptr<AppExecFwk::EventRunner> runner_;
    static std::shared_ptr<AppExecFwk::EventHandler> handler_;
};


sptr<RSScreenManager> RSUniRenderVisitorTest::screenManager_ = sptr<RSScreenManager>::MakeSptr();
std::shared_ptr<AppExecFwk::EventRunner> RSUniRenderVisitorTest::runner_ = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> RSUniRenderVisitorTest::handler_ = nullptr;

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode) {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_METHOD1(CheckParticipateInOcclusion, bool(bool));
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_METHOD0(CheckIfOcclusionChanged, bool());
};

void RSUniRenderVisitorTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
    runner_ = AppExecFwk::EventRunner::Create(true);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    if (screenManager_ != nullptr) {
        screenManager_->Init(handler_);
    }
}

void RSUniRenderVisitorTest::TearDownTestCase()
{
    screenManager_->preprocessor_ = nullptr;
    runner_->Stop();
}

void RSUniRenderVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest001
 * @tc.desc: Test UpdateDrawingCacheInfoBeforeChildren while isDrawingCacheEnabled_ is false
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDrawingCacheInfoBeforeChildrenTest001, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(node, nullptr);

    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    visitor->UpdateDrawingCacheInfoBeforeChildren(*node);
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest002
 * @tc.desc: Test UpdateDrawingCacheInfoBeforeChildren while isDrawingCacheEnabled_ is true
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDrawingCacheInfoBeforeChildrenTest002, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(node, nullptr);
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoBeforeChildren(*node);
}

/**
 * @tc.name: RSUniRenderVisitorHelperMethodsTest001
 * @tc.desc: Test RSUniRenderVisitor helper methods
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, RSUniRenderVisitorHelperMethodsTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    uint16_t seq1 = visitor->IncreasePrepareSeq();
    uint16_t seq2 = visitor->IncreasePrepareSeq();
    ASSERT_EQ(seq2, seq1 + 1);

    uint16_t poseSeq1 = visitor->IncreasePostPrepareSeq();
    uint16_t poseSeq2 = visitor->IncreasePostPrepareSeq();
    ASSERT_EQ(poseSeq2, poseSeq1 + 1);
}

/**
 * @tc.name: MarkHardwareForcedDisabledTest001
 * @tc.desc: Test RSUniRenderVisitor MarkHardwareForcedDisabled
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkHardwareForcedDisabledTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    visitor->MarkHardwareForcedDisabled();
    ASSERT_FALSE(visitor->IsHardwareComposerEnabled());
}

/**
 * @tc.name: MarkFilterInForegroundFilterAndCheckNeedForceClearCacheTest001
 * @tc.desc: Test RSUniRenderVisitor MarkFilterInForegroundFilterAndCheckNeedForceClearCache
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkFilterInForegroundFilterAndCheckNeedForceClearCacheTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    constexpr NodeId nodeId = 1;
    RSEffectRenderNode effectNode(nodeId);
    visitor->MarkFilterInForegroundFilterAndCheckNeedForceClearCache(effectNode);
}

/**
 * @tc.name: RegisterHpaeCallbackTest001
 * @tc.desc: Test RSUniRenderVisitor RegisterHpaeCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, RegisterHpaeCallbackTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    constexpr NodeId nodeId = 1;
    RSRenderNode rsRenderNode(nodeId);
    visitor->RegisterHpaeCallback(rsRenderNode);
}
} // namespace OHOS::Rosen
#endif // RS_ENABLE_UNI_RENDER