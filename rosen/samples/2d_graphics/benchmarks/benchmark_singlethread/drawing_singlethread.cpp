/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "drawing_singlethread.h"

namespace OHOS {
namespace Rosen {
void DrawingSinglethread::Start()
{
    std::cout << "DrawingSinglethread::Start+" << std::endl;
    std::cout << "DrawingSinglethread::Start-" << std::endl;
}

void DrawingSinglethread::Stop()
{
    std::cout << "DrawingSinglethread::Stop+" << std::endl;
    std::cout << "DrawingSinglethread::Stop-" << std::endl;
}

void DrawingSinglethread::Test(Drawing::Canvas* canvas, int width, int height)
{
    std::cout << "DrawingSinglethread::Test bypass+" << std::endl;

    std::cout << "DrawingSinglethread::Test bypass-" << std::endl;
}

void DrawingSinglethread::Output()
{
}
}
}