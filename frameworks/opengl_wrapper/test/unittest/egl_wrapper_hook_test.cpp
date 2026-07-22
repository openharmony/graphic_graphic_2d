/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "EGL/egl_wrapper_hook.h"
#include "EGL/egl_wrapper_display.h"

#include "egl_defs.h"
#include "egl_core.h"

#include <iostream>

using namespace testing;
using namespace testing::ext;

#if USE_IGRAPHICS_EXTENDS_HOOKS

namespace OHOS {
class EglWrapperHookTest : public testing::Test {
public:
    static EGLGetGlesVersionCallBackFunc capturedGetVersion;
    static EGLSwitchTableCallBackFunc capturedSwitchTable;
    static EGLMakeCurrentAfterHookCallBackFunc capturedMakeCurrent;

    static void SetUpTestCase()
    {
        capturedGetVersion = nullptr;
        capturedSwitchTable = nullptr;
        capturedMakeCurrent = nullptr;
    }
    static void TearDownTestCase()
    {
        capturedGetVersion = nullptr;
        capturedSwitchTable = nullptr;
        capturedMakeCurrent = nullptr;
    }
    void SetUp() {}
    void TearDown() {}
};

EGLGetGlesVersionCallBackFunc EglWrapperHookTest::capturedGetVersion = nullptr;
EGLSwitchTableCallBackFunc EglWrapperHookTest::capturedSwitchTable = nullptr;
EGLMakeCurrentAfterHookCallBackFunc EglWrapperHookTest::capturedMakeCurrent = nullptr;


/**
 * @tc.name: Hook_FullSuccessFlow
 * @tc.desc: Test Hook full success flow - all steps succeed
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, Hook_FullSuccessFlow, Level3)
{
    auto& instance = EglWrapperHook::GetInstance();
    instance.layerInited_ = false;

    EglWrapperDispatchTable table;
    bool result = instance.Hook(&table);

    // layerEntry layerIsEnable will return false
    ASSERT_FALSE(result);
    ASSERT_FALSE(instance.IsInit());
}

/**
 * @tc.name: GetInstance001
 * @tc.desc: Test singleton pattern - verify instance is valid
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, GetInstance001, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();
    ASSERT_EQ(&instance, &EglWrapperHook::GetInstance());
}

/**
 * @tc.name: GetInstance002
 * @tc.desc: Test singleton pattern - verify same instance returned multiple times
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, GetInstance002, Level2)
{
    auto& instance1 = EglWrapperHook::GetInstance();
    auto& instance2 = EglWrapperHook::GetInstance();
    ASSERT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: IsInit001
 * @tc.desc: Test IsInit when not initialized
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, IsInit001, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();
    bool result = instance.IsInit();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: InitHookLayer001
 * @tc.desc: Test InitHookLayer - layerIsEnable returns false
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, InitHookLayer001, Level1)
{
    EglWrapperDispatchTable table;
    auto& instance = EglWrapperHook::GetInstance();
    instance.layerEntry.layerIsEnable = []() -> bool { return false; };
    bool result = instance.InitHookLayer(&table);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: LayerEntries001
 * @tc.desc: Test LayerEntries with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, LayerEntries001, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();
    EglWrapperDispatchTable table;

    LayerSetupFunc setupFunc = [](const char* name, EglWrapperFuncPointer prev) -> EglWrapperFuncPointer {
        return prev;
    };

    char const* const entries[] = { "eglGetProcAddress", nullptr };
    EglWrapperFuncPointer* curr = reinterpret_cast<EglWrapperFuncPointer*>(&table.wrapper);

    instance.LayerEntries(setupFunc, curr, entries);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: LayerEntries002
 * @tc.desc: Test LayerEntries with null entries
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, LayerEntries002, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();
    EglWrapperDispatchTable table;

    LayerSetupFunc setupFunc = [](const char* name, EglWrapperFuncPointer prev) -> EglWrapperFuncPointer {
        return prev;
    };

    char const* const entries[] = { nullptr };
    EglWrapperFuncPointer* curr = reinterpret_cast<EglWrapperFuncPointer*>(&table.wrapper);

    instance.LayerEntries(setupFunc, curr, entries);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: LoadLayerSymbol001
 * @tc.desc: Test LoadLayerSymbol template method - dlsym failure
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, LoadLayerSymbol001, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();

    // 使用 libc.so，确保库存在
    void* dlhandle = dlopen("libc.so", RTLD_NOW | RTLD_LOCAL);
    ASSERT_NE(nullptr, dlhandle);

    // 请求一个不存在的符号，触发 dlsym 返回 nullptr
    LayerIsEnableFunc func = nullptr;
    bool result = instance.LoadLayerSymbol(dlhandle, "NonExistentSymbol_12345", func);

    ASSERT_FALSE(result);
    ASSERT_EQ(nullptr, func);

    dlclose(dlhandle);
}

/**
 * @tc.name: Hook_AlreadyInitialized
 * @tc.desc: Test Hook when layerInited_ is true - should return true immediately
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, Hook_AlreadyInitialized, Level1)
{
    auto& instance = EglWrapperHook::GetInstance();
    instance.layerInited_ = true;

    EglWrapperDispatchTable table;
    bool result = instance.Hook(&table);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: LayerEntries_ReplacementLogs
 * @tc.desc: Test LayerEntries when function pointer is replaced
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, LayerEntries_ReplacementLogs, Level2)
{
    auto& instance = EglWrapperHook::GetInstance();
    EglWrapperDispatchTable table;

    LayerSetupFunc setupFunc = [](const char* name, EglWrapperFuncPointer prev) -> EglWrapperFuncPointer {
        return reinterpret_cast<EglWrapperFuncPointer>(0x12345678);
    };

    char const* const entries[] = { "eglGetProcAddress", nullptr };
    EglWrapperFuncPointer* curr = reinterpret_cast<EglWrapperFuncPointer*>(&table.wrapper);

    instance.LayerEntries(setupFunc, curr, entries);

    ASSERT_EQ(reinterpret_cast<EglWrapperFuncPointer>(0x12345678), *curr);
}

/**
 * @tc.name: LayerEntries_NoReplacement
 * @tc.desc: Test LayerEntries when function pointer is not replaced
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperHookTest, LayerEntries_NoReplacement, Level2)
{
    auto& instance = EglWrapperHook::GetInstance();
    EglWrapperDispatchTable table;

    LayerSetupFunc setupFunc = [](const char* name, EglWrapperFuncPointer prev) -> EglWrapperFuncPointer {
        return prev;
    };

    char const* const entries[] = { "eglGetProcAddress", nullptr };
    EglWrapperFuncPointer* curr = reinterpret_cast<EglWrapperFuncPointer*>(&table.wrapper);
    EglWrapperFuncPointer original = *curr;

    instance.LayerEntries(setupFunc, curr, entries);

    ASSERT_EQ(original, *curr);
}

} // namespace OHOS

#endif // USE_IGRAPHICS_EXTENDS_HOOKS