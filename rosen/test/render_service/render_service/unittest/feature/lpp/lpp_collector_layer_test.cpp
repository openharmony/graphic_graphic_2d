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
#include <memory>
 
#include "lpp/render_server/lpp_layer_collector.h"
#include "gtest/gtest.h"
#include "rs_surface_layer.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class LppLayerCollectorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override;
 
private:
    std::shared_ptr<LppLayerCollector> lppLayerCollector_;
};
 
void LppLayerCollectorTest::SetUp()
{
    lppLayerCollector_ = std::make_shared<LppLayerCollector>();
}
 
void LppLayerCollectorTest::TearDown()
{
    lppLayerCollector_ = nullptr;
}
 
/**
 * @tc.name: AddLppLayerIdTest001
 * @tc.desc: Test AddLppLayerId when layers length is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, AddLppLayerIdTest001, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
 
    std::vector<RSLayerPtr> layers;
    lppLayerCollector_->AddLppLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
 
    EXPECT_TRUE(lppLayerIds.empty());
}
 
/**
 * @tc.name: AddLppLayerIdTest002
 * @tc.desc: Test AddLppLayerId when !isLppLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, AddLppLayerIdTest002, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
 
    std::vector<RSLayerPtr> layers;
    layers.push_back(nullptr);
 
    lppLayerCollector_->AddLppLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
    EXPECT_TRUE(lppLayerIds.empty());
}
 
/**
 * @tc.name: AddLppLayerIdTest003
 * @tc.desc: Test AddLppLayerId when isLppLayer == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, AddLppLayerIdTest004, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
 
    std::vector<RSLayerPtr> layers;
    layers.push_back(std::make_shared<RSSurfaceLayer>(0, nullptr));
    uint64_t nodeId = 100;
    uint64_t tunnelLayerId = 101;
    for (auto& layer : layers) {
        layer->SetNodeId(nodeId);
        layer->SetTunnelLayerId(tunnelLayerId);
        layer->SetTunnelLayerProperty(LPP_LAYER_PROPERTY); // Assuming LPP_LAYER_PROPERTY means LPP
    }
 
    lppLayerCollector_->AddLppLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
    EXPECT_FALSE(lppLayerIds.empty());
    EXPECT_EQ(lppLayerIds.size(), 1);
}
 
/**
 * @tc.name: RemoveLayerIdTest001
 * @tc.desc: Test RemoveLayerId when layers is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, RemoveLayerIdTest001, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
    uint64_t layerId = 100;
    lppLayerCollector_->lppLayerId_.insert(layerId);
    std::vector<RSLayerPtr> layers;
 
    lppLayerCollector_->RemoveLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
    EXPECT_FALSE(lppLayerIds.empty());
    EXPECT_TRUE(lppLayerIds.find(layerId) != lppLayerIds.end());
}
 
/**
 * @tc.name: RemoveLayerIdTest002
 * @tc.desc: Test RemoveLayerId when layers contain non-LPP layers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, RemoveLayerIdTest002, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
 
    uint64_t layerId = 100;
    lppLayerCollector_->lppLayerId_.insert(layerId);
    std::vector<RSLayerPtr> layers;
    layers.push_back(nullptr);
 
    lppLayerCollector_->RemoveLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
    EXPECT_FALSE(lppLayerIds.empty());
    EXPECT_TRUE(lppLayerIds.find(layerId) != lppLayerIds.end());
}
 
/**
 * @tc.name: RemoveLayerIdTest003
 * @tc.desc: Test RemoveLayerId when layers contain LPP layers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppLayerCollectorTest, RemoveLayerIdTest003, TestSize.Level1)
{
    ASSERT_NE(lppLayerCollector_, nullptr);
 
    uint64_t layerId = 100;
    uint64_t tunnelLayerId = 101;
    lppLayerCollector_->lppLayerId_.insert(layerId);
    std::vector<RSLayerPtr> layers;
    layers.push_back(std::make_shared<RSSurfaceLayer>(0, nullptr));
    layers[0]->SetNodeId(layerId);
    layers[0]->SetTunnelLayerId(tunnelLayerId);
    layers[0]->SetTunnelLayerProperty(LPP_LAYER_PROPERTY); // Assuming LPP_LAYER_PROPERTY means LPP
 
    lppLayerCollector_->RemoveLayerId(layers);
 
    const std::unordered_set<uint64_t>& lppLayerIds = lppLayerCollector_->GetLppLayerId();
    EXPECT_TRUE(lppLayerIds.empty());
    EXPECT_TRUE(lppLayerIds.find(layerId) == lppLayerIds.end());
}
} // namespace OHOS::Rosen