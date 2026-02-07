#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <memory>
#include <string>

#include "rs_composer_client_manager.h"
#include "layer_backend/hdi_output.h"
#include "rs_composer_client.h"
#include "rs_composer_context.h"
#include "rs_surface_layer.h"
#include "rs_layer_transaction_data.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSComposerClientManagerTest : public Test {};

static std::shared_ptr<RSComposerClient> MakeClient()
{
    sptr<IRSRenderToComposerConnection> conn = nullptr; // keep nullptr to avoid remote interactions
    return std::make_shared<RSComposerClient>(conn);
}

/**
 * Function: AddGet_And_NoOverride
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client for screen
 *                  2. Add again with another client
 *                  3. Verify existing client retained
 */
HWTEST_F(RSComposerClientManagerTest, AddGet_And_NoOverride, TestSize.Level1)
{
    RSComposerClientManager mgr;
    ScreenId sid = 1001;
    auto client1 = MakeClient();
    mgr.AddComposerClient(sid, client1);
    ASSERT_NE(mgr.GetComposerClient(sid), nullptr);
    // Add again with a different client should be ignored (existing retained)
    auto client2 = MakeClient();
    mgr.AddComposerClient(sid, client2);
    auto got = mgr.GetComposerClient(sid);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got.get(), client1.get());
}

/**
 * Function: Delete_And_GetNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client for screen
 *                  2. Delete client
 *                  3. Get returns nullptr
 */
HWTEST_F(RSComposerClientManagerTest, Delete_And_GetNull, TestSize.Level1)
{
    RSComposerClientManager mgr;
    ScreenId sid = 1002;
    mgr.AddComposerClient(sid, MakeClient());
    ASSERT_NE(mgr.GetComposerClient(sid), nullptr);
    mgr.DeleteComposerClient(sid);
    EXPECT_EQ(mgr.GetComposerClient(sid), nullptr);
}

/**
 * Function: GetClient_NotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Query non-existent screen id
 *                  2. Expect nullptr path
 */
HWTEST_F(RSComposerClientManagerTest, GetClient_NotFound, TestSize.Level1)
{
    RSComposerClientManager mgr;
    EXPECT_EQ(mgr.GetComposerClient(9999), nullptr);
}

/**
 * Function: ClearRedrawGPUCompositionCache_EmptySet_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add multiple clients
 *                  2. Call ClearRedrawGPUCompositionCache with empty set
 *                  3. Ensure no crash
 */
HWTEST_F(RSComposerClientManagerTest, ClearRedrawGPUCompositionCache_EmptySet_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.AddComposerClient(1, MakeClient());
    mgr.AddComposerClient(2, MakeClient());
    std::unordered_set<uint64_t> ids; // empty
    mgr.ClearRedrawGPUCompositionCache(ids);
    EXPECT_NE(mgr.GetComposerClient(1), nullptr);
    EXPECT_NE(mgr.GetComposerClient(2), nullptr);
}

/**
 * Function: RenderFrameStart_EmptyAndNonEmpty_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call RenderFrameStart with empty client map
 *                  2. Add clients then call again
 *                  3. Ensure both paths execute
 */
HWTEST_F(RSComposerClientManagerTest, RenderFrameStart_EmptyAndNonEmpty_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.RenderFrameStart(0); // empty map path
    mgr.AddComposerClient(1, MakeClient());
    mgr.AddComposerClient(2, MakeClient());
    mgr.RenderFrameStart(123456789ULL); // non-empty path
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0);
}

/**
 * Function: CleanLayerBufferBySurfaceId_NoLayer_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client
 *                  2. Call CleanLayerBufferBySurfaceId with nodeId not present
 *                  3. Ensure no crash
 */
HWTEST_F(RSComposerClientManagerTest, CleanLayerBufferBySurfaceId_NoLayer_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 2001;
    mgr.AddComposerClient(sid, client);
    mgr.CleanLayerBufferBySurfaceId(/*surfaceId*/42ULL, /*nodeId*/777ULL);
    auto ctx = client->GetComposerContext();
    EXPECT_EQ(ctx->GetRSLayer(777ULL), nullptr);
    EXPECT_NE(mgr.GetComposerClient(sid), nullptr);
}

/**
 * Function: CleanLayerBufferBySurfaceId_WithLayer_Forward_CalledNoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client and create layer with matching nodeId
 *                  2. Call CleanLayerBufferBySurfaceId
 *                  3. Ensure forward path hits and no crash
 */
HWTEST_F(RSComposerClientManagerTest, CleanLayerBufferBySurfaceId_WithLayer_Forward_CalledNoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 2002;
    mgr.AddComposerClient(sid, client);

    // Create a layer with matching node id in the client's context so GetRSLayer(nodeId) != nullptr
    auto ctx = client->GetComposerContext();
    RSLayerId layerId = 3001ULL;
    auto layer = RSSurfaceLayer::Create(layerId, ctx);
    ASSERT_NE(layer, nullptr);
    layer->SetNodeId(layerId); // make NodeId equal RSLayerId to satisfy Dump filter logic later

    mgr.CleanLayerBufferBySurfaceId(/*surfaceId*/55ULL, /*nodeId*/layerId);
    EXPECT_NE(ctx->GetRSLayer(layerId), nullptr);
}

/**
 * Function: SetScreenBacklight_WithClientAndNoClient_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call SetScreenBacklight without client
 *                  2. Add client then call again
 *                  3. Ensure both paths run
 */
HWTEST_F(RSComposerClientManagerTest, SetScreenBacklight_WithClientAndNoClient_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.SetScreenBacklight(9999, 80); // no client path
    mgr.AddComposerClient(1, MakeClient());
    mgr.SetScreenBacklight(1, 90); // with client path
    EXPECT_EQ(mgr.GetComposerClient(9999), nullptr);
    EXPECT_NE(mgr.GetComposerClient(1), nullptr);
}

/**
 * Function: GetPipelineParam_NoClient_ReturnsDefault
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Query pipeline param for non-existent screen
 *                  2. Verify default-initialized return
 */
HWTEST_F(RSComposerClientManagerTest, GetPipelineParam_NoClient_ReturnsDefault, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto pp = mgr.GetPipelineParam(7777);
    // Default fields should be zero-initialized
    EXPECT_EQ(pp.frameTimestamp, 0u);
    EXPECT_EQ(pp.pendingScreenRefreshRate, 0u);
    EXPECT_FALSE(pp.hasGameScene);
}

/**
 * Function: UpdateAndGetPipelineParam_Forward
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client
 *                  2. Update pipeline param
 *                  3. Get pipeline param and verify
 */
HWTEST_F(RSComposerClientManagerTest, UpdateAndGetPipelineParam_Forward, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 4001;
    mgr.AddComposerClient(sid, client);

    PipelineParam param;
    param.frameTimestamp = 123456789ULL;
    param.pendingScreenRefreshRate = 75;
    param.hasGameScene = true;
    mgr.UpdatePipelineParam(sid, param);

    auto got = mgr.GetPipelineParam(sid);
    EXPECT_EQ(got.frameTimestamp, 123456789ULL);
    EXPECT_EQ(got.pendingScreenRefreshRate, 75u);
    EXPECT_TRUE(got.hasGameScene);
}

/**
 * Function: PreAllocProtectedFrameBuffers_Twice_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client
 *                  2. Call PreAllocProtectedFrameBuffers twice
 *                  3. Ensure internal short-circuit path handled
 */
HWTEST_F(RSComposerClientManagerTest, PreAllocProtectedFrameBuffers_Twice_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 5001;
    mgr.AddComposerClient(sid, client);
    sptr<SurfaceBuffer> buf; // nullptr acceptable for branch coverage
    mgr.PreAllocProtectedFrameBuffers(sid, buf);
    // Call again to hit RSComposerClient's internal early-return (already preallocated)
    mgr.PreAllocProtectedFrameBuffers(sid, buf);
    EXPECT_NE(mgr.GetComposerClient(sid), nullptr);
}

/**
 * Function: ReleaseLayerBuffers_CallbackInvoked
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Register release callback
 *                  2. Call ReleaseLayerBuffers
 *                  3. Verify callback invoked
 */
HWTEST_F(RSComposerClientManagerTest, ReleaseLayerBuffers_CallbackInvoked, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 6001;
    mgr.AddComposerClient(sid, client);

    bool called = false;
    mgr.RegisterOnReleaseLayerBuffersCB(sid, [&called](auto&, auto&, auto){ called = true; });

    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec;
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    mgr.ReleaseLayerBuffers(sid, timestampVec, releaseVec);

    EXPECT_TRUE(called);
}

/**
 * Function: GetMinAccumulatedBufferCount_EmptyAndNonEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call with empty client map (returns 0)
 *                  2. Add clients and verify minimum count
 */
HWTEST_F(RSComposerClientManagerTest, GetMinAccumulatedBufferCount_EmptyAndNonEmpty, TestSize.Level1)
{
    RSComposerClientManager mgr;
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0); // empty map path
    mgr.AddComposerClient(1, MakeClient());
    mgr.AddComposerClient(2, MakeClient());
    // With default clients, accumulated count resolves to 0
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0);
}

/**
 * Function: DumpSurfaceInfo_WithCommittedLayer_HasOutput
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client and create layer
 *                  2. Mark layer as last committed via ReleaseLayerBuffers
 *                  3. Dump and verify screen marker present
 */
HWTEST_F(RSComposerClientManagerTest, DumpSurfaceInfo_WithCommittedLayer_HasOutput, TestSize.Level1)
{
    RSComposerClientManager mgr;
    auto client = MakeClient();
    ScreenId sid = 7001;
    mgr.AddComposerClient(sid, client);

    // Create a layer and mark it as part of last commit via ReleaseLayerBuffers
    auto ctx = client->GetComposerContext();
    RSLayerId layerId = 8001ULL;
    auto layer = RSSurfaceLayer::Create(layerId, ctx);
    ASSERT_NE(layer, nullptr);
    layer->SetNodeId(layerId);
    layer->SetSurfaceName("ut-surface");

    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec;
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(layerId, nullptr, nullptr);
    mgr.ReleaseLayerBuffers(sid, timestampVec, releaseVec);

    std::string dump;
    mgr.DumpSurfaceInfo(dump);
    // Expect dump contains our screen and surface marker
    EXPECT_NE(dump.find("-- LayerInfo " + std::to_string(sid)), std::string::npos);
}

/**
 * Function: DumpCurrentFrameLayers_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add client
 *                  2. Call DumpCurrentFrameLayers
 *                  3. Ensure no crash
 */
HWTEST_F(RSComposerClientManagerTest, DumpCurrentFrameLayers_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.AddComposerClient(1, MakeClient());
    mgr.DumpCurrentFrameLayers();
    std::string dump;
    mgr.DumpSurfaceInfo(dump);
    EXPECT_NE(dump.find("-- LayerInfo 1"), std::string::npos);
}

/**
 * Function: ClearFrameBuffers_WithProtectedSet_SkipsProtected
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add multiple clients
 *                  2. Provide protected screen set containing one screen
 *                  3. Call ClearFrameBuffers and ensure protected skipped
 */
HWTEST_F(RSComposerClientManagerTest, ClearFrameBuffers_WithProtectedSet_SkipsProtected, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.AddComposerClient(1, MakeClient());
    mgr.AddComposerClient(2, MakeClient());
    std::unordered_set<ScreenId> protectedSet { 1 };
    mgr.ClearFrameBuffers(protectedSet);
    // Ensure clients remain valid and manager state intact
    EXPECT_NE(mgr.GetComposerClient(1), nullptr);
    EXPECT_NE(mgr.GetComposerClient(2), nullptr);
}

/**
 * Function: UpdatePipelineParam_NoClient_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call UpdatePipelineParam with unknown screenId
 *                  2. Verify no crash path
 */
HWTEST_F(RSComposerClientManagerTest, UpdatePipelineParam_NoClient_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    PipelineParam param;
    param.pendingScreenRefreshRate = 1;
    mgr.UpdatePipelineParam(9999, param);
    auto pp = mgr.GetPipelineParam(9999);
    EXPECT_EQ(pp.frameTimestamp, 0u);
    EXPECT_EQ(pp.pendingScreenRefreshRate, 0u);
    EXPECT_FALSE(pp.hasGameScene);
}

/**
 * Function: PreAllocProtectedFrameBuffers_NoClient_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call PreAllocProtectedFrameBuffers with unknown screenId
 *                  2. Ensure no crash path
 */
HWTEST_F(RSComposerClientManagerTest, PreAllocProtectedFrameBuffers_NoClient_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    sptr<SurfaceBuffer> buf;
    mgr.PreAllocProtectedFrameBuffers(8888, buf);
    EXPECT_EQ(mgr.GetComposerClient(8888), nullptr);
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0);
}

/**
 * Function: RegisterOnReleaseLayerBuffersCB_NoClient_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Register callback on unknown screenId
 *                  2. Ensure no crash path
 */
HWTEST_F(RSComposerClientManagerTest, RegisterOnReleaseLayerBuffersCB_NoClient_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.RegisterOnReleaseLayerBuffersCB(7777, [](auto&, auto&, auto){});
    EXPECT_EQ(mgr.GetComposerClient(7777), nullptr);
}

/**
 * Function: ReleaseLayerBuffers_NoClient_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call ReleaseLayerBuffers with unknown screenId
 *                  2. Ensure no crash path
 */
HWTEST_F(RSComposerClientManagerTest, ReleaseLayerBuffers_NoClient_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec;
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    mgr.ReleaseLayerBuffers(9999, timestampVec, releaseVec);
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0);
}

/**
 * Function: DumpSurfaceInfo_NullClient_Skips
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add a nullptr client for a screen
 *                  2. Call DumpSurfaceInfo and ensure it skips null client
 */
HWTEST_F(RSComposerClientManagerTest, DumpSurfaceInfo_NullClient_Skips, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.AddComposerClient(1234, std::shared_ptr<RSComposerClient>(nullptr));
    std::string dump;
    mgr.DumpSurfaceInfo(dump);
    // Should not append LayerInfo for null client
    EXPECT_EQ(dump.find("-- LayerInfo 1234"), std::string::npos);
}

/**
 * Function: DumpCurrentFrameLayers_NullClient_Skips
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Add nullptr client
 *                  2. Call DumpCurrentFrameLayers and ensure no crash
 */
HWTEST_F(RSComposerClientManagerTest, DumpCurrentFrameLayers_NullClient_Skips, TestSize.Level1)
{
    RSComposerClientManager mgr;
    mgr.AddComposerClient(4321, std::shared_ptr<RSComposerClient>(nullptr));
    mgr.DumpCurrentFrameLayers();
    EXPECT_EQ(mgr.GetComposerClient(4321), nullptr);
    std::string dump2;
    mgr.DumpSurfaceInfo(dump2);
    EXPECT_EQ(dump2.find("-- LayerInfo 4321"), std::string::npos);
}

/**
 * Function: ClearRedrawGPUCompositionCache_EmptyClientMap_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Call ClearRedrawGPUCompositionCache with empty client map
 *                  2. Ensure no crash path
 */
HWTEST_F(RSComposerClientManagerTest, ClearRedrawGPUCompositionCache_EmptyClientMap_NoCrash, TestSize.Level1)
{
    RSComposerClientManager mgr;
    std::unordered_set<uint64_t> ids;
    mgr.ClearRedrawGPUCompositionCache(ids);
    EXPECT_EQ(mgr.GetMinAccumulatedBufferCount(), 0);
}

} // namespace OHOS::Rosen

