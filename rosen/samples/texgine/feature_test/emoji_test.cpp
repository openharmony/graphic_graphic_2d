/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "text_define.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
#define FAMILY1 WOMAN SKIN1 ZWJ RED_HEART ZWJ MAN SKIN0
#define FAMILY2 PERSON SKIN4 ZWJ RED_HEART ZWJ MAN ZWJ PERSON SKIN2
#define ROLE1 PERSON SKIN3 ZWJ AIRPLANE
#define ROLE2 PERSON SKIN1 ZWJ LAPTOP
#define GENERED1 BOUNCING_BALL SKIN2 ZWJ WOMAN_SPORT_VARIANT EMOJI_VARIANT
#define GENERED2 LIFTING_WEIGHTS SKIN0 ZWJ MAN_SPORT_VARIANT EMOJI_VARIANT
#define HAIR1 MAN SKIN2 ZWJ HAIR_BALD
#define HAIR2 PERSON SKIN4 ZWJ HAIR_CURLY
#define OTHER1 RED_HEART ZWJ "\U0001F525"
#define OTHER2 FLAG EMOJI_VARIANT ZWJ TRANSGENDER EMOJI_VARIANT

struct Emoji {
    TextStyle style = { .fontSize_ = 32 };
    std::string emoji;
    std::string title;
} g_emojis[] = {
    {
        .emoji = WATCH,
        .title = "watch",
    },
    {
        .emoji = FAST_FORWARD_BUTTON,
        .title = "fast forward button",
    },
    {
        .emoji = ALART_CLOCK,
        .title = "alart clock",
    },
    {
        .emoji = UMBRELLA,
        .title = "umbrella",
    },
    {
        .emoji = RED_CIRCLE,
        .title = "red circle",
    },
    {
        .emoji = BLACK_LARGE_SQUARE,
        .title = "black large square",
    },
    {
        .emoji = CURLY_LOOP,
        .title = "curly loop",
    },
    {
        .emoji = PLUS,
        .title = "plus",
    },
    {
        .emoji = SNOWMAN,
        .title = "snowman",
    },
    {
        .emoji = VOLTAGE,
        .title = "voltage",
    },
    {
        .emoji = COPYRIGHT,
        .title = "copyright",
    },
    {
        .emoji = REGISTERED,
        .title = "registered",
    },
    {
        .emoji = DOUBLE_EXCLAMATION_MARK,
        .title = "double exclamation mark",
    },
    {
        .emoji = EXCLAMATION_QUESTION_MARK,
        .title = "exclamation question mark",
    },
    {
        .emoji = WAVY_DASH,
        .title = "wavy dash",
    },
    {
        .emoji = LEFT_ARROW,
        .title = "left arrow",
    },
    {
        .emoji = RED_HEART,
        .title = "red heart",
    },
    {
        .emoji = PENCIL,
        .title = "pencil",
    },
    {
        .emoji = AIRPLANE,
        .title = "airplan",
    },
    {
        .emoji = ATOM,
        .title = "atom",
    },
    {
        .emoji = RED_DRAGON,
        .title = "red dragon",
    },
    {
        .emoji = JOKER,
        .title = "joker",
    },
    {
        .emoji = CYCLONE,
        .title = "cyclone",
    },
    {
        .emoji = NEW_MOON_FACE,
        .title = "new moon face",
    },
    {
        .emoji = CAT,
        .title = "cat",
    },
    {
        .emoji = DOG,
        .title = "dog",
    },
    {
        .emoji = BOY,
        .title = "boy",
    },
    {
        .emoji_ = POST_BOX,
        .title_ = "postbox",
    },
    {
        .emoji_ = GRINNING_FACE,
        .title_ = "grinning face",
    },
    {
        .emoji_ = ROCKET,
        .title_ = "rocket",
    },
    {
        .emoji_ = CLOUD_WITH_RAIN,
        .title_ = "cloud with rain",
    },
    {
        .emoji_ = TORNADO,
        .title_ = "tornado",
    },
    {
        .emoji_ = FOG,
        .title_ = "fog",
    },
    {
        .emoji_ = PERSON_GOLFING,
        .title_ = "person golfing",
    },
    {
        .emoji_ = MOTORCYCLE,
        .title_ = "motorcycle",
    },
    {
        .emoji_ = CANDLE,
        .title_ = "candle",
    },
    {
        .emoji_ = SPIDER,
        .title_ = "spider",
    },
    {
        .emoji_ = BED,
        .title_ = "bed",
    },
    {
        .emoji_ = WORLD_MAP,
        .title_ = "world map",
    },
    {
        .emoji_ = SPEAKING_HEAD,
        .title_ = "speaking head",
    },
    {
        .emoji_ = SHARP,
        .title_ = "sharp",
    },
    {
        .emoji_ = ASTERISK,
        .title_ = "asterisk",
    },
    {
        .emoji_ = ZERO,
        .title_ = "zero",
    },
    {
        .emoji_ = ONE,
        .title_ = "one",
    },
    {
        .emoji_ = TWO,
        .title_ = "two",
    },
    {
        .emoji_ = THREE,
        .title_ = "three",
    },
    {
        .emoji_ = FOUR,
        .title_ = "four",
    },
    {
        .emoji_ = FIVE,
        .title_ = "five",
    },
    {
        .emoji_ = SIX,
        .title_ = "six",
    },
    {
        .emoji_ = SEVEN,
        .title_ = "seven"
    },
    {
        .emoji_ = CHINA,
        .title_ = "China",
    },
    {
        .emoji_ = AUSTRALIA,
        .title_ = "Australia",
    },
    {
        .emoji_ = CANADA,
        .title_ = "Canada",
    },
    {
        .emoji_ = FRANCE,
        .title_ = "France",
    },
    {
        .emoji_ = IRELAND,
        .title_ = "Ireland",
    },
    {
        .emoji_ = INDIA,
        .title_ = "India",
    },
    {
        .emoji_ = ICELAND,
        .title_ = "Iceland",
    },
    {
        .emoji_ = ITALY,
        .title_ = "Italy",
    },
    {
        .emoji_ = JAPAN,
        .title_ = "Japan",
    },
    {
        .emoji_ = RUSSIA,
        .title_ = "Russia",
    },
    {
        .emoji_ = BOY0,
        .title_ = "boy ligth",
    },
    {
        .emoji_ = BOY1,
        .title_ = "boy medium light",
    },
    {
        .emoji_ = BOY2,
        .title_ = "boy medium skin",
    },
    {
        .emoji_ = BOY3,
        .title_ = "boy medium dark",
    },
    {
        .emoji_ = BOY4,
        .title_ = "boy dark",
    },
    {
        .emoji_ = GIRL0,
        .title_ = "girl ligth",
    },
    {
        .emoji_ = GIRL1,
        .title_ = "girl medium light",
    },
    {
        .emoji_ = GIRL2,
        .title_ = "girl medium skin",
    },
    {
        .emoji_ = GIRL3,
        .title_ = "girl medium dark",
    },
    {
        .emoji_ = GIRL4,
        .title_ = "girl dark",
    },
    {
        .emoji_ = FAMILY1,
        .title_ = "family 1",
    },
    {
        .emoji_ = FAMILY2,
        .title_ = "family 2",
    },
    {
        .emoji_ = ROLE1,
        .title_ = "role 1",
    },
    {
        .emoji_ = ROLE2,
        .title_ = "role 2",
    },
    {
        .emoji_ = GENERED1,
        .title_ = "genered 1",
    },
    {
        .emoji_ = GENERED2,
        .title_ = "genered 2",
    },
    {
        .emoji_ = HAIR1,
        .title_ = "hair 1",
    },
    {
        .emoji_ = HAIR2,
        .title_ = "hair 2",
    },
    {
        .emoji_ = OTHER1,
        .title_ = "other 1",
    },
    {
        .emoji_ = OTHER2,
        .title_ = "other 2",
    }
};

class EmojiTest : public TestFeature {
public:
    EmojiTest() : TestFeature("EmojiTest")
    {
    }

    void Layout()
    {
        for (auto &emoji : g_emojis) {
            TypographyStyle tystyle;
            auto dfProvider = DynamicFileFontProvider::Create();
            dfProvider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfProvider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            emoji.style.fontFamilies_ = {"Segoe UI Emoji"};
            builder->PushStyle(emoji.style);
            builder->AppendSpan(emoji.emoji);
            builder->PopStyle();

            auto typography = builder->Build();
            double widthLimit = 100;
            typography->Layout(widthLimit);
            typographies_.push_back({ .typography = typography, .comment = emoji.title });
        }
    }
} g_test;
} // namespace
