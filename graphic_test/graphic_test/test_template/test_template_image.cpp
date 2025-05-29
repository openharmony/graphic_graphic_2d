/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_text.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestTemplateImageTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    // called after each tests
    void AfterEach() override {}

private:
    const int screenWidth = 600;
    const int screenHeight = 1000;
};

/*
 * @tc.name: test1
 * @tc.desc: GRAPHIC_TEST with derived test class, will take surface capture
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TestTemplateImageTest, CONTENT_DISPLAY_TEST, test1)
{
    uint8_t pngData[] = {
        #include ""
    };

    uint32_t pngsize = sizeof(pngData) / sizeof(uint8_t);
    auto testNodeBgImg = SetUpNodeBgImage(pngData, pngsize, { 0, 0, screenWidth, screenHeight });
    GetRootNode()->AddChild(testNodeBgImg);
    RegisterNode(testNodeBgImg);
}

}