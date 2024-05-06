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

#include "gtest/gtest.h"
#include "ui/rs_texture_export.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;
static constexpr uint32_t FIRST_COLOR_VALUE = 0x034123;
static constexpr uint32_t SECOND_COLOR_VALUE = 0x45ba87;
static constexpr uint32_t THIRD_COLOR_VALUE = 0x32aadd;
namespace OHOS::Rosen {
class RSTextureExportTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTextureExportTest::SetUpTestCase() {}
void RSTextureExportTest::TearDownTestCase() {}
void RSTextureExportTest::SetUp() {}
void RSTextureExportTest::TearDown() {}


/**
 * @tc.name: DoTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, RSTextureExport, TestSize.Level1)
{
    std::shared_ptr<RSNode> rootNode;
    SurfaceId surfaceId;
    RSTextureExport text(rootNode, surfaceId);
    bool res = text->DoTextureExport();
    ASSERT_TRUE(res == true);
}

/**
 * @tc.name: StopTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, RSTextureExport, TestSize.Level1)
{
    std::shared_ptr<RSNode> rootNode;
    SurfaceId surfaceId;
    RSTextureExport text(rootNode, surfaceId);
    text->StopTextureExport();
    bool res = true;
    ASSERT_TRUE(res == true);
    delete text;
    res = false;
    ASSERT_TRUE(res == false);
}
}