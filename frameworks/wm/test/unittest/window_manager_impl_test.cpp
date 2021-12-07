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

#include "window_manager_impl_test.h"

#include "mock/mock_iwindow_manager_service.h"
#include "mock/mock_static_call.h"
#include "mock/mock_subwindow_option.h"
#include "mock/mock_window.h"
#include "mock/mock_window_manager_service_client.h"
#include "mock/mock_wl_display.h"
#include "mock/singleton_mocker.h"
#include "test_header.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
void WindowManagerImplTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
}

void WindowManagerImplTest::TearDownTestCase()
{
}

void WindowManagerImplTest::SetUp()
{
}

void WindowManagerImplTest::TearDown()
{
}

void WindowManagerImplTest::OnScreenShot(const struct WMImageInfo &info)
{
}

void WindowManagerImplTest::OnWindowShot(const struct WMImageInfo &info)
{
}

namespace {
/*
 * Function: Init
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WlDisplay
 *                  2. Connect return false
 *                  3. call Init
 *                  4. check wret is WM_ERROR_CONNOT_CONNECT_WESTON
 */
HWTEST_F(WindowManagerImplTest, Init01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlDisplay, MockWlDisplay>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. mock WlDisplay") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. Connect return false") {
            EXPECT_CALL(*m->Mock(), GetFd()).Times(1).WillRepeatedly(Return(-1));
            EXPECT_CALL(*m->Mock(), Connect(_)).Times(1).WillRepeatedly(Return(false));
        }

        WMError wret;
        STEP("3. call Init") {
            wret = wmi->Init();
        }

        STEP("4. check wret is WM_ERROR_CONNOT_CONNECT_WESTON") {
            STEP_ASSERT_EQ(wret, WM_ERROR_CONNOT_CONNECT_WESTON);
        }
    }
}

/*
 * Function: Init
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WindowManagerServiceClient
 *                  2. Init return WM_ERROR_NOT_INIT
 *                  3. call Init
 *                  4. check wret is WM_ERROR_NOT_INIT
 *                  5. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, Init02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. Init return WM_ERROR_NOT_INIT") {
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_ERROR_NOT_INIT));
        }

        WMError wret;
        STEP("3. call Init") {
            wret = wmi->Init();
        }

        STEP("4. check wret is WM_ERROR_NOT_INIT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_INIT);
        }

        STEP("5. call Deinit, remove MockWindowManagerServiceClient") {
            wmi->Deinit();
        }
    }
}

/*
 * Function: Init
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init return WM_OK
 */
HWTEST_F(WindowManagerImplTest, Init03, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Init return WM_OK") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }
    }
}

/*
 * Function: Init
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: wmi->init is true
 * CaseDescription: 1. mock WlDisplay
 *                  2. GetError return 0
 *                  3. expect GetFd call 0 times
 *                  4. call Init
 */
HWTEST_F(WindowManagerImplTest, Init04, Function | SmallTest | Level2)
{
    sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();
    PART("EnvConditions") {
        STEP("wmi->init is true") {
            STEP_ASSERT_EQ(wmi->init, true);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlDisplay, MockWlDisplay>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock WlDisplay") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. GetError return 0") {
            EXPECT_CALL(*m->Mock(), GetError()).Times(1).WillRepeatedly(Return(0));
        }

        STEP("3. expect GetFd call 0 times") {
            EXPECT_CALL(*m->Mock(), GetFd()).Times(0);
        }

        STEP("4. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }
    }
}

/*
 * Function: Init
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: wmi->init is true
 * CaseDescription: 1. mock WlDisplay
 *                  2. GetError return -1
 *                  3. expect GetFd call 1 times
 *                  4. call Init
 */
HWTEST_F(WindowManagerImplTest, Init05, Function | SmallTest | Level2)
{
    sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();
    PART("EnvConditions") {
        STEP("wmi->init is true") {
            STEP_ASSERT_EQ(wmi->init, true);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlDisplay, MockWlDisplay>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock WlDisplay") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), StopDispatchThread()).Times(1);
            EXPECT_CALL(*m->Mock(), Roundtrip()).Times(3);
            EXPECT_CALL(*m->Mock(), Disconnect()).Times(1);
            EXPECT_CALL(*m->Mock(), GetRawPtr()).Times(1)
                .WillRepeatedly(Return(m->Origin()->GetRawPtr()));
            EXPECT_CALL(*m->Mock(), StartDispatchThread()).Times(1);
        }

        STEP("2. GetError return -1") {
            EXPECT_CALL(*m->Mock(), GetError()).Times(1).WillRepeatedly(Return(-1));
        }

        STEP("3. expect GetFd call 1 times") {
            EXPECT_CALL(*m->Mock(), GetFd()).Times(1).WillRepeatedly(Return(m->Origin()->GetFd()));
        }

        STEP("4. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }
    }
}

/*
 * Function: GetDisplays
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. call GetDisplays
 *                  3. check wret is WM_ERROR_NOT_INIT
 */
HWTEST_F(WindowManagerImplTest, GetDisplays01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        WMError wret;
        STEP("2. call GetDisplays") {
            std::vector<struct WMDisplayInfo> displays;
            wret = wmi->GetDisplays(displays);
        }

        STEP("3. check wret is WM_ERROR_NOT_INIT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_INIT);
        }
    }
}

/*
 * Function: GetDisplays
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WindowManagerServiceClient
 *                  2. WindowManagerServiceClient GetService return MockIWindowManagerService
 *                  3. call Init
 *                  4. expect GetDisplays call 1 times
 *                  5. call GetDisplays
 *                  6. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, GetDisplays02, Function | MediumTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_OK));
        }

        sptr<MockIWindowManagerService> mockiwms = new MockIWindowManagerService();
        STEP("2. WindowManagerServiceClient GetService return MockIWindowManagerService") {
            EXPECT_CALL(*m->Mock(), GetService()).Times(1).WillRepeatedly(Return(mockiwms));
        }

        STEP("3. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        STEP("4. expect GetDisplays call 1 times") {
            EXPECT_CALL(*mockiwms, GetDisplays(_)).Times(1);
        }

        STEP("5. call GetDisplays") {
            std::vector<struct WMDisplayInfo> displays;
            wmi->GetDisplays(displays);
        }

        STEP("6. call Deinit, remove MockWindowManagerServiceClient") {
            wmi->Deinit();
        }
    }
}

/*
 * Function: CreateWindow
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. call CreateWindow
 *                  3. check wret is WM_ERROR_NOT_INIT
 */
HWTEST_F(WindowManagerImplTest, CreateWindow01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        WMError wret;
        STEP("2. call CreateWindow") {
            sptr<Window> window = nullptr;
            auto option = WindowOption::Get();
            wret = wmi->CreateWindow(window, option);
        }

        STEP("3. check wret is WM_ERROR_NOT_INIT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_INIT);
        }
    }
}

/*
 * Function: CreateWindow
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init
 *                  2. mock StaticCall
 *                  3. expect WindowImplCreate call 1 times
 *                  4. call CreateWindow
 */
HWTEST_F(WindowManagerImplTest, CreateWindow02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        STEP("2. mock StaticCall") {
            m = std::make_unique<Mocker>();
        }

        STEP("3. expect WindowImplCreate call 1 times") {
            EXPECT_CALL(*m->Mock(), WindowImplCreate(_, _, _)).Times(1);
        }

        STEP("4. call CreateWindow") {
            sptr<Window> window = nullptr;
            auto option = WindowOption::Get();
            wmi->CreateWindow(window, option);
        }
    }
}

/*
 * Function: CreateSubwindow
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock StaticCall
 *                  2. expect SubwindowNormalImplCreate call 1 times
 *                  3. call CreateSubwindow
 */
HWTEST_F(WindowManagerImplTest, CreateSubwindow01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        sptr<Window> window = nullptr;
        auto woption = WindowOption::Get();
        wmi->CreateWindow(window, woption);

        STEP("1. mock StaticCall") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. expect SubwindowNormalImplCreate call 1 times") {
            EXPECT_CALL(*m->Mock(), SubwindowNormalImplCreate(_, _, _)).Times(1);
        }

        STEP("3. call CreateSubwindow") {
            sptr<Subwindow> subwindow = nullptr;
            auto option = SubwindowOption::Get();
            option->SetWindowType(SUBWINDOW_TYPE_NORMAL);
            wmi->CreateSubwindow(subwindow, window, option);
        }
    }
}

/*
 * Function: CreateSubwindow
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock StaticCall
 *                  2. expect SubwindowVideoImplCreate call 1 times
 *                  3. call CreateSubwindow
 */
HWTEST_F(WindowManagerImplTest, CreateSubwindow02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        sptr<Window> window = nullptr;
        auto woption = WindowOption::Get();
        wmi->CreateWindow(window, woption);

        STEP("1. mock StaticCall") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. expect SubwindowVideoImplCreate call 1 times") {
            EXPECT_CALL(*m->Mock(), SubwindowVideoImplCreate(_, _, _)).Times(1);
        }

        STEP("3. call CreateSubwindow") {
            sptr<Subwindow> subwindow = nullptr;
            auto option = SubwindowOption::Get();
            option->SetWindowType(SUBWINDOW_TYPE_VIDEO);
            wmi->CreateSubwindow(subwindow, window, option);
        }
    }
}

/*
 * Function: CreateSubwindow
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock SubwindowOption
 *                  2. GetWindowType return -1, -1
 *                  3. call CreateSubwindow
 *                  4. check wret is WM_ERROR_NOT_SUPPORT
 */
HWTEST_F(WindowManagerImplTest, CreateSubwindow03, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        sptr<MockSubwindowOption> mockSubwindowOption = nullptr;
        STEP("1. mock SubwindowOption") {
            mockSubwindowOption = new MockSubwindowOption();
        }

        STEP("2. GetWindowType return -1, -1") {
            EXPECT_CALL(*mockSubwindowOption, GetWindowType())
                .Times(2)
                .WillRepeatedly(Return(static_cast<SubwindowType>(-1)));
        }

        WMError wret;
        STEP("3. call CreateSubwindow") {
            sptr<Subwindow> subwindow = nullptr;
            sptr<Window> window = nullptr;
            auto woption = WindowOption::Get();
            wmi->CreateWindow(window, woption);
            wret = wmi->CreateSubwindow(subwindow, window, mockSubwindowOption);
        }

        STEP("4. check wret is WM_ERROR_NOT_SUPPORT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_SUPPORT);
        }
    }
}

/*
 * Function: ListenNextScreenShot
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. call ListenNextScreenShot
 *                  3. check wret is WM_ERROR_NOT_INIT
 */
HWTEST_F(WindowManagerImplTest, ListenNextScreenShot01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        WMError wret;
        STEP("2. call ListenNextScreenShot") {
            wret = wmi->ListenNextScreenShot(0, this);
        }

        STEP("3. check wret is WM_ERROR_NOT_INIT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_INIT);
        }
    }
}

/*
 * Function: ListenNextScreenShot
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init
 *                  2. call ListenNextScreenShot by nullptr
 *                  3. check wret is WM_ERROR_NULLPTR
 */
HWTEST_F(WindowManagerImplTest, ListenNextScreenShot02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("2. call ListenNextScreenShot by nullptr") {
            wret = wmi->ListenNextScreenShot(0, nullptr);
        }

        STEP("3. check wret is WM_ERROR_NULLPTR") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NULLPTR);
        }
    }
}

/*
 * Function: ListenNextScreenShot
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. mock WindowManagerServiceClient
 *                  3. WindowManagerServiceClient GetService return mock IWindowManagerService
 *                  4. IWindowManagerService ShotScreen return nullptr
 *                  5. call Init
 *                  6. call ListenNextScreenShot
 *                  7. check wret is WM_ERROR_NEW
 *                  8. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, ListenNextScreenShot03, Reliability | MediumTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        STEP("2. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_OK));
        }

        sptr<MockIWindowManagerService> mockiwms = new MockIWindowManagerService();
        STEP("3. WindowManagerServiceClient GetService return mock IWindowManagerService") {
            EXPECT_CALL(*m->Mock(), GetService())
                .Times(1)
                .WillRepeatedly(Return(mockiwms));
        }

        STEP("4. IWindowManagerService ShotScreen return nullptr") {
            EXPECT_CALL(*mockiwms, ShotScreen(_))
                .Times(1)
                .WillRepeatedly(Return(nullptr));
        }

        STEP("5. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("6. call ListenNextScreenShot") {
            wret = wmi->ListenNextScreenShot(0, this);
        }

        STEP("7. check wret is WM_ERROR_NEW") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NEW);
        }

        STEP("8. call Deinit") {
            wmi->Deinit();
        }
    }
}

/*
 * Function: ListenNextScreenShot
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. mock WindowManagerServiceClient
 *                  3. WindowManagerServiceClient GetService return mock IWindowManagerService
 *                  4. IWindowManagerService ShotScreen return Promise
 *                  5. call Init
 *                  6. call ListenNextScreenShot
 *                  7. check wret is WM_OK
 *                  8. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, ListenNextScreenShot04, Function | MediumTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        STEP("2. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_OK));
        }

        sptr<MockIWindowManagerService> mockiwms = new MockIWindowManagerService();
        STEP("3. WindowManagerServiceClient GetService return mock IWindowManagerService") {
            EXPECT_CALL(*m->Mock(), GetService())
                .Times(1)
                .WillRepeatedly(Return(mockiwms));
        }

        STEP("4. IWindowManagerService ShotScreen return Promise") {
            EXPECT_CALL(*mockiwms, ShotScreen(_))
                .Times(1)
                .WillRepeatedly(Return(new Promise<WMSImageInfo>));
        }

        STEP("5. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("6. call ListenNextScreenShot") {
            wret = wmi->ListenNextScreenShot(0, this);
        }

        STEP("7. check wret is WM_OK") {
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        STEP("8. call Deinit, remove MockWindowManagerServiceClient") {
            wmi->Deinit();
        }
    }
}

/*
 * Function: ListenNextWindowShot
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. call ListenNextWindowShot
 *                  3. check wret is WM_ERROR_NOT_INIT
 */
HWTEST_F(WindowManagerImplTest, ListenNextWindowShot01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        WMError wret;
        STEP("2. call ListenNextWindowShot") {
            wret = wmi->ListenNextWindowShot(0, this);
        }

        STEP("3. check wret is WM_ERROR_NOT_INIT") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NOT_INIT);
        }
    }
}

/*
 * Function: ListenNextWindowShot
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init
 *                  2. call ListenNextWindowShot by nullptr
 *                  3. check wret is WM_ERROR_NULLPTR
 */
HWTEST_F(WindowManagerImplTest, ListenNextWindowShot02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("2. call ListenNextWindowShot by nullptr") {
            wret = wmi->ListenNextWindowShot(0, nullptr);
        }

        STEP("3. check wret is WM_ERROR_NULLPTR") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NULLPTR);
        }
    }
}

/*
 * Function: ListenNextWindowShot
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. mock WindowManagerServiceClient
 *                  3. WindowManagerServiceClient GetService return mock IWindowManagerService
 *                  4. IWindowManagerService ShotWindow return nullptr
 *                  5. call Init
 *                  6. call ListenNextWindowShot
 *                  7. check wret is WM_ERROR_NEW
 *                  8. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, ListenNextWindowShot03, Reliability | MediumTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        STEP("2. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_OK));
        }

        sptr<MockIWindowManagerService> mockiwms = new MockIWindowManagerService();
        STEP("3. WindowManagerServiceClient GetService return mock IWindowManagerService") {
            EXPECT_CALL(*m->Mock(), GetService()).Times(1).WillRepeatedly(Return(mockiwms));
        }

        STEP("4. IWindowManagerService ShotWindow return nullptr") {
            EXPECT_CALL(*mockiwms, ShotWindow(_)).Times(1).WillRepeatedly(Return(nullptr));
        }

        STEP("5. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("6. call ListenNextWindowShot") {
            sptr<MockWindow> mw = new MockWindow();
            EXPECT_CALL(*mw, GetID()).Times(1).WillRepeatedly(Return(0));

            wret = wmi->ListenNextWindowShot(mw, this);
        }

        STEP("7. check wret is WM_ERROR_NEW") {
            STEP_ASSERT_EQ(wret, WM_ERROR_NEW);
        }

        STEP("8. call Deinit, remove MockWindowManagerServiceClient") {
            wmi->Deinit();
        }
    }
}

/*
 * Function: ListenNextWindowShot
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Deinit
 *                  2. mock WindowManagerServiceClient
 *                  3. WindowManagerServiceClient GetService return mock IWindowManagerService
 *                  4. IWindowManagerService ShotWindow return Promise
 *                  5. call Init
 *                  6. call ListenNextWindowShot
 *                  7. check wret is WM_OK
 *                  8. call Deinit, remove MockWindowManagerServiceClient
 */
HWTEST_F(WindowManagerImplTest, ListenNextWindowShot04, Function | MediumTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServiceClient, MockWindowManagerServiceClient>;
        std::unique_ptr<Mocker> m = nullptr;
        sptr<WindowManagerImpl> wmi = WindowManagerImpl::GetInstance();

        STEP("1. call Deinit") {
            wmi->Deinit();
        }

        STEP("2. mock WindowManagerServiceClient") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Init()).Times(1).WillRepeatedly(Return(WM_OK));
        }

        sptr<MockIWindowManagerService> mockiwms = new MockIWindowManagerService();
        STEP("3. WindowManagerServiceClient GetService return mock IWindowManagerService") {
            EXPECT_CALL(*m->Mock(), GetService()).Times(1).WillRepeatedly(Return(mockiwms));
        }

        STEP("4. IWindowManagerService ShotWindow return Promise") {
            EXPECT_CALL(*mockiwms, ShotWindow(_)).Times(1)
                .WillRepeatedly(Return(new Promise<WMSImageInfo>));
        }

        STEP("5. call Init") {
            auto wret = wmi->Init();
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        WMError wret;
        STEP("6. call ListenNextWindowShot") {
            sptr<MockWindow> mw = new MockWindow();
            EXPECT_CALL(*mw, GetID()).Times(1).WillRepeatedly(Return(0));

            wret = wmi->ListenNextWindowShot(mw, this);
        }

        STEP("7. check wret is WM_OK") {
            STEP_ASSERT_EQ(wret, WM_OK);
        }

        STEP("8. call Deinit, remove MockWindowManagerServiceClient") {
            wmi->Deinit();
        }
    }
}
} // namespace
} // namespace OHOS
