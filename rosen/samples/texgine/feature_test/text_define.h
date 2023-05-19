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

#ifndef ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_TEXT_DEFINE_H
#define ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_TEXT_DEFINE_H

#define HAND "\xe2\x98\x9d"
#define BLACK "\xf0\x9f\x8f\xbf"
#define NOHAIR "\xf0\x9f\xa6\xb2"
#define SHAKEHAND "\xf0\x9f\xa4\x9d"
#define BROTHER "\xf0\x9f\x91\xac"
#define ARABIC_HELLO "\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7"
#define ARABIC_WORLD "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85"
#define HEBREW "\x5e\x25\xd5\x5d\xc5\xdd"
#define CHINESE_HELLO_WORLD "\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c"
#define KEY_CONNECTOR "\xef\xb8\x8f"
#define KEY "\xe2\x83\xa3"
#define SNOW_MAN "\xe2\x9b\x84"

// baisc emoji first plane
#define WATCH "\u231A"
#define FAST_FORWARD_BUTTON "\u23E9"
#define ALART_CLOCK "\u23F0"
#define UMBRELLA "\u2614"
#define RED_CIRCLE "\u2B55"
#define BLACK_LARGE_SQUARE "\u2B1B"
#define CURLY_LOOP "\u27B0"
#define PLUS "\u2795"
#define SNOWMAN "\u26C4"
#define VOLTAGE "\u26A1"

// basic emoji first plane with FEOF
#define EMOJI_VARIANT "\uFE0F"
#define COPYRIGHT "\u00A9" EMOJI_VARIANT
#define REGISTERED "\u00AE" EMOJI_VARIANT
#define DOUBLE_EXCLAMATION_MARK "\u203C" EMOJI_VARIANT
#define EXCLAMATION_QUESTION_MARK "\u2049" EMOJI_VARIANT
#define WAVY_DASH "\u3030" EMOJI_VARIANT
#define LEFT_ARROW "\u2B05" EMOJI_VARIANT
#define RED_HEART "\u2764" EMOJI_VARIANT
#define PENCIL "\u270F" EMOJI_VARIANT
#define AIRPLANE "\u2708" EMOJI_VARIANT
#define ATOM "\u269B" EMOJI_VARIANT
#define LAPTOP "\U0001F4BB" EMOJI_VARIANT

// basic emoji second plane
#define RED_DRAGON "\U0001F004"
#define JOKER "\U0001F0CF"
#define CYCLONE "\U0001F300"
#define NEW_MOON_FACE "\U0001F31A"
#define CAT "\U0001F408"
#define DOG "\U0001F415"
#define BOY "\U0001F466"
#define POST_BOX "\U0001F4EE"
#define GRINNING_FACE "\U0001F600"
#define ROCKET "\U0001F680"

// basic emoji second plane with FE0F
#define CLOUD_WITH_RAIN "\U0001F327" EMOJI_VARIANT
#define TORNADO "\U0001F32A" EMOJI_VARIANT
#define FOG "\U0001F32B" EMOJI_VARIANT
#define PERSON_GOLFING "\U0001F3CC" EMOJI_VARIANT
#define MOTOR_CYCLE "\U0001F3CD" EMOJI_VARIANT
#define CANDLE "\U0001F56F" EMOJI_VARIANT
#define SPIDER "\U0001F577" EMOJI_VARIANT
#define BED "\U0001F6CF" EMOJI_VARIANT
#define WORLD_MAP "\U0001F5FA" EMOJI_VARIANT
#define SPEAKING_HEAD "\U0001F5E3" EMOJI_VARIANT

// keyCap sequence
#define KEY_CAP "\u20E3"
#define SHARP "#" EMOJI_VARIANT KEY_CAP
#define ASTERISK "*" EMOJI_VARIANT KEY_CAP
#define ZERO "0" EMOJI_VARIANT KEY_CAP
#define ONE "1" EMOJI_VARIANT KEY_CAP
#define TWO "2" EMOJI_VARIANT KEY_CAP
#define THREE "3" EMOJI_VARIANT KEY_CAP
#define FOUR "4" EMOJI_VARIANT KEY_CAP
#define FIVE "5" EMOJI_VARIANT KEY_CAP
#define SIX "6" EMOJI_VARIANT KEY_CAP
#define SEVEN "7" EMOJI_VARIANT KEY_CAP
#define EIGHT "8" EMOJI_VARIANT KEY_CAP
#define NINE "9" EMOJI_VARIANT KEY_CAP

// flag emoji
#define FLAGA "\U0001F1E6"
#define FLAGB "\U0001F1E7"
#define FLAGC "\U0001F1E8"
#define FLAGD "\U0001F1E9"
#define FLAGE "\U0001F1EA"
#define FLAGF "\U0001F1EB"
#define FLAGG "\U0001F1EC"
#define FLAGH "\U0001F1ED"
#define FLAGI "\U0001F1EE"
#define FLAGJ "\U0001F1EF"
#define FLAGK "\U0001F1F0"
#define FLAGL "\U0001F1F1"
#define FLAGM "\U0001F1F2"
#define FLAGN "\U0001F1F3"
#define FLAGO "\U0001F1F4"
#define FLAGP "\U0001F1F5"
#define FLAGQ "\U0001F1F6"
#define FLAGR "\U0001F1F7"
#define FLAGS "\U0001F1F8"
#define FLAGT "\U0001F1F9"
#define FLAGU "\U0001F1FA"
#define FLAGV "\U0001F1FB"
#define FLAGW "\U0001F1FC"
#define FLAGX "\U0001F1FD"
#define FLAGY "\U0001F1FE"
#define FLAGZ "\U0001F1FF"
#define FLAG(pre, post) FLAG##pre FLAG##post

#define CHINA FLAG(C, N)
#define AUSTRALIA FLAG(A, U)
#define CANADA FLAG(C, A)
#define FRANCE FLAG(F, R)
#define IRELAND FLAG(I, E)
#define INDIA FLAG(I, N)
#define ICELAND FLAG(I, S)
#define ITALY FLAG(I, T)
#define JAPAN FLAG(J, P)
#define RUSSIA FLAG(R, U)

// color emoji
#define GIRL "\U0001F467"
#define SKIN0 "\U0001F3FB"
#define SKIN1 "\U0001F3FC"
#define SKIN2 "\U0001F3FD"
#define SKIN3 "\U0001F3FE"
#define SKIN4 "\U0001F3FF"

#define BOY0  BOY  SKIN0
#define BOY1  BOY  SKIN1
#define BOY2  BOY  SKIN2
#define BOY3  BOY  SKIN3
#define BOY4  BOY  SKIN4
#define GIRL0 GIRL SKIN0
#define GIRL1 GIRL SKIN1
#define GIRL2 GIRL SKIN2
#define GIRL3 GIRL SKIN3
#define GIRL4 GIRL SKIN4

// combination emoji
#define ZWJ "\u200D"
#define MAN "\U0001F468"
#define WOMAN "\U0001F469"
#define PERSON "\U0001F9D1"
#define MAN_SPORT_VARIANT "\u2640"
#define WOMAN_SPORT_VARIANT "\u2642"
#define BOUNCING_BALL "\u26F9"
#define LIFTING_WEIGHTS "\U0001F3CB"
#define HAIR_RED "\U0001F9B0"
#define HAIR_CURLY "\U0001F9B1"
#define HAIR_BALD "\U0001F9B2"
#define HAIR_WHITE "\U0001F9B3"
#define F_FLAG "\U0001F3F3"
#define TRANSGENDER "\u26A7"

#endif // ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_TEXT_DEFINE_H
