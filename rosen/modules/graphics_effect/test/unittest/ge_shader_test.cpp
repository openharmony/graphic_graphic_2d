/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ge_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEShaderTest::SetUpTestCase(void) {}
void GEShaderTest::TearDownTestCase(void) {}

void GEShaderTest::SetUp()
{
}

void GEShaderTest::TearDown() {}


/**
 * @tc.name: GESSGetIntance001
 * @tc.desc: Verify the GetInstance 
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderTest, GESSGetIntance001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderTest GESSGetIntance001 start";

    auto instance = GEShaderStore::GetInstance();
    
    EXPECT_NE(instance, nullptr);
    GTEST_LOG_(INFO) << "GEShaderTest DrawImageEffect001 end";
}

/**
 * @tc.name: GESSGetShader001
 * @tc.desc: Verify the GetShader which single key
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderTest, GESSGetShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader001 start";

    auto instance = GEShaderStore::GetInstance();
    
    auto shader1 = instance->GetShader(Rosen::SHADER_AIBAR);
    EXPECT_NE(shader1, nullptr);

    auto shader2 = instance->GetShader(Rosen::SHADER_GREY);
    EXPECT_NE(shader2, nullptr);

    auto shader3 = instance->GetShader(Rosen::SHADER_BLUR);
    EXPECT_NE(shader3, nullptr);

    auto shader4 = instance->GetShader(Rosen::SHADER_BLURAF);
    EXPECT_NE(shader4, nullptr);
    EXPECT_NE(shader4->GetOptions(), nullptr);

    auto shader5 = instance->GetShader(Rosen::SHADER_MIX);
    EXPECT_NE(shader5, nullptr);

    auto shader6 = instance->GetShader(Rosen::SHADER_SIMPLE);
    EXPECT_NE(shader6, nullptr);

    auto shader7 = instance->GetShader(Rosen::SHADER_MASKBLUR);
    EXPECT_NE(shader7, nullptr);

    auto shader8 = instance->GetShader(Rosen::SHADER_HORIBLUR);
    EXPECT_NE(shader8, nullptr);

    auto shader9 = instance->GetShader(Rosen::SHADER_VERTBLUR);
    EXPECT_NE(shader9, nullptr);

    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader001 end";
}

/**
 * @tc.name: GESSGetShader002
 * @tc.desc: Verify the GetShader which multi shader failed
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderTest, GESSGetShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader002 start";

    auto instance = GEShaderStore::GetInstance();
    
    auto shader = instance->GetShader({Rosen::SHADER_AIBAR, Rosen::SHADER_BLUR});
    
    // expect result shader is nullptr because the shader_blur has reference
    EXPECT_EQ(shader, nullptr);

    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader002 end";
}

/**
 * @tc.name: GESSGetShader003
 * @tc.desc: Verify the GetShader which multi shader success
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderTest, GESSGetShader003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader003 start";

    auto instance = GEShaderStore::GetInstance();
    
    auto shader = instance->GetShader({Rosen::SHADER_BLUR, Rosen::SHADER_AIBAR});
    
    EXPECT_NE(shader, nullptr);

    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader003 end";
}
/**
 * @tc.name: GESSGetShader004
 * @tc.desc: Verify the GetShader which multi shader success
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderTest, GESSGetShader004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader004 start";

    auto instance = GEShaderStore::GetInstance();
    
    auto shader = instance->GetShader({Rosen::SHADER_BLUR, Rosen::SHADER_AIBAR, Rosen::SHADER_GREY});
    
    EXPECT_NE(shader, nullptr);

    GTEST_LOG_(INFO) << "GEShaderTest GESSGetShader004 end";
}
} // namespace GraphicsEffectEngine
} // namespace OHOS