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
    static inline MockSys::HdiDevice* mockDevice_ = nullptr;
    static inline std::shared_ptr<HdiOutput> output_ = nullptr;
    static inline std::shared_ptr<MockSys::HdiLayerContext> hdiLayerTemp_ = nullptr;
};

void HdiBackendSysTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    output_ = HdiOutput::CreateHdiOutput(screenId);
    output_->Init();
    std::vector<LayerInfoPtr> layerInfos;
    int32_t width = 50;
    int32_t height = 50;
    IRect srcRect = {0, 0, width, height};
    IRect dstRect = {0, 0, width, height};
    uint32_t zOrder = 0;
    
    hdiLayerTemp_ = std::make_unique<MockSys::HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    layerInfos.emplace_back(hdiLayerTemp_->GetHdiLayer());
    zOrder = 1;
    hdiLayerTemp_ = std::make_unique<MockSys::HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp_->DrawBufferColor();
    hdiLayerTemp_->FillHdiLayer();
    layerInfos.emplace_back(hdiLayerTemp_->GetHdiLayer());
    output_->SetLayerInfo(layerInfos);

    hdiBackend_ = HdiBackend::GetInstance();
    // mockDevice_ is nullptr
    hdiBackend_->SetHdiBackendDevice(mockDevice_);
    // init mockDevice_
    mockDevice_ = MockSys::HdiDevice::GetInstance();
    hdiBackend_->SetHdiBackendDevice(mockDevice_);
    // the device_ in hdiBackend_ is not nullptr alredy
    hdiBackend_->SetHdiBackendDevice(mockDevice_);
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
* Function: RegScreenHotplug001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, RegScreenHotplug001, Function | MediumTest| Level3)
{
    std::vector<std::shared_ptr<HdiOutput>> outputs {HdiBackendSysTest::output_};
    EXPECT_CALL(*mockDevice_, PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(0));
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    const std::unordered_map<uint32_t, LayerPtr> &layersMap = output_->GetLayers();
    for (auto iter = layersMap.begin(); iter != layersMap.end(); iter++) {
        uint32_t layerId = iter->first;
        layersId.emplace_back(layerId);
        types.emplace_back(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    }
    EXPECT_CALL(*mockDevice_, GetScreenCompChange(_, layersId, types)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, Commit(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenVsyncEnabled(_, _)).WillRepeatedly(testing::Return(0));
    HdiBackendSysTest::hdiBackend_->Repaint(outputs);
    ASSERT_EQ(HdiBackendSysTest::hdiBackend_->RegScreenHotplug(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegScreenHotplug002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, RegScreenHotplug002, Function | MediumTest| Level3)
{
    auto func = [](OutputPtr &, bool, void*) -> void {};
    ASSERT_EQ(HdiBackendSysTest::hdiBackend_->RegScreenHotplug(func, nullptr), ROSEN_ERROR_OK);
}

/*
* Function: RegPrepareComplete001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, RegPrepareComplete001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendSysTest::hdiBackend_->RegPrepareComplete(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegPrepareComplete002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, RegPrepareComplete002, Function | MediumTest| Level3)
{
    auto func = [](sptr<Surface> &, const struct PrepareCompleteParam &, void*) -> void {};
    RosenError ret = HdiBackendSysTest::hdiBackend_->RegPrepareComplete(func, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_OK);
}

/*
* Function: GetLayersReleaseFence001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetLayersReleaseFence
*                  2. check ret
*/
HWTEST_F(HdiBackendSysTest, GetLayersReleaseFence001, Function | MediumTest| Level3)
{
    const std::unordered_map<uint32_t, LayerPtr> &layersMap = output_->GetLayers();
    std::vector<uint32_t> layersId;
    std::vector<sptr<SyncFence>> fences;
    int32_t fenceId = 1;
    for (auto iter = layersMap.begin(); iter != layersMap.end(); iter++) {
        uint32_t layerId = iter->first;
        layersId.emplace_back(layerId);
        fences.emplace_back(new SyncFence(fenceId));
        fenceId++;
    }
    EXPECT_CALL(*mockDevice_, GetScreenReleaseFence(_, layersId, fences)).WillRepeatedly(testing::Return(0));
    std::map<LayerInfoPtr, sptr<SyncFence>> layersReleaseFence = hdiBackend_->GetLayersReleaseFence(output_);
    size_t layersReleaseFenceSize = layersReleaseFence.size();
    size_t fencesSize = fences.size();
    bool ret = (layersReleaseFenceSize == fencesSize);
    size_t i = 0;
    for (auto iter = layersReleaseFence.begin(); iter != layersReleaseFence.end(); iter++) {
        if (iter->second->Get() != fences[i]->Get()) {
            ret = false;
            break;
        }
        i++;
    }
    ASSERT_EQ(ret, true);
}

} // namespace
} // namespace Rosen
} // namespace OHOS