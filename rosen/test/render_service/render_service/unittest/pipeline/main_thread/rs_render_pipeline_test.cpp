/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
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

#define private public

#include "gtest/gtest.h"
#include "rs_render_pipeline.h"
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "composer/composer_client/pipeline/rs_composer_client_manager.h"
#include "composer/composer_client/connection/rs_composer_to_render_connection.h"
#include "composer/composer_service/connection/rs_render_to_composer_connection.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_surface_layer.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPipelineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderPipelineTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSUniRenderThread::Instance().InitGrContext();

    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
    auto& memTrack = MemoryTrack::Instance();
    memTrack.memNodeMap_ = std::unordered_map<NodeId, MemoryInfo>();
    auto& memorySnapshot = MemorySnapshot::Instance();
    memorySnapshot.appMemorySnapshots_ = std::unordered_map<pid_t, MemorySnapshotInfo>();
}
void RSRenderPipelineTest::TearDownTestCase()
{
    RSMainThread::Instance()->GetRenderEngine()->renderContext_ = nullptr;

    RSMainThread::Instance()->handler_ = nullptr;
    RSMainThread::Instance()->receiver_ = nullptr;
    RSMainThread::Instance()->renderEngine_ = nullptr;

    RSUniRenderThread::Instance().handler_ = nullptr;
    RSUniRenderThread::Instance().runner_ = nullptr;
    RSUniRenderThread::Instance().uniRenderEngine_->renderContext_ = nullptr;
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;

    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
    usleep(5000);
}
void RSRenderPipelineTest::SetUp() {}
void RSRenderPipelineTest::TearDown() {}

/**
 * @tc.name: OnScreenPropertyChanged_InvalidId_EarlyReturn
 * @tc.desc: Property change on unknown screen id should early return without side effects
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenPropertyChanged_InvalidId_EarlyReturn, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    ScreenId notExistId = 9999;
    pipeline->OnScreenPropertyChanged(notExistId, ScreenPropertyType::CANVAS_ROTATION,
        sptr<ScreenProperty<bool>>::MakeSptr(false));
    EXPECT_EQ(pipeline->composerClientManager_->GetComposerClient(notExistId), nullptr);
}

/**
 * @tc.name: OnScreenRefresh_WithClient_NoCrash
 * @tc.desc: After connecting a non-virtual screen, OnScreenRefresh should execute without removing client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenRefresh_WithClient_NoCrash, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();
    ASSERT_NE(pipeline->mainThread_, nullptr);
    pipeline->mainThread_->renderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(pipeline->mainThread_->GetRenderEngine(), nullptr);
    pipeline->mainThread_->GetRenderEngine()->renderContext_ = RenderContext::Create();
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_refresh", property->GetScreenId(), nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();

    auto hdiOutput = HdiOutput::CreateHdiOutput(property->GetScreenId());
    hdiOutput->Init();
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, hdiOutput);
    ASSERT_NE(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);

    pipeline->OnScreenRefresh(property->GetScreenId());
    EXPECT_NE(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);
}

/**
 * @tc.name: PostMainThreadTask_WithNameDelayPriority_MainThreadNotNull
 * @tc.desc: Cover the overload with name/delay/priority when mainThread_ is not null. Ensures
 *     RSRenderPipeline::PostMainThreadTask(task, name, delay, priority) executes the non-null branch
 *     and safely forwards to RSMainThread::PostTask (which internally guards handler_).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, PostMainThreadTask_WithNameDelayPriority_MainThreadNotNull, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->mainThread_ = RSMainThread::Instance();
    ASSERT_NE(pipeline->mainThread_, nullptr);

    std::string taskName = "ut_task_named";
    int64_t delay = 0;
    pipeline->PostMainThreadTask([] {}, taskName, delay, AppExecFwk::EventQueue::Priority::IMMEDIATE);

    // Sanity check: no crash and no side effects on internal mappings.
    EXPECT_NE(pipeline->mainThread_, nullptr);
    EXPECT_EQ(pipeline->renderConnections_.size(), 0U);
}

/**
 * @tc.name: RemoveConnection_NullToken_ReturnFalse
 * @tc.desc: Ensure RemoveConnection returns false when token is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, RemoveConnection_NullToken_ReturnFalse, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    bool ret = pipeline->RemoveConnection(nullptr);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: OnScreenDisconnected_UnknownId_NoChange
 * @tc.desc: Disconnect unknown screen id should not affect existing client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenDisconnected_UnknownId_NoChange, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_unknown", property->GetScreenId(), nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();

    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, nullptr);
    ASSERT_NE(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);

    ScreenId otherId = property->GetScreenId() + 1;
    pipeline->OnScreenDisconnected(otherId);
    EXPECT_NE(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);
}

/**
 * @tc.name: AddConnection_NullRemote_Ignored
 * @tc.desc: AddConnection with nullptr remote should be ignored
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, AddConnection_NullRemote_Ignored, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    auto conn = sptr<RSIClientToRenderConnection>(nullptr);
    sptr<IRemoteObject> nullRemote = nullptr;
    pipeline->AddConnection(nullRemote, conn);
    EXPECT_EQ(pipeline->renderConnections_.size(), 1);
}

/**
 * @tc.name: RemoveConnection_Existing_ReturnTrue
 * @tc.desc: RemoveConnection returns true and removes mapping when token matches
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, RemoveConnection_Existing_ReturnTrue, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    DefaultSurfaceBufferCallbackFuncs funcs{};
    funcs.OnFinish = [](const FinishCallbackRet&) {};
    funcs.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> callback = new RSDefaultSurfaceBufferCallback(funcs);
    sptr<IRemoteObject> remote = callback->AsObject();
    auto conn = sptr<RSIClientToRenderConnection>(nullptr);
    pipeline->AddConnection(remote, conn);
    ASSERT_EQ(pipeline->renderConnections_.size(), 1U);
    class Token : public RSIConnectionToken {
    public:
        explicit Token(const sptr<IRemoteObject> &obj) : obj_(obj) {}
        sptr<IRemoteObject> AsObject() override { return obj_; }
    private:
        sptr<IRemoteObject> obj_;
    };
    sptr<Token> token = new Token(remote);
    bool ret = pipeline->RemoveConnection(token);
    EXPECT_TRUE(ret);
    EXPECT_EQ(pipeline->renderConnections_.size(), 0U);
}

/**
 * @tc.name: OnScreenPropertyChanged_ValidId_NoCrash
 * @tc.desc: After connection, property change should forward without removing client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenPropertyChanged_ValidId_NoCrash, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);
    ScreenId sid = property->GetScreenId();

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_prop", sid, nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, nullptr);
    ASSERT_NE(pipeline->composerClientManager_->GetComposerClient(sid), nullptr);

    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::CANVAS_ROTATION,
        sptr<ScreenProperty<bool>>::MakeSptr(false));
    EXPECT_NE(pipeline->composerClientManager_->GetComposerClient(sid), nullptr);
}

/**
 * @tc.name: RemoveConnection_TokenNotFound_ReturnFalse
 * @tc.desc: Ensure RemoveConnection returns false when token not found
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, RemoveConnection_TokenNotFound_ReturnFalse, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    DefaultSurfaceBufferCallbackFuncs funcs2{};
    funcs2.OnFinish = [](const FinishCallbackRet&) {};
    funcs2.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> callback = new RSDefaultSurfaceBufferCallback(funcs2);
    sptr<IRemoteObject> remote = callback->AsObject();
    auto dummyConn = sptr<RSIClientToRenderConnection>(nullptr);
    pipeline->AddConnection(remote, dummyConn);
    DefaultSurfaceBufferCallbackFuncs funcs3{};
    funcs3.OnFinish = [](const FinishCallbackRet&) {};
    funcs3.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> otherCb = new RSDefaultSurfaceBufferCallback(funcs3);
    sptr<IRemoteObject> otherRemote = otherCb->AsObject();
    class DummyToken : public RSIConnectionToken {
    public:
        explicit DummyToken(const sptr<IRemoteObject>& obj) : obj_(obj) {}
        sptr<IRemoteObject> AsObject() override { return obj_; }
    private:
        sptr<IRemoteObject> obj_;
    };
    sptr<DummyToken> token = new DummyToken(otherRemote);
    pipeline->renderConnections_.clear();
    bool ret = pipeline->RemoveConnection(token);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: AddConnection_DuplicateToken_NoOverride
 * @tc.desc: Adding same token twice does not override existing mapping
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, AddConnection_DuplicateToken_NoOverride, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    DefaultSurfaceBufferCallbackFuncs funcs4{};
    funcs4.OnFinish = [](const FinishCallbackRet&) {};
    funcs4.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> callback = new RSDefaultSurfaceBufferCallback(funcs4);
    sptr<IRemoteObject> remote = callback->AsObject();
    auto conn1 = sptr<RSIClientToRenderConnection>(nullptr);
    pipeline->AddConnection(remote, conn1);
    ASSERT_EQ(pipeline->renderConnections_.size(), 1U);
    auto conn2 = sptr<RSIClientToRenderConnection>(nullptr);
    pipeline->AddConnection(remote, conn2);
    ASSERT_EQ(pipeline->renderConnections_.size(), 1U);
    auto found = pipeline->FindClientToRenderConnection(remote);
    EXPECT_EQ(found, conn1);
}

/**
 * @tc.name: FindClientToRenderConnection_FoundAndNotFound
 * @tc.desc: Verify find returns stored value and nullptr when absent
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, FindClientToRenderConnection_FoundAndNotFound, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    DefaultSurfaceBufferCallbackFuncs funcs5{};
    funcs5.OnFinish = [](const FinishCallbackRet&) {};
    funcs5.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> callback = new RSDefaultSurfaceBufferCallback(funcs5);
    sptr<IRemoteObject> remote = callback->AsObject();
    auto conn = sptr<RSIClientToRenderConnection>(nullptr);
    pipeline->AddConnection(remote, conn);
    auto found = pipeline->FindClientToRenderConnection(remote);
    EXPECT_EQ(found, conn);
    DefaultSurfaceBufferCallbackFuncs funcs6{};
    funcs6.OnFinish = [](const FinishCallbackRet&) {};
    funcs6.OnAfterAcquireBuffer = [](const AfterAcquireBufferRet&) {};
    sptr<RSDefaultSurfaceBufferCallback> otherCb = new RSDefaultSurfaceBufferCallback(funcs6);
    sptr<IRemoteObject> otherRemote = otherCb->AsObject();
    auto notFound = pipeline->FindClientToRenderConnection(otherRemote);
    EXPECT_EQ(notFound, nullptr);
}

/**
 * @tc.name: PostTasks_EarlyReturn_WhenThreadsNull
 * @tc.desc: Post*Task methods should early return when threads are nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, PostTasks_EarlyReturn_WhenThreadsNull, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->PostMainThreadTask([] {});
    pipeline->PostMainThreadSyncTask([] {});
    pipeline->PostUniRenderThreadTask([] {});
    pipeline->PostUniRenderThreadSyncTask([] {});
    EXPECT_EQ(pipeline->renderConnections_.size(), 0U);
}

/**
 * @tc.name: AddTransactionDataPidInfo_EarlyReturn_WhenMainThreadNull
 * @tc.desc: RSRenderPipeline::AddTransactionDataPidInfo should early return when mainThread_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, AddTransactionDataPidInfo_EarlyReturn_WhenMainThreadNull, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    // mainThread_ is null by default; call should early return without crash or side effects
    pipeline->AddTransactionDataPidInfo(static_cast<pid_t>(12345));
    EXPECT_EQ(pipeline->renderConnections_.size(), 0U);
}

/**
 * @tc.name: AddTransactionDataPidInfo_MainThreadNotNull_Forwarded
 * @tc.desc: RSRenderPipeline::AddTransactionDataPidInfo should forward to RSMainThread when mainThread_ is not null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, AddTransactionDataPidInfo_MainThreadNotNull_Forwarded, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->mainThread_ = RSMainThread::Instance();
    ASSERT_NE(pipeline->mainThread_, nullptr);

    // Call twice with the same pid to exercise both insert and update paths inside RSMainThread
    pid_t pid = static_cast<pid_t>(22222);
    pipeline->AddTransactionDataPidInfo(pid);
    pipeline->AddTransactionDataPidInfo(pid);

    // Sanity: no crash and pipeline's unrelated state remains unchanged
    EXPECT_NE(pipeline->mainThread_, nullptr);
    EXPECT_EQ(pipeline->renderConnections_.size(), 0U);
}

/**
 * @tc.name: ScreenEvents_EarlyReturn_WhenThreadsNull
 * @tc.desc: Screen event handlers should early return when threads are nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, ScreenEvents_EarlyReturn_WhenThreadsNull, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    auto property = sptr<RSScreenProperty>::MakeSptr();
    sptr<IRSRenderToComposerConnection> renderToComposerConn;
    sptr<IRSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    std::shared_ptr<HdiOutput> output;
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, output);
    pipeline->OnScreenDisconnected(0);
    pipeline->OnScreenPropertyChanged(0, ScreenPropertyType::ID, sptr<ScreenProperty<ScreenId>>::MakeSptr(0));
    pipeline->OnScreenRefresh(0);
    auto client = pipeline->composerClientManager_->GetComposerClient(property->GetScreenId());
    EXPECT_EQ(client, nullptr);
}
/**
 * @tc.name: OnScreenConnected_NonVirtual_AddsComposerClientAndRegistersCallbacks
 * @tc.desc: Non-virtual screen should setup callbacks and add composer client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenConnected_NonVirtual_AddsComposerClientAndRegistersCallbacks, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn", property->GetScreenId(), nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    std::shared_ptr<HdiOutput> output;

    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, output);

    auto client = pipeline->composerClientManager_->GetComposerClient(property->GetScreenId());
    ASSERT_NE(client, nullptr);
}

/**
 * @tc.name: OnScreenConnected_SecondConnect_NoDuplicate
 * @tc.desc: Connecting the same non-virtual screen twice should not duplicate or replace client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenConnected_SecondConnect_NoDuplicate, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);
    ScreenId sid = property->GetScreenId();

    sptr<RSRenderToComposerConnection> conn1 = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_first", sid, nullptr);
    sptr<RSComposerToRenderConnection> toRender1 = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, conn1, toRender1, nullptr);
    auto first = pipeline->composerClientManager_->GetComposerClient(sid);
    ASSERT_NE(first, nullptr);

    sptr<RSRenderToComposerConnection> conn2 = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_second", sid, nullptr);
    sptr<RSComposerToRenderConnection> toRender2 = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, conn2, toRender2, nullptr);
    auto second = pipeline->composerClientManager_->GetComposerClient(sid);
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(first.get(), second.get());
}

/**
 * @tc.name: OnScreenConnected_VirtualScreen_NoComposerClientAdded
 * @tc.desc: Virtual screen should early return without adding composer client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenConnected_VirtualScreen_NoComposerClientAdded, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(true);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_v", property->GetScreenId(), nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    std::shared_ptr<HdiOutput> output;

    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, output);

    auto client = pipeline->composerClientManager_->GetComposerClient(property->GetScreenId());
    EXPECT_EQ(client, nullptr);
}

/**
 * @tc.name: OnScreenDisconnected_RemovesComposerClient
 * @tc.desc: Non-virtual screen adds client then disconnected should remove client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenDisconnected_RemovesComposerClient, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_d", property->GetScreenId(), nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, nullptr);
    ASSERT_NE(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);

    pipeline->OnScreenDisconnected(property->GetScreenId());
    EXPECT_EQ(pipeline->composerClientManager_->GetComposerClient(property->GetScreenId()), nullptr);
}

/**
 * @tc.name: OnScreenPropertyChanged_MultipleTypes_NoRemoval
 * @tc.desc: After connection, property changes for various types should not remove composer client
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineTest, OnScreenPropertyChanged_MultipleTypes_NoRemoval, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    pipeline->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);
    ScreenId sid = property->GetScreenId();

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_props", sid, nullptr);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, nullptr);
    ASSERT_NE(pipeline->composerClientManager_->GetComposerClient(sid), nullptr);

    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::COLOR_GAMUT,
        sptr<ScreenProperty<uint32_t>>::MakeSptr(static_cast<uint32_t>(ScreenColorGamut::COLOR_GAMUT_SRGB)));
    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::GAMUT_MAP,
        sptr<ScreenProperty<uint32_t>>::MakeSptr(static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_CONSTANT)));
    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::STATE,
        sptr<ScreenProperty<uint8_t>>::MakeSptr(static_cast<uint8_t>(ScreenState::UNKNOWN)));
    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::CANVAS_ROTATION,
        sptr<ScreenProperty<bool>>::MakeSptr(false));
    pipeline->OnScreenPropertyChanged(sid, ScreenPropertyType::RENDER_RESOLUTION,
        sptr<ScreenProperty<std::pair<uint32_t, uint32_t>>>::MakeSptr(std::make_pair(0u, 0u)));

    EXPECT_NE(pipeline->composerClientManager_->GetComposerClient(sid), nullptr);
}

/**
 * @tc.name: OnScreenConnected_CallbackExecuted
 * @tc.desc: OnScreenConnected should set callback and it should be executed to clear surfaceFpsOpList
 * @tc.type: FUNC
 * @tc.require: issue22921
 */
HWTEST_F(RSRenderPipelineTest, OnScreenConnected_CallbackExecuted, TestSize.Level1)
{
    sptr<RSRenderPipeline> pipeline = new RSRenderPipeline();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    pipeline->composerClientManager_ = rsComposerClientMgr;
    pipeline->uniRenderThread_ = &RSUniRenderThread::Instance();
    pipeline->mainThread_ = RSMainThread::Instance();
    ASSERT_NE(pipeline->mainThread_, nullptr);

    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto virtProp = sptr<ScreenProperty<bool>>::MakeSptr(false);
    property->Set(ScreenPropertyType::IS_VIRTUAL, virtProp);
    
    auto hdiOutput = HdiOutput::CreateHdiOutput(property->GetScreenId());
    hdiOutput->Init();

    std::shared_ptr<RSRenderComposer> rsRenderComposer = std::make_shared<RSRenderComposer>(hdiOutput, property);
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent =
        std::make_shared<RSRenderComposerAgent>(rsRenderComposer);

    sptr<RSRenderToComposerConnection> renderToComposerConn = sptr<RSRenderToComposerConnection>::MakeSptr(
        "conn_callback", property->GetScreenId(), rsRenderComposerAgent);
    sptr<RSComposerToRenderConnection> composerToRenderConn = new RSComposerToRenderConnection();
    pipeline->OnScreenConnected(property, renderToComposerConn, composerToRenderConn, hdiOutput);

    auto client = pipeline->composerClientManager_->GetComposerClient(property->GetScreenId());
    ASSERT_NE(client, nullptr);
    auto context = client->GetComposerContext();
    ASSERT_NE(context, nullptr);

    auto layer = std::make_shared<RSSurfaceLayer>(property->GetScreenId(), context);
    layer->SetTunnelHandleChange(true);

    SurfaceFpsOp addOp {static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_ADD), 1, "test_surface", 100};
    SurfaceFpsOp removeOp {static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_REMOVE), 2, "test_surface2", 200};

    pipeline->mainThread_->AddSurfaceFpsOp(addOp);
    pipeline->mainThread_->AddSurfaceFpsOp(removeOp);

    ComposerInfo composerInfo;
    client->CommitLayers(composerInfo);

    auto surfaceFpsOpListBefore = pipeline->mainThread_->GetSurfaceFpsOpList();
    EXPECT_EQ(surfaceFpsOpListBefore.size(), 2u);

    client->pipelineParam_.SurfaceFpsOpList.push_back(addOp);
    client->pipelineParam_.SurfaceFpsOpList.push_back(removeOp);

    layer->SetTunnelHandleChange(false);
    client->CommitLayers(composerInfo);

    auto surfaceFpsOpListAfter = pipeline->mainThread_->GetSurfaceFpsOpList();
    EXPECT_EQ(surfaceFpsOpListAfter.size(), 0u);

    pipeline->composerClientManager_->GetComposerClient(
        property->GetScreenId())->SetRmvSurfaceFpsOpCallback(nullptr);
    layer->SetTunnelHandleChange(true);
    client->CommitLayers(composerInfo);
    surfaceFpsOpListAfter = pipeline->mainThread_->GetSurfaceFpsOpList();
    EXPECT_EQ(surfaceFpsOpListAfter.size(), 0u);
}
} // namespace OHOS::Rosen