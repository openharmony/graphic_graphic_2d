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

#include "wp_viewport_factory_test.h"

#include "wl_surface_factory.h"

#include "wl_surface_factory.h"

namespace OHOS {
void WpViewportFactoryTest::SetUp()
{
}

void WpViewportFactoryTest::TearDown()
{
}

void WpViewportFactoryTest::SetUpTestCase()
{
    initRet = WindowManager::GetInstance()->Init();
}

void WpViewportFactoryTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WpViewportFactory create
 * Function: WpViewportFactory
 * SubFunction: create
 * FunctionPoints: WpViewportFactory create
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. create WlSurface
 *                  2. create WpViewport
 *                  3. check it isn't nullptr
 */
HWTEST_F(WpViewportFactoryTest, Create, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. create WlSurface
    sptr<WlSurface> wlSurface = WlSurfaceFactory::GetInstance()->Create();
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. create WpViewport
    const auto wpViewport = WpViewportFactory::GetInstance()->Create(wlSurface);

    // 3. check it isn't nullptr
    ASSERT_NE(wpViewport, nullptr) << "CaseDescription: 3. check it isn't nullptr (wpViewport != nullptr)";
}
} // namespace
} // namespace OHOS
