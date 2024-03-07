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
#define OTHER2 F_FLAG EMOJI_VARIANT ZWJ TRANSGENDER EMOJI_VARIANT

struct Emoji {
    TextStyle style = { .fontSize = 32 };
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
        .emoji = POST_BOX,
        .title = "postbox",
    },
    {
        .emoji = GRINNING_FACE,
        .title = "grinning face",
    },
    {
        .emoji = ROCKET,
        .title = "rocket",
    },
    {
        .emoji = CLOUD_WITH_RAIN,
        .title = "cloud with rain",
    },
    {
        .emoji = TORNADO,
        .title = "tornado",
    },
    {
        .emoji = FOG,
        .title = "fog",
    },
    {
        .emoji = PERSON_GOLFING,
        .title = "person golfing",
    },
    {
        .emoji = MOTOR_CYCLE,
        .title = "motorcycle",
    },
    {
        .emoji = CANDLE,
        .title = "candle",
    },
    {
        .emoji = SPIDER,
        .title = "spider",
    },
    {
        .emoji = BED,
        .title = "bed",
    },
    {
        .emoji = WORLD_MAP,
        .title = "world map",
    },
    {
        .emoji = SPEAKING_HEAD,
        .title = "speaking head",
    },
    {
        .emoji = SHARP,
        .title = "sharp",
    },
    {
        .emoji = ASTERISK,
        .title = "asterisk",
    },
    {
        .emoji = ZERO,
        .title = "zero",
    },
    {
        .emoji = ONE,
        .title = "one",
    },
    {
        .emoji = TWO,
        .title = "two",
    },
    {
        .emoji = THREE,
        .title = "three",
    },
    {
        .emoji = FOUR,
        .title = "four",
    },
    {
        .emoji = FIVE,
        .title = "five",
    },
    {
        .emoji = SIX,
        .title = "six",
    },
    {
        .emoji = SEVEN,
        .title = "seven"
    },
    {
        .emoji = CHINA,
        .title = "China",
    },
    {
        .emoji = AUSTRALIA,
        .title = "Australia",
    },
    {
        .emoji = CANADA,
        .title = "Canada",
    },
    {
        .emoji = FRANCE,
        .title = "France",
    },
    {
        .emoji = IRELAND,
        .title = "Ireland",
    },
    {
        .emoji = INDIA,
        .title = "India",
    },
    {
        .emoji = ICELAND,
        .title = "Iceland",
    },
    {
        .emoji = ITALY,
        .title = "Italy",
    },
    {
        .emoji = JAPAN,
        .title = "Japan",
    },
    {
        .emoji = RUSSIA,
        .title = "Russia",
    },
    {
        .emoji = BOY0,
        .title = "boy ligth",
    },
    {
        .emoji = BOY1,
        .title = "boy medium light",
    },
    {
        .emoji = BOY2,
        .title = "boy medium skin",
    },
    {
        .emoji = BOY3,
        .title = "boy medium dark",
    },
    {
        .emoji = BOY4,
        .title = "boy dark",
    },
    {
        .emoji = GIRL0,
        .title = "girl ligth",
    },
    {
        .emoji = GIRL1,
        .title = "girl medium light",
    },
    {
        .emoji = GIRL2,
        .title = "girl medium skin",
    },
    {
        .emoji = GIRL3,
        .title = "girl medium dark",
    },
    {
        .emoji = GIRL4,
        .title = "girl dark",
    },
    {
        .emoji = FAMILY1,
        .title = "family 1",
    },
    {
        .emoji = FAMILY2,
        .title = "family 2",
    },
    {
        .emoji = ROLE1,
        .title = "role 1",
    },
    {
        .emoji = ROLE2,
        .title = "role 2",
    },
    {
        .emoji = GENERED1,
        .title = "genered 1",
    },
    {
        .emoji = GENERED2,
        .title = "genered 2",
    },
    {
        .emoji = HAIR1,
        .title = "hair 1",
    },
    {
        .emoji = HAIR2,
        .title = "hair 2",
    },
    {
        .emoji = OTHER1,
        .title = "other 1",
    },
    {
        .emoji = OTHER2,
        .title = "other 2",
    }
};

class EmojiTest : public TestFeature {
public:
    EmojiTest() : TestFeature("EmojiTest")
    {
    }

    void Layout() override
    {
        for (auto &emoji : g_emojis) {
            TypographyStyle tystyle;
            auto dfProvider = DynamicFileFontProvider::Create();
            dfProvider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfProvider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            emoji.style.fontFamilies = {"Segoe UI Emoji"};
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
