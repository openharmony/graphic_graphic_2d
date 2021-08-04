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

#include "window_manager_server_test.h"

#include "wl_surface_factory.h"

namespace OHOS {
void WindowManagerServerTest::SetUp()
{
}

void WindowManagerServerTest::TearDown()
{
}

void WindowManagerServerTest::SetUpTestCase()
{
    initRet = WindowManager::GetInstance()->Init();
}

void WindowManagerServerTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WindowManagerServer CreateWindow by normal arguments
 * Function: WindowManagerServer
 * SubFunction: CreateWindow
 * FunctionPoints: WindowManagerServer CreateWindow
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create a Normal Window
 *                  3. check it
 */
HWTEST_F(WindowManagerServerTest, CreateNormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. create WlSurface
    sptr<WlSurface> wlSurface = WlSurfaceFactory::GetInstance()->Create();
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create a Normal Window
    auto promise = WindowManagerServer::GetInstance()->CreateWindow(wlSurface, 0, WINDOW_TYPE_NORMAL);
    ASSERT_NE(promise, nullptr) << "CaseDescription: 2. Create a Normal Window (promise != nullptr)";

    // 3. check it
    auto wminfo = promise->Await();
    ASSERT_EQ(wminfo.wret, WM_OK) << "CaseDescription: 3. check it (wret == WM_OK)";
    ASSERT_GE(wminfo.wid, 0) << "CaseDescription: 3. check it (wid >= 0)";
    ASSERT_GT(wminfo.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(wminfo.height, 0u) << "CaseDescription: 3. check it (height > 0)";
}

/*
 * Feature: WindowManagerServer CreateWindow by some abnormal arguments
 * Function: WindowManagerServer
 * SubFunction: CreateWindow
 * FunctionPoints: WindowManagerServer CreateWindow
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. create WlSurface
 *                  2. CreateWindow by some abnormal arguments
 *                  3. check they
 */
HWTEST_F(WindowManagerServerTest, CreateAbnormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. create WlSurface
    sptr<WlSurface> wlSurface = WlSurfaceFactory::GetInstance()->Create();
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. CreateWindow by some abnormal arguments
    sptr<WlSurface> wlSurfaceNullptr = nullptr;
    auto promise1 = WindowManagerServer::GetInstance()->CreateWindow(wlSurfaceNullptr, 0, WINDOW_TYPE_NORMAL);
    auto promise2 = WindowManagerServer::GetInstance()->CreateWindow(wlSurface, -1, WINDOW_TYPE_NORMAL);
    auto promise3 = WindowManagerServer::GetInstance()->CreateWindow(wlSurface, 0, static_cast<WindowType>(-1));

    // 3. check they
    auto wminfo1 = promise1->Await();
    auto wminfo2 = promise2->Await();
    auto wminfo3 = promise3->Await();

    ASSERT_NE(wminfo1.wret, WM_OK) << "CaseDescription: 3. check they (wret1 != WM_OK)";
    ASSERT_EQ(wminfo1.wid, -1) << "CaseDescription: 3. check they (wid1 == -1)";
    ASSERT_EQ(wminfo1.x, 0) << "CaseDescription: 3. check they (x1 == 0)";
    ASSERT_EQ(wminfo1.y, 0) << "CaseDescription: 3. check they (y1 == 0)";
    ASSERT_EQ(wminfo1.width, 0u) << "CaseDescription: 3. check they (width1 == 0)";
    ASSERT_EQ(wminfo1.height, 0u) << "CaseDescription: 3. check they (height1 == 0)";

    ASSERT_NE(wminfo2.wret, WM_OK) << "CaseDescription: 3. check they (wret2 != WM_OK)";
    ASSERT_EQ(wminfo2.wid, -1) << "CaseDescription: 3. check they (wid2 == -1)";
    ASSERT_EQ(wminfo2.x, 0) << "CaseDescription: 3. check they (x2 == 0)";
    ASSERT_EQ(wminfo2.y, 0) << "CaseDescription: 3. check they (y2 == 0)";
    ASSERT_EQ(wminfo2.width, 0u) << "CaseDescription: 3. check they (width2 == 0)";
    ASSERT_EQ(wminfo2.height, 0u) << "CaseDescription: 3. check they (height2 == 0)";

    ASSERT_NE(wminfo3.wret, WM_OK) << "CaseDescription: 3. check they (wret3 != WM_OK)";
    ASSERT_EQ(wminfo3.wid, -1) << "CaseDescription: 3. check they (wid3 == -1)";
    ASSERT_EQ(wminfo3.x, 0) << "CaseDescription: 3. check they (x3 == 0)";
    ASSERT_EQ(wminfo3.y, 0) << "CaseDescription: 3. check they (y3 == 0)";
    ASSERT_EQ(wminfo3.width, 0u) << "CaseDescription: 3. check they (width3 == 0)";
    ASSERT_EQ(wminfo3.height, 0u) << "CaseDescription: 3. check they (height3 == 0)";
}
} // namespace
} // namespace OHOS
