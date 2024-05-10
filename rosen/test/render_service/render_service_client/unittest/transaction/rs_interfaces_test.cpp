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
#include "impl_interface/typeface_impl.h"
#include "skia_adapter/skia_typeface.h"

#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSInterfacesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInterfacesTest::SetUpTestCase() {}
void RSInterfacesTest::TearDownTestCase() {}
void RSInterfacesTest::SetUp() {}
void RSInterfacesTest::TearDown() {}

/**
 * @tc.name: TakeSurfaceCaptureForUI001
 * @tc.desc: test results of TakeSurfaceCaptureForUI
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, TakeSurfaceCaptureForUI001, TestSize.Level1)
{
    class TestSurfaceCapture : public SurfaceCaptureCallback {
    public:
        explicit TestSurfaceCapture() {}
        ~TestSurfaceCapture() {}
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
    };
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto callback = std::make_shared<TestSurfaceCapture>();
    bool res = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(res == false);

    auto node = std::make_shared<RSNode>(true);
    res = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(res == false);

    RSUINodeType type = node->GetType();
    type = RSUINodeType::UNKNOW;
    res = instance.TakeSurfaceCaptureForUI(nullptr, callback, 1.f, 1.f, true);
    EXPECT_TRUE(res == false);
}

/**
 * @tc.name: RegisterTypeface001
 * @tc.desc: test results of RegisterTypeface
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, RegisterTypeface001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto typefaceImpl = std::make_shared<Drawing::SkiaTypeface>();
    auto typeface = std::make_shared<Drawing::Typeface>(typefaceImpl);
    bool res = instance.RegisterTypeface(typeface);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: UnRegisterTypeface001
 * @tc.desc: test results of UnRegisterTypeface
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, UnRegisterTypeface001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    auto typefaceImpl = std::make_shared<Drawing::SkiaTypeface>();
    auto typeface = std::make_shared<Drawing::Typeface>(typefaceImpl);
    bool res = instance.UnRegisterTypeface(typeface);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: GetTotalAppMemSize001
 * @tc.desc: test results of GetTotalAppMemSize
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, GetTotalAppMemSize001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    float cpuMemSize = 1.f;
    float gpuMemSize = 1.f;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    bool res = instance.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: SetAppWindowNum001
 * @tc.desc: test results of SetAppWindowNum
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, SetAppWindowNum001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetAppWindowNum(1);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportJankStats001
 * @tc.desc: test results of ReportJankStats
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, ReportJankStats001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportJankStats();
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventResponse001
 * @tc.desc: test results of ReportEventResponse
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, ReportEventResponse001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventResponse(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventComplete001
 * @tc.desc: test results of ReportEventComplete
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, ReportEventComplete001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventComplete(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportEventJankFrame001
 * @tc.desc: test results of ReportEventJankFrame
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, ReportEventJankFrame001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    DataBaseRs info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportEventJankFrame(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: ReportGameStateData001
 * @tc.desc: test results of ReportGameStateData
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, ReportGameStateData001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    GameStateData info;
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.ReportGameStateData(info);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: SetOnRemoteDiedCallback001
 * @tc.desc: test results of SetOnRemoteDiedCallback
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, SetOnRemoteDiedCallback001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    OnRemoteDiedCallback callback = []() {};
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetOnRemoteDiedCallback(callback);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

/**
 * @tc.name: GetCurrentDirtyRegionInfo001
 * @tc.desc: test results of GetCurrentDirtyRegionInfo
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, GetCurrentDirtyRegionInfo001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.GetCurrentDirtyRegionInfo(1);
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}

#ifdef TP_FEATURE_ENABLE
/**
 * @tc.name: SetTpFeatureConfig001
 * @tc.desc: test results of SetTpFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueI9N0I9
 */
HWTEST_F(RSInterfacesTest, SetTpFeatureConfig001, TestSize.Level1)
{
    RSInterfaces& instance = RSInterfaces::GetInstance();
    int32_t feature = 1;
    std::string config = "config";
    instance.renderServiceClient_ = std::make_unique<RSRenderServiceClient>();
    instance.SetTpFeatureConfig(feature, config.c_str());
    EXPECT_TRUE(instance.renderServiceClient_ != nullptr);
}
#endif
} // namespace OHOS::Rosen
