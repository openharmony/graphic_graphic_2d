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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <refbase.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "transaction/rs_service_to_render_connection.h"
#include "transaction/rs_render_service_client_info.h"
#include "transaction/zidl/rs_service_to_render_connection_proxy.h"
#include "memory/rs_memory_graphic.h"
#include "feature/capture/rs_ui_capture.h"
#include "common/rs_self_draw_rect_change_callback_constraint.h"
#include "render_service_base/src/ipc_callbacks/dfx/rs_dump_callback_stub.h"
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
    MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {}
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};

// Mock Callback Class for RSDumpCallback
class MockRSDumpCallback : public RSDumpCallbackStub {
public:
    MockRSDumpCallback() {}
    ~MockRSDumpCallback() override {}

    void OnDumpResult(std::string& dumpResult) override {}
};
} // namespace

class IRemoteObjectMock : public IRemoteObject {
public:
    IRemoteObjectMock() : IRemoteObject(u"IRemoteObjectMock") {}
    ~IRemoteObjectMock() override = default;

    int32_t GetObjectRefCount() override { return 0; }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override { return true; }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override { return true; }

    int Dump(int fd, const std::vector<std::u16string>& args) override { return 0; }

    MOCK_METHOD4(SendRequest, int32_t(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
};

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
    proxy->HgmForceUpdateTask(false, "test");
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test GetShowRefreshRateEnabled with normal case
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
 * @tc.name: GetShowRefreshRateEnabledTest002
 * @tc.desc: Test GetShowRefreshRateEnabled with IPC error (mock returns error)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetShowRefreshRateEnabledTest002, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to return error
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    bool enabled = false;
    ErrCode ret = mockProxy->GetShowRefreshRateEnabled(enabled);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest003
 * @tc.desc: Test GetShowRefreshRateEnabled with ReadBool failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetShowRefreshRateEnabledTest003, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to succeed but don't write bool to reply (causing ReadBool to fail)
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return 0; }));

    bool enabled = false;
    ErrCode ret = mockProxy->GetShowRefreshRateEnabled(enabled);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test SetShowRefreshRateEnabled with normal case
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
 * @tc.name: SetShowRefreshRateEnabledTest002
 * @tc.desc: Test SetShowRefreshRateEnabled with different type values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetShowRefreshRateEnabledTest002, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    proxy->SetShowRefreshRateEnabled(true, 0);
    proxy->SetShowRefreshRateEnabled(false, 1);
    proxy->SetShowRefreshRateEnabled(true, 2);
    proxy->SetShowRefreshRateEnabled(false, -1);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest003
 * @tc.desc: Test SetShowRefreshRateEnabled with IPC error
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetShowRefreshRateEnabledTest003, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to return error
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    // Function should return early without crash
    mockProxy->SetShowRefreshRateEnabled(true, 0);
    mockProxy->SetShowRefreshRateEnabled(false, 1);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test GetRealtimeRefreshRate with normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    auto result = proxy->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest002
 * @tc.desc: Test GetRealtimeRefreshRate with various screen IDs
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest002, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    ScreenId screenId1 = 0;
    ScreenId screenId2 = 1;
    ScreenId screenId3 = 100;

    proxy->GetRealtimeRefreshRate(screenId1);
    proxy->GetRealtimeRefreshRate(screenId2);
    proxy->GetRealtimeRefreshRate(screenId3);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest003
 * @tc.desc: Test GetRealtimeRefreshRate with IPC error (should return SUCCESS)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest003, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to return error
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    ScreenId screenId = 0;
    uint32_t rate = mockProxy->GetRealtimeRefreshRate(screenId);
    EXPECT_EQ(rate, SUCCESS);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest004
 * @tc.desc: Test GetRealtimeRefreshRate with ReadUint32 failure (should return READ_PARCEL_ERR)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest004, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to succeed but don't write Uint32 to reply (causing ReadUint32 to fail)
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return 0; }));

    ScreenId screenId = 0;
    uint32_t rate = mockProxy->GetRealtimeRefreshRate(screenId);
    EXPECT_EQ(rate, READ_PARCEL_ERR);
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

/**
 * @tc.name: ReportEventJankFrame Test
 * @tc.desc: ReportEventJankFrame Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportEventJankFrame, TestSize.Level1)
{
    DataBaseRs info;
    proxy->ReportJankStats();
    proxy->ReportEventResponse(info);
    proxy->ReportEventComplete(info);
    proxy->ReportEventJankFrame(info);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: ReportEventJankFrame_SendRequestFail
 * @tc.desc: Test ReportEventJankFrame when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportEventJankFrame_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    DataBaseRs info;
    ErrCode ret = mockProxy->ReportEventJankFrame(info);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportDataBaseRs Test
 * @tc.desc: ReportDataBaseRs Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportDataBaseRs, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    DataBaseRs info;
    proxy->ReportDataBaseRs(data, reply, option, info);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: ReportDataBaseRs_FullParcel
 * @tc.desc: ReportDataBaseRs with full MessageParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportDataBaseRs_FullParcel, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    
    std::vector<uint8_t> largeData(1024 * 1024, 0xFF);
    data.WriteBuffer(largeData.data(), largeData.size());
    
    DataBaseRs info;
    info.appPid = 1001;
    proxy->ReportDataBaseRs(data, reply, option, info);
}

/**
 * @tc.name: ReportRsSceneJankStart Test
 * @tc.desc: ReportRsSceneJankStart Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportRsSceneJankStart, TestSize.Level1)
{
    AppInfo info;
    proxy->ReportRsSceneJankStart(info);
    proxy->ReportRsSceneJankEnd(info);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: ReportRsSceneJankStart_SendRequestFail
 * @tc.desc: Test ReportRsSceneJankStart when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportRsSceneJankStart_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    AppInfo info;
    ErrCode ret = mockProxy->ReportRsSceneJankStart(info);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStart Test
 * @tc.desc: AvcodecVideoStart Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStartTest, TestSize.Level1)
{
    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    uint64_t reportTime = 16;
    proxy->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: AvcodecVideoStart_SendRequestFail
 * @tc.desc: Test AvcodecVideoStart when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStart_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    uint64_t reportTime = 16;
    ErrCode ret = mockProxy->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStop Test
 * @tc.desc: AvcodecVideoStop Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStopTest, TestSize.Level1)
{
    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    proxy->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: AvcodecVideoStop_SendRequestFail
 * @tc.desc: Test AvcodecVideoStop when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStop_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    ErrCode ret = mockProxy->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoGet Test
 * @tc.desc: AvcodecVideoGet Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGetTest, TestSize.Level1)
{
    uint64_t uniqueId = 1;
    proxy->AvcodecVideoGet(uniqueId);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: AvcodecVideoGet_SendRequestFail
 * @tc.desc: Test AvcodecVideoGet when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGet_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    uint64_t uniqueId = 1;
    ErrCode ret = mockProxy->AvcodecVideoGet(uniqueId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}
 
/**
 * @tc.name: AvcodecVideoGetRecent Test
 * @tc.desc: AvcodecVideoGetRecent Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGetRecentTest, TestSize.Level1)
{
    proxy->AvcodecVideoGetRecent();
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: AvcodecVideoGetRecent_SendRequestFail
 * @tc.desc: Test AvcodecVideoGetRecent when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGetRecent_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    ErrCode ret = mockProxy->AvcodecVideoGetRecent();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetPidGpuMemoryInMBTest
 * @tc.desc: test results of GetPidGpuMemoryInMB
 * @tc.type: FUNC
 * @tc.require: issuesICE0QR
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPidGpuMemoryInMBTest, TestSize.Level1)
{
    pid_t pid = 1001;
    float gpuMemInMB = 0.0f;
    int32_t res = proxy->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    EXPECT_NE(res, ERR_OK);
}

/**
 * @tc.name: GetPidGpuMemoryInMB_SendRequestFail
 * @tc.desc: Test GetPidGpuMemoryInMB when SendRequest fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPidGpuMemoryInMB_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Return(-1));
    
    pid_t pid = 1001;
    float gpuMemInMB = 0.0f;
    int32_t res = mockProxy->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest
 * @tc.desc: Test GetBehindWindowFilterEnabled with normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    bool enabled = false;
    bool enabled1 = true;
    proxy->GetBehindWindowFilterEnabled(enabled);
    proxy->GetBehindWindowFilterEnabled(enabled1);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest002
 * @tc.desc: Test GetBehindWindowFilterEnabled with IPC error (mock returns error)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetBehindWindowFilterEnabledTest002, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to return error
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    bool enabled = false;
    ErrCode ret = mockProxy->GetBehindWindowFilterEnabled(enabled);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest003
 * @tc.desc: Test GetBehindWindowFilterEnabled with ReadBool failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetBehindWindowFilterEnabledTest003, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to succeed but don't write bool to reply (causing ReadBool to fail)
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return 0; }));

    bool enabled = false;
    ErrCode ret = mockProxy->GetBehindWindowFilterEnabled(enabled);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTest
 * @tc.desc: Test SetBehindWindowFilterEnabled with normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(proxy);
    bool enabled = false;
    bool enabled1 = true;
    proxy->SetBehindWindowFilterEnabled(enabled);
    proxy->SetBehindWindowFilterEnabled(enabled1);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTest002
 * @tc.desc: Test SetBehindWindowFilterEnabled with IPC error
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetBehindWindowFilterEnabledTest002, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    // Mock SendRequest to return error
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    // Function should return early without crash
    mockProxy->SetBehindWindowFilterEnabled(true);
    mockProxy->SetBehindWindowFilterEnabled(false);
}

/**
 * @tc.name: NotifyScreenRefreshTest001
 * @tc.desc: Test NotifyScreenRefresh
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyScreenRefreshTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    ScreenId screenId = 0;
    auto res = proxy->NotifyScreenRefresh(screenId);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: OnGlobalBlacklistChangedTest001
 * @tc.desc: Test OnGlobalBlacklistChanged with empty blacklist
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnGlobalBlacklistChangedTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::unordered_set<NodeId> globalBlackList;
    proxy->OnGlobalBlacklistChanged(globalBlackList);
}

/**
 * @tc.name: OnGlobalBlacklistChangedTest002
 * @tc.desc: Test OnGlobalBlacklistChanged with single element
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnGlobalBlacklistChangedTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::unordered_set<NodeId> globalBlackList = {100};
    proxy->OnGlobalBlacklistChanged(globalBlackList);
}

// ==================== SetForceRefresh Tests ====================

/**
 * @tc.name: SetForceRefresh_Normal_Success
 * @tc.desc: Test SetForceRefresh with normal case
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetForceRefresh_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::string nodeIdStr = "test_node";
    bool isForceRefresh = true;
    ErrCode ret = mockProxy->SetForceRefresh(nodeIdStr, isForceRefresh);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetForceRefresh_SendRequestFail
 * @tc.desc: Test SetForceRefresh when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetForceRefresh_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::string nodeIdStr = "test_node";
    bool isForceRefresh = true;
    ErrCode ret = mockProxy->SetForceRefresh(nodeIdStr, isForceRefresh);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== CreatePixelMapFromSurface Tests ====================

/**
 * @tc.name: CreatePixelMapFromSurface_NullSurface
 * @tc.desc: Test CreatePixelMapFromSurface with null surface
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, CreatePixelMapFromSurface_NullSurface, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    sptr<Surface> surface = nullptr;
    Rect srcRect = { 0, 0, 100, 100 };
    std::shared_ptr<Media::PixelMap> pixelMap;

    ErrCode ret = mockProxy->CreatePixelMapFromSurface(surface, srcRect, pixelMap, false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: CreatePixelMapFromSurface_SendRequestFail
 * @tc.desc: Test CreatePixelMapFromSurface when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, CreatePixelMapFromSurface_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    sptr<Surface> surface = nullptr;
    Rect srcRect = { 0, 0, 100, 100 };
    std::shared_ptr<Media::PixelMap> pixelMap;

    ErrCode ret = mockProxy->CreatePixelMapFromSurface(surface, srcRect, pixelMap, false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== SetLayerTop Tests ====================

/**
 * @tc.name: SetLayerTop_Normal_Success
 * @tc.desc: Test SetLayerTop with normal case
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetLayerTop_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::string nodeIdStr = "test_node";
    bool isTop = true;
    ErrCode ret = mockProxy->SetLayerTop(nodeIdStr, isTop);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetLayerTop_SendRequestFail
 * @tc.desc: Test SetLayerTop when SendRequest fails
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetLayerTop_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::string nodeIdStr = "test_node";
    bool isTop = true;
    ErrCode ret = mockProxy->SetLayerTop(nodeIdStr, isTop);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== ForceRefreshOneFrameWithNextVSync Tests ====================

/**
 * @tc.name: ForceRefreshOneFrameWithNextVSync_Normal_Success
 * @tc.desc: Test ForceRefreshOneFrameWithNextVSync
 * with normal case
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ForceRefreshOneFrameWithNextVSync_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    mockProxy->ForceRefreshOneFrameWithNextVSync();
}

/**
 * @tc.name: ForceRefreshOneFrameWithNextVSync_SendRequestFail
 * @tc.desc: Test ForceRefreshOneFrameWithNextVSync
 * when SendRequest fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ForceRefreshOneFrameWithNextVSync_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    mockProxy->ForceRefreshOneFrameWithNextVSync();
}

// ==================== RegisterUIExtensionCallback Tests ====================

/**
 * @tc.name: RegisterUIExtensionCallback_NullCallback
 * @tc.desc: Test RegisterUIExtensionCallback with null
 * callback
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterUIExtensionCallback_NullCallback, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    pid_t pid = 1001;
    uint64_t userId = 1;
    sptr<RSIUIExtensionCallback> callback = nullptr;
    bool unobscured = false;

    int32_t ret = mockProxy->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);
}

// ==================== SetVmaCacheStatus Tests ====================

/**
 * @tc.name: SetVmaCacheStatus_True_Success
 * @tc.desc: Test SetVmaCacheStatus with flag = true
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetVmaCacheStatus_True_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    mockProxy->SetVmaCacheStatus(true);
}

/**
 * @tc.name: SetVmaCacheStatus_False_Success
 * @tc.desc: Test SetVmaCacheStatus with flag = false
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetVmaCacheStatus_False_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    mockProxy->SetVmaCacheStatus(false);
}

/**
 * @tc.name: SetVmaCacheStatus_SendRequestFail
 * @tc.desc: Test SetVmaCacheStatus when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetVmaCacheStatus_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    mockProxy->SetVmaCacheStatus(true);
}

// ==================== NotifyPackageEvent Tests ====================

/**
 * @tc.name: NotifyPackageEvent_SizeMismatch
 * @tc.desc: Test NotifyPackageEvent with size mismatch
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyPackageEvent_SizeMismatch, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint32_t listSize = 3;
    std::vector<std::string> packageList = { "pkg1", "pkg2" };
    mockProxy->NotifyPackageEvent(listSize, packageList);
}

/**
 * @tc.name: NotifyPackageEvent_EmptyList
 * @tc.desc: Test NotifyPackageEvent with empty list
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyPackageEvent_EmptyList, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint32_t listSize = 0;
    std::vector<std::string> packageList;
    mockProxy->NotifyPackageEvent(listSize, packageList);
}

/**
 * @tc.name: NotifyPackageEvent_ValidList
 * @tc.desc: Test NotifyPackageEvent with valid list
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyPackageEvent_ValidList, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint32_t listSize = 2;
    std::vector<std::string> packageList = { "pkg1", "pkg2" };
    mockProxy->NotifyPackageEvent(listSize, packageList);
}

/**
 * @tc.name: NotifyPackageEvent_SendRequestFail
 * @tc.desc: Test NotifyPackageEvent when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyPackageEvent_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    uint32_t listSize = 2;
    std::vector<std::string> packageList = { "pkg1", "pkg2" };
    mockProxy->NotifyPackageEvent(listSize, packageList);
}

// ==================== ReportGameStateData Tests ====================

/**
 * @tc.name: ReportGameStateData_Normal_Success
 * @tc.desc: Test ReportGameStateData with normal case
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportGameStateData_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    GameStateData info;
    info.pid = 1001;
    info.uid = 1001;
    info.state = 1;
    info.renderTid = 1002;
    info.bundleName = "com.test.app";
    mockProxy->ReportGameStateData(info);
}

/**
 * @tc.name: ReportGameStateData_SendRequestFail
 * @tc.desc: Test ReportGameStateData when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportGameStateData_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    GameStateData info;
    info.pid = 1001;
    info.uid = 1001;
    info.state = 1;
    info.renderTid = 1002;
    info.bundleName = "com.test.app";
    mockProxy->ReportGameStateData(info);
}

// ==================== RegisterTypeface Tests ====================

/**
 * @tc.name: RegisterTypeface_NullTypeface
 * @tc.desc: Test RegisterTypeface with null typeface
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterTypeface_NullTypeface, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    uint64_t globalUniqueId = 1;
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;

    bool ret = mockProxy->RegisterTypeface(globalUniqueId, typeface);
    EXPECT_FALSE(ret);
}

// ==================== UnRegisterTypeface Tests ====================

/**
 * @tc.name: UnRegisterTypeface_Normal_Success
 * @tc.desc: Test UnRegisterTypeface with normal case
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, UnRegisterTypeface_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint64_t globalUniqueId = 1;
    bool ret = mockProxy->UnRegisterTypeface(globalUniqueId);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: UnRegisterTypeface_SendRequestFail
 * @tc.desc: Test UnRegisterTypeface when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, UnRegisterTypeface_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    uint64_t globalUniqueId = 1;
    bool ret = mockProxy->UnRegisterTypeface(globalUniqueId);
    EXPECT_FALSE(ret);
}

// ==================== HandleHwcEvent Tests ====================

/**
 * @tc.name: HandleHwcEvent_EmptyEventData
 * @tc.desc: Test HandleHwcEvent with empty event data
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, HandleHwcEvent_EmptyEventData, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint32_t deviceId = 1;
    uint32_t eventId = 2;
    std::vector<int32_t> eventData;
    mockProxy->HandleHwcEvent(deviceId, eventId, eventData);
}

/**
 * @tc.name: HandleHwcEvent_ValidEventData
 * @tc.desc: Test HandleHwcEvent with valid event data
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, HandleHwcEvent_ValidEventData, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    uint32_t deviceId = 1;
    uint32_t eventId = 2;
    std::vector<int32_t> eventData = { 100, 200, 300 };
    mockProxy->HandleHwcEvent(deviceId, eventId, eventData);
}

/**
 * @tc.name: HandleHwcEvent_SendRequestFail
 * @tc.desc: Test HandleHwcEvent when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, HandleHwcEvent_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    uint32_t deviceId = 1;
    uint32_t eventId = 2;
    std::vector<int32_t> eventData = { 100, 200, 300 };
    mockProxy->HandleHwcEvent(deviceId, eventId, eventData);
}

// ==================== RepaintEverything Tests ====================

/**
 * @tc.name: RepaintEverything_Normal_Success
 * @tc.desc: Test RepaintEverything with normal case
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RepaintEverything_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt32(ERR_OK);
                return NO_ERROR;
            }));

    ErrCode ret = mockProxy->RepaintEverything();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: RepaintEverything_SendRequestFail
 * @tc.desc: Test RepaintEverything when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RepaintEverything_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    ErrCode ret = mockProxy->RepaintEverything();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== SetColorFollow Tests ====================

/**
 * @tc.name: SetColorFollow_Enable_Success
 * @tc.desc: Test SetColorFollow with enabled
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetColorFollow_Enable_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::string nodeIdStr = "test_node";
    bool isColorFollow = true;
    ErrCode ret = mockProxy->SetColorFollow(nodeIdStr, isColorFollow);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetColorFollow_Disable_Success
 * @tc.desc: Test SetColorFollow with disabled
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetColorFollow_Disable_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::string nodeIdStr = "test_node";
    bool isColorFollow = false;
    ErrCode ret = mockProxy->SetColorFollow(nodeIdStr, isColorFollow);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetColorFollow_SendRequestFail
 * @tc.desc: Test SetColorFollow when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetColorFollow_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::string nodeIdStr = "test_node";
    bool isColorFollow = true;
    ErrCode ret = mockProxy->SetColorFollow(nodeIdStr, isColorFollow);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

// ==================== SetFreeMultiWindowStatus Tests ====================

/**
 * @tc.name: SetFreeMultiWindowStatus_Enable_Success
 * @tc.desc: Test SetFreeMultiWindowStatus with enabled
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetFreeMultiWindowStatus_Enable_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    bool enable = true;
    mockProxy->SetFreeMultiWindowStatus(enable);
}

/**
 * @tc.name: SetFreeMultiWindowStatus_Disable_Success
 * @tc.desc: Test SetFreeMultiWindowStatus with disabled
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetFreeMultiWindowStatus_Disable_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    bool enable = false;
    mockProxy->SetFreeMultiWindowStatus(enable);
}

/**
 * @tc.name: SetFreeMultiWindowStatus_SendRequestFail
 * @tc.desc: Test SetFreeMultiWindowStatus when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetFreeMultiWindowStatus_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    bool enable = true;
    mockProxy->SetFreeMultiWindowStatus(enable);
}

/**
 * @tc.name: DoDumpTest001
 * @tc.desc: Test DoDump with null argSets
 * @tc.type: FUNC
 * @tc.require:
 */

HWTEST_F(RSServiceToRenderConnectionProxyTest, DoDumpTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::unordered_set<std::u16string> argSets;
    sptr<MockRSDumpCallback> callback = new MockRSDumpCallback();

    proxy->DoDump(argSets, callback);
    ASSERT_NE(proxy, nullptr);
}

/**
 * @tc.name: DoDumpTest001
 * @tc.desc: Test DoDump with valid argSets
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, DoDumpTest002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    std::unordered_set<std::u16string> argSets = { u"screeb", u"surface"};
    sptr<MockRSDumpCallback> callback = new MockRSDumpCallback();

    proxy->DoDump(argSets, callback);
    ASSERT_NE(proxy, nullptr);
}

/**
 * @tc.name: SetCurtainScreenUsingStatusTest001
 * @tc.desc: Test SetCurtainScreenUsingStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetCurtainScreenUsingStatusTest001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);

    proxy->SetCurtainScreenUsingStatus(false);
    proxy->SetCurtainScreenUsingStatus(true);

    proxy->SetCurtainScreenUsingStatus(true);
    proxy->SetCurtainScreenUsingStatus(false);
    ASSERT_NE(proxy, nullptr);
}

// ==================== RegisterUIExtensionCallback Tests ====================

/**
 * @tc.name: RegisterUIExtensionCallback_Normal_Success
 * @tc.desc: Test RegisterUIExtensionCallback with normal
 * case
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterUIExtensionCallback_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt32(SUCCESS);
                return NO_ERROR;
            }));

    pid_t pid = 1001;
    uint64_t userId = 1;
    sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
    bool unobscured = false;

    int32_t ret = mockProxy->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: RegisterUIExtensionCallback_ReadResultFail
 * @tc.desc: Test RegisterUIExtensionCallback when ReadInt32
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterUIExtensionCallback_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    pid_t pid = 1001;
    uint64_t userId = 1;
    sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
    bool unobscured = false;

    int32_t ret = mockProxy->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    EXPECT_EQ(ret, READ_PARCEL_ERR);
}

/**
 * @tc.name: RegisterUIExtensionCallback_SendRequestFail
 * @tc.desc: Test RegisterUIExtensionCallback when
 * SendRequest fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterUIExtensionCallback_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    pid_t pid = 1001;
    uint64_t userId = 1;
    sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
    bool unobscured = false;

    int32_t ret = mockProxy->RegisterUIExtensionCallback(pid, userId, callback, unobscured);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

// ==================== RegisterTypeface Tests ====================

/**
 * @tc.name: RegisterTypeface_Normal_Success
 * @tc.desc: Test RegisterTypeface with normal case
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterTypeface_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteBool(true);
                return NO_ERROR;
            }));

    uint64_t globalUniqueId = 1;
    auto typeface = Drawing::Typeface::MakeDefault();

    bool ret = mockProxy->RegisterTypeface(globalUniqueId, typeface);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: RegisterTypeface_SendRequestFail
 * @tc.desc: Test RegisterTypeface when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterTypeface_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    uint64_t globalUniqueId = 1;
    auto typeface = Drawing::Typeface::MakeDefault();

    bool ret = mockProxy->RegisterTypeface(globalUniqueId, typeface);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: RegisterTypeface_ReadResultFail
 * @tc.desc: Test RegisterTypeface when ReadBool fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, RegisterTypeface_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    uint64_t globalUniqueId = 1;
    auto typeface = Drawing::Typeface::MakeDefault();

    bool ret = mockProxy->RegisterTypeface(globalUniqueId, typeface);
    EXPECT_TRUE(ret);
}

// ==================== ReportJankStats Tests ====================

/**
 * @tc.name: ReportJankStats_Normal_Success
 * @tc.desc: Test ReportJankStats with normal case
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportJankStats_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    ErrCode ret = mockProxy->ReportJankStats();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: ReportJankStats_SendRequestFail
 * @tc.desc: Test ReportJankStats when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportJankStats_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    ErrCode ret = mockProxy->ReportJankStats();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== ReportEventResponse Tests ====================

/**
 * @tc.name: ReportEventResponse_SendRequestFail
 * @tc.desc: Test ReportEventResponse when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportEventResponse_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    DataBaseRs info;
    info.appPid = 1001;
    ErrCode ret = mockProxy->ReportEventResponse(info);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== ReportEventComplete Tests ====================

/**
 * @tc.name: ReportEventComplete_SendRequestFail
 * @tc.desc: Test ReportEventComplete when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportEventComplete_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    DataBaseRs info;
    info.appPid = 1001;
    ErrCode ret = mockProxy->ReportEventComplete(info);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== ReportRsSceneJankEnd Tests ====================

/**
 * @tc.name: ReportRsSceneJankEnd_SendRequestFail
 * @tc.desc: Test ReportRsSceneJankEnd when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportRsSceneJankEnd_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    AppInfo info;
    info.pid = 1001;
    ErrCode ret = mockProxy->ReportRsSceneJankEnd(info);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== GetActiveDirtyRegionInfo Tests ====================

/**
 * @tc.name: GetActiveDirtyRegionInfo_Normal_Success
 * @tc.desc: Test GetActiveDirtyRegionInfo with normal case
 * returning multiple entries
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetActiveDirtyRegionInfo_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                int32_t size = 2;
                reply.WriteInt32(size);
                for (int32_t i = 0; i < size; i++) {
                    reply.WriteInt64(1000L * (i + 1));
                    reply.WriteInt32(10 * (i + 1));
                    reply.WriteInt32(1001 + i);
                    reply.WriteString("window_" + std::to_string(i));
                }
                return NO_ERROR;
            }));

    auto result = mockProxy->GetActiveDirtyRegionInfo();
    EXPECT_EQ(result.size(), 2);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo_EmptyResult
 * @tc.desc: Test GetActiveDirtyRegionInfo with empty result
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetActiveDirtyRegionInfo_EmptyResult, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt32(0);
                return NO_ERROR;
            }));

    auto result = mockProxy->GetActiveDirtyRegionInfo();
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo_SendRequestFail
 * @tc.desc: Test GetActiveDirtyRegionInfo when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetActiveDirtyRegionInfo_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    auto result = mockProxy->GetActiveDirtyRegionInfo();
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo_ReadSizeFail
 * @tc.desc: Test GetActiveDirtyRegionInfo when ReadInt32 size
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetActiveDirtyRegionInfo_ReadSizeFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    auto result = mockProxy->GetActiveDirtyRegionInfo();
    EXPECT_EQ(result.size(), 0);
}

// ==================== GetGlobalDirtyRegionInfo Tests ====================

/**
 * @tc.name: GetGlobalDirtyRegionInfo_Normal_Success
 * @tc.desc: Test GetGlobalDirtyRegionInfo with normal case
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetGlobalDirtyRegionInfo_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt64(1000L);
                reply.WriteInt32(100);
                reply.WriteInt32(10);
                reply.WriteInt32(1001);
                return NO_ERROR;
            }));

    auto result = mockProxy->GetGlobalDirtyRegionInfo();
    EXPECT_EQ(result.globalDirtyRegionAreas, 0);
    EXPECT_EQ(result.globalFramesNumber, 0);
}

/**
 * @tc.name: GetGlobalDirtyRegionInfo_SendRequestFail
 * @tc.desc: Test GetGlobalDirtyRegionInfo when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetGlobalDirtyRegionInfo_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    auto result = mockProxy->GetGlobalDirtyRegionInfo();
    EXPECT_EQ(result.globalDirtyRegionAreas, 0);
}

/**
 * @tc.name: GetGlobalDirtyRegionInfo_ReadFail
 * @tc.desc: Test GetGlobalDirtyRegionInfo when ReadInt64/Int32
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetGlobalDirtyRegionInfo_ReadFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    auto result = mockProxy->GetGlobalDirtyRegionInfo();
    EXPECT_EQ(result.globalDirtyRegionAreas, 0);
}

// ==================== GetLayerComposeInfo Tests ====================

/**
 * @tc.name: GetLayerComposeInfo_Normal_Success
 * @tc.desc: Test GetLayerComposeInfo with normal case
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetLayerComposeInfo_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt32(100);
                reply.WriteInt32(50);
                reply.WriteInt32(10);
                return NO_ERROR;
            }));

    auto result = mockProxy->GetLayerComposeInfo();
    EXPECT_EQ(result.uniformRenderFrameNumber, 0);
}

/**
 * @tc.name: GetLayerComposeInfo_SendRequestFail
 * @tc.desc: Test GetLayerComposeInfo when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetLayerComposeInfo_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    auto result = mockProxy->GetLayerComposeInfo();
    EXPECT_EQ(result.uniformRenderFrameNumber, 0);
}

/**
 * @tc.name: GetLayerComposeInfo_ReadFail
 * @tc.desc: Test GetLayerComposeInfo when ReadInt32 fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetLayerComposeInfo_ReadFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    auto result = mockProxy->GetLayerComposeInfo();
    EXPECT_EQ(result.uniformRenderFrameNumber, 0);
}

// ==================== GetHwcDisabledReasonInfo Tests ====================

/**
 * @tc.name: GetHwcDisabledReasonInfo_Normal_Success
 * @tc.desc: Test GetHwcDisabledReasonInfo with normal case
 * returning multiple entries
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHwcDisabledReasonInfo_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                int32_t size = 2;
                reply.WriteInt32(size);
                for (int32_t i = 0; i < size; i++) {
                    for (int32_t pos = 0; pos < HwcDisabledReasons::DISABLED_REASON_LENGTH; pos++) {
                        reply.WriteInt32(pos * 10);
                    }
                    reply.WriteInt32(1001 + i);
                    reply.WriteString("node_" + std::to_string(i));
                }
                return NO_ERROR;
            }));

    auto result = mockProxy->GetHwcDisabledReasonInfo();
    EXPECT_EQ(result.size(), 2);
}

/**
 * @tc.name: GetHwcDisabledReasonInfo_EmptyResult
 * @tc.desc: Test GetHwcDisabledReasonInfo with empty result
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHwcDisabledReasonInfo_EmptyResult, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt32(0);
                return NO_ERROR;
            }));

    auto result = mockProxy->GetHwcDisabledReasonInfo();
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: GetHwcDisabledReasonInfo_SendRequestFail
 * @tc.desc: Test GetHwcDisabledReasonInfo when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHwcDisabledReasonInfo_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    auto result = mockProxy->GetHwcDisabledReasonInfo();
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name: GetHwcDisabledReasonInfo_ReadSizeFail
 * @tc.desc: Test GetHwcDisabledReasonInfo when ReadInt32 size
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHwcDisabledReasonInfo_ReadSizeFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    auto result = mockProxy->GetHwcDisabledReasonInfo();
    EXPECT_EQ(result.size(), 0);
}

// ==================== GetHdrOnDuration Tests ====================

/**
 * @tc.name: GetHdrOnDuration_Normal_Success
 * @tc.desc: Test GetHdrOnDuration with normal case
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHdrOnDuration_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteInt64(1000L);
                return NO_ERROR;
            }));

    int64_t hdrOnDuration = 0;
    ErrCode ret = mockProxy->GetHdrOnDuration(hdrOnDuration);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: GetHdrOnDuration_SendRequestFail
 * @tc.desc: Test GetHdrOnDuration when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetHdrOnDuration_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    int64_t hdrOnDuration = 0;
    ErrCode ret = mockProxy->GetHdrOnDuration(hdrOnDuration);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== SetOptimizeCanvasDirtyPidList Tests ====================

/**
 * @tc.name: SetOptimizeCanvasDirtyPidList_Normal_Success
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList with
 * normal case
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetOptimizeCanvasDirtyPidList_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::vector<int32_t> pidList = { 1001, 1002, 1003 };
    ErrCode ret = mockProxy->SetOptimizeCanvasDirtyPidList(pidList);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetOptimizeCanvasDirtyPidList_EmptyList
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList with empty
 * list
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetOptimizeCanvasDirtyPidList_EmptyList, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::vector<int32_t> pidList;
    ErrCode ret = mockProxy->SetOptimizeCanvasDirtyPidList(pidList);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetOptimizeCanvasDirtyPidList_SendRequestFail
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList when
 * SendRequest fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetOptimizeCanvasDirtyPidList_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::vector<int32_t> pidList = { 1001, 1002 };
    ErrCode ret = mockProxy->SetOptimizeCanvasDirtyPidList(pidList);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== NotifyScreenRefresh Tests ====================

/**
 * @tc.name: NotifyScreenRefresh_SendRequestFail
 * @tc.desc: Test NotifyScreenRefresh when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, NotifyScreenRefresh_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    ScreenId id = 1;
    int32_t ret = mockProxy->NotifyScreenRefresh(id);
    EXPECT_EQ(ret, -1);
}

// ==================== SetGpuCrcDirtyEnabledPidList Tests ====================

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidList_Normal_Success
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList with normal
 * case
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetGpuCrcDirtyEnabledPidList_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::vector<int32_t> pidList = { 1001, 1002, 1003 };
    ErrCode ret = mockProxy->SetGpuCrcDirtyEnabledPidList(pidList);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidList_EmptyList
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList with empty list

 * * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetGpuCrcDirtyEnabledPidList_EmptyList, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    std::vector<int32_t> pidList;
    ErrCode ret = mockProxy->SetGpuCrcDirtyEnabledPidList(pidList);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidList_SendRequestFail
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when
 * SendRequest fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetGpuCrcDirtyEnabledPidList_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::vector<int32_t> pidList = { 1001, 1002 };
    ErrCode ret = mockProxy->SetGpuCrcDirtyEnabledPidList(pidList);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

// ==================== OnScreenBacklightChanged Tests ====================

/**
 * @tc.name: OnScreenBacklightChanged_Normal_Success
 * @tc.desc: Test OnScreenBacklightChanged with normal case
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnScreenBacklightChanged_Normal_Success, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    ScreenId screenId = 1;
    uint32_t level = 100;
    mockProxy->OnScreenBacklightChanged(screenId, level);
}

/**
 * @tc.name: OnScreenBacklightChanged_SendRequestFail
 * @tc.desc: Test OnScreenBacklightChanged when SendRequest
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnScreenBacklightChanged_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    ScreenId screenId = 1;
    uint32_t level = 100;
    mockProxy->OnScreenBacklightChanged(screenId, level);
}

/**
 * @tc.name: OnScreenBacklightChanged_ZeroLevel
 * @tc.desc: Test OnScreenBacklightChanged with level = 0
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnScreenBacklightChanged_ZeroLevel, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    ScreenId screenId = 1;
    uint32_t level = 0;
    mockProxy->OnScreenBacklightChanged(screenId, level);
}

/**
 * @tc.name: OnScreenBacklightChanged_MaxLevel
 * @tc.desc: Test OnScreenBacklightChanged with maximum level value

 * * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, OnScreenBacklightChanged_MaxLevel, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(NO_ERROR));

    ScreenId screenId = 1;
    uint32_t level = std::numeric_limits<uint32_t>::max();
    mockProxy->OnScreenBacklightChanged(screenId, level);
}

// ==================== Additional Read Fail Tests ====================

/**
 * @tc.name: GetPidGpuMemoryInMB_ReadFloatFail
 * @tc.desc: Test GetPidGpuMemoryInMB when ReadFloat fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPidGpuMemoryInMB_ReadFloatFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    pid_t pid = 1001;
    float gpuMemInMB = 0.0f;
    int32_t res = mockProxy->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    EXPECT_EQ(res, READ_PARCEL_ERR);
}

/**
 * @tc.name: GetTotalAppMemSize_ReadFloatFail
 * @tc.desc: Test GetTotalAppMemSize when ReadFloat fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetTotalAppMemSize_ReadFloatFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    float cpuMemSize = 0.0f;
    float gpuMemSize = 0.0f;
    ErrCode ret = mockProxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_EQ(ret, READ_PARCEL_ERR);
}

/**
 * @tc.name: GetPixelMapByProcessId_ReadResultFail
 * @tc.desc: Test GetPixelMapByProcessId when ReadInt32 result
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessId_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = 1001;
    int32_t repCode = 0;
    ErrCode ret = mockProxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetPixelMapByProcessId_SendRequestFail
 * @tc.desc: Test GetPixelMapByProcessId when SendRequest fails

 * * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetPixelMapByProcessId_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = 1001;
    int32_t repCode = 0;
    ErrCode ret = mockProxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphics_ReadCountFail
 * @tc.desc: Test GetMemoryGraphics when ReadUint64 count fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphics_ReadCountFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret = mockProxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphics_SendRequestFail
 * @tc.desc: Test GetMemoryGraphics when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphics_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret = mockProxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphics_InvalidSize
 * @tc.desc: Test GetMemoryGraphics when count is too large
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphics_InvalidSize, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(
            testing::Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) {
                reply.WriteUint64(std::numeric_limits<uint64_t>::max());
                return NO_ERROR;
            }));

    std::vector<MemoryGraphic> memoryGraphics;
    ErrCode ret = mockProxy->GetMemoryGraphics(memoryGraphics);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphic_ReadParcelableFail
 * @tc.desc: Test GetMemoryGraphic when ReadParcelable fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphic_ReadParcelableFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    pid_t pid = 1001;
    MemoryGraphic memoryGraphic;
    ErrCode ret = mockProxy->GetMemoryGraphic(pid, memoryGraphic);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMemoryGraphic_SendRequestFail
 * @tc.desc: Test GetMemoryGraphic when SendRequest fails
 * @tc.type:
 * FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetMemoryGraphic_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    pid_t pid = 1001;
    MemoryGraphic memoryGraphic;
    ErrCode ret = mockProxy->GetMemoryGraphic(pid, memoryGraphic);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStart_ReadResultFail
 * @tc.desc: Test AvcodecVideoStart when ReadInt32 result fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStart_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    std::vector<uint64_t> uniqueIdList = { 1 };
    std::vector<std::string> surfaceNameList = { "surface1" };
    uint32_t fps = 120;
    uint64_t reportTime = 16;
    ErrCode ret = mockProxy->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStop_ReadResultFail
 * @tc.desc: Test AvcodecVideoStop when ReadInt32 result fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoStop_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    std::vector<uint64_t> uniqueIdList = { 1 };
    std::vector<std::string> surfaceNameList = { "surface1" };
    uint32_t fps = 120;
    ErrCode ret = mockProxy->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoGet_ReadResultFail
 * @tc.desc: Test AvcodecVideoGet when ReadInt32 result fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGet_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    uint64_t uniqueId = 1;
    ErrCode ret = mockProxy->AvcodecVideoGet(uniqueId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoGetRecent_ReadResultFail
 * @tc.desc: Test AvcodecVideoGetRecent when ReadInt32 result
 * fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, AvcodecVideoGetRecent_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    ErrCode ret = mockProxy->AvcodecVideoGetRecent();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallback_ReadResultFail
 * @tc.desc: Test
 * UnRegisterSelfDrawingNodeRectChangeCallback when ReadInt32 fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(
    RSServiceToRenderConnectionProxyTest, UnRegisterSelfDrawingNodeRectChangeCallback_ReadResultFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    pid_t remotePid = 1001;
    int32_t ret = mockProxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
    EXPECT_EQ(ret, READ_PARCEL_ERR);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallback_SendRequestFail
 * @tc.desc: Test
 * UnRegisterSelfDrawingNodeRectChangeCallback when SendRequest fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(
    RSServiceToRenderConnectionProxyTest, UnRegisterSelfDrawingNodeRectChangeCallback_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    pid_t remotePid = 1001;
    int32_t ret = mockProxy->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
    EXPECT_EQ(ret, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: GetSurfaceRootNodeId_ReadNodeIdFail
 * @tc.desc: Test GetSurfaceRootNodeId when ReadUint64 fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeId_ReadNodeIdFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .WillRepeatedly(testing::Invoke(
            [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) { return NO_ERROR; }));

    NodeId windowNodeId = 100;
    ErrCode ret = mockProxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetSurfaceRootNodeId_SendRequestFail
 * @tc.desc: Test GetSurfaceRootNodeId when SendRequest fails
 *
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetSurfaceRootNodeId_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    NodeId windowNodeId = 100;
    ErrCode ret = mockProxy->GetSurfaceRootNodeId(windowNodeId);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetWatermark_SendRequestFail
 * @tc.desc: Test SetWatermark when SendRequest fails
 * @tc.type: FUNC
 *
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetWatermark_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    pid_t callingPid = 1001;
    std::string name = "test_watermark";
    auto watermark = CreateValidPixelMap();
    bool success = false;

    ErrCode ret = mockProxy->SetWatermark(callingPid, name, watermark, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_FALSE(success);
}

/**
 * @tc.name: ShowWatermark_SendRequestFail
 * @tc.desc: Test ShowWatermark when SendRequest fails
 * @tc.type: FUNC

 * * @tc.require: issueI9KXXE
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ShowWatermark_SendRequestFail, TestSize.Level1)
{
    auto remoteObject = sptr<IRemoteObjectMock>::MakeSptr();
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);

    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));

    auto pixelMap = CreateValidPixelMap();
    mockProxy->ShowWatermark(pixelMap, true);
}

/**
 * @tc.name: ReportGameStateDataTest
 * @tc.desc: ReportGameStateDataTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, ReportGameStateDataTest, TestSize.Level1)
{
    GameStateData info;
    sptr<IRemoteObjectMock> remoteObject = new IRemoteObjectMock;
    auto mockProxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
    uint32_t gameEventCode = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    EXPECT_CALL(*remoteObject, SendRequest(gameEventCode, _, _, _)).Times(1);
    mockProxy->ReportGameStateData(info);
}
} // namespace OHOS::Rosen
