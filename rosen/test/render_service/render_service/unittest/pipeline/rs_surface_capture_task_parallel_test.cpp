#include <gtest/gtest.h>

#include "pipeline/rs_surface_capture_task_parallel.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceCaptureTaskParallelTest::SetUpTestCase() {}
void RSSurfaceCaptureTaskParallelTest::TearDownTestCase() {}
void RSSurfaceCaptureTaskParallelTest::SetUp() {}
void RSSurfaceCaptureTaskParallelTest::TearDown() {}

/*
 * @tc.name: FindSecurityOrSkipOrProtectedLayer
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: 
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, FindSecurityOrSkipOrProtectedLayer, TestSize.Level2)
{
    float scaleX = 0.f;
    float scaleY = 0.f;
    NodeId id = 1;
    RSSurfaceCaptureTaskParallel task(0, scaleX, scaleY);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    /* nullptr */
    nodeMap.surfaceNodeMap_[0] = nullptr;
    ASSERT_FALSE(task.FindSecurityOrSkipOrProtectedLayer());
    /* SetIsOnTheTree */
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    node1->SetIsOnTheTree(false);
    nodeMap.surfaceNodeMap_[0] = node1;
    ASSERT_FALSE(task.FindSecurityOrSkipOrProtectedLayer());
    /* isSecurityLayer_ */
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node2->SetIsOnTheTree(true);
    node2->isSecurityLayer_ = true;
    nodeMap.surfaceNodeMap_[0] = node2;
    ASSERT_TRUE(task.FindSecurityOrSkipOrProtectedLayer());
    /* isSkipLayer_ */
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id);
    node3->SetIsOnTheTree(true);
    node3->isSkipLayer_ = true;
    nodeMap.surfaceNodeMap_[0] = node3;
    ASSERT_TRUE(task.FindSecurityOrSkipOrProtectedLayer());
    /* isProtectedLayer_ */
    auto node4 = std::make_shared<RSSurfaceRenderNode>(id);
    node4->SetIsOnTheTree(true);
    node4->isProtectedLayer_ = true;
    nodeMap.surfaceNodeMap_[0] = node4;
    ASSERT_TRUE(task.FindSecurityOrSkipOrProtectedLayer());
    nodeMap.surfaceNodeMap_.clear();
}

/*
 * @tc.name: ScreenCorrection
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: 
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, ScreenCorrection, TestSize.Level2)
{
    float scaleX = 0.f;
    float scaleY = 0.f;
    int32_t rotation_0 = 0;
    int32_t rotation_90 = -90;
    int32_t rotation_180 = -180;
    int32_t rotation_270 = -270;
    RSSurfaceCaptureTaskParallel task(0, scaleX, scaleY);

    ASSERT_EQ(task.ScreenCorrection(ScreenRotation::ROTATION_0), rotation_0);
    ASSERT_EQ(task.ScreenCorrection(ScreenRotation::ROTATION_90), rotation_90);
    ASSERT_EQ(task.ScreenCorrection(ScreenRotation::ROTATION_180), rotation_180);
    ASSERT_EQ(task.ScreenCorrection(ScreenRotation::ROTATION_270), rotation_270);
}
}
}
