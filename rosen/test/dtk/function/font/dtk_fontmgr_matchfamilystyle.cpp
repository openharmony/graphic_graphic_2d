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
#include "rosen/test/dtk/dtk_test_ext.h"
#include "rosen/modules/2d_graphics/include/text/font.h"
#include "rosen/modules/2d_graphics/include/recording/mem_allocator.h"
#include "rosen/modules/2d_graphics/include/text/font_mgr.h"
#include "rosen/modules/2d_graphics/include/text/font_style_set.h"
#include "rosen/modules/2d_graphics/include/text/rs_xform.h"
#include "rosen/modules/2d_graphics/include/utils/point.h"
#include "rosen/modules/2d_graphics/include/utils/rect.h"
#include "rosen/modules/2d_graphics/include/text/typeface.h"

/*
测试类：FontMgr
测试接口：MatchFamilyStyle
测试内容：对接口入参familyName和fontStyle取组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/
namespace OHOS {
namespace Rosen {

void CommonMatchfamilystyle(TestPlaybackCanvas* playbackCanvas,
                            std::shared_ptr<Drawing::FontMgr> fontMgr,
                            bool bSlant,
                            std::string familyName)
{
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    if (bSlant) {
        fontStyle = Drawing::FontStyle{fontStyle.GetWeight(), fontStyle.GetWidth(), Drawing::FontStyle::OBLIQUE_SLANT};
    }
    if (familyName == "get") {
        fontMgr->GetFamilyName(0, familyName);
    }
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));

    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = {text1, text2, text3};
    int line = 200;
    int interval2 = 200;
    for (auto text :texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        line += interval2;
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        line += interval2;
        playbackCanvas->DetachPen();
    }
}

//对应用例 FontMgr_MatchFamilyStyle_3001
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 1)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "abcd");
}

//对应用例 FontMgr_MatchFamilyStyle_3002
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 2)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "HMOS Color Emoji");
}

//对应用例 FontMgr_MatchFamilyStyle_3003
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 3)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "get");
}

//对应用例 FontMgr_MatchFamilyStyle_3004
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 4)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "get");
}

//对应用例 FontMgr_MatchFamilyStyle_3005
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 5)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "abcd");
}

//对应用例 FontMgr_MatchFamilyStyle_3006
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 6)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "abcd");
}

//对应用例 FontMgr_MatchFamilyStyle_3007
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 7)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "get");
}

//对应用例 FontMgr_MatchFamilyStyle_3008
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 8)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "HMOS Color Emoji");
}

//对应用例 FontMgr_MatchFamilyStyle_3009
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 9)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "");
}

//对应用例 FontMgr_MatchFamilyStyle_3010
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 10)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "HMOS Color Emoji");
}

//对应用例 FontMgr_MatchFamilyStyle_3011
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 11)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "");
}

//对应用例 FontMgr_MatchFamilyStyle_3012
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 12)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "get");
}

//对应用例 FontMgr_MatchFamilyStyle_3013
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 13)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "");
}

//对应用例 FontMgr_MatchFamilyStyle_3014
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 14)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "abcd");
}

//对应用例 FontMgr_MatchFamilyStyle_3015
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 15)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, true, "HMOS Color Emoji");
}

//对应用例 FontMgr_MatchFamilyStyle_3016
DEF_DTK(fontmgr_matchfamilystyle, TestLevel::L1, 16)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    CommonMatchfamilystyle(playbackCanvas_, fontMgr, false, "");
}

}
}
