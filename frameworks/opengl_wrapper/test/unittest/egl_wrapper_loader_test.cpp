/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "EGL/egl_wrapper_loader.h"

#include "egl_defs.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglWrapperLoaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: GetProcAddrFromDriver001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, GetProcAddrFromDriver001, Level1)
{
    std::string name = std::string("");
    auto result = EglWrapperLoader::GetInstance().GetProcAddrFromDriver(name.c_str());

    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: GetProcAddrFromDriver002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, GetProcAddrFromDriver002, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    EglWrapperLoader::GetInstance().Load(&dispatchTable);

    std::string name = std::string("eglGetProcAddress");
    auto result = EglWrapperLoader::GetInstance().GetProcAddrFromDriver(name.c_str());

    ASSERT_NE(nullptr, result);
}

/**
 * @tc.name: Load001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Load001, Level1)
{
    EglWrapperDispatchTable dispatchTable;

    auto result = EglWrapperLoader::GetInstance().Load(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Load002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Load002, Level2)
{
    auto result = EglWrapperLoader::GetInstance().Load(nullptr);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: Load003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Load003, Level2)
{
    EglWrapperDispatchTable dispatchTable;

    auto result = EglWrapperLoader::GetInstance().Load(&dispatchTable);
    ASSERT_TRUE(result);
    result = EglWrapperLoader::GetInstance().Load(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Unload001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Unload001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    EglWrapperLoader::GetInstance().Load(&dispatchTable);

    auto result = EglWrapperLoader::GetInstance().Unload(&dispatchTable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Unload002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Unload002, Level2)
{
    auto result = EglWrapperLoader::GetInstance().Unload(nullptr);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: Unload003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, Unload003, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    dispatchTable.isLoad = false;

    auto result = EglWrapperLoader::GetInstance().Unload(&dispatchTable);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: LoadEgl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadEgl001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    std::string eglPath = std::string("libEGL_impl.so");
    auto result = EglWrapperLoader::GetInstance().LoadEgl(eglPath.c_str(), &dispatchTable.egl);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: LoadEgl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadEgl002, Level2)
{
    std::string eglPath = std::string("");
    auto result = EglWrapperLoader::GetInstance().LoadEgl(eglPath.c_str(), nullptr);

    ASSERT_EQ(false, result);
}

/**
 * @tc.name: LoadEgl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadEgl003, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    std::string eglPath = std::string("libc.so");
    auto result = EglWrapperLoader::GetInstance().LoadEgl(eglPath.c_str(), &dispatchTable.egl);

    ASSERT_EQ(false, result);
}

/**
 * @tc.name: LoadGl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadGl001, Level1)
{
    EglWrapperDispatchTable dispatchTable;
    std::string eglPath = std::string("hilog.so");
    auto result = EglWrapperLoader::GetInstance().LoadGl(eglPath.c_str(),
        gGlApiNames1, (FunctionPointerType *)&dispatchTable.gl.table1);

    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: LoadGl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadGl002, Level2)
{
    std::string eglPath = std::string("");
    auto result = EglWrapperLoader::GetInstance().LoadGl(eglPath.c_str(), gGlApiNames1, nullptr);

    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: LoadGl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadGl003, Level2)
{
    EglWrapperDispatchTable dispatchTable;
    std::string eglPath = std::string("libEGL_impl.so");
    auto result = EglWrapperLoader::GetInstance().LoadEgl(eglPath.c_str(), &dispatchTable.egl);
    ASSERT_TRUE(result);

    std::string glPath = std::string("libGLESv1_impl.so");
    auto ret = EglWrapperLoader::GetInstance().LoadGl(glPath.c_str(),
        gGlApiNames1, (FunctionPointerType *)&dispatchTable.gl.table1);

    ASSERT_NE(nullptr, ret);
}

#ifdef OPENGL_WRAPPER_ENABLE_GL4
/**
 * @tc.name: LoadGlFromMesa001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadGlFromMesa001, Level1)
{
    gWrapperHook.useMesa = false;
    EglWrapperDispatchTable table;
    std::string eglPath = std::string("libEGL_mesa.so");
    auto loader = EglWrapperLoader::GetInstance();
    loader.LoadEgl(eglPath.c_str(), &table.egl);
    auto result = loader.LoadGlFromMesa(gGlApiNames3, (FunctionPointerType *)&table.gl.table3);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: LoadVendorDriver001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperLoaderTest, LoadVendorDriver001, Level1)
{
    EglWrapperDispatchTable table;
    gWrapperHook.useMesa = true;
    auto result = EglWrapperLoader::GetInstance().LoadVendorDriver(&table);
    ASSERT_TRUE(result);
}
#endif
} // OHOS::Rosen
