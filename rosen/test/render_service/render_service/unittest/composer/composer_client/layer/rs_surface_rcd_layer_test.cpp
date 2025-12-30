/**
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
#include <memory>
#include "rs_surface_rcd_layer.h"
#include "rs_composer_context.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerContextProbe : public RSComposerContext {
public:
    using RSComposerContext::GetRSLayerTransaction;
    using RSComposerContext::AddRSLayer;
    using RSComposerContext::RemoveRSLayer;
    using RSComposerContext::GetRSLayer;
};

class RSSurfaceRCDLayerTest : public Test {};

/**
 * Function: RCDLayer_Create_NullContext
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. pass null context to Create
 *                  2. expect nullptr returned
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Create_NullContext, TestSize.Level1)
{
    std::shared_ptr<RSComposerContext> ctx;
    auto layer = RSSurfaceRCDLayer::Create(ctx, 1u);
    EXPECT_EQ(layer, nullptr);
}

/**
 * Function: RCDLayer_Create_NewAndExisting
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create new layer and add to context
 *                  2. create again to reuse existing layer
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Create_NewAndExisting, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer1 = RSSurfaceRCDLayer::Create(ctx, 200u);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetRSLayerId(), 200u);

    auto layer2 = RSSurfaceRCDLayer::Create(ctx, 200u);
    ASSERT_NE(layer2, nullptr);
    EXPECT_EQ(layer2.get(), layer1.get());
    EXPECT_EQ(layer2->GetRSLayerId(), 200u);
}

/**
 * Function: RCDLayer_SetPixelMap_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set non-null pixel map
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_SetPixelMap_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceRCDLayer::Create(ctx, 201u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    // Construct a minimal PixelMap instance
    std::shared_ptr<Media::PixelMap> pm(new Media::PixelMap());
    dynamic_cast<RSSurfaceRCDLayer*>(layer.get())->SetPixelMap(pm);
    EXPECT_FALSE(handler->IsEmpty());
    EXPECT_EQ(dynamic_cast<RSSurfaceRCDLayer*>(layer.get())->GetPixelMap().get(), pm.get());
}

/**
 * Function: RCDLayer_IsScreenFlag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. verify IsScreenRCDLayer returns true
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_IsScreenFlag, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceRCDLayer::Create(ctx, 202u);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsScreenRCDLayer());
}
/**
 * Function: RCDLayer_SetPixelMap_Null_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set null pixel map
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_SetPixelMap_Null_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceRCDLayer::Create(ctx, 203u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    std::shared_ptr<Media::PixelMap> pm;
    dynamic_cast<RSSurfaceRCDLayer*>(layer.get())->SetPixelMap(pm);
    EXPECT_FALSE(handler->IsEmpty());
    EXPECT_EQ(dynamic_cast<RSSurfaceRCDLayer*>(layer.get())->GetPixelMap(), pm);
}
} // namespace OHOS::Rosen
