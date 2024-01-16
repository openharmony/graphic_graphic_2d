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

#include "EGL/egl_core.h"

#include "egl_defs.h"
#include "egl_wrapper_layer.h"
#include "egl_wrapper_loader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglCoreTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: WrapperHookTableInit001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglCoreTest, WrapperHookTableInit001, Level1)
{
    int res = 0;
    EglCoreInit();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: EglCoreInit001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglCoreTest, EglCoreInit001, Level1)
{
    auto result = EglCoreInit();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: EglCoreInit002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglCoreTest, EglCoreInit002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;

    auto result = EglCoreInit();
    ASSERT_TRUE(result);

    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglCoreInit003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglCoreTest, EglCoreInit003, Level2)
{
    EglWrapperDispatchTable *gWrapperHook;
    gWrapperHook = nullptr;
    auto result = EglWrapperLoader::GetInstance().Load(gWrapperHook);

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: EglCoreInit004
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglCoreTest, EglCoreInit004s, Level2)
{
    EglWrapperDispatchTable *gWrapperHook;
    gWrapperHook = nullptr;
    int res = true;
    EglWrapperLayer::GetInstance().Init(gWrapperHook);
    ASSERT_EQ(res, true);
}
} // OHOS::Rosen
