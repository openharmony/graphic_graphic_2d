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
    dispatchTable.isLoad = true;

    auto result = EglWrapperLoader::GetInstance().Load(&dispatchTable);
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
    std::string eglPath = std::string("/system/lib/") + std::string("libEGL.so");
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
    std::string eglPath = std::string("/system/lib/") + std::string("libc.so");
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
    std::string glPath = std::string("/system/lib/") + std::string("libGLESv1.so");
    auto result = EglWrapperLoader::GetInstance().LoadGl(glPath.c_str(),
        gGlApiNames1, (FunctionPointerType *)&dispatchTable.gl.table1);

    ASSERT_NE(nullptr, result);
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
} // OHOS::Rosen