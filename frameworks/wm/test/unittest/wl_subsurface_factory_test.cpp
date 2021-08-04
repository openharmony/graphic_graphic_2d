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

#include "wl_subsurface_factory_test.h"

#include "wl_surface_factory.h"

namespace OHOS {
void WlSubsurfaceFactoryTest::SetUp()
{
}

void WlSubsurfaceFactoryTest::TearDown()
{
}

void WlSubsurfaceFactoryTest::SetUpTestCase()
{
    initRet = WindowManager::GetInstance()->Init();
}

void WlSubsurfaceFactoryTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WlSubsurfaceFactory create
 * Function: WlSubsurfaceFactory
 * SubFunction: create
 * FunctionPoints: WlSubsurfaceFactory create
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. create two WlSurface
 *                  2. create WlSubsurface
 *                  3. check it isn't nullptr
 */
HWTEST_F(WlSubsurfaceFactoryTest, Create, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. create two WlSurface
    sptr<WlSurface> wlSurface1 = WlSurfaceFactory::GetInstance()->Create();
    sptr<WlSurface> wlSurface2 = WlSurfaceFactory::GetInstance()->Create();
    ASSERT_NE(wlSurface1, nullptr) << "CaseDescription: 1. create two WlSurface (wlSurface1 != nullptr)";
    ASSERT_NE(wlSurface2, nullptr) << "CaseDescription: 1. create two WlSurface (wlSurface2 != nullptr)";

    // 2. create WlSubsurface
    const auto wlSubsurface = WlSubsurfaceFactory::GetInstance()->Create(wlSurface1, wlSurface2);

    // 3. check it isn't nullptr
    ASSERT_NE(wlSubsurface, nullptr) << "CaseDescription: 3. check it isn't nullptr (wlSubsurface != nullptr)";
}
} // namespace
} // namespace OHOS
