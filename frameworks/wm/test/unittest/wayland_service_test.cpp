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

#include "wayland_service_test.h"

#include "wl_display.h"

namespace OHOS {
void WaylandServiceTest::SetUp()
{
}

void WaylandServiceTest::TearDown()
{
}

void WaylandServiceTest::SetUpTestCase()
{
}

void WaylandServiceTest::TearDownTestCase()
{
}

void WaylandServiceTest::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    callCount++;
}

namespace {
/*
 * Feature: WaylandService start getting service
 * Function: WaylandService
 * SubFunction: Start
 * FunctionPoints: WaylandService start
 * EnvConditions: NotAvaliable
 * CaseDescription: 1. display connect to socket
 *                  2. start WaylandService
 *                  3. display roundtrip
 *                  4. check callback is called
 */
HWTEST_F(WaylandServiceTest, Start, testing::ext::TestSize.Level0)
{
    // once before all
    const auto appear = std::bind(&WaylandServiceTest::OnAppear, this, __BIND3_ARGS);
    WaylandService::GetInstance()->OnAppear(appear);

    const auto& display = WlDisplay::GetInstance();
    const auto& ws = WaylandService::GetInstance();

    // 1. display connect to socket
    bool bret = display->Connect(nullptr);
    ASSERT_EQ(bret, true) << "CaseDescription: 1. display connect to socket (bret == true)";

    // 2. start WaylandService
    callCount = 0;
    WMError wret = ws->Start();
    ASSERT_EQ(wret, WM_OK) << "CaseDescription: 2. start WaylandService (wret == WM_OK)";

    // 3. display roundtrip
    int32_t iret = display->Roundtrip();
    ASSERT_NE(iret, -1) << "CaseDescription: 3. display roundtrip (iret != -1)";

    iret = display->Roundtrip();
    ASSERT_NE(iret, -1) << "CaseDescription: 3. display roundtrip (iret != -1)";

    // 4. check callback is called
    ASSERT_GT(callCount, 0u) << "CaseDescription: 4. check callback is called (callCount > 0u)";

    // postprocess
    ws->Stop();
    display->Disconnect();
}

/*
 * Feature: start WaylandService before display connect
 * Function: WaylandService
 * SubFunction: Start
 * FunctionPoints: WaylandService start
 * EnvConditions: NotAvaliable
 * CaseDescription: 1. start WaylandService
 *                  2. display roundtrip
 *                  3. check callback is not callled
 */
HWTEST_F(WaylandServiceTest, StartBeforeConnect, testing::ext::TestSize.Level0)
{
    const auto& display = WlDisplay::GetInstance();
    const auto& ws = WaylandService::GetInstance();

    // 1. start WaylandService
    callCount = 0;
    WMError wret = ws->Start();
    ASSERT_EQ(wret, WM_ERROR_NOT_INIT) << "CaseDescription: "
        << "1. start WaylandService (wret == WM_ERROR_NOT_INIT)";

    // 2. display roundtrip
    int32_t iret = display->Roundtrip();
    ASSERT_EQ(iret, -1) << "CaseDescription: 2. display roundtrip (iret == -1)";

    iret = display->Roundtrip();
    ASSERT_EQ(iret, -1) << "CaseDescription: 2. display roundtrip (iret == -1)";

    // 3. check callback is not callled
    ASSERT_EQ(callCount, 0u) << "CaseDescription: 3. check callback is not callled (callCount == 0u)";

    // postprocess
    ws->Stop();
}

/*
 * Feature: cannot get service after WaylandService start
 * Function: WaylandService
 * SubFunction: Stop
 * FunctionPoints: WaylandService stop
 * EnvConditions: display connected, WaylandService started
 * CaseDescription: 1. register listener
 *                  2. stop
 *                  3. check callback is not callled
 */
HWTEST_F(WaylandServiceTest, Stop, testing::ext::TestSize.Level0)
{
    const auto& display = WlDisplay::GetInstance();
    const auto& ws = WaylandService::GetInstance();

    // display connected, WaylandService started
    bool bret = display->Connect(nullptr);
    ASSERT_EQ(bret, true) << "EnvConditions: display connected, WaylandService started (bret == true)";
    WMError wret = ws->Start();
    ASSERT_EQ(wret, WM_OK) << "EnvConditions: display connected, WaylandService started (wret == WM_OK)";

    int32_t iret = display->Roundtrip();
    ASSERT_NE(iret, -1) << "EnvConditions: display connected, WaylandService started (iret != -1)";

    iret = display->Roundtrip();
    ASSERT_NE(iret, -1) << "EnvConditions: display connected, WaylandService started (iret != -1)";

    // 1. register listener
    callCount = 0;

    // 2. stop
    ws->Stop();

    // 3. check callback is not callled
    ASSERT_EQ(callCount, 0u) << "CaseDescription: 3. check callback is not callled (callCount == 0u)";

    // postprocess
    display->Disconnect();
}
} // namespace
} // namespace OHOS
