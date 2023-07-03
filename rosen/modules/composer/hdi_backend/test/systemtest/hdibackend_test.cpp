/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_backend.h"
#include "mock_hdi_device_systest.h"
#include "hdilayer_context_systest.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiBackendSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline HdiBackend* hdiBackend_ = nullptr;
    static inline MockSys::HdiDeviceMock* mockDevice_ = nullptr;
    static inline std::shared_ptr<HdiOutput> output_ = nullptr;
    static inline std::shared_ptr<MockSys::HdiLayerContext> hdiLayerTemp_ = nullptr;
    static inline std::vector<LayerInfoPtr> layerInfos_ = {};
};

void HdiBackendSysTest::SetUpTestCase()
{
    mockDevice_ = MockSys::HdiDeviceMock::GetInstance();
    uint32_t screenId = 0;
    output_ = HdiOutput::CreateHdiOutput(screenId);
    output_->SetHdiOutputDevice(mockDevice_);
    output_->Init();

    int32_t width = 50;
    int32_t height = 50;
    GraphicIRect srcRect = {0, 0, width, height};
    GraphicIRect dstRect = {0, 0, width, height};
    uint32_t zOrder = 0;
    hdiLayerTemp_ = std::make_unique<MockSys::HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    hdiLayerTemp_->GetHdiLayer()->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layerInfos_.emplace_back(hdiLayerTemp_->GetHdiLayer());

    zOrder = 1;
    hdiLayerTemp_ = std::make_unique<MockSys::HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    hdiLayerTemp_->GetHdiLayer()->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    hdiBackend_ = HdiBackend::GetInstance();
}

void HdiBackendSysTest::TearDownTestCase()
{
    hdiBackend_ = nullptr;
    mockDevice_ = nullptr;
    output_ = nullptr;
    hdiLayerTemp_ = nullptr;
}

namespace {
/*
* Function: Repaint001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Repaint
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, Repaint001, Function | MediumTest| Level3)
{
    // Repaint before SetHdiBackendDevice
    hdiBackend_->Repaint(nullptr);
    
    // repaint when layermap in output is empty
    hdiBackend_->Repaint(output_);

    // repaint when layermap in output is not empty
    output_->SetLayerInfo(layerInfos_);
    EXPECT_CALL(*mockDevice_, PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, Commit(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenVsyncEnabled(_, _)).WillRepeatedly(testing::Return(0));
    hdiBackend_->Repaint(output_);
    EXPECT_CALL(*mockDevice_, Commit(_, _)).WillRepeatedly(testing::Return(1));
    hdiBackend_->Repaint(output_);

    // client composition while onPrepareCompleteCb_ is nullptr
    layerInfos_.emplace_back(hdiLayerTemp_->GetHdiLayer());
    output_->SetLayerInfo(layerInfos_);
    hdiBackend_->Repaint(output_);

    // client composition while onPrepareCompleteCb_ is not nullptr
    auto func = [](sptr<Surface> &, const struct PrepareCompleteParam &param, void* data) -> void {};
    ASSERT_EQ(hdiBackend_->RegPrepareComplete(func, nullptr), ROSEN_ERROR_OK);
    hdiBackend_->Repaint(output_);
}

} // namespace
} // namespace Rosen
} // namespace OHOS