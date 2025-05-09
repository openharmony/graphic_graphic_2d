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

class RestoreNodeTreeFileTest : public RSGraphicTest {
private:
    const int screenWidth = 1216;
    const int screenHeight = 2688;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        system("setenforce 0");
    }

    void LoadRenderNodeTreeFile(const std::string& filePath)
    {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        AddFileRenderNodeTreeToNode(testNode, filePath);
    }
};

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yingyongshichangTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yingyongshichang_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweiyueduTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweiyuedu_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yundongjiankangTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yundongjiankang_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishipinTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishipin_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweimusicTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweimusic_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, huaweishangchengTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/huaweishangcheng_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest016)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_016");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest017)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_017");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest018)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_018");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest019)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_019");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest020)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_020");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, shezhiTest021)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/shezhi_021");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, wanjijiqiaoTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/wanjijiqiao_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qianbaoTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qianbao_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yunkongjianTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yunkongjian_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yunkongjianTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yunkongjian_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yunkongjianTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yunkongjian_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, yunkongjianTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/yunkongjian_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, changlianTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/changlian_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, betaclubTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/betaclub_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest017)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_017");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, youxizhongxinTest018)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/youxizhongxin_018");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meiyanxiangjiTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meiyanxiangji_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, xingtuTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/xingtu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, xingtuTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/xingtu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, xingtuTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/xingtu_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, xingtuTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/xingtu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, haluoTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/haluo_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, haluoTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/haluo_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, haluoTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/haluo_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pingankodaiyinhangTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pingankodaiyinhang_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pingankodaiyinhangTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pingankodaiyinhang_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pingankodaiyinhangTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pingankodaiyinhang_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pingankodaiyinhangTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pingankodaiyinhang_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaiduiTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaidui_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaiduiTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaidui_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaiduiTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaidui_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaiduiTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaidui_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest2)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, pinganhaochezhuTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/pinganhaochezhu_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, jiaotongyinhangTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/jiaotongyinhang_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, jiaotongyinhangTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/jiaotongyinhang_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, jiaotongyinhangTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/jiaotongyinhang_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest018)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_018");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, qichezhijiaTest019)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/qichezhijia_019");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhangshangshenghuoTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhangshangshenghuo_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhangshangshenghuoTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhangshangshenghuo_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhangshangshenghuoTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhangshangshenghuo_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weixinTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weixin_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weixinTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weixin_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weixinTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weixin_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weixinTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weixin_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weixinTest6)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weixin_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhifubaoTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhifubao_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baiduTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baidu_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQ_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest016)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_016");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest017)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_017");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest018)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_018");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest019)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_019");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, gaodedituTest020)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/gaodeditu_020");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kuaishouTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kuaishou_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, JDTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/JD_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, meituanTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/meituan_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQliulangqiTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/QQliulangqi_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, tengxunshipinTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/tengxunshipin_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, baidudituTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/baiduditu_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, kugouTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/kugou_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, weiboTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/weibo_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, aiqiyiTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/aiqiyi_015");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, dingdingTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/dingding_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_03");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, zhongguoyinhangTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/zhongguoyinhang_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_01");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_02");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_04");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_05");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_06");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_07");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_08");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_09");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest010)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_010");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest011)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_011");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest012)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_012");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest013)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_013");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest014)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_014");
}

GRAPHIC_TEST(RestoreNodeTreeFileTest, CONTENT_DISPLAY_TEST, bilibiliTest015)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/bilibili_015");
}
} // namespace OHOS::Rosen