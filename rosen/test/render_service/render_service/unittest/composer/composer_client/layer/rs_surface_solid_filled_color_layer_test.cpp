/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <memory>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "layer/rs_surface_solid_filled_color_layer.h"
#include "pipeline/rs_composer_client.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "rs_composer_context.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint64_t TEST_LAYER_ID = 1200u;
} // namespace

class RSSurfaceSolidFilledColorLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceSolidFilledColorLayerTest::SetUpTestCase() {}
void RSSurfaceSolidFilledColorLayerTest::TearDownTestCase() {}
void RSSurfaceSolidFilledColorLayerTest::SetUp() {}
void RSSurfaceSolidFilledColorLayerTest::TearDown() {}

/**
 * Function: Create_ContextNullptr_ReturnNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Create with nullptr context
 *                  2. verify nullptr is returned
 *                   Cover branch: context == nullptr (line 35 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextNullptr_ReturnNullptr, TestSize.Level1)
{
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, nullptr);
    EXPECT_EQ(layer, nullptr);
}

/**
 * Function: Create_ContextValidNoCache_NewLayerCreated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create valid context
 *                  2. call Create when no cached layer exists
 *                  3. verify new layer is created and added to context
 *                   Cover branch: context != nullptr (line 35 false), layer == nullptr (line 40 first condition false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextValidNoCache_NewLayerCreated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());

    auto cachedLayer = context->GetRSLayer(TEST_LAYER_ID);
    EXPECT_NE(cachedLayer, nullptr);
}

/**
 * Function: Create_CachedLayerNotSolidFilledColor_NewLayerCreated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context and add non-solid-filled-color layer
 *                  2. call Create for same layer id
 *                  3. verify new solid filled color layer is created
 *                   Cover branch: context != nullptr (line 35 false),
 *                                  layer != nullptr && !IsSolidFilledColorLayer() (line 40 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_CachedLayerNotSolidFilledColor_NewLayerCreated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto normalLayer = std::make_shared<RSSurfaceLayer>(TEST_LAYER_ID, context);
    normalLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    context->AddRSLayer(normalLayer);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_CachedSolidFilledColorLayer_ReuseCached
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context and solid filled color layer
 *                  2. call Create for same layer id
 *                  3. verify cached layer is reused
 *                   Cover branch: context != nullptr (line 35 false),
 *                                  layer != nullptr && IsSolidFilledColorLayer() (line 40 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_CachedSolidFilledColorLayer_ReuseCached, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer1, nullptr);
    EXPECT_TRUE(layer1->IsSolidFilledColorLayer());

    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer2, nullptr);
    EXPECT_TRUE(layer2->IsSolidFilledColorLayer());

    EXPECT_EQ(layer1.get(), layer2.get());
}

/**
 * Function: Create_MultipleLayers_EachHasUniqueId
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context
 *                  2. create multiple solid filled color layers with different ids
 *                  3. verify each has unique pointer
 *                   Cover branch: context != nullptr (line 35 false), layer == nullptr (line 40 first condition false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_MultipleLayers_EachHasUniqueId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID + 1, context);

    ASSERT_NE(layer1, nullptr);
    ASSERT_NE(layer2, nullptr);
    EXPECT_NE(layer1.get(), layer2.get());
}

/**
 * Function: Create_ValidContext_ReturnsSolidFilledColorLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create valid context
 *                  2. call Create
 *                  3. verify returned layer is solid filled color layer
 *                   Cover branch: context != nullptr (line 35 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ValidContext_ReturnsSolidFilledColorLayer, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_ReuseCachedLayer_RsLayerIdUpdated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create cached solid filled color layer
 *                  2. call Create with new layer id
 *                  3. verify layer id is updated
 *                   Cover branch: layer != nullptr && IsSolidFilledColorLayer() (line 40 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ReuseCachedLayer_RsLayerIdUpdated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetRSLayerId(), TEST_LAYER_ID);

    constexpr uint64_t NEW_LAYER_ID = TEST_LAYER_ID + 100;
    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(NEW_LAYER_ID, context);
    ASSERT_NE(layer2, nullptr);

    EXPECT_EQ(layer2->GetRSLayerId(), NEW_LAYER_ID);
}

/**
 * Function: IsSolidFilledColorLayer_AlwaysTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call IsSolidFilledColorLayer
 *                  3. verify always returns true
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, IsSolidFilledColorLayer_AlwaysTrue, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_ContextWithNullptrInnerContext_StillWorks
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context with nullptr inner context
 *                  2. call Create
 *                  3. verify layer is created successfully
 *                   Cover branch: context != nullptr (line 35 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextWithNullptrInnerContext_StillWorks, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}
} // namespace Rosen
} // namespace OHOS