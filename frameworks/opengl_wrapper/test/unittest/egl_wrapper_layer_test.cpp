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

    auto result = EglWrapperLayer::GetInstance().Init(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Init002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, Init002, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    auto result = EglWrapperLayer::GetInstance().Init(&dispatchTable);
    ASSERT_TRUE(result);
    result = EglWrapperLayer::GetInstance().Init(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Init003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, Init003, Level1)
{
    EglWrapperLayer WrapperLayer;
    EglWrapperDispatchTable *dispatchTable;
    dispatchTable = nullptr;
    bool result = WrapperLayer.Init(dispatchTable);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: InitLayers001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, InitLayers001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    bool result = true;
    EglWrapperLayer::GetInstance().InitLayers(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: LoadLayers001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLayerTest, LoadLayers001, Level1)
{
    EglWrapperDispatchTable dispatchTable;

    bool result = EglWrapperLayer::GetInstance().LoadLayers();
    ASSERT_TRUE(result);
}
} // OHOS::Rosen