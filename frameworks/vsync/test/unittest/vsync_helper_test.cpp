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

#include "vsync_helper_test.h"

#include <event_handler.h>
#include <gtest/gtest.h>
#include <system_ability_definition.h>

#include "vsync_helper_impl.h"
#include "vsync_module_impl.h"
#include "vsync_log.h"

using namespace OHOS;

#define SWITCH()                    \
    do {                            \
        usleep(MICROSECOND_SWITCH); \
        sleep(0);                   \
    } while (0)

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncHelperTest" };
} // namespace

class TestVsyncModule : public VsyncModuleImpl {
public:
    static TestVsyncModule* GetInstance()
    {
        static TestVsyncModule m;
        return &m;
    }

    virtual VsyncError InitSA() override
    {
        return VsyncModuleImpl::InitSA(VSYNC_MANAGER_TEST_ID);
    }

private:
    TestVsyncModule() = default;
    ~TestVsyncModule() = default;
};

class TestVsyncHelper : public VsyncHelperImpl {
public:
    static sptr<VsyncHelperImpl> Current()
    {
        if (currentTestHelper == nullptr) {
            auto currentRunner = AppExecFwk::EventRunner::Current();
            if (currentRunner == nullptr) {
                VLOG_FAILURE("AppExecFwk::EventRunner::Current() return nullptr");
                return nullptr;
            }

            std::shared_ptr<AppExecFwk::EventHandler> handler =
                std::make_shared<AppExecFwk::EventHandler>(currentRunner);
            VLOG_SUCCESS("new TestVsyncHelper");
            currentTestHelper = new TestVsyncHelper(handler);
        }

        return currentTestHelper;
    }

    explicit TestVsyncHelper(std::shared_ptr<AppExecFwk::EventHandler>& handler)
        : VsyncHelperImpl(handler)
    {
    }

    ~TestVsyncHelper()
    {
    }

    virtual VsyncError InitSA() override
    {
        return VsyncHelperImpl::InitSA(VSYNC_MANAGER_TEST_ID);
    }

private:
    static thread_local sptr<TestVsyncHelper> currentTestHelper;
};
thread_local sptr<TestVsyncHelper> TestVsyncHelper::currentTestHelper;

class GlobalData {
public:
    static GlobalData *GetInstance()
    {
        static GlobalData data;
        return &data;
    }

    sptr<TestVsyncHelper> helper_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;

private:
    GlobalData() = default;
    ~GlobalData() = default;
};

class VsyncHelperTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        TestVsyncModule::GetInstance()->Start();

        auto d = GlobalData::GetInstance();
        d->runner_ = AppExecFwk::EventRunner::Create(true);
        d->handler_ = std::make_shared<AppExecFwk::EventHandler>(d->runner_);

        d->helper_ = new TestVsyncHelper(d->handler_);
    }

    static void TearDownTestCase()
    {
        GlobalData::GetInstance()->helper_ = nullptr;

        TestVsyncModule::GetInstance()->Stop();
    }

private:
    static constexpr useconds_t MICROSECOND_SWITCH = 50 * 1000;
};

class TestVsyncCallback : public VsyncCallbackStub {
public:
    explicit TestVsyncCallback(int &num) : num_(num)
    {
    }

    ~TestVsyncCallback()
    {
    }

    void OnVsync(int64_t timestamp) override
    {
        num_++;
    }

private:
    int &num_;
};

namespace {
HWTEST_F(VsyncHelperTest, RequestFrameCallback, testing::ext::TestSize.Level0)
{
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    handler->PostTask([&]() {
        sptr<VsyncHelperImpl> helper = new TestVsyncHelper(handler);

        static int num = 0;
        struct FrameCallback cb = {
            .timestamp_ = 0,
            .userdata_ = &num,
            .callback_ = [](int64_t timestamp, void* userdata) {
                (*(int*)userdata)++;
            },
        };

        VsyncError ret = helper->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(helper->callbacks_.size(), 1u);

        ret = helper->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(helper->callbacks_.size(), 2u);

        ret = helper->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(helper->callbacks_.size(), 3u);

        ASSERT_EQ(num, 0);
        handler->PostTask([&]() {
            ASSERT_EQ(num, 3);

            handler->PostTask([&]() { runner->Stop(); });
        });
    });
}

HWTEST_F(VsyncHelperTest, EventHandler, testing::ext::TestSize.Level0)
{
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    handler->PostTask([&]() {
        static int num = 0;
        struct FrameCallback cb = {
            .timestamp_ = 0,
            .userdata_ = &num,
            .callback_ = [](int64_t timestamp, void* userdata) {
                (*(int*)userdata)++;
            },
        };

        VsyncError ret = GlobalData::GetInstance()->helper_->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(GlobalData::GetInstance()->helper_->callbacks_.size(), 1u);

        ASSERT_EQ(num, 0);

        handler->PostTask([&]() {
            ASSERT_EQ(num, 1);

            VsyncError ret = GlobalData::GetInstance()->helper_->RequestFrameCallback(cb);
            ASSERT_EQ(ret, VSYNC_ERROR_OK);
            ASSERT_EQ(GlobalData::GetInstance()->helper_->callbacks_.size(), 1u);
            ASSERT_EQ(num, 1);

            handler->PostTask([&]() {
                ASSERT_EQ(num, 2);
            });
        });
    });
}

HWTEST_F(VsyncHelperTest, CurrentWithNullptr, testing::ext::TestSize.Level0)
{
    auto runner = AppExecFwk::EventRunner::Current();
    ASSERT_EQ(runner, nullptr);

    auto helper = VsyncHelper::Current();
    ASSERT_EQ(helper, nullptr);
}

HWTEST_F(VsyncHelperTest, Current, testing::ext::TestSize.Level0)
{
    int num = 0;
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask([&handler, &num]() {
        auto runner = AppExecFwk::EventRunner::Current();
        ASSERT_NE(runner, nullptr);

        auto helper = TestVsyncHelper::Current();
        ASSERT_NE(helper, nullptr);

        struct FrameCallback cb = {
            .timestamp_ = 0,
            .userdata_ = &num,
            .callback_ = [](int64_t timestamp, void* userdata) {
                (*(int*)userdata)++;
            },
        };

        VsyncError ret = helper->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(helper->callbacks_.size(), 1u);

        ret = helper->RequestFrameCallback(cb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ASSERT_EQ(helper->callbacks_.size(), 2u);

        ASSERT_EQ(num, 0);

        handler->PostTask([&num](){
            ASSERT_EQ(num, 2);
        }, MICROSECOND_SWITCH);
    });

    SWITCH();
}
} // namespace
