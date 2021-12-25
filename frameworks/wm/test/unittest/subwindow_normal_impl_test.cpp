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

#include "subwindow_normal_impl_test.h"

#include <thread>
#include <unistd.h>

#include <window_manager.h>

#include "mock/mock_static_call.h"
#include "mock/mock_wl_subsurface_factory.h"
#include "mock/mock_wl_surface_factory.h"
#include "mock/singleton_mocker.h"
#include "subwindow_normal_impl.h"
#include "test_header.h"
#include "tester.h"

namespace OHOS {
using namespace ::testing;
using namespace ::testing::ext;

void SubwindowNormalImplTest::SetUp()
{
}

void SubwindowNormalImplTest::TearDown()
{
}

void SubwindowNormalImplTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;

    auto option = WindowOption::Get();
    auto wm = WindowManager::GetInstance();
    initRet = wm->Init();
    wm->CreateWindow(window, option);
    subwindowOption = SubwindowOption::Get();
}

void SubwindowNormalImplTest::TearDownTestCase()
{
}

namespace {
/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create ($2=nullptr)
 *                  2. check return GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(SubwindowNormalImplTest, Create01, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        sptr<SubwindowNormalImpl> subwindow = nullptr;
        GSError wret;

        STEP("1. Create ($2=nullptr)") {
            sptr<Window> w = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(w, subwindowOption);
        }

        STEP("2. check return GSERROR_INVALID_ARGUMENTS") {
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create ($3=nullptr)
 *                  2. check return GSERROR_INVALID_ARGUMENTS
 */
HWTEST_F(SubwindowNormalImplTest, Create02, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        sptr<SubwindowNormalImpl> subwindow = nullptr;
        GSError wret;

        STEP("1. Create ($3=nullptr)") {
            sptr<SubwindowOption> option = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(window, option);
        }

        STEP("2. check return GSERROR_INVALID_ARGUMENTS") {
            STEP_ASSERT_EQ(wret, GSERROR_INVALID_ARGUMENTS);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. SingletonContainer::SetSingleton MockWlSurfaceFactory
 *                  2. except MockWlSurfaceFactory Create return nullptr
 *                  3. Create (all normal)
 *                  4. check return GSERROR_API_FAILED
 */
HWTEST_F(SubwindowNormalImplTest, Create04, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlSurfaceFactory, MockWlSurfaceFactory>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. SingletonContainer::SetSingleton MockWlSurfaceFactory") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. except MockWlSurfaceFactory Create return nullptr") {
            EXPECT_CALL(*m->Mock(), Create())
                .Times(1)
                .WillOnce(Return(nullptr));
        }

        GSError wret;
        STEP("3. Create (all normal)") {
            sptr<SubwindowNormalImpl> subwindow = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(window, subwindowOption);
        }

        STEP("4. check return GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. SingletonContainer::SetSingleton MockWlSubsurfaceFactory
 *                  2. except MockWlSubsurfaceFactory Create return nullptr
 *                  3. Create (all normal)
 *                  4. check return GSERROR_API_FAILED
 */
HWTEST_F(SubwindowNormalImplTest, Create05, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<WlSubsurfaceFactory, MockWlSubsurfaceFactory>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. SingletonContainer::SetSingleton MockWlSubsurfaceFactory") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. except MockWlSubsurfaceFactory Create return nullptr") {
            EXPECT_CALL(*m->Mock(), Create(_, _))
                .Times(1)
                .WillOnce(Return(nullptr));
        }

        GSError wret;
        STEP("3. Create (all normal)") {
            sptr<SubwindowNormalImpl> subwindow = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(window, subwindowOption);
        }

        STEP("4. check return GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. SingletonContainer::SetSingleton MockStaticCall
 *                  2. except MockStaticCall SurfaceCreateSurfaceAsConsumer return nullptr
 *                  3. Create (all normal)
 *                  4. check return GSERROR_API_FAILED
 */
HWTEST_F(SubwindowNormalImplTest, Create06, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. SingletonContainer::SetSingleton MockStaticCall") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. except MockStaticCall SurfaceCreateSurfaceAsConsumer return nullptr") {
            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsConsumer(_))
                .Times(1)
                .WillOnce(Return(nullptr));
        }

        GSError wret;
        STEP("3. Create (all normal)") {
            sptr<SubwindowNormalImpl> subwindow = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(window, subwindowOption);
        }

        STEP("4. check return GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: Create
 * Type: Reliability
 * Rank: Rare(4)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. SingletonContainer::SetSingleton MockStaticCall
 *                  2. except MockStaticCall SurfaceCreateSurfaceAsProducer return nullptr
 *                  3. Create (all normal)
 *                  4. check return GSERROR_API_FAILED
 */
HWTEST_F(SubwindowNormalImplTest, Create07, Reliability | SmallTest | Level4)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    PART("CaseDescription") {
        using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
        std::unique_ptr<Mocker> m = nullptr;

        STEP("1. SingletonContainer::SetSingleton MockStaticCall") {
            m = std::make_unique<Mocker>();
        }

        STEP("2. except MockStaticCall SurfaceCreateSurfaceAsProducer return nullptr") {
            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsConsumer(_))
                .Times(1)
                .WillOnce(Return(m->Origin()->SurfaceCreateSurfaceAsConsumer()));

            EXPECT_CALL(*m->Mock(), SurfaceCreateSurfaceAsProducer(_))
                .Times(1)
                .WillOnce(Return(nullptr));
        }

        GSError wret;
        STEP("3. Create (all normal)") {
            sptr<SubwindowNormalImpl> subwindow = nullptr;
            subwindow = new SubwindowNormalImpl();
            wret = subwindow->Init(window, subwindowOption);
        }

        STEP("4. check return GSERROR_API_FAILED") {
            STEP_ASSERT_EQ(wret, GSERROR_API_FAILED);
        }
    }
}

/*
 * Function: GetSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. GetSurface
 *                  3. check return normal
 */
HWTEST_F(SubwindowNormalImplTest, GetSurface01, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    sptr<Surface> surf = nullptr;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. GetSurface") {
            surf = subwindow->GetSurface();
        }

        STEP("3. check return normal") {
            STEP_ASSERT_NE(surf, nullptr);
        }
    }
}

/*
 * Function: GetSurface
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. Destroy
 *                  3. GetSurface
 *                  4. check return nullptr
 */
HWTEST_F(SubwindowNormalImplTest, GetSurface02, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    sptr<Surface> surf = nullptr;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. Destroy") {
            subwindow->Destroy();
        }

        STEP("3. GetSurface") {
            surf = subwindow->GetSurface();
        }

        STEP("4. check return nullptr") {
            STEP_ASSERT_EQ(surf, nullptr);
        }
    }
}

/*
 * Function: Move
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. Move
 *                  3. check return GSERROR_OK
 */
HWTEST_F(SubwindowNormalImplTest, Move01, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    GSError moveRet;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. Move") {
            moveRet = subwindow->Move(1, 1);
        }

        STEP("3. check return GSERROR_OK") {
            STEP_ASSERT_EQ(moveRet, GSERROR_OK);
        }
    }
}

/*
 * Function: Move
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. Destroy
 *                  3. Move
 *                  4. check return GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(SubwindowNormalImplTest, Move02, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    GSError moveRet;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. Destroy") {
            subwindow->Destroy();
        }

        STEP("3. Move") {
            moveRet = subwindow->Move(1, 1);
        }

        STEP("4. check return GSERROR_DESTROYED_OBJECT") {
            STEP_ASSERT_EQ(moveRet, GSERROR_DESTROYED_OBJECT);
        }
    }
}

/*
 * Function: Resize
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. Resize
 *                  3. check return normal
 */
HWTEST_F(SubwindowNormalImplTest, Resize01, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    GSError resizeRet;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. Resize") {
            constexpr uint32_t width = 100;
            constexpr uint32_t height = 200;
            resizeRet = subwindow->Resize(width, height);
        }

        STEP("3. check return normal") {
            STEP_ASSERT_EQ(resizeRet, GSERROR_OK);
        }
    }
}

/*
 * Function: Resize
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create (all normal)
 *                  2. Destroy
 *                  3. Resize
 *                  4. check return GSERROR_DESTROYED_OBJECT
 */
HWTEST_F(SubwindowNormalImplTest, Resize02, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        STEP("WindowManager init success.") {
            STEP_ASSERT_EQ(initRet, GSERROR_OK);
        }
    }

    sptr<SubwindowNormalImpl> subwindow = nullptr;
    GSError resizeRet;

    PART("CaseDescription") {
        STEP("1. Create (all normal)") {
            subwindow = new SubwindowNormalImpl();
            auto wret = subwindow->Init(window, subwindowOption);
            STEP_ASSERT_EQ(wret, GSERROR_OK);
        }

        STEP("2. Destroy") {
            subwindow->Destroy();
        }

        STEP("3. Resize") {
            constexpr uint32_t width = 100;
            constexpr uint32_t height = 200;
            resizeRet = subwindow->Resize(width, height);
        }

        STEP("4. check return GSERROR_DESTROYED_OBJECT") {
            STEP_ASSERT_EQ(resizeRet, GSERROR_DESTROYED_OBJECT);
        }
    }
}
} // namespace
} // namespace OHOS
