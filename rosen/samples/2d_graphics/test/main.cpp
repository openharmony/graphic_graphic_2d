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

#include <iostream>
#include <string>
#include <unordered_map>

#include "drawing_sample.h"

#include "bitmap_test.h"
#include "camera_test.h"
#include "filter_test.h"
#include "image_test.h"
#include "matrix_test.h"
#include "path_effect_test.h"
#include "path_test.h"
#include "shader_test.h"
#include "shadow_test.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;
using namespace std;

using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;

static void TestHello()
{
    std::cout << "Drawing module test start.\n";
    std::cout << "If you want to get more information, \n";
    std::cout << "please type 'hilog | grep Drawing' in another hdc shell window\n";
    std::cout << "-------------------------------------------------------\n";
}

int main()
{
    TestHello();
    std::vector<TestFunc> testFuncVec;
    std::map<string, std::vector<TestFunc>> testFuncMap;

    testFuncMap.insert(pair<string, std::vector<TestFunc>>("BitmapTest", BitmapTest::GetInstance().BitmapTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("CameraTest", CameraTest::GetInstance().CameraTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("FilterTest", FilterTest::GetInstance().FilterTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("ImageTest", ImageTest::GetInstance().ImageTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("MatrixTest", MatrixTest::GetInstance().MatrixTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("PathTest", PathTest::GetInstance().PathTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("ShaderTest", ShaderTest::GetInstance().ShaderTestCase()));
    testFuncMap.insert(pair<string, std::vector<TestFunc>>("ShadowTest", ShadowTest::GetInstance().ShadowTestCase()));

    DrawingSample m;
    m.SetDraw(testFuncMap).Run();
    std::cout << "Drawing module test end.\n";
    return 0;
}