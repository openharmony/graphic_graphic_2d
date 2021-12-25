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

#include "window_impl_test.h"

#include <memory>

#include "mock/mock_iwindow_manager_service.h"
#include "mock/mock_static_call.h"
#include "mock/mock_window_manager_server.h"
#include "mock/mock_wl_surface_factory.h"
#include "mock/singleton_mocker.h"
#include "test_header.h"
#include "tester.h"
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
void WindowImplTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    wo = WindowOption::Get();
    WindowManager::GetInstance()->Init();
    WindowManager::GetInstance()->CreateWindow(normalWindow, wo);
    WindowManager::GetInstance()->CreateWindow(destroyedWindow, wo);
    destroyedWindow->Destroy();
}

void WindowImplTest::TearDownTestCase()
{
}

void WindowImplTest::SetUp()
{
}

void WindowImplTest::TearDown()
{
}

namespace {
/*
 * Function: Create
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call WindowImpl::Create with option=nullptr
 *                  2. check wret is GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, Create01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        GSError wret;
        STEP("1. call WindowImpl::Create with option=nullptr") {
            sptr<Window> window = nullptr;
            sptr<WindowOption> nullOption = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, nullOption, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("2. check wret is GSERROR_INVALID_ARGUMENTS") {
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. call WindowImpl::Create with wms=nullptr
 *                  2. check wret is GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, Create02, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        GSError wret;
        STEP("1. call WindowImpl::Create with wms=nullptr") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = nullptr;
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("2. check wret is GSERROR_INVALID_ARGUMENTS") {
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. SetTestNew(WindowImpl) return nullptr
 *                  2. call WindowImpl::Create
 *                  3. check wret is GSERROR_NO_MEM
 */
HWTEST_F(WindowImplTest, Create03, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        STEP("1. SetTestNew(WindowImpl) return nullptr") {
            Tester::Get().SetTestNew("WindowImpl", nullptr);
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is GSERROR_NO_MEM") {
            STEP_ASSERT_EQ(wret, GSERROR_NO_MEM);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. Mock WlSurfaceFactory, Create return nullptr
 *                  2. call WindowImpl::Create
 *                  3. check wret is GSERROR_API_FAILED
 */
HWTEST_F(WindowImplTest, Create04, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlSurfaceFactory, MockWlSurfaceFactory>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. Mock WlSurfaceFactory, Create return nullptr") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Create()).Times(1).WillRepeatedly(Return(nullptr));
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. Mock WindowManagerServer, CreateWindow return nullptr
 *                  2. call WindowImpl::Create
 *                  3. check wret is GSERROR_NO_MEM
 */
HWTEST_F(WindowImplTest, Create05, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServer, MockWindowManagerServer>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. Mock WindowManagerServer, CreateWindow return nullptr") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), CreateWindow(_, _, _)).Times(1).WillRepeatedly(Return(nullptr));
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is GSERROR_NO_MEM") {
            STEP_ASSERT_EQ(wret, GSERROR_NO_MEM);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. Mock WindowManagerServer, CreateWindow return Promise(NO_MEM)
 *                  2. call WindowImpl::Create
 *                  3. check wret is NO_MEM
 */
HWTEST_F(WindowImplTest, Create06, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServer, MockWindowManagerServer>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. Mock WindowManagerServer, CreateWindow return Promise(NO_MEM)") {
            m = std::make_unique<Mocker>();
            struct WMSWindowInfo info = { .wret = GSERROR_NO_MEM };
            EXPECT_CALL(*m->Mock(), CreateWindow(_, _, _))
                .Times(1).WillRepeatedly(Return(new Promise<struct WMSWindowInfo>(info)));
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is NO_MEM") {
            STEP_ASSERT_EQ(wret, GSERROR_NO_MEM);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. Mock StaticCall, SurfaceCreateSurfaceAsConsumer return nullptr
 *                  2. call WindowImpl::Create
 *                  3. check wret is GSERROR_API_FAILED
 */
HWTEST_F(WindowImplTest, Create07, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. Mock StaticCall, SurfaceCreateSurfaceAsConsumer return nullptr") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsConsumer(_))
                .Times(1).WillRepeatedly(Return(nullptr));
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: N/A
 * CaseDescription: 1. Mock StaticCall, SurfaceCreateSurfaceAsProducer return nullptr
 *                  2. call WindowImpl::Create
 *                  3. check wret is GSERROR_API_FAILED
 */
HWTEST_F(WindowImplTest, Create08, Reliability | SmallTest | Level4)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. Mock StaticCall, SurfaceCreateSurfaceAsProducer return nullptr") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsProducer(_))
                .Times(1).WillRepeatedly(Return(nullptr));

            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsConsumer(_))
                .Times(1).WillRepeatedly(Return(m->Origin()->SurfaceCreateSurfaceAsConsumer()));
        }

        GSError wret;
        STEP("2. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_EQ(window, nullptr);
        }

        STEP("3. check wret is GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Function
 * Rank: Basic(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call WindowImpl::Create
 *                  2. check wret is GSERROR_OK
 */
HWTEST_F(WindowImplTest, Create09, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        GSError wret;
        STEP("1. call WindowImpl::Create") {
            sptr<Window> window = nullptr;
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("2. check wret is GSERROR_OK") {
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Create
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Create Window not default prop
 *                  2. check type
 *                  3. check visibility
 *                  4. check xywh
 *                  5. check dest_width, dest_height
 *                  6. check id
 */
HWTEST_F(WindowImplTest, Create10, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        GSError wret;
        sptr<Window> window = nullptr;

        STEP("1. Create Window not default prop") {
            sptr<WindowOption> wo = WindowOption::Get();
            wret = wo->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            wret = wo->SetWindowMode(WINDOW_MODE_FULL);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            wret = wo->SetX(1);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            wret = wo->SetY(1);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            wret = wo->SetWidth(1);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            wret = wo->SetHeight(1);
            STEP_ASSERT_EQ(wret, GSERROR_OK);

            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            EXPECT_CALL(*wms, Move(_, _, _)).Times(1).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
            EXPECT_CALL(*wms, Resize(_, _, _)).Times(1).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
            wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_NE(window, nullptr);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        sptr<WindowImpl> wi = static_cast<WindowImpl *>(window.GetRefPtr());
        STEP("2. check type") {
            STEP_ASSERT_EQ(wi->attr.GetType(), WINDOW_TYPE_ALARM_SCREEN);
        }
        STEP("3. check visibility") {
            STEP_ASSERT_EQ(wi->attr.GetVisibility(), true);
        }
        STEP("4. check xywh") {
            STEP_ASSERT_EQ(wi->attr.GetX(), 1);
            STEP_ASSERT_EQ(wi->attr.GetY(), 1);
            STEP_ASSERT_EQ(wi->attr.GetWidth(), 1u);
            STEP_ASSERT_EQ(wi->attr.GetHeight(), 1u);
        }
        STEP("5. check dest_width, dest_height") {
            STEP_ASSERT_EQ(wi->attr.GetDestWidth(), 1u);
            STEP_ASSERT_EQ(wi->attr.GetDestHeight(), 1u);
        }
        STEP("6. check id") {
            STEP_ASSERT_GE(wi->attr.GetID(), 0);
        }
    }
}

/*
 * Function: GetSurface
 * Type: Function
 * Rank: Basic(1)
 * EnvConditions: N/A
 * CaseDescription: 1. NormalWindow call GetSurface
 *                  2. check sret is not nullptr
 */
HWTEST_F(WindowImplTest, GetSurface01, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        sptr<Surface> surf = nullptr;
        STEP("1. NormalWindow call GetSurface") {
            surf = normalWindow->GetSurface();
        }

        STEP("2. check sret is not nullptr") {
            STEP_ASSERT_NE(surf, nullptr);
        }
    }
}

/*
 * Function: GetSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. DestroyedWindow call GetSurface
 *                  2. check sret is nullptr
 */
HWTEST_F(WindowImplTest, GetSurface02, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        sptr<Surface> surf = nullptr;
        STEP("1. DestroyedWindow call GetSurface") {
            surf = destroyedWindow->GetSurface();
        }

        STEP("2. check sret is nullptr") {
            STEP_ASSERT_EQ(surf, nullptr);
        }
    }
}

/*
 * Function: GetID
 * Type: Function
 * Rank: Basic(1)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WindowManagerServer, CreateWindow return wminfo.id = 0
 *                  2. call WindowImpl::Create
 *                  3. call GetID, check id is 0
 */
HWTEST_F(WindowImplTest, GetID01, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<WindowManagerServer, MockWindowManagerServer>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock WindowManagerServer, CreateWindow return wminfo.id = 0") {
            m = std::make_unique<Mocker>();
            struct WMSWindowInfo info = { .wret = GSERROR_OK, .wid = 0 };
            EXPECT_CALL(*m->Mock(), CreateWindow(_, _, _))
                .Times(1).WillRepeatedly(Return(new Promise<struct WMSWindowInfo>(info)));
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            sptr<MockIWindowManagerService> wms = new MockIWindowManagerService();
            auto wret = WindowImpl::Create(window, wo, wms);
            STEP_ASSERT_NE(window, nullptr);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("3. call GetID, check id is 0") {
            auto id = window->GetID();
            STEP_ASSERT_EQ(id, 0);
        }
    }
}

/*
 * Function: GetID
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. DestroyedWindow call GetID
 *                  2. check id is -1
 */
HWTEST_F(WindowImplTest, GetID02, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        int32_t id = 0;
        STEP("1. DestroyedWindow call GetID") {
            id = destroyedWindow->GetID();
        }

        STEP("2. check id is -1") {
            STEP_ASSERT_EQ(id, -1);
        }
    }
}

/*
 * Function: Show
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->Show called 2 times
 *                  4. call Show, check GSERROR_OK
 *                  5. call Show again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Show01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->Show called 2 times") {
            EXPECT_CALL(*m->Mock(), Show(_)).Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("4. call Show, check GSERROR_OK") {
            auto wret = window->Show()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("5. call Show again, check GSERROR_OK") {
            auto wret = window->Show()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Show
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Show, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Show02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Show, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Show()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: Hide
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. call Show, check GSERROR_OK
 *                  4. expect wms->Hide called 2 times
 *                  5. call Hide, check GSERROR_OK
 *                  6. call Hide again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Hide01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
            EXPECT_CALL(*m->Mock(), Show(_)).Times(1).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. call Show, check GSERROR_OK") {
            auto wret = window->Show()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("4. expect wms->Hide called 2 times") {
            EXPECT_CALL(*m->Mock(), Hide(_)).Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("5. call Hide, check GSERROR_OK") {
            auto wret = window->Hide()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("6. call Hide again, check GSERROR_OK") {
            auto wret = window->Hide()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Hide
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Hide, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Hide02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Hide, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Hide()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: Move
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->Move called 2 times
 *                  4. call Move 1 1, check GSERROR_OK
 *                  5. call Move 1 1 again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Move01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->Move called 2 times") {
            EXPECT_CALL(*m->Mock(), Move(_, _, _))
                .Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("4. call Move 1 1, check GSERROR_OK") {
            auto wret = window->Move(1, 1)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("5. call Move 1 1 again, check GSERROR_OK") {
            auto wret = window->Move(1, 1)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Move
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Move, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Move02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Move, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Move(0, 0)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: SwitchTop
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->SwitchTop called 1 times
 *                  4. call SwitchTop, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, SwitchTop01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->SwitchTop called 1 times") {
            EXPECT_CALL(*m->Mock(), SwitchTop(_))
                .Times(1).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("4. call SwitchTop, check GSERROR_OK") {
            auto wret = window->SwitchTop()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: SwitchTop
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow SwitchTop, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, SwitchTop02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow SwitchTop, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->SwitchTop()->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: SetWindowType
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->SetWindowType called 2 times
 *                  4. call SetWindowType WINDOW_TYPE_ALARM_SCREEN, check GSERROR_OK
 *                  5. call SetWindowType WINDOW_TYPE_ALARM_SCREEN again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, SetWindowType01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->SetWindowType called 2 times") {
            EXPECT_CALL(*m->Mock(), SetWindowType(_, _))
                .Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("4. call SetWindowType WINDOW_TYPE_ALARM_SCREEN, check GSERROR_OK") {
            auto wret = window->SetWindowType(WINDOW_TYPE_ALARM_SCREEN)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("5. call SetWindowType WINDOW_TYPE_ALARM_SCREEN again, check GSERROR_OK") {
            auto wret = window->SetWindowType(WINDOW_TYPE_ALARM_SCREEN)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: SetWindowType
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow SetWindowType, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, SetWindowType02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow SetWindowType, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->SetWindowType(WINDOW_TYPE_NORMAL)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: SetWindowType
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow SetWindowType with -1, check GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, SetWindowType03, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow SetWindowType with -1, check GSERROR_INVALID_ARGUMENTS") {
            auto wret = normalWindow->SetWindowType(static_cast<WindowType>(-1))->Await();
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: SetWindowMode
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow SetWindowMode, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, SetWindowMode01, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow SetWindowMode, check GSERROR_OK") {
            auto wret = normalWindow->SetWindowMode(WINDOW_MODE_FULL)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: SetWindowMode
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow SetWindowMode, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, SetWindowMode02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow SetWindowMode, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->SetWindowMode(WINDOW_MODE_FULL)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: SetWindowMode
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow SetWindowMode with -1, check GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, SetWindowMode03, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow SetWindowMode with -1, check GSERROR_INVALID_ARGUMENTS") {
            auto wret = normalWindow->SetWindowMode(static_cast<WindowMode>(-1))->Await();
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Resize
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->Resize called 2 times
 *                  4. call Resize 2 2, check GSERROR_OK
 *                  5. call Resize 2 2 again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Resize01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->Resize called 2 times") {
            EXPECT_CALL(*m->Mock(), Resize(_, _, _))
                .Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        constexpr int32_t xy = 2;
        STEP("4. call Resize 2 2, check GSERROR_OK") {
            auto wret = window->Resize(xy, xy)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("5. call Resize 2 2 again, check GSERROR_OK") {
            auto wret = window->Resize(xy, xy)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Resize
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Resize, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Resize02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Resize, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Resize(1, 1)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: Resize
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Resize with 0 0, check GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, Resize03, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow Resize with 0 0, check GSERROR_INVALID_ARGUMENTS") {
            auto wret = normalWindow->Resize(0, 0)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: ScaleTo
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->ScaleTo called 2 times
 *                  4. call ScaleTo 2 2, check GSERROR_OK
 *                  5. call ScaleTo 2 2 again, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, ScaleTo01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->ScaleTo called 2 times") {
            EXPECT_CALL(*m->Mock(), ScaleTo(_, _, _))
                .Times(2).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        constexpr int32_t xy = 2;
        STEP("4. call ScaleTo 2 2, check GSERROR_OK") {
            auto wret = window->ScaleTo(xy, xy)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("5. call ScaleTo 2 2 again, check GSERROR_OK") {
            auto wret = window->ScaleTo(xy, xy)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: ScaleTo
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow ScaleTo, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, ScaleTo02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow ScaleTo, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->ScaleTo(1, 1)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: ScaleTo
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow ScaleTo with 0 0, check GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, ScaleTo03, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow ScaleTo with 0 0, check GSERROR_INVALID_ARGUMENTS") {
            auto wret = normalWindow->ScaleTo(0, 0)->Await();
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Rotate
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Rotate WINDOW_ROTATE_TYPE_90, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Rotate01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow Rotate WINDOW_ROTATE_TYPE_90, check GSERROR_OK") {
            auto wret = normalWindow->Rotate(WINDOW_ROTATE_TYPE_90);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Rotate
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Rotate, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Rotate02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Rotate, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Rotate(WINDOW_ROTATE_TYPE_90);
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: Rotate
 * Type: Reliability
 * Rank: Normal(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Rotate with -1, check GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(WindowImplTest, Rotate03, Reliability | SmallTest | Level3)
{
    PART("CaseDescription") {
        STEP("1. call NormalWindow Rotate with 0 0, check GSERROR_INVALID_ARGUMENTS") {
            auto wret = normalWindow->Rotate(static_cast<WindowRotateType>(-1));
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Destroy
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock IWindowManagerService as wms
 *                  2. call WindowImpl::Create
 *                  3. expect wms->Destroy called 1 times
 *                  4. call Destroy, check GSERROR_OK
 */
HWTEST_F(WindowImplTest, Destroy01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        using Mocker = SingletonMocker<IWindowManagerService, MockIWindowManagerService>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. mock IWindowManagerService as wms") {
            m = std::make_unique<Mocker>();
        }

        sptr<Window> window = nullptr;
        STEP("2. call WindowImpl::Create") {
            auto wret = WindowImpl::Create(window, wo, m->Mock());
            STEP_ASSERT_EQ(wret, GSERROR_OK);
            STEP_ASSERT_NE(window, nullptr);
        }

        STEP("3. expect wms->Destroy called 1 times") {
            EXPECT_CALL(*m->Mock(), DestroyWindow(_))
                .Times(1).WillRepeatedly(Return(new Promise<GSError>(GSERROR_OK)));
        }

        STEP("4. call Destroy, check GSERROR_OK") {
            auto wret = window->Destroy();
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }
    }
}

/*
 * Function: Destroy
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call DestroyedWindow Destroy, check GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(WindowImplTest, Destroy02, Reliability | SmallTest | Level2)
{
    PART("CaseDescription") {
        STEP("1. call DestroyedWindow Destroy, check GSERROR_DESTROYED_OBJECT") {
            auto wret = destroyedWindow->Destroy();
            STEP_ASSERT_EQ(wret, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: OnPositionChange
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Move 0 0
 *                  2. call OnPositionChange cb
 *                  3. call NormalWindow Move 1 1
 *                  4. check cb called
 */
HWTEST_F(WindowImplTest, OnPositionChange01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        int32_t callCount = 0;
        WindowPositionChangeFunc func = [&callCount](int32_t x, int32_t y) {
            callCount++;
        };

        STEP("1. call NormalWindow Move 0 0") {
            normalWindow->Move(0, 0);
        }

        STEP("2. call OnPositionChange cb") {
            normalWindow->OnPositionChange(func);
        }

        STEP("3. call NormalWindow Move 1 1") {
            STEP_ASSERT_EQ(callCount, 0);
            normalWindow->Move(1, 1);
        }

        STEP("4. check cb called") {
            STEP_ASSERT_EQ(callCount, 1);
        }
    }
}

/*
 * Function: OnSizeChange
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Resize 2 2
 *                  2. call OnSizeChange cb
 *                  3. call NormalWindow Resize 1 1
 *                  4. check cb called
 */
HWTEST_F(WindowImplTest, OnSizeChange01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        int32_t callCount = 0;
        WindowSizeChangeFunc func = [&callCount](uint32_t w, uint32_t h) {
            callCount++;
        };

        constexpr uint32_t xy = 2;
        STEP("1. call NormalWindow Resize 2 2") {
            normalWindow->Resize(xy, xy);
        }

        STEP("2. call OnSizeChange cb") {
            normalWindow->OnSizeChange(func);
        }

        STEP("3. call NormalWindow Resize 1 1") {
            STEP_ASSERT_EQ(callCount, 0);
            normalWindow->Resize(1, 1);
        }

        STEP("4. check cb called") {
            STEP_ASSERT_EQ(callCount, 1);
        }
    }
}

/*
 * Function: OnVisibilityChange
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow Hide
 *                  2. call OnVisibilityChange cb
 *                  3. call NormalWindow Show
 *                  4. check cb called
 */
HWTEST_F(WindowImplTest, OnVisibilityChange01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        int32_t callCount = 0;
        WindowVisibilityChangeFunc func = [&callCount](bool) {
            callCount++;
        };

        STEP("1. call NormalWindow Hide") {
            normalWindow->Hide();
        }

        STEP("2. call OnVisibilityChange cb") {
            normalWindow->OnVisibilityChange(func);
        }

        STEP("3. call NormalWindow Show") {
            STEP_ASSERT_EQ(callCount, 0);
            normalWindow->Show();
        }

        STEP("4. check cb called") {
            STEP_ASSERT_EQ(callCount, 1);
        }
    }
}

/*
 * Function: OnTypeChange
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NormalWindow SetWindowType WINDOW_TYPE_NORMAL
 *                  2. call OnTypeChange cb
 *                  3. call NormalWindow SetWindowType WINDOW_TYPE_ALARM_SCREEN
 *                  4. check cb called
 */
HWTEST_F(WindowImplTest, OnTypeChange01, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        int32_t callCount = 0;
        WindowTypeChangeFunc func = [&callCount](WindowType) {
            callCount++;
        };

        STEP("1. call NormalWindow SetWindowType WINDOW_TYPE_NORMAL") {
            normalWindow->SetWindowType(WINDOW_TYPE_NORMAL);
        }

        STEP("2. call OnTypeChange cb") {
            normalWindow->OnTypeChange(func);
        }

        STEP("3. call NormalWindow SetWindowType WINDOW_TYPE_ALARM_SCREEN") {
            STEP_ASSERT_EQ(callCount, 0);
            normalWindow->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
        }

        STEP("4. check cb called") {
            STEP_ASSERT_EQ(callCount, 1);
        }
    }
}
} // namespace
} // namespace OHOS
