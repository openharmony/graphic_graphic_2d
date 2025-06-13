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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class PlaybackRecoverTest : public RSGraphicTest {
private:
    const int screenWidth = GetScreenSize()[0];
    const int screenHeight = GetScreenSize()[1];

public:
    //called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        system("setenforce 0");
    }
};

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, performanceTest)
{
    PlaybackRecover("/data/local/tmp/Playback/running.ohr", 8.0f);
}
}