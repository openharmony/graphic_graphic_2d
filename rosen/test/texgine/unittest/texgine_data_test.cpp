/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#include "texgine_data.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    std::shared_ptr<RSData> skData_ = std::make_shared<RSData>();
} g_tdMockvars;

void InitTdMockVars(struct MockVars &&vars)
{
    g_tdMockvars = std::move(vars);
}


namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineDataTest : public testing::Test {
};

/**
 * @tc.name:MakeFromFileName
 * @tc.desc: Verify the MakeFromFileName
 * @tc.type:FUNC
 */
HWTEST_F(TexgineDataTest, MakeFromFileName, TestSize.Level1)
{
    EXPECT_NO_THROW({
        InitTdMockVars({});
        std::string str = "";
        auto td = TexgineData::MakeFromFileName(str);
        EXPECT_EQ(td->GetData(), g_tdMockvars.skData_);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
