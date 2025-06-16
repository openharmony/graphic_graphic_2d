/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "EGL/egl_wrapper_layer.h"

#include "egl_defs.h"

#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglWrapperLayerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: Init001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, Init001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    auto& layer = EglWrapperLayer::GetInstance();
    auto status = layer.initialized_;
    layer.initialized_ = true;
    EXPECT_TRUE(layer.Init(&dispatchTable));
    layer.initialized_ = status;
}

/**
 * @tc.name: Init002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, Init002, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    auto& layer = EglWrapperLayer::GetInstance();
    layer.initialized_ = true;
    EXPECT_TRUE(layer.Init(&dispatchTable));
    layer.initialized_ = false;
    EXPECT_FALSE(layer.Init(nullptr));
}

/**
 * @tc.name: InitLayers001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, InitLayers001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    auto& layer = EglWrapperLayer::GetInstance();
    layer.layerInit_ = {};
    layer.InitLayers(&dispatchTable);
    LayerInitFunc initFunc = [](const void* data, GetNextLayerAddr getAddr) -> EglWrapperFuncPointer {
        return [](){};
    };
    layer.layerInit_.push_back(initFunc);
    EXPECT_FALSE(layer.layerInit_.empty());
    layer.layerSetup_ = {};
    layer.InitLayers(&dispatchTable);
    LayerSetupFunc setupFunc = [](const char* data, EglWrapperFuncPointer getAddr) -> EglWrapperFuncPointer {
        return [](){};
    };
    layer.layerSetup_.push_back(setupFunc);
    layer.InitLayers(&dispatchTable);
    layer.layerInit_.clear();
    layer.InitLayers(&dispatchTable);
}

/**
 * @tc.name: InitBundleInfo001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, InitBundleInfo001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    bool result = EglWrapperLayer::GetInstance().InitBundleInfo();
    ASSERT_FALSE(result);
}
} // OHOS::Rosen