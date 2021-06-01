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

#include "vsync_module_test.h"

#include <gtest/gtest.h>
#include <system_ability_definition.h>

#include "vsync_module_impl.h"

using namespace OHOS;

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

class VsyncModuleTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }

    void SetUp()
    {
        vm = TestVsyncModule::GetInstance();
    }

private:
    VsyncModuleImpl* vm;
};

namespace {
HWTEST_F(VsyncModuleTest, Start, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(vm->drmFd_, -1);
    ASSERT_EQ(vm->vsyncThread_, nullptr);
    ASSERT_FALSE(vm->vsyncThreadRunning_);

    ASSERT_EQ(vm->Start(), VSYNC_ERROR_OK);

    ASSERT_GT(vm->drmFd_, -1);
    ASSERT_NE(vm->vsyncThread_, nullptr);
    ASSERT_TRUE(vm->vsyncThreadRunning_);
}

HWTEST_F(VsyncModuleTest, Stop, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(vm->Stop(), VSYNC_ERROR_OK);

    ASSERT_EQ(vm->drmFd_, -1);
    ASSERT_EQ(vm->vsyncThread_, nullptr);
    ASSERT_FALSE(vm->vsyncThreadRunning_);

    ASSERT_NE(vm->Stop(), VSYNC_ERROR_OK);
}
} // namespace
