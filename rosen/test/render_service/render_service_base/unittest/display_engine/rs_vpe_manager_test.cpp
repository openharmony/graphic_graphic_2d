/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "meta/format.h"
#include "display_engine/rs_vpe_manager.h"

#include "iconsumer_surface.h"
#include "surface_buffer_impl.h"

#define private public

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media::VideoProcessingEngine;
using namespace OHOS::Rosen;

bool IsSupportReset = false;

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {

bool g_isProductSupportRest = false;
VPEAlgoErrCode g_setParameterResult = VPE_ALGO_ERR_OK;

class MockVpeVideo : public VpeVideo {
public:
    MOCK_METHOD(std::shared_ptr<VpeVideo>, Create, (uint32_t type));
    MOCK_METHOD(VPEAlgoErrCode, RegisterCallback, (const std::shared_ptr<VpeVideoCallback>& callback), (override));
    MOCK_METHOD(sptr<OHOS::Surface>, GetInputSurface, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, SetParameter, (const Format& parameter), (override));
    MOCK_METHOD(VPEAlgoErrCode, GetParameter, (Format& parameter), (override));
    MOCK_METHOD(VPEAlgoErrCode, Start, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, Stop, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, Release, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, Enable, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, NotifyEos, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, ReleaseOutputBuffer, (uint32_t index, bool render), (override));
    MOCK_METHOD(VPEAlgoErrCode, IsSupported, (uint32_t type, const Format& parameter));

    virtual ~MockVpeVideo()
    {}
};

bool VpeVideo::IsSupported(uint32_t type, [[maybe_unused]] const Format& parameter)
{
    (void) type;
    return IsSupportReset;
}
bool VpeVideo::IsSupported()
{
    return g_isProductSupportRest;
}
VPEAlgoErrCode VpeVideo::SetParameter([[maybe_unused]] const Format& parameter)
{
    return g_setParameterResult;
}
}
}
}
namespace OHOS {
namespace Rosen {
class RSVpeManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void RSVpeManagerTest::SetUpTestCase(void) {}
void RSVpeManagerTest::TearDownTestCase(void) {}
void RSVpeManagerTest::SetUp(void) {}
void RSVpeManagerTest::TearDown(void) {}

HWTEST_F(RSVpeManagerTest, GetInstance001, TestSize.Level1)
{
    RSVpeManager& instance1 = RSVpeManager::GetInstance();
    RSVpeManager& instance2 = RSVpeManager::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

HWTEST_F(RSVpeManagerTest, ReleaseVpeVideo001, TestSize.Level1)
{
    RSVpeManager manager;
    uint64_t nodeId = 123;
    auto mockVpeVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVpeVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_NO_MEMORY));
    manager.allVpeVideo_[nodeId] = mockVpeVideo;
    manager.ReleaseVpeVideo(nodeId);
    EXPECT_EQ(manager.allVpeVideo_.find(nodeId), manager.allVpeVideo_.end());
}

HWTEST_F(RSVpeManagerTest, ReleaseVpeVideo002, TestSize.Level1)
{
    RSVpeManager manager;
    uint64_t nodeId = 123;
    auto mockVpeVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVpeVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.allVpeVideo_[nodeId] = mockVpeVideo;
    manager.ReleaseVpeVideo(nodeId);
    EXPECT_EQ(manager.allVpeVideo_.find(nodeId), manager.allVpeVideo_.end());
}

HWTEST_F(RSVpeManagerTest, ReleaseVpeVideo003, TestSize.Level1)
{
    RSVpeManager manager;
    uint64_t nodeId = 123;
    auto mockVpeVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVpeVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.allVpeVideo_[nodeId] = mockVpeVideo;
    manager.ReleaseVpeVideo(nodeId);
    EXPECT_EQ(manager.allVpeVideo_.find(nodeId), manager.allVpeVideo_.end());
}

HWTEST_F(RSVpeManagerTest, ReleaseVpeVideo004, TestSize.Level1)
{
    RSVpeManager manager;
    uint64_t nodeId = 123;
    auto mockVpeVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVpeVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_NO_MEMORY));
    manager.allVpeVideo_[nodeId] = mockVpeVideo;
    manager.ReleaseVpeVideo(nodeId);
    EXPECT_EQ(manager.allVpeVideo_.find(nodeId), manager.allVpeVideo_.end());
}

HWTEST_F(RSVpeManagerTest, GetVpeVideo001, TestSize.Level1)
{
    RSVpeManager manager;
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto result = manager.GetVpeVideo(1, config);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(RSVpeManagerTest, GetVpeVideoSurface001, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer = consumer->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);

    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::DEFAULT;
    OHOS::sptr<OHOS::Surface> result = manager.GetVpeVideoSurface(0, RSSurface, config);
    EXPECT_EQ(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, GetVpeVideoSurface002, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer = consumer->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);

    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    sptr<Surface> result = manager.GetVpeVideoSurface(0, RSSurface, config);
    EXPECT_EQ(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, GetVpeVideoSurface003, TestSize.Level1)
{
    uint32_t type = VIDEO_TYPE_DETAIL_ENHANCER;
    OHOS::sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer = consumer->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);

    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    sptr<Surface> result = RSVpeManager::GetInstance().GetVpeVideoSurface(type, RSSurface, config);
    EXPECT_NE(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, CheckAndGetSurface001, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer = consumer->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);
    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    IsSupportReset = false;
    g_isProductSupportRest = true;
    sptr<Surface> result = manager.CheckAndGetSurface(RSSurface, config);
    EXPECT_EQ(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, SetVpeVideoParameterSucceed, TestSize.Level1)
{
    RSVpeManager manager;
    RSSurfaceRenderNodeConfig config = {1};
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    g_setParameterResult = VPE_ALGO_ERR_OK;
    std::shared_ptr<VpeVideo> validVideo = VpeVideo::Create(VIDEO_TYPE_DETAIL_ENHANCER);

    bool result = manager.SetVpeVideoParameter(validVideo, VIDEO_TYPE_DETAIL_ENHANCER, config);
    EXPECT_TRUE(result);
}

HWTEST_F(RSVpeManagerTest, SetVpeVideoParameterfails, TestSize.Level1)
{
    RSVpeManager manager;
    RSSurfaceRenderNodeConfig config = {1};
    g_setParameterResult = VPE_ALGO_ERR_INVALID_PARAM;
    std::shared_ptr<VpeVideo> vpeVideo = VpeVideo::Create(VIDEO_TYPE_DETAIL_ENHANCER);

    bool result = manager.SetVpeVideoParameter(vpeVideo, VIDEO_TYPE_DETAIL_ENHANCER, config);
    EXPECT_FALSE(result);
}

HWTEST_F(RSVpeManagerTest, SetVpeVideoParameterSucceed01, TestSize.Level1)
{
    RSVpeManager manager;
    RSSurfaceRenderNodeConfig config = {1};
    g_setParameterResult = VPE_ALGO_ERR_OK;
    std::shared_ptr<VpeVideo> vpeVideo = VpeVideo::Create(VIDEO_TYPE_DETAIL_ENHANCER);

    bool result = manager.SetVpeVideoParameter(vpeVideo, 0, config);
    EXPECT_FALSE(result);
}

HWTEST_F(RSVpeManagerTest, SetVpeVideoParameterfails001, TestSize.Level1)
{
    RSVpeManager manager;
    RSSurfaceRenderNodeConfig config = {1};
    g_setParameterResult = VPE_ALGO_ERR_INVALID_PARAM;
    std::shared_ptr<VpeVideo> vpeVideo = VpeVideo::Create(VIDEO_TYPE_DETAIL_ENHANCER);

    bool result = manager.SetVpeVideoParameter(vpeVideo, 0, config);
    EXPECT_FALSE(result);
}

HWTEST_F(RSVpeManagerTest, CheckAndGetSurface002, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer = consumer->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);
    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::DEFAULT;

    sptr<Surface> result = manager.CheckAndGetSurface(RSSurface, config);
    EXPECT_EQ(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, CheckAndGetSurface003, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer1 = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer1 = consumer1->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer1);
    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    IsSupportReset = true;
    g_isProductSupportRest = false;

    OHOS::sptr<OHOS::Surface> result = manager.CheckAndGetSurface(RSSurface, config);
    EXPECT_EQ(result, RSSurface);
}

HWTEST_F(RSVpeManagerTest, CheckAndGetSurface005, TestSize.Level1)
{
    RSVpeManager manager;
    OHOS::sptr<IConsumerSurface> consumer1 = IConsumerSurface::Create("DisplayNode");
    OHOS::sptr<IBufferProducer> producer1 = consumer1->GetProducer();
    OHOS::sptr<OHOS::Surface> RSSurface = OHOS::Surface::CreateSurfaceAsProducer(producer1);
    RSSurfaceRenderNodeConfig config;
    config.nodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    IsSupportReset = true;
    g_isProductSupportRest = true;

    OHOS::sptr<OHOS::Surface> result = manager.CheckAndGetSurface(RSSurface, config);
    EXPECT_NE(result, RSSurface);
}

}
}
class VpeVideoCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void VpeVideoCallbackImplTest::SetUpTestCase(void) {}
void VpeVideoCallbackImplTest::TearDownTestCase(void) {}
void VpeVideoCallbackImplTest::SetUp(void) {}
void VpeVideoCallbackImplTest::TearDown(void) {}

HWTEST_F(VpeVideoCallbackImplTest, OnOutputBufferAvailable001, TestSize.Level1)
{
    VpeVideoCallbackImpl callback;
    IsSupportReset = true;
    std::weak_ptr<VpeVideo> expiredFilter;
    callback.videoFilter_ = expiredFilter;
    OHOS::Media::Format parameter{};
    uint32_t type = VIDEO_TYPE_DETAIL_ENHANCER;

    callback.OnOutputBufferAvailable(1, VpeBufferFlag::VPE_BUFFER_FLAG_NONE);
    EXPECT_EQ(IsSupportReset, VpeVideo::IsSupported(type, parameter));
}

HWTEST_F(VpeVideoCallbackImplTest, OnOutputBufferAvailable002, TestSize.Level1)
{
    VpeVideoCallbackImpl callback;
    IsSupportReset = true;
    std::shared_ptr<VpeVideo> validVideo = std::make_shared<VpeVideo>();
    callback.videoFilter_ = validVideo;
    OHOS::Media::Format parameter{};
    uint32_t type = VIDEO_TYPE_DETAIL_ENHANCER;

    callback.OnOutputBufferAvailable(1, VpeBufferFlag::VPE_BUFFER_FLAG_NONE);
    EXPECT_EQ(IsSupportReset, VpeVideo::IsSupported(type, parameter));
}
