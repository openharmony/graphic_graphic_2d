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
    const int screenWidth = 1260;
    const int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        system("setenforce 0");
    }
};

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_start.ohr", 1.065f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_start.ohr", 1.21f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_start.ohr", 2.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_dianji.ohr", 1.28f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_dianji.ohr", 2.8f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiDianJiTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_dianji.ohr", 3.7f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_huadong.ohr", 1.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_huadong.ohr", 2.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, shezhiHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/shezhi_huadong.ohr", 3.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_dianji.ohr", 0.7f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_dianji.ohr", 1.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinDianJiTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_dianji.ohr", 1.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_huadong.ohr", 0.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_huadong.ohr", 1.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweshipinHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweishipin_huadong.ohr", 2.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, beiwangluStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/beiwanglu_start.ohr", 0.6f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, beiwangluStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/beiwanglu_start.ohr", 0.75f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, beiwangluStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/beiwanglu_start.ohr", 1.3f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, changlianStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/changlian_start.ohr", 1.458f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, changlianStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/changlian_start.ohr", 1.550f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, changlianStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/changlian_start.ohr", 2.13f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dianziyoujianStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/dianziyoujian_start.ohr", 1.2f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dianziyoujianStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/dianziyoujian_start.ohr", 1.56f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dianziyoujianStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/dianziyoujian_start.ohr", 1.8f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, lianxirenStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/lianxiren_start.ohr", 0.7f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, lianxirenStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/lianxiren_start.ohr", 0.942f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, lianxirenStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/lianxiren_start.ohr", 1.20f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dituStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/ditu_start.ohr", 0.8f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dituStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/ditu_start.ohr", 1.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, dituStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/ditu_start.ohr", 4.2f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, jisuanqiStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/jisuanqi_start.ohr", 0.6f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, jisuanqiStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/jisuanqi_start.ohr", 0.853f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, jisuanqiStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/jisuanqi_start.ohr", 1.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, qianbaoStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/qianbao_start.ohr", 1.315f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, qianbaoStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/qianbao_start.ohr", 1.93f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, qianbaoStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/qianbao_start.ohr", 2.35f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_dianji.ohr", 0.58f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_dianji.ohr", 0.85f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinDianJiTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_dianji.ohr", 1.8f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_huadong.ohr", 1.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_huadong.ohr", 1.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, youxizhongxinHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/youxizhongxin_huadong.ohr", 2.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhuomianDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuomian_dianji.ohr", 1.196f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhuomianDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuomian_dianji.ohr", 1.3f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhuomianDianJiTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuomian_dianji.ohr", 1.42f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhuomianDianJiTest04)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuomian_dianji.ohr", 2.6f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhuomianDianJiTest05)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuomian_dianji.ohr", 2.8f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_dianji.ohr", 0.39f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_dianji.ohr", 1.428f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiDianJiTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_dianji.ohr", 2.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_huadong.ohr", 0.35f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_huadong.ohr", 0.785f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_huadong.ohr", 1.45f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, zhutiHuaDongTest04)
{
    PlaybackRecover("/data/local/tmp/Playback/zhuti_huadong.ohr", 2.3f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, tongzhizhongxinStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/tongzhizhongxin_start.ohr", 0.75f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, tongzhizhongxinStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/tongzhizhongxin_start.ohr", 2.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, tongzhizhongxinStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/tongzhizhongxin_start.ohr", 2.2f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, kongzhizhongxinStartTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/kongzhizhongxin_start.ohr", 1.4f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, kongzhizhongxinStartTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/kongzhizhongxin_start.ohr", 1.5f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, kongzhizhongxinStartTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/kongzhizhongxin_start.ohr", 1.65f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoDianJiTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_dianji.ohr", 0.125f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoDianJiTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_dianji.ohr", 1.1f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_huadong.ohr", 1.43f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_huadong.ohr", 1.47f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_huadong.ohr", 1.6f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoHuaDongTest04)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_huadong.ohr", 3.85f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, haluoHuaDongTest05)
{
    PlaybackRecover("/data/local/tmp/Playback/haluo_huadong.ohr", 4.0f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweiyueduHuaDongTest01)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweiyuedu_huadong.ohr", 0.322f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweiyueduHuaDongTest02)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweiyuedu_huadong.ohr", 0.54f);
}

GRAPHIC_D_TEST(PlaybackRecoverTest, CONTENT_DISPLAY_TEST, huaweiyueduHuaDongTest03)
{
    PlaybackRecover("/data/local/tmp/Playback/huaweiyuedu_huadong.ohr", 1.0f);
}
} // namespace OHOS::Rosen