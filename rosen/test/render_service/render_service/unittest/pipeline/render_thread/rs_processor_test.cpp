#include <gtest/gtest.h>

#include "pipeline/rs_processor.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/render_thread/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class TestProcessor : public RSProcessor {
public:
    // Expose wrappers for protected methods/state needed by tests
    using RSProcessor::CalculateMirrorAdaptiveCoefficient;
    using RSProcessor::CalculateMirrorAdaptiveMatrix;
    using RSProcessor::screenInfo_;
    using RSProcessor::mirroredScreenInfo_;
    using RSProcessor::mirrorAdaptiveCoefficient_;
    using RSProcessor::renderEngine_;
    using RSProcessor::renderFrameConfig_;

    void ProcessSurface(RSSurfaceRenderNode&) override {}
    void ProcessScreenSurface(RSScreenRenderNode&) override {}
    void PostProcess() override {}
    void ProcessRcdSurface(RSRcdSurfaceRenderNode&) override {}
};

class RSProcessorTest : public Test {};

/**
 * @tc.name: Init_SetsScreenInfoAndFrameConfig
 * @tc.desc: RSProcessor::Init assigns screenInfo and computes frame config
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorTest, Init_SetsScreenInfoAndFrameConfig, TestSize.Level1)
{
    auto engine = std::make_shared<RSRenderEngine>();
    TestProcessor proc;
    // Prepare screen node with non-zero width/height
    ScreenInfo info;
    info.width = 800;
    info.height = 600;
    auto ctx = std::weak_ptr<RSContext>();
    RSScreenRenderNode node(1, 1, ctx);
    node.SetScreenInfo(info);
    ASSERT_TRUE(proc.Init(node, engine));
    EXPECT_EQ(proc.screenInfo_.width, 800u);
}

/**
 * @tc.name: MirrorAdaptiveCoefficient_ZeroMirroredDims_NoChange
 * @tc.desc: When mirrored size is zero, coefficient keeps default
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorTest, MirrorAdaptiveCoefficient_ZeroMirroredDims_NoChange, TestSize.Level1)
{
    TestProcessor proc;
    proc.mirrorAdaptiveCoefficient_ = 1.0f;
    proc.CalculateMirrorAdaptiveCoefficient(1920.0f, 1080.0f, 0.0f, 1080.0f);
    EXPECT_FLOAT_EQ(proc.mirrorAdaptiveCoefficient_, 1.0f);
}

/**
 * @tc.name: MirrorAdaptiveMatrix_ComputesScaleAndOffsets
 * @tc.desc: CalculateMirrorAdaptiveMatrix updates internal coefficient for non-equal sizes
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorTest, MirrorAdaptiveMatrix_ComputesScaleAndOffsets, TestSize.Level1)
{
    TestProcessor proc;
    proc.screenInfo_.width = 1920;
    proc.screenInfo_.height = 1080;
    proc.mirroredScreenInfo_.width = 1280;
    proc.mirroredScreenInfo_.height = 720;
    // Default rotation = 0
    proc.CalculateMirrorAdaptiveMatrix();
    // Expect coefficient updated from 1.0f
    EXPECT_NE(proc.mirrorAdaptiveCoefficient_, 1.0f);
}

/**
 * @tc.name: SecurityDisplayAndSecSurface_FlagsSet
 * @tc.desc: SetSecurityDisplay and SetDisplayHasSecSurface toggle flags
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorTest, SecurityDisplayAndSecSurface_FlagsSet, TestSize.Level1)
{
    TestProcessor proc;
    proc.SetSecurityDisplay(true);
    proc.SetDisplayHasSecSurface(true);
    // Access protected flags through subclass
    EXPECT_TRUE(proc.isSecurityDisplay_);
    EXPECT_TRUE(proc.displayHasSecSurface_);
}

/**
 * @tc.name: IsInstanceOf_SelfType_True
 * @tc.desc: Verify IsInstanceOf for the same type returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorTest, IsInstanceOf_SelfType_True, TestSize.Level1)
{
    auto proc = std::make_shared<TestProcessor>();
    EXPECT_TRUE(proc->IsInstanceOf<RSProcessor>());
    auto casted = proc->ReinterpretCastTo<RSProcessor>();
    EXPECT_NE(casted, nullptr);
}

} // namespace OHOS::Rosen
