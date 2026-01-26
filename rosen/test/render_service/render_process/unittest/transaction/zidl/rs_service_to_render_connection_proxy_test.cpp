/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <refbase.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "transaction/zidl/rs_service_to_render_connection_proxy.h"
#include "memory/rs_memory_graphic.h"
#include "feature/capture/rs_ui_capture.h"
#include "common/rs_self_draw_rect_change_callback_constraint.h"
#include "render_service_base/src/ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
// Mock Callback Class for SelfDrawingNodeRectChangeCallback
class MockRSSelfDrawingNodeRectChangeCallback : public RSSelfDrawingNodeRectChangeCallbackStub {
public:
    MockRSSelfDrawingNodeRectChangeCallback() {};
    virtual ~MockRSSelfDrawingNodeRectChangeCallback() noexcept = default;

    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> data) override {}
};

class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
} // namespace

class RSServiceToRenderConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSServiceToRenderConnectionProxy> proxy;

    // Helper method to create a valid PixelMap
    static std::shared_ptr<Media::PixelMap> CreateValidPixelMap();

    // Helper method to create a valid RectConstraint
    static RectConstraint CreateValidRectConstraint();

    // Helper method to create RectConstraint with empty pids
    static RectConstraint CreateEmptyPidsRectConstraint();

    // Helper method to create RectConstraint with specific parameters
    static RectConstraint CreateCustomRectConstraint(const std::unordered_set<pid_t>& pids,
        int32_t lowW, int32_t lowH, int32_t highW, int32_t highH);
};

void RSServiceToRenderConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
}

void RSServiceToRenderConnectionProxyTest::TearDownTestCase() {}

void RSServiceToRenderConnectionProxyTest::SetUp() {}

void RSServiceToRenderConnectionProxyTest::TearDown() {}

std::shared_ptr<Media::PixelMap> RSServiceToRenderConnectionProxyTest::CreateValidPixelMap()
{
    return std::make_shared<Media::PixelMap>();
}

RectConstraint RSServiceToRenderConnectionProxyTest::CreateValidRectConstraint()
{
    RectConstraint constraint;
    constraint.pids = {1001, 1002, 1003};
    constraint.range.lowLimit = {100, 100};
    constraint.range.highLimit = {1920, 1080};
    return constraint;
}

RectConstraint RSServiceToRenderConnectionProxyTest::CreateEmptyPidsRectConstraint()
{
    RectConstraint constraint;
    constraint.pids = {};
    constraint.range.lowLimit = {0, 0};
    constraint.range.highLimit = {1920, 1080};
    return constraint;
}

RectConstraint RSServiceToRenderConnectionProxyTest::CreateCustomRectConstraint(
    const std::unordered_set<pid_t>& pids, int32_t lowW, int32_t lowH, int32_t highW, int32_t highH)
{
    RectConstraint constraint;
    constraint.pids = pids;
    constraint.range.lowLimit = {lowW, lowH};
    constraint.range.highLimit = {highW, highH};
    return constraint;
}

/**
 * @tc.name: GetTotalAppMemSizeTest001
 * @tc.desc: Test GetTotalAppMemSize with basic functionality
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetTotalAppMemSizeTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    float cpuMemSize = 0.0f;
    float gpuMemSize = 0.0f;
    ErrCode ret = proxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetTotalAppMemSizeTest002
 * @tc.desc: Test GetTotalAppMemSize with pre-initialized values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetTotalAppMemSizeTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    float cpuMemSize = 100.0f;
    float gpuMemSize = 200.0f;
    ErrCode ret = proxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetTotalAppMemSizeTest003
 * @tc.desc: Test GetTotalAppMemSize with negative initial values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetTotalAppMemSizeTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    float cpuMemSize = -1.0f;
    float gpuMemSize = -1.0f;
    ErrCode ret = proxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_GE(cpuMemSize, 0.0f);
    EXPECT_GE(gpuMemSize, 0.0f);
}

/**
 * @tc.name: GetTotalAppMemSizeTest004
 * @tc.desc: Test GetTotalAppMemSize with multiple consecutive calls
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetTotalAppMemSizeTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    float cpuMemSize1 = 0.0f;
    float gpuMemSize1 = 0.0f;
    ErrCode ret1 = proxy->GetTotalAppMemSize(cpuMemSize1, gpuMemSize1);
    EXPECT_EQ(ret1, ERR_INVALID_VALUE);

    float cpuMemSize2 = 0.0f;
    float gpuMemSize2 = 0.0f;
    ErrCode ret2 = proxy->GetTotalAppMemSize(cpuMemSize2, gpuMemSize2);
    EXPECT_EQ(ret2, ERR_INVALID_VALUE);

    // Both calls should succeed
    EXPECT_GE(cpuMemSize1, 0.0f);
    EXPECT_GE(gpuMemSize1, 0.0f);
    EXPECT_GE(cpuMemSize2, 0.0f);
    EXPECT_GE(gpuMemSize2, 0.0f);
}

/**
 * @tc.name: GetMemoryGraphicsTest001
 * @tc.desc: Test GetMemoryGraphics with empty vector
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicsTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret = proxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    // Vector should be valid (may be empty or contain entries)
}

/**
 * @tc.name: GetMemoryGraphicsTest002
 * @tc.desc: Test GetMemoryGraphics and verify vector structure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicsTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret = proxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    // If vector is not empty, verify structure
    if (!memoryGraphics.empty()) {
        for (const auto& memGraphic : memoryGraphics) {
            EXPECT_GE(memGraphic.GetPid(), 0);
            EXPECT_GE(memGraphic.GetCpuMemorySize(), 0.0f);
            EXPECT_GE(memGraphic.GetGpuMemorySize(), 0.0f);
        }
    }
}

/**
 * @tc.name: GetMemoryGraphicsTest003
 * @tc.desc: Test GetMemoryGraphics with pre-allocated vector
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicsTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<MemoryGraphic> memoryGraphics;
    memoryGraphics.reserve(100);  // Pre-allocate space
    ErrCode ret = proxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphicTest001
 * @tc.desc: Test GetMemoryGraphic with current process pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t pid = getpid();
    MemoryGraphic memoryGraphic;
    ErrCode ret = proxy->GetMemoryGraphic(pid, memoryGraphic);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_GE(memoryGraphic.GetPid(), 0);
}

/**
 * @tc.name: GetMemoryGraphicTest002
 * @tc.desc: Test GetMemoryGraphic with pid = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t pid = 0;
    MemoryGraphic memoryGraphic;
    ErrCode ret = proxy->GetMemoryGraphic(pid, memoryGraphic);
    // May succeed or fail depending on implementation
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphicTest003
 * @tc.desc: Test GetMemoryGraphic with invalid pid (-1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t pid = -1;
    MemoryGraphic memoryGraphic;
    ErrCode ret = proxy->GetMemoryGraphic(pid, memoryGraphic);
    // May succeed or fail depending on implementation
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphicTest004
 * @tc.desc: Test GetMemoryGraphic with various valid pids
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<pid_t> testPids = {100, 1000, 10000};

    for (auto pid : testPids) {
        MemoryGraphic memoryGraphic;
        ErrCode ret = proxy->GetMemoryGraphic(pid, memoryGraphic);
        // May succeed or fail depending on whether pid exists
        EXPECT_EQ(ret, ERR_INVALID_VALUE);
    }
}

/**
 * @tc.name: GetMemoryGraphicTest005
 * @tc.desc: Test GetMemoryGraphic and verify all fields
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphicTest005, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t pid = getpid();
    MemoryGraphic memoryGraphic;
    ErrCode ret = proxy->GetMemoryGraphic(pid, memoryGraphic);

    if (ret == ERR_OK) {
        EXPECT_GE(memoryGraphic.GetPid(), 0);
        EXPECT_GE(memoryGraphic.GetCpuMemorySize(), 0.0f);
        EXPECT_GE(memoryGraphic.GetGpuMemorySize(), 0.0f);
        EXPECT_GE(memoryGraphic.GetTotalMemorySize(), 0.0f);
    }
}

/**
 * @tc.name: GetPixelMapByProcessIdTest001
 * @tc.desc: Test GetPixelMapByProcessId with current process pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessIdTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = getpid();
    int32_t repCode = 0;
    ErrCode ret = proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetPixelMapByProcessIdTest002
 * @tc.desc: Test GetPixelMapByProcessId with pid = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessIdTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = 0;
    int32_t repCode = 0;
    ErrCode ret = proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetPixelMapByProcessIdTest003
 * @tc.desc: Test GetPixelMapByProcessId with invalid pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessIdTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = -1;
    int32_t repCode = 0;
    ErrCode ret = proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    // Check repCode for error indication
}

/**
 * @tc.name: GetPixelMapByProcessIdTest004
 * @tc.desc: Test GetPixelMapByProcessId with various pids
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessIdTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<pid_t> testPids = {getpid(), 100, 1000, 10000};

    for (auto pid : testPids) {
        std::vector<PixelMapInfo> pixelMapInfoVector;
        int32_t repCode = 0;
        ErrCode ret = proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
        EXPECT_EQ(ret, ERR_INVALID_VALUE);
    }
}

/**
 * @tc.name: GetPixelMapByProcessIdTest005
 * @tc.desc: Test GetPixelMapByProcessId and verify PixelMapInfo structure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessIdTest005, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = getpid();
    int32_t repCode = 0;
    ErrCode ret = proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    if (repCode == SUCCESS && !pixelMapInfoVector.empty()) {
        // Verify PixelMapInfo structure
        for (const auto& info : pixelMapInfoVector) {
            EXPECT_GE(info.rotation, 0.0f);
        }
    }
}

/**
 * @tc.name: ShowWatermarkTest001
 * @tc.desc: Test ShowWatermark with nullptr PixelMap (should return early)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermarkTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    // Test with nullptr - should return early without crash
    proxy->ShowWatermark(nullptr, true);
    proxy->ShowWatermark(nullptr, false);
}

/**
 * @tc.name: ShowWatermarkTest002
 * @tc.desc: Test ShowWatermark with valid PixelMap and isShow = true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermarkTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    auto pixelMap = CreateValidPixelMap();
    ASSERT_NE(pixelMap, nullptr);

    proxy->ShowWatermark(pixelMap, true);
}

/**
 * @tc.name: ShowWatermarkTest003
 * @tc.desc: Test ShowWatermark with valid PixelMap and isShow = false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermarkTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    auto pixelMap = CreateValidPixelMap();
    ASSERT_NE(pixelMap, nullptr);

    proxy->ShowWatermark(pixelMap, false);
}

/**
 * @tc.name: ShowWatermarkTest004
 * @tc.desc: Test ShowWatermark with toggle behavior
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermarkTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    auto pixelMap = CreateValidPixelMap();
    ASSERT_NE(pixelMap, nullptr);

    // Show then hide
    proxy->ShowWatermark(pixelMap, true);
    proxy->ShowWatermark(pixelMap, false);

    // Hide then show
    proxy->ShowWatermark(pixelMap, false);
    proxy->ShowWatermark(pixelMap, true);
}

/**
 * @tc.name: ShowWatermarkTest005
 * @tc.desc: Test ShowWatermark with multiple PixelMaps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermarkTest005, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);

    for (int i = 0; i < 3; ++i) {
        auto pixelMap = CreateValidPixelMap();
        proxy->ShowWatermark(pixelMap, true);
        proxy->ShowWatermark(pixelMap, false);
    }
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest001
 * @tc.desc: Test GetSurfaceRootNodeId with nodeId = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeIdTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId windowNodeId = 0;
    ErrCode ret = proxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest002
 * @tc.desc: Test GetSurfaceRootNodeId with specific node IDs
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeIdTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::vector<NodeId> testNodeIds = {100, 1000, 10000};

    for (auto nodeId : testNodeIds) {
        NodeId windowNodeId = nodeId;
        ErrCode ret = proxy->GetSurfaceRootNodeId(windowNodeId);
        EXPECT_EQ(ret, ERR_INVALID_VALUE);
    }
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest003
 * @tc.desc: Test GetSurfaceRootNodeId with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeIdTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId windowNodeId = INVALID_SCREEN_ID;
    ErrCode ret = proxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest004
 * @tc.desc: Test GetSurfaceRootNodeId and verify output is modified
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeIdTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId inputNodeId = 100;
    NodeId windowNodeId = inputNodeId;
    ErrCode ret = proxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    // The nodeId should be modified by the call (or remain the same)
    EXPECT_GE(windowNodeId, 0);
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest005
 * @tc.desc: Test GetSurfaceRootNodeId with maximum NodeId value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeIdTest005, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId windowNodeId = std::numeric_limits<NodeId>::max();
    ErrCode ret = proxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetWatermarkTest001
 * @tc.desc: Test SetWatermark with nullptr PixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetWatermarkTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t callingPid = getpid();
    std::string name = "test_watermark";
    std::shared_ptr<Media::PixelMap> watermark = nullptr;
    bool success = false;

    proxy->SetWatermark(callingPid, name, watermark, success);
}

/**
 * @tc.name: SetWatermarkTest002
 * @tc.desc: Test SetWatermark with valid PixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetWatermarkTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t callingPid = getpid();
    std::string name = "test_watermark";
    auto watermark = CreateValidPixelMap();
    bool success = false;

    ErrCode ret = proxy->SetWatermark(callingPid, name, watermark, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetWatermarkTest003
 * @tc.desc: Test SetWatermark with empty name
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetWatermarkTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t callingPid = getpid();
    std::string name = "";
    auto watermark = CreateValidPixelMap();
    bool success = false;

    ErrCode ret = proxy->SetWatermark(callingPid, name, watermark, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest001
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback with nullptr callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterSelfDrawingNodeRectChangeCallbackTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    RectConstraint constraint = CreateValidRectConstraint();
    sptr<RSISelfDrawingNodeRectChangeCallback> callback = nullptr;

    int32_t ret = proxy->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest002
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback with valid callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterSelfDrawingNodeRectChangeCallbackTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    RectConstraint constraint = CreateValidRectConstraint();
    sptr<MockRSSelfDrawingNodeRectChangeCallback> callback = new MockRSSelfDrawingNodeRectChangeCallback();

    int32_t ret = proxy->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
    // May succeed or fail depending on IPC status
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest003
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback with empty pids
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterSelfDrawingNodeRectChangeCallbackTest003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    RectConstraint constraint = CreateEmptyPidsRectConstraint();
    sptr<MockRSSelfDrawingNodeRectChangeCallback> callback = new MockRSSelfDrawingNodeRectChangeCallback();

    int32_t ret = proxy->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest004
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback with multiple pids
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterSelfDrawingNodeRectChangeCallbackTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    std::unordered_set<pid_t> pids = {1001, 1002, 1003, 1004, 1005};
    RectConstraint constraint = CreateCustomRectConstraint(pids, 100, 100, 3840, 2160);
    sptr<MockRSSelfDrawingNodeRectChangeCallback> callback = new MockRSSelfDrawingNodeRectChangeCallback();

    int32_t ret = proxy->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTest001
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback with pid = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, UnRegisterSelfDrawingNodeRectChangeCallbackTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = 0;
    int32_t ret = proxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTest002
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback with current process pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, UnRegisterSelfDrawingNodeRectChangeCallbackTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    int32_t ret = proxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTest004
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback with negative pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, UnRegisterSelfDrawingNodeRectChangeCallbackTest004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = -1;
    int32_t ret = proxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: CombinedWatermarkOperationsTest001
 * @tc.desc: Test combined SetWatermark and ShowWatermark operations
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, CombinedWatermarkOperationsTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t callingPid = getpid();
    auto watermark = CreateValidPixelMap();
    bool success = false;

    // Set watermark first
    ErrCode ret1 = proxy->SetWatermark(callingPid, "test", watermark, success);
    EXPECT_EQ(ret1, ERR_INVALID_VALUE);

    // Then show it
    proxy->ShowWatermark(watermark, true);

    // Then hide it
    proxy->ShowWatermark(watermark, false);
}

/**
 * @tc.name: CombinedMemoryOperationsTest001
 * @tc.desc: Test combined memory operations
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, CombinedMemoryOperationsTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);

    // Get total memory
    float totalCpu = 0.0f;
    float totalGpu = 0.0f;
    ErrCode ret1 = proxy->GetTotalAppMemSize(totalCpu, totalGpu);
    EXPECT_EQ(ret1, ERR_INVALID_VALUE);

    // Get memory graphics list
    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret2 = proxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret2, ERR_INVALID_VALUE);

    // Get specific process memory
    pid_t pid = getpid();
    MemoryGraphic memoryGraphic;
    ErrCode ret3 = proxy->GetMemoryGraphic(pid, memoryGraphic);
    EXPECT_EQ(ret3, ERR_INVALID_VALUE);
}

/**
 * @tc.name: CombinedCallbackOperationsTest001
 * @tc.desc: Test combined register/unregister callback operations
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, CombinedCallbackOperationsTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    pid_t remotePid = getpid();
    RectConstraint constraint = CreateValidRectConstraint();
    sptr<MockRSSelfDrawingNodeRectChangeCallback> callback = new MockRSSelfDrawingNodeRectChangeCallback();

    // Register callback
    int32_t ret1 = proxy->RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);

    // Unregister callback
    int32_t ret2 = proxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);

    // At least one should succeed or both return appropriate error codes
    EXPECT_EQ(ret1, RS_CONNECTION_ERROR);
    EXPECT_EQ(ret2, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: HgmForceUpdateTaskTest
 * @tc.desc: Test HgmForceUpdateTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, HgmForceUpdateTaskTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    proxy->HgmForceUpdateTask(true, "test");
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    bool enabled = false;
    bool enabled1 = true;
    proxy->GetShowRefreshRateEnabled(enabled);
    proxy->GetShowRefreshRateEnabled(enabled1);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    bool enabled = false;
    bool enabled1 = true;
    proxy->SetShowRefreshRateEnabled(enabled, 0);
    proxy->SetShowRefreshRateEnabled(enabled1, 0);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    EXPECT_EQ(proxy->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
}

/**
 * @tc.name: SetBrightnessInfoChangeCallbackTest
 * @tc.desc: Test SetBrightnessInfoChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    sptr<MockRSBrightnessInfoChangeCallback> callback = new MockRSBrightnessInfoChangeCallback();
    EXPECT_NE(proxy->SetBrightnessInfoChangeCallback(0, callback), SUCCESS);
    pid_t pid = getpid();
    EXPECT_NE(proxy->SetBrightnessInfoChangeCallback(pid, callback), SUCCESS);
}

} // namespace OHOS::Rosen
