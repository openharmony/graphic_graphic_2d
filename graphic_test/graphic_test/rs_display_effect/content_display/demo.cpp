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

#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class LoadRenderNodeTreeFileTest : public RSGraphicTest {
private:
    const int screenWidth = 1260;
    const int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
        system("setenforce 0");
    }
};

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, Load_RenderNodeTree_File_Test)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    AddFileRenderNodeTreeToNode(testNode, "/data/rssubtree_test_14426795147348");
}

} // namespace OHOS::Rosen