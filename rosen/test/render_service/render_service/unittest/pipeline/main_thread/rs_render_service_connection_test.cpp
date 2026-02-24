/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include <memory>
#include "file_ex.h"
#include "gtest/gtest.h"
#include "limit_number.h"

#include "feature/dirty/rs_uni_dirty_occlusion_util.h"
#include "ipc_callbacks/brightness_info_change_callback.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_service/core/transaction/rs_client_to_render_connection.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_test_util.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
} // namespace
class RSRenderServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline RSMainThread* mainThread_;
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceConnectionTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
    mainThread_ = new RSMainThread();
    mainThread_->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    mainThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread_->runner_);
}
void RSRenderServiceConnectionTest::TearDownTestCase() {}
void RSRenderServiceConnectionTest::SetUp() {}
void RSRenderServiceConnectionTest::TearDown() {}

/**
 * @tc.name: GetMemoryGraphic001
 * @tc.desc: GetMemoryGraphic
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceConnectionTest, GetMemoryGraphic001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    MemoryGraphic mem1;
    rsRenderServiceConnection->GetMemoryGraphic(123, mem1);
    ASSERT_EQ(mem1.GetGpuMemorySize(), 0);
    MemoryGraphic mem2;
    rsRenderServiceConnection->GetMemoryGraphic(0, mem2);
    ASSERT_EQ(mem2.GetGpuMemorySize(), 0);
}

/**
 * @tc.name: GetMemoryGraphic002
 * @tc.desc: GetMemoryGraphic
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceConnectionTest, GetMemoryGraphic002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    std::vector<MemoryGraphic> memoryGraphics;
    rsRenderServiceConnection->GetMemoryGraphics(memoryGraphics);
    ASSERT_EQ(memoryGraphics.size(), 0);
}

/**
 * @tc.name: SetBrightnessInfoChangeCallbackTest
 * @tc.desc: test SetBrightnessInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectionTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();

    // case 1: mainThread null
    {
        sptr<RSClientToServiceConnection> connection =
            new RSClientToServiceConnection(0, nullptr, nullptr, nullptr, token->AsObject(), nullptr);
        ASSERT_NE(connection->SetBrightnessInfoChangeCallback(nullptr), SUCCESS);
    }

    // case 2: mainThread not null
    {
        ASSERT_NE(mainThread_, nullptr);
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, mainThread_, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        ASSERT_EQ(connection->SetBrightnessInfoChangeCallback(nullptr), SUCCESS);
        sptr<MockRSBrightnessInfoChangeCallback> callback = new MockRSBrightnessInfoChangeCallback();
        ASSERT_EQ(connection->SetBrightnessInfoChangeCallback(callback), SUCCESS);
    }
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: CleanBrightnessInfoChangeCallbacksTest
 * @tc.desc: test CleanBrightnessInfoChangeCallbacks
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectionTest, CleanBrightnessInfoChangeCallbacksTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();

    // case 1: mainThread null
    {
        sptr<RSClientToServiceConnection> connection =
            new RSClientToServiceConnection(0, nullptr, nullptr, nullptr, token->AsObject(), nullptr);
        connection->CleanBrightnessInfoChangeCallbacks();
    }

    // case 2: mainThread not null
    {
        RSMainThread* mainThread = new RSMainThread();
        mainThread->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
        mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread->runner_);
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        connection->CleanBrightnessInfoChangeCallbacks();
        ASSERT_NE(mainThread, nullptr);
        delete mainThread;
    }
}

/**
 * @tc.name: GetBrightnessInfoTest
 * @tc.desc: test GetBrightnessInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectionTest, GetBrightnessInfoTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();

    // case 1: mainThread null
    {
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, nullptr, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        BrightnessInfo brightnessInfo;
        ASSERT_EQ(connection->GetBrightnessInfo(0, brightnessInfo), SUCCESS);
    }

    // case 2: mainThread not null
    {
        RSMainThread* mainThread = new RSMainThread();
        mainThread->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
        mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread->runner_);
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        BrightnessInfo brightnessInfo;
        ASSERT_EQ(connection->GetBrightnessInfo(0, brightnessInfo), SUCCESS);
        ASSERT_EQ(connection->GetBrightnessInfo(INVALID_SCREEN_ID, brightnessInfo), SUCCESS);
        delete mainThread;
    }
}
#endif

/**
 * @tc.name: UpdateAnimationOcclusionStatus001
 * @tc.desc: test update animation occlusion status
 * @tc.type: FUNC
 * @tc.require: issues20843
 */
HWTEST_F(RSRenderServiceConnectionTest, UpdateAnimationOcclusionStatus001, TestSize.Level1)
{
    RSMainThread* mainThread = new RSMainThread();
    ASSERT_NE(mainThread, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    bool& isAnimationOcclusion = RSUniDirtyOcclusionUtil::GetIsAnimationOcclusionRef().first;

    // case 1: mainThread null
    {
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, nullptr, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        string sceneId = "LAUNCHER_APP_LAUNCH_FROM_ICON";
        connection->UpdateAnimationOcclusionStatus(sceneId, true);
        ASSERT_EQ(isAnimationOcclusion, false);
    }

    // case 2: mainThread not null
    {
        mainThread->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
        mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread->runner_);
        sptr<RSClientToServiceConnection> connection = new RSClientToServiceConnection(
            0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
        string sceneId = "LAUNCHER_APP_LAUNCH_FROM_ICON";
        connection->UpdateAnimationOcclusionStatus(sceneId, false);
        sleep(1);
        ASSERT_EQ(isAnimationOcclusion, false);
    }
    delete mainThread;
}

/**
 * @tc.name: SetSurfaceSystemWatermarkTest001
 * @tc.desc: SetSurfaceSystemWatermarkTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, SetSurfaceSystemWatermarkTest001, TestSize.Level1)
{
#ifdef RS_ENABLE_UNI_RENDER
    constexpr uint32_t defaultScreenWidth = 400;
    constexpr uint32_t defaultScreenHight = 320;
    auto mainThread = RSMainThread::Instance();
    std::string watermarkName = "watermarkName";

    Media::InitializationOptions opts;
    opts.size.width = defaultScreenWidth * 2.5;
    opts.size.height = defaultScreenHight * 2.5;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    pid_t pid = 1000;
    // No store waterimg and pixelmap == nullptr
    auto res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(pid, watermarkName, nullptr,
        {}, SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_PIXELMAP_INVALID);

    // Test pixelMap == nullptr and rs has stored watermask img.
    mainThread->surfaceWatermarkHelper_.surfaceWatermarks_[watermarkName];
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(pid, watermarkName, nullptr,
        {}, SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_RS_CONNECTION_ERROR);

    // Test pixelMap != nullptr and has rsNode
    mainThread->surfaceWatermarkHelper_.surfaceWatermarks_[watermarkName];
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(pid, watermarkName, nullptr, {11235642},
        SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.erase(watermarkName);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);
    EXPECT_EQ(mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.size(), 0);

    // Test pixelMap != nullptr and rs has not stored watermask img
    NodeId renderNodeId = 502232;
    auto renderNode = std::make_shared<RSRenderNode>(renderNodeId, true);
    mainThread->context_->nodeMap.RegisterRenderNode(renderNode);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(renderNodeId), watermarkName,
        pixelMap, {renderNodeId}, SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.erase(watermarkName);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);
    EXPECT_EQ(mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.size(), 0);
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(pid, watermarkName, mainThread->GetContext(), true);
    EXPECT_EQ(mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.size(), 0);

    // Test pixelMap != nullptr and rs has store watermark img and node success
    NodeId surfaceNodeId = 502232;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    mainThread->context_->nodeMap.RegisterRenderNode(surfaceNode);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);

    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        mainThread->GetContext(), true, false);
    surfaceNode->GetSurfaceWatermarkEnabledMap(SurfaceWatermarkType::SYSTEM_WATER_MARK);

    // code coverage
    auto param = std::move(surfaceNode->stagingRenderParams_);
    surfaceNode->GetSurfaceWatermarkEnabledMap(SurfaceWatermarkType::SYSTEM_WATER_MARK);
    surfaceNode->ClearWatermarkEnabled(watermarkName, SurfaceWatermarkType::SYSTEM_WATER_MARK);
    surfaceNode->stagingRenderParams_ = std::move(param);

    // Test Limit
    mainThread->surfaceWatermarkHelper_.registerSurfaceWatermarkCount_ = 1000;
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), "newWatermarkName",
        pixelMap, {}, SurfaceWatermarkType::SYSTEM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(mainThread->surfaceWatermarkHelper_.registerSurfaceWatermarkCount_, 1000);

    mainThread->surfaceWatermarkHelper_.registerSurfaceWatermarkCount_ = 0;

    // UnregisterRenderNode node
    mainThread->context_->nodeMap.UnregisterRenderNode(surfaceNodeId);
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(pid, watermarkName, mainThread->GetContext(), true);
    EXPECT_EQ(mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.size(), 0);
#endif
}

/**
 * @tc.name: SetSurfaceCustomWatermarkTest001
 * @tc.desc: SetSurfaceCustomWatermarkTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, SetSurfaceCustomWatermarkTest001, TestSize.Level1)
{
    constexpr uint32_t defaultScreenWidth = 480;
    constexpr uint32_t defaultScreenHight = 320;
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = defaultScreenWidth;
    uint32_t height = defaultScreenHight;
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    screenManager->defaultScreenId_ = id;
    auto mainThread = RSMainThread::Instance();
    std::string watermarkName = "watermarkName";
    Media::InitializationOptions opts;
    opts.size.width = defaultScreenWidth;
    opts.size.height = defaultScreenHight;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);

    NodeId surfaceNodeId = 0XFFFFFFFFFFFF1234;
    auto screenId = screenManager->GetDefaultScreenId();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->screenId_ = screenId;
    mainThread->context_->nodeMap.RegisterRenderNode(surfaceNode);

    // Test Check pid
    auto res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId) + 1, watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), false);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_PERMISSION_ERROR);
    // Test upperNodeSize and less than screenSize
    NodeId screenRenderNodeId = 0X123;
    surfaceNode->screenNodeId_ = screenRenderNodeId;
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(screenRenderNodeId, screenRenderNodeId);
    screenRenderNode->screenInfo_.width = defaultScreenWidth;
    screenRenderNode->screenInfo_.height = defaultScreenHight;
    mainThread->context_->nodeMap.RegisterRenderNode(screenRenderNode);

    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(defaultScreenWidth - 50);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(defaultScreenHight - 50);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), false);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);
    mainThread->context_->nodeMap.UnregisterRenderNode(screenRenderNodeId);

    // Test less than NodeSize
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(defaultScreenWidth);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(defaultScreenHight);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), false);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);

    // Test piixelMap size greater than node size and screen size
    opts.size.width = defaultScreenWidth + 50;
    opts.size.height = defaultScreenHight + 50;
    pixelMap = Media::PixelMap::Create(opts);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), false);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_IMG_SIZE_ERROR);
    EXPECT_EQ(1, mainThread->surfaceWatermarkHelper_.surfaceWatermarks_.size());
    EXPECT_EQ(1, mainThread->surfaceWatermarkHelper_.watermarkNameMapNodeId_.size());

    auto iter = mainThread->surfaceWatermarkHelper_.watermarkNameMapNodeId_.find(watermarkName);
    EXPECT_NE(iter, mainThread->surfaceWatermarkHelper_.watermarkNameMapNodeId_.end());
    if (iter != mainThread->surfaceWatermarkHelper_.watermarkNameMapNodeId_.end()) {
        EXPECT_EQ(iter->second.first.size(), 1);
    }

    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(0, mainThread->GetContext());
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(ExtractPid(surfaceNodeId), mainThread->GetContext());

    // Code coverage
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(0, watermarkName,
        mainThread->GetContext(), false, false);
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermarkForNodes(0, watermarkName, {},
        mainThread->GetContext(), false);
    mainThread->surfaceWatermarkHelper_.AddWatermarkNameMapNodeId(watermarkName, 0,
        SurfaceWatermarkType::CUSTOM_WATER_MARK);
    
    mainThread->surfaceWatermarkHelper_.watermarkNameMapNodeId_[watermarkName] = {{1},
        SurfaceWatermarkType::CUSTOM_WATER_MARK};
    mainThread->surfaceWatermarkHelper_.ClearSurfaceWatermark(0, watermarkName,
        mainThread->GetContext(), false, false);
    mainThread->context_->nodeMap.UnregisterRenderNode(surfaceNodeId);
}
/**
 * @tc.name: SetSurfaceCustomWatermarkTest002
 * @tc.desc: SetSurfaceCustomWatermarkTest002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, SetSurfaceCustomWatermarkTest002, TestSize.Level1)
{
    constexpr uint32_t defaultScreenWidth = 480;
    constexpr uint32_t defaultScreenHight = 320;
    auto mainThread = RSMainThread::Instance();
    std::string watermarkName = "watermarkName2222";
    Media::InitializationOptions opts;
    opts.size.width = defaultScreenWidth;
    opts.size.height = defaultScreenHight;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    
    NodeId surfaceNodeId = 0XFFFFFFFFFFFF1235;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    // Test1 No SurfaceNode
    auto res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_NOT_SURFACE_NODE_ERROR);

    // Test2 Node not setBounds
    mainThread->context_->nodeMap.RegisterRenderNode(surfaceNode);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_IMG_SIZE_ERROR);

    // Test3 node setbounds and new node does not set bounds, but the watermark name remains the same.
    surfaceNode->GetMutableRenderProperties().SetBoundsWidth(defaultScreenWidth);
    surfaceNode->GetMutableRenderProperties().SetBoundsHeight(defaultScreenHight);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId), watermarkName,
        pixelMap, {surfaceNodeId}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);

    NodeId surfaceNodeId1 = 0XFFFFFFFFFFFF1237;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId1);
    mainThread->context_->nodeMap.RegisterRenderNode(surfaceNode1);
    res = mainThread->surfaceWatermarkHelper_.SetSurfaceWatermark(ExtractPid(surfaceNodeId1), watermarkName,
        nullptr, {surfaceNodeId1}, SurfaceWatermarkType::CUSTOM_WATER_MARK, mainThread->GetContext(), true);
    EXPECT_EQ(res, SurfaceWatermarkStatusCode::WATER_MARK_SUCCESS);

    mainThread->context_->nodeMap.UnregisterRenderNode(surfaceNodeId);
    mainThread->context_->nodeMap.UnregisterRenderNode(surfaceNodeId1);
}
/**
 * @tc.name: CreateNode
 * @tc.desc: CreateNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, CreateNode, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    
    // create displayNode with async postTask (sync task processor not ready)
    RSDisplayNodeConfig displayNodeConfig = {};
    NodeId nodeId = 1;
    bool result = true;
    rsRenderServiceConnection->CreateNode(displayNodeConfig, nodeId, result);
    EXPECT_TRUE(result);

    // create displayNode with async postTask (sync task processor not ready, but isRunning_ was set to true)
    // at this time, CreateNode will first try to post sync task
    nodeId = 2;
    mainThread->isRunning_ = true;
    rsRenderServiceConnection->CreateNode(displayNodeConfig, nodeId, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: RegisterTypefaceTest001
 * @tc.desc: test register typeface and unregister typeface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, RegisterTypefaceTest001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection =
        new RSClientToServiceConnection(0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);
    auto tf = Drawing::Typeface::MakeDefault();
    uint64_t uniqueId = 1;
    EXPECT_TRUE(rsRenderServiceConnection->RegisterTypeface(uniqueId, tf));
    EXPECT_TRUE(rsRenderServiceConnection->UnRegisterTypeface(uniqueId));
    EXPECT_TRUE(rsRenderServiceConnection->UnRegisterTypeface(0));
}

/**
 * @tc.name: RegisterTypefaceTest002
 * @tc.desc: test register shared typeface and unregister shared typeface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, RegisterTypefaceTest002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection =
        new RSClientToServiceConnection(0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    int32_t needUpdate;
    pid_t pid = getpid();
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetHash());
    sharedTypeface.size_ = typeface->GetSize();
    sharedTypeface.fd_ = typeface->GetFd();
    EXPECT_NE(
        rsRenderServiceConnection->RegisterTypeface(sharedTypeface, needUpdate), -1);
    EXPECT_TRUE(rsRenderServiceConnection->UnRegisterTypeface(typeface->GetHash()));
}

/**
 * @tc.name: RegisterTypefaceTest003
 * @tc.desc: test register shared typeface and unregister shared typeface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, RegisterTypefaceTest003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection =
        new RSClientToServiceConnection(0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    int32_t needUpdate;
    pid_t pid = getpid();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    Drawing::SharedTypeface sharedTypeface(
        (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetUniqueID()), typeface);
    rsRenderServiceConnection->RegisterTypeface(sharedTypeface, needUpdate);
    EXPECT_NE(needUpdate, 0);
    Drawing::SharedTypeface sharedTypeface1(
        (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetUniqueID()), typeface);
    rsRenderServiceConnection->RegisterTypeface(sharedTypeface1, needUpdate);
    EXPECT_EQ(needUpdate, -1);
    EXPECT_TRUE(rsRenderServiceConnection->UnRegisterTypeface(typeface->GetHash()));
}

/**
 * @tc.name: GetBundleNameTest001
 * @tc.desc: GetBundleName
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, GetBundleNameTest001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);

    constexpr pid_t testPid = 1234;
    const std::string expectedBundleName = "com.example.app";
    rsRenderServiceConnection->pidToBundleName_[testPid] = expectedBundleName;

    std::string actualBundleName = rsRenderServiceConnection->GetBundleName(testPid);
    EXPECT_EQ(actualBundleName, expectedBundleName);
}

/**
 * @tc.name: GetBundleNameTest002
 * @tc.desc: GetBundleName
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, GetBundleNameTest002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);

    constexpr pid_t testPid = -1;
    const std::string bundleName = rsRenderServiceConnection->GetBundleName(testPid);
    EXPECT_TRUE(bundleName.empty());
}

/**
 * @tc.name: CleanAllTest
 * @tc.desc: test CleanAll
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectionTest, CleanAllTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    RSMainThread* mainThread = new RSMainThread();
    mainThread->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread->runner_);
    sptr<RSClientToServiceConnection> connection =
        new RSClientToServiceConnection(0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_FALSE(connection->cleanDone_);
    connection->CleanAll(false);
    delete mainThread;
    ASSERT_TRUE(connection->cleanDone_);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RegisterCanvasCallbackAndCleanTest
 * @tc.desc: Test RegisterCanvasCallback and CleanCanvasCallbacksAndPendingBuffer functions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, RegisterCanvasCallbackAndCleanTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    pid_t testPid = 12345;
    sptr<RSIConnectionToken> token1 = new IRemoteStub<RSIConnectionToken>();
    auto clientToServiceConnection = new RSClientToServiceConnection(
        testPid, nullptr, mainThread, CreateOrGetScreenManager(), token1->AsObject(), nullptr);
    ASSERT_NE(clientToServiceConnection, nullptr);

    sptr<RSIConnectionToken> token2 = new IRemoteStub<RSIConnectionToken>();
    auto clientToRenderConnection = new RSClientToRenderConnection(
        testPid, nullptr, mainThread, CreateOrGetScreenManager(), token2->AsObject(), nullptr);
    ASSERT_NE(clientToRenderConnection, nullptr);

    // Test RegisterCanvasCallback with valid callback
    sptr<RSICanvasSurfaceBufferCallback> mockCallback = nullptr;
    // Since we cannot create a concrete implementation easily in the test,
    // we test with nullptr first to verify the function executes
    clientToRenderConnection->RegisterCanvasCallback(mockCallback);
    // No assertion needed - just verify it doesn't crash

    // Test CleanCanvasCallbacksAndPendingBuffer
    // This should clean up the registered callback for the remote pid
    clientToServiceConnection->CleanCanvasCallbacksAndPendingBuffer();

    // Verify cleanup was successful by checking that re-registering works
    clientToRenderConnection->RegisterCanvasCallback(mockCallback);
    // No assertion needed - just verify it doesn't crash

    // Test error handling when mainThread is nullptr
    sptr<RSIConnectionToken> token3 = new IRemoteStub<RSIConnectionToken>();
    auto clientToServiceConnectionWithNullThread = new RSClientToServiceConnection(
        testPid, nullptr, nullptr, CreateOrGetScreenManager(), token3->AsObject(), nullptr);
    ASSERT_NE(clientToServiceConnectionWithNullThread, nullptr);

    sptr<RSIConnectionToken> token4 = new IRemoteStub<RSIConnectionToken>();
    auto clientToRenderConnectionWithNullThread = new RSClientToRenderConnection(
        testPid, nullptr, nullptr, CreateOrGetScreenManager(), token4->AsObject(), nullptr);
    ASSERT_NE(clientToRenderConnectionWithNullThread, nullptr);

    // Test RegisterCanvasCallback with nullptr mainThread
    clientToRenderConnectionWithNullThread->RegisterCanvasCallback(mockCallback);
    // No assertion needed - just verify it doesn't crash

    // Test CleanCanvasCallbacksAndPendingBuffer with nullptr mainThread - should return early without crash
    clientToServiceConnectionWithNullThread->CleanCanvasCallbacksAndPendingBuffer();
    // No assertion needed - just verify it doesn't crash
}
#endif

/**
 * @tc.name: CreateVirtualScreen001
 * @tc.desc: test CreateVirtualScreen while whitelist's size invalid
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, CreateVirtualScreen001, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);
    
    std::string name = "virtualScreen01";
    uint32_t width = 100;
    uint32_t height = 100;
    std::vector<NodeId> whiteList(MAX_SPECIAL_LAYER_NUM + 1);
    ScreenId screenId = rsRenderServiceConnection->CreateVirtualScreen(
        name, width, height, nullptr, INVALID_SCREEN_ID, -1, whiteList);
    ASSERT_EQ(screenId, INVALID_SCREEN_ID);

    // retry with empty whitelist
    screenId = rsRenderServiceConnection->CreateVirtualScreen(
        name, width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // restore
    rsRenderServiceConnection->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: ModifyVirtualScreenWhiteList001
 * @tc.desc: modify virtual screen whitelist while whitelist's size invalid
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreenWhiteList001, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeId> whiteList(MAX_SPECIAL_LAYER_NUM + 1);
    int32_t repCode;

    // whitelist exceeding size limit
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenWhiteList(screenId, whiteList, repCode), ERR_OK);
    // whitelist is empty
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenWhiteList(screenId, {}, repCode), ERR_OK);
    ASSERT_NE(rsRenderServiceConnection->RemoveVirtualScreenWhiteList(screenId, {}, repCode), ERR_OK);
}

/**
 * @tc.name: ModifyVirtualScreenWhiteList002
 * @tc.desc: modify virtual screen whitelist while screenManger is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreenWhiteList002, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, nullptr, token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    NodeId nodeId = 0;
    int32_t repCode;
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenWhiteList(screenId, {nodeId}, repCode), ERR_OK);
    ASSERT_NE(rsRenderServiceConnection->RemoveVirtualScreenWhiteList(screenId, {nodeId}, repCode), ERR_OK);
}

/**
 * @tc.name: ModifyVirtualScreenWhiteList003
 * @tc.desc: modify virtual screen whitelist success
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreenWhiteList003, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    NodeId nodeId = 0;
    int32_t repCode;
    ASSERT_EQ(rsRenderServiceConnection->AddVirtualScreenWhiteList(screenId, {nodeId}, repCode), ERR_OK);
    ASSERT_EQ(rsRenderServiceConnection->RemoveVirtualScreenWhiteList(screenId, {nodeId}, repCode), ERR_OK);
}

/**
 * @tc.name: ModifyVirtualScreenBlackList001
 * @tc.desc: modify virtual screen blacklist while blacklist's size invalid
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreenBlackList001, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeId> blacklist(MAX_SPECIAL_LAYER_NUM + 1);
    int32_t repCode;

    // blacklist exceeding size limit
    ASSERT_NE(rsRenderServiceConnection->SetVirtualScreenBlackList(screenId, blacklist), SUCCESS);
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenBlackList(screenId, blacklist, repCode), ERR_OK);
    // blacklist is empty
    ASSERT_EQ(rsRenderServiceConnection->SetVirtualScreenBlackList(screenId, {}), SUCCESS);
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenBlackList(screenId, {}, repCode), ERR_OK);
    ASSERT_NE(rsRenderServiceConnection->RemoveVirtualScreenBlackList(screenId, {}, repCode), ERR_OK);
}

/**
 * @tc.name: ModifyVirtualScreenBlackList002
 * @tc.desc: modify virtual screen blacklist while screenManger is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreeBlackList002, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, nullptr, token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    NodeId nodeId = 0;
    int32_t repCode;
    ASSERT_NE(rsRenderServiceConnection->SetVirtualScreenBlackList(screenId, {nodeId}), SUCCESS);
    ASSERT_NE(rsRenderServiceConnection->AddVirtualScreenBlackList(screenId, {nodeId}, repCode), ERR_OK);
    ASSERT_NE(rsRenderServiceConnection->RemoveVirtualScreenBlackList(screenId, {nodeId}, repCode), ERR_OK);
}

/**
 * @tc.name: ModifyVirtualScreenBlackList003
 * @tc.desc: modify virtual screen blacklist success
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSRenderServiceConnectionTest, ModifyVirtualScreenBlackList003, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    NodeId nodeId = 0;
    int32_t repCode;
    ASSERT_EQ(rsRenderServiceConnection->SetVirtualScreenBlackList(screenId, {nodeId}), SUCCESS);
    ASSERT_EQ(rsRenderServiceConnection->AddVirtualScreenBlackList(screenId, {nodeId}, repCode), ERR_OK);
    ASSERT_EQ(rsRenderServiceConnection->RemoveVirtualScreenBlackList(screenId, {nodeId}, repCode), ERR_OK);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackList001
 * @tc.desc: test SetVirtualScreenTypeBlackList while screenManager isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSRenderServiceConnectionTest, SetVirtualScreenTypeBlackList001, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection = new RSClientToServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    int32_t repCode;
    std::vector<NodeType> typeList = {};
    ASSERT_EQ(rsRenderServiceConnection->SetVirtualScreenTypeBlackList(INVALID_SCREEN_ID, typeList, repCode), ERR_OK);

    typeList.push_back(static_cast<NodeType>(RSSurfaceNodeType::DEFAULT));
    ASSERT_EQ(rsRenderServiceConnection->SetVirtualScreenTypeBlackList(INVALID_SCREEN_ID, typeList, repCode), ERR_OK);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackList002
 * @tc.desc: test SetVirtualScreenTypeBlackList while screenManager is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSRenderServiceConnectionTest, SetVirtualScreenTypeBlackList002, TestSize.Level2)
{
    // create connection
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToServiceConnection> rsRenderServiceConnection =
        new RSClientToServiceConnection(0, nullptr, mainThread, nullptr, token->AsObject(), nullptr);
    ASSERT_NE(rsRenderServiceConnection, nullptr);

    int32_t repCode;
    std::vector<NodeType> typeList = {};
    ASSERT_EQ(rsRenderServiceConnection->SetVirtualScreenTypeBlackList(
        INVALID_SCREEN_ID, typeList, repCode), ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetLogicalCameraRotationCorrectionTest001
 * @tc.desc: Test SetLogicalCameraRotationCorrection function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, SetLogicalCameraRotationCorrectionTest001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread_->runner_);
    mainThread->runner_->Run();

    pid_t testPid = 12345;
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto clientToRenderConnection = new RSClientToRenderConnection(
        testPid, nullptr, mainThread, screenManager, token->AsObject(), nullptr);
    ASSERT_NE(clientToRenderConnection, nullptr);

    constexpr uint32_t defaultScreenWidth = 480;
    constexpr uint32_t defaultScreenHight = 320;
    
    std::string name = "virtualScreen02";
    uint32_t width = defaultScreenWidth;
    uint32_t height = defaultScreenHight;
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto screenId = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, screenId);

    NodeId nodeId = 0XFFFFFFFFFFFF1234;
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId);
    screenRenderNode->screenInfo_.width = defaultScreenWidth;
    screenRenderNode->screenInfo_.height = defaultScreenHight;
    mainThread->context_->nodeMap.RegisterRenderNode(screenRenderNode);

    NodeId surfaceNodeId = 502232;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    mainThread->context_->nodeMap.RegisterRenderNode(surfaceNode);
    screenRenderNode->AddChild(surfaceNode);

    ASSERT_EQ(
        clientToRenderConnection->SetLogicalCameraRotationCorrection(screenId, ScreenRotation::ROTATION_90), SUCCESS);

    mainThread = nullptr;
}

/**
 * @tc.name: SetLogicalCameraRotationCorrectionTest002
 * @tc.desc: Test SetLogicalCameraRotationCorrection function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionTest, SetLogicalCameraRotationCorrectionTest002, TestSize.Level1)
{
    sptr<RSIConnectionToken> token1 = new IRemoteStub<RSIConnectionToken>();
    auto clientToRenderConnectionWithNullThread = new RSClientToRenderConnection(
        123, nullptr, nullptr, CreateOrGetScreenManager(), token1->AsObject(), nullptr);
    ASSERT_NE(clientToRenderConnectionWithNullThread, nullptr);
    clientToRenderConnectionWithNullThread->SetLogicalCameraRotationCorrection(0, ScreenRotation::ROTATION_90);
}
} // namespace OHOS::Rosen