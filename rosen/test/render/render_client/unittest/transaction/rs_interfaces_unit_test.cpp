/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "gtest/gtest.h"
#include "parameters.h"
#include "impl_interface/typeface_impl.h"
#include "skia_adapter/skia_typeface.h"

#include "render/rs_typeface_cache.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();rev
    void SetUp() override;
    void TearDown() override;
};

void RsInterfaceTest::SetUpTestCase() {}
void RsInterfaceTest::TearDownTestCase() {}
void RsInterfaceTest::SetUp() {}
void RsInterfaceTest::TearDown() {}

/**
 * @tc.name: TakeSurfaceCaptureForUI01
 * @tc.desc: test results of TakeSurfaceCaptureForUI
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, TakeSurfaceCaptureForUI01, TestSize.Level1)
{
    class TestSurfaceCapture : public SurfaceCaptureCallback {
    public:
        explicit TestSurfaceCapture() {}
        ~TestSurfaceCapture() {}
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
    };
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto callback = std::make_shared<TestSurfaceCapture>();
    bool rev = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);

    auto node = std::make_shared<RSNode>(true);
    rev = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);

    RSUINodeType type = node->GetType();
    type = RSUINodeType::UNKNOW;
    rev = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);

    RSDisplayNodeConfig config;
    auto rsDisplayNode = RSDisplayNode::Create(config);
    rev = instance.TakeSurfaceCaptureForUI(rsDisplayNode, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);
}

/**
 * @tc.name: TakeSurfaceCaptureForUI02
 * @tc.desc: test results of TakeSurfaceCaptureForUI
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsInterfaceTest, TakeSurfaceCaptureForUI02, TestSize.Level1)
{
    std::shared_ptr<RSNode> node = nullptr;
    std::shared_ptr<SurfaceCaptureCallback> callback = nullptr;
    RSInterfaces& instance = RSInterfaces::GetInstance();
    bool rev = instance.TakeSurfaceCaptureForUI(node, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);

    node = std::make_shared<RSNode>(true);
    rev = instance.TakeSurfaceCaptureForUI(node, callback, 1.f, 1.f, true);
    EXPECT_TRUE(rev == false);
}

/**
 * @tc.name: SetHwcNodeBoundsTest
 * @tc.desc: test results of SetHwcNodeBounds
 * @tc.type: FUNC
 * @tc.require: issueIB2QCC
 */
HWTEST_F(RsInterfaceTest, SetHwcNodeBoundsTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    RSInterfaces& instance = RSInterfaces::GetInstance();
    bool rev = instance.SetHwcNodeBounds(nodeId, 1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: RegisterTypeface01
 * @tc.desc: test results of RegisterTypeface
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsInterfaceTest, RegisterTypeface01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto typefaceImpl = std::make_shared<Drawing::SkiaTypeface>();
    EXPECT_NE(typefaceImpl, nullptr);
    auto typeface = std::make_shared<Drawing::Typeface>(typefaceImpl);
    EXPECT_NE(typeface, nullptr);
    auto globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    RSTypefaceCache& typefaceCache = RSTypefaceCache::Instance();
    typefaceCache.typefaceHashCode_.emplace(globalUniqueId, 0);
    instance.RegisterTypeface(typeface);
    typefaceCache.typefaceHashCode_.clear();
}

/**
 * @tc.name: UnRegisterTypeface01
 * @tc.desc: test results of UnRegisterTypeface
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, UnRegisterTypeface01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto typefaceImpl = std::make_shared<Drawing::SkiaTypeface>();
    auto typeface = std::make_shared<Drawing::Typeface>(typefaceImpl);
    bool rev = instance.UnRegisterTypeface(typeface);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: GetMemoryGraphics01
 * @tc.desc: test results of GetMemoryGraphics
 * @tc.type: FUNC
 * @tc.require: issueIAS4B8
 */
HWTEST_F(RsInterfaceTest, GetMemoryGraphics01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    auto rev = instance.GetMemoryGraphics();
    EXPECT_FALSE(rev.empty());
}

/**
 * @tc.name: GetTotalAppMemSize01
 * @tc.desc: test results of GetTotalAppMemSize
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, GetTotalAppMemSize01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    float cpuMemSize = 1.f;
    float gpuMemSize = 1.f;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    bool rev = instance.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: SetAppWindowNum01
 * @tc.desc: test results of SetAppWindowNum
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, SetAppWindowNum01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetAppWindowNum(1);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportJankStats01
 * @tc.desc: test results of ReportJankStats
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, ReportJankStats01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportJankStats();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventResponse01
 * @tc.desc: test results of ReportEventResponse
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, ReportEventResponse01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventResponse(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventComplete01
 * @tc.desc: test results of ReportEventComplete
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, ReportEventComplete01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventComplete(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventJankFrame01
 * @tc.desc: test results of ReportEventJankFrame
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, ReportEventJankFrame01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventJankFrame(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportGameStateData01
 * @tc.desc: test results of ReportGameStateData
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, ReportGameStateData01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    GameStateData info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportGameStateData(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: SetOnRemoteDiedCallback01
 * @tc.desc: test results of SetOnRemoteDiedCallback
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, SetOnRemoteDiedCallback01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    OnRemoteDiedCallback callback = []() {};
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetOnRemoteDiedCallback(callback);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo01
 * @tc.desc: test results of GetActiveDirtyRegionInfo
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, GetActiveDirtyRegionInfo01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.GetActiveDirtyRegionInfo();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: GetGlobalDirtyRegionInfo01
 * @tc.desc: test results of GetGlobalDirtyRegionInfo
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, GetGlobalDirtyRegionInfo01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.GetGlobalDirtyRegionInfo();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: GetLayerComposeInfo01
 * @tc.desc: test results of GetLayerComposeInfo
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, GetLayerComposeInfo01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.GetLayerComposeInfo();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: GetHardwareComposeDisabledReasonInfo01
 * @tc.desc: test results of GetHwcDisabledReasonInfo
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, GetHardwareComposeDisabledReasonInfo01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.GetHwcDisabledReasonInfo();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: SetVmaCacheStatus01
 * @tc.desc: test results of SetVmaCacheStatus
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, SetVmaCacheStatus01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetVmaCacheStatus(true);
    instance.SetVmaCacheStatus(false);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

#ifdef TP_FEATURE_ENABLE
/**
 * @tc.name: SetTpFeatureConfig01
 * @tc.desc: test results of SetTpFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RsInterfaceTest, SetTpFeatureConfig01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    int32_t feature = 1;
    std::string config = "config";
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetTpFeatureConfig(feature, config.c_str());
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: SetTpFeatureConfig02
 * @tc.desc: test results of SetTpFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueIB39L8
 */
HWTEST_F(RsInterfaceTest, SetTpFeatureConfig02, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    int32_t feature = 1;
    std::string config = "config";
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetTpFeatureConfig(feature, config.c_str(), TpFeatureConfigType::AFT_TP_FEATURE);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}
#endif

/**
 * @tc.name: GetRefreshInfo01
 * @tc.desc: test results of GetRefreshInfo
 * @tc.type: FUNC
 * @tc.require: issueI97N4E
 */
HWTEST_F(RsInterfaceTest, GetRefreshInfo01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    GameStateData info;
    std::string str = instance.GetRefreshInfo(info.pid);
    EXPECT_TRUE(str == "");
}

/**
 * @tc.name: SetWatermark01
 * @tc.desc: test results of SetWatermark
 * @tc.type: FUNC
 * @tc.require: issueIASMZG
 */
HWTEST_F(RsInterfaceTest, SetWatermark01, TestSize.Level1)
{
    bool isEnableFeature = system::GetBoolParameter("const.graphic.enable_surface_watermark", false);
    if (!isEnableFeature) {
        return;
    }
    RSInterfaces& instance = RSInterfaces::GetInstance();
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    instance.renderServiceClient_ = nullptr;
    bool rev = instance.SetWatermark("test", pixelmap);
    EXPECT_FALSE(rev);

    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    rev = instance.SetWatermark("test", pixelmap);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: SetWatermark02
 * @tc.desc: test results of SetWatermark
 * @tc.type: FUNC
 * @tc.require: issueIASMZG
 */
HWTEST_F(RsInterfaceTest, SetWatermark02, TestSize.Level1)
{
    if (!RSSystemProperties::IsPcType()) {
        return;
    }
    RSInterfaces& instance = RSInterfaces::GetInstance();
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();

    std::string name1 = "";
    bool rev = instance.SetWatermark(name1, pixelmap);
    EXPECT_FALSE(rev);

    std::string name2(1, 't');
    rev = instance.SetWatermark(name2, pixelmap);
    EXPECT_TRUE(rev);

    std::string name3(2, 't');
    rev = instance.SetWatermark(name3, pixelmap);
    EXPECT_TRUE(rev);

    std::string name4(128, 't');
    rev = instance.SetWatermark(name4, pixelmap);
    EXPECT_TRUE(rev);

    std::string name5(129, 't');
    rev = instance.SetWatermark(name5, pixelmap);
    EXPECT_FALSE(rev);

    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    rev = instance.SetWatermark("test", pixelmap);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: RegisterSurfaceBufferCallback01
 * @tc.desc: test results of RegisterSurfaceBufferCallback
 * @tc.type: FUNC
 * @tc.require: issueIASMZG
 */
HWTEST_F(RsInterfaceTest, RegisterSurfaceBufferCallback01, TestSize.Level1)
{
    class TestSurfaceBufferCallback : public SurfaceBufferCallback {
    public:
        void OnFinish(const FinishCallbackRet& ret) {}
        void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) {}
    };
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();

    bool rev = instance.RegisterSurfaceBufferCallback(1, 1, nullptr);
    EXPECT_FALSE(rev);

    auto callback = std::make_shared<TestSurfaceBufferCallback>();
    rev = instance.RegisterSurfaceBufferCallback(1, 1, callback);
    EXPECT_TRUE(rev);
}

/**
 * @tc.name: UnregisterSurfaceBufferCallback01
 * @tc.desc: test results of UnregisterSurfaceBufferCallback
 * @tc.type: FUNC
 * @tc.require: issueIASMZG
 */
HWTEST_F(RsInterfaceTest, UnregisterSurfaceBufferCallback01, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    bool rev = instance.UnregisterSurfaceBufferCallback(1, 1);
    EXPECT_FALSE(rev);
}
} // namespace OHOS::Rosen
