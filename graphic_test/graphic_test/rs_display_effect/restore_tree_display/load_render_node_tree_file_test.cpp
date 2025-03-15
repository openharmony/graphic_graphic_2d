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

    void LoadRenderNodeTreeFile(const std::string& filePath)
    {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        AddFileRenderNodeTreeToNode(testNode, filePath);
    }
};

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BEI_WANG_LU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_beiwanglu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BEI_WANG_LU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_beiwanglu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CLOCK_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_clock_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CLOCK_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_clock_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CLOCK_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_clock_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CLOCK_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_clock_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CLOCK_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_clock_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DIAN_HUA_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dianhua_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DIAN_HUA_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dianhua_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JI_SUAN_QI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jisuanqi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, LIU_LAN_QI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_liulanqi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, LIU_LAN_QI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_liulanqi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, LIU_LAN_QI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_liulanqi_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, RI_LI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_rili_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, RI_LI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_rili_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, RI_LI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_rili_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, RI_LI_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_rili_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, RI_LI_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_rili_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHI_CHANG_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shichang_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHI_CHANG_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shichang_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHI_CHANG_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shichang_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHI_CHANG_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shichang_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHI_CHANG_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shichang_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TIAN_QI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tianqi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TIAN_QI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tianqi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TIAN_QI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tianqi_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TU_KU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tuku_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TU_KU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tuku_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TU_KU_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tuku_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TU_KU_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tuku_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEN_GUAN_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wenguan_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEN_GUAN_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wenguan_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEN_GUAN_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wenguan_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEN_GUAN_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wenguan_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEN_GUAN_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wenguan_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, XIANG_JI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_xiangji_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, XIN_XI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_xinxi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHI_NAN_ZHEN_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhinanzhen_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHI_NAN_ZHEN_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhinanzhen_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHU_TI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhuti_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHU_TI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhuti_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHU_TI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhuti_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUE_DU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yuedu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUE_DU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yuedu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUE_DU_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yuedu_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUN_DONG_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yundong_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUN_DONG_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yundong_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YIN_YUE_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yinyue_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YIN_YUE_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yinyue_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YIN_YUE_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yinyue_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YIN_YUE_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yinyue_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHANG_CHENG_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shangcheng_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHANG_CHENG_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shangcheng_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHANG_CHENG_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shangcheng_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHANG_CHENG_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shangcheng_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHANG_CHENG_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shangcheng_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_06");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_07");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_08");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_09)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_09");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_10)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_10");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_11)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_11");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_12)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_12");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SHE_ZHI_TEST_13)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_shezhi_13");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_06)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_06");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_07)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_07");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WAN_JI_TEST_08)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wanji_08");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QIAN_BAO_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qianbao_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QIAN_BAO_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qianbao_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QIAN_BAO_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qianbao_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YIN_LIANG_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yinliang_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TONG_ZHI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tongzhi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KONG_ZHI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kongzhi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KONG_ZHI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kongzhi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CHANG_LIAN_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_changlian_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CHANG_LIAN_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_changlian_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CHANG_LIAN_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_changlian_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, CHANG_LIAN_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_changlian_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUN_KONG_JIAN_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yunkongjian_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, YUN_KONG_JIAN_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_yunkongjian_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEI_XIN_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_weixin_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TAO_BAO_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_taobao_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TAO_BAO_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_taobao_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, PING_DUO_DUO_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_pingduoduo_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, PING_DUO_DUO_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_pingduoduo_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BAI_DU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_baidu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BAI_DU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_baidu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, GAO_DE_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_gaode_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, GAO_DE_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_gaode_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, GAO_DE_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_gaode_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, GAO_DE_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_gaode_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, GAO_DE_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_gaode_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQ_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qq_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KUAI_SHOU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kuaishou_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KUAI_SHOU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kuaishou_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JING_DONG_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jingdong_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JING_DONG_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jingdong_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JING_DONG_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jingdong_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JING_DONG_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jingdong_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQ_LIU_LAN_QI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qq_liulanqi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQ_LIU_LAN_QI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qq_liulanqi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TOU_TIAO_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_toutiao_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TOU_TIAO_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_toutiao_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TOU_TIAO_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_toutiao_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TOU_TIAO_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_toutiao_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, SAN_GUO_SHA_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_sanguosha_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, XIAO_HONG_SHU_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_xiaohongshu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TENG_XUN_SHI_PIN_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tengxun_shipin_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, TENG_XUN_SHI_PIN_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_tengxun_shipin_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BAI_DU_DI_TU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_baidu_ditu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BAI_DU_DI_TU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_baidu_ditu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KU_GOU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kugou_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, KU_GOU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_kugou_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEI_BO_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_weibo_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEI_BO_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_weibo_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WEI_BO_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_weibo_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BILI_BILI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_bilibili_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BILI_BILI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_bilibili_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, BILI_BILI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_bilibili_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, AI_QI_YI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_aiqiyi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, AI_QI_YI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_aiqiyi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, AI_QI_YI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_aiqiyi_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, AI_QI_YI_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_aiqiyi_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, WPS_OFFICE_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_wps_office_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JIAN_SHE_BANK_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jianshe_bank_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JIAN_SHE_BANK_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jianshe_bank_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JIAN_SHE_BANK_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jianshe_bank_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JIAN_SHE_BANK_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jianshe_bank_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, JIAN_SHE_BANK_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_jianshe_bank_05");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQ_YIN_YUE_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qq_yinyue_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, QQ_YIN_YUE_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_qq_yinyue_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHI_HU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhihu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, ZHI_HU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_zhihu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DEI_WU_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_deiwu_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DEI_WU_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_deiwu_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DEI_WU_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_deiwu_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DEI_WU_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_deiwu_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DI_DI_CHU_XING_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_didi_chuxing_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DI_DI_CHU_XING_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_didi_chuxing_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DI_DI_CHU_XING_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_didi_chuxing_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, XIAO_XIAO_LE_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_xiaoxiaole_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DONG_CHE_DI_TEST_01)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dongchedi_01");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DONG_CHE_DI_TEST_02)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dongchedi_02");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DONG_CHE_DI_TEST_03)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dongchedi_03");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DONG_CHE_DI_TEST_04)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dongchedi_04");
}

GRAPHIC_TEST(LoadRenderNodeTreeFileTest, CONTENT_DISPLAY_TEST, DONG_CHE_DI_TEST_05)
{
    LoadRenderNodeTreeFile("/data/local/tmp/NodeTree/rs_dongchedi_05");
}
} // namespace OHOS::Rosen