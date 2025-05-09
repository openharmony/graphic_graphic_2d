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
#include "rs_graphic_test_director.h"
#include "rs_parameter_parse.h"
#include "rs_graphic_test_ext.h"

#include <iostream>
#include <string>

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr size_t ARGS_ONE = 1;
constexpr size_t ARGS_TWO = 2;
constexpr size_t ARGS_THREE = 3;
constexpr size_t ARGS_FOUR = 4;

typedef int (*ProcFunc)(int argc, char **argv);
typedef struct {
    string oid;
    ProcFunc procFunc;
} GraphicTestCommandTb;

static void SplitString(const string& str, vector<string>& vec, const string& pattern)
{
    string::size_type pos1 = 0;
    string::size_type pos2 = str.find(pattern);
    while (pos2 != string::npos) {
        vec.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + pattern.size();
        pos2 = str.find(pattern, pos1);
    }

    if (pos1 != str.length()) {
        vec.push_back(str.substr(pos1));
    }
}

static void DisplayCaseLayer(vector<string>& curlayerInfo, vector<string>& layerInfo)
{
    for (uint32_t loop = 0; loop < curlayerInfo.size(); loop++) {
        if (loop >= layerInfo.size()) {
            layerInfo.push_back(curlayerInfo[loop]);
        } else if (curlayerInfo[loop] == layerInfo[loop]) {
            continue;
        } else {
            layerInfo[loop] = curlayerInfo[loop];
        }

        string out {};
        for (uint32_t idx = 0; idx < loop; idx++) {
            out.append("|  ");
        }
        out.append("|--").append(curlayerInfo[loop]);
        cout << out << endl;
    }
}

static int DisplayAllCaseInfo(int argc, char **argv)
{
    vector<const TestDefInfo*> info = ::OHOS::Rosen::TestDefManager::Instance().GetAllTestInfos();
    vector<string> layerInfo {};
    vector<string> curlayerInfo {};
    string findPath = "graphic_test";
    if (argc == ARGS_THREE) {
        findPath = string(argv[ARGS_TWO]);
    }
    cout << findPath << endl;
    findPath.append("/");

    for (uint32_t loop = 0; loop < info.size(); loop++) {
        string filePath = info[loop]->filePath;
        if (filePath.find(findPath) == string::npos) {
            continue;
        }

        size_t startPos = filePath.find(findPath) + findPath.length();
        if (filePath.rfind("/") > startPos) {
            string subPath = filePath.substr(startPos, filePath.rfind("/") - startPos);
            SplitString(subPath, curlayerInfo, "/");
        }

        curlayerInfo.push_back(info[loop]->testCaseName);
        DisplayCaseLayer(curlayerInfo, layerInfo);

        string out {};
        for (uint32_t idx = 0; idx < curlayerInfo.size() - 1; idx++) {
            out.append("|  ");
        }

        out.append("   |--").append(info[loop]->testName);
        cout << out << endl;
        curlayerInfo.clear();
    }
    return 0;
}

static int FilterTestUnit(int argc, char **argv)
{
    string unitName;
    string caseName = "*";
    switch (argc) {
        case ARGS_THREE:
            unitName = argv[ARGS_TWO];
            break;
        case ARGS_FOUR:
            unitName = argv[ARGS_TWO];
            caseName = argv[ARGS_THREE];
            RSGraphicTestDirector::Instance().SetSingleTest(true);
            break;
        default:
            cout << "format fail [-unit unitName [caseName]]" << endl;
            return 0;
    }

    int argcTemp = ARGS_TWO;
    string filter = "--gtest_filter=";
    filter.append(unitName).append(".").append(caseName);
    argv[ARGS_ONE] = filter.data();
    RSGraphicTestDirector::Instance().Run();
    testing::InitGoogleTest(&argcTemp, argv);
    return RUN_ALL_TESTS();
}

static int RunAllTest(int argc, char **argv)
{
    int argcTemp = ARGS_ONE;
    RSGraphicTestDirector::Instance().Run();
    testing::GTEST_FLAG(output) = "xml:./";
    testing::InitGoogleTest(&argcTemp, argv);
    return RUN_ALL_TESTS();
}

int main(int argc, char **argv)
{
    GraphicTestCommandTb funcTbl[] = {
        { "-list", DisplayAllCaseInfo },
        { "-unit", FilterTestUnit },
        { "-all", RunAllTest }
    };

    if (argc >= ARGS_TWO) {
        size_t tblCnt = sizeof(funcTbl) / sizeof(funcTbl[0]);
        for (uint32_t paraNo = 0; paraNo < tblCnt; paraNo++) {
            if (funcTbl[paraNo].oid == string(argv[ARGS_ONE])) {
                return funcTbl[paraNo].procFunc(argc, argv);
            }
        }
    }

    RSParameterParse::Instance().SetSkipCapture(true);
    RSGraphicTestDirector::Instance().Run();
    testing::GTEST_FLAG(output) = "xml:./";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
