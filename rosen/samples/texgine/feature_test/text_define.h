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

#define Hand "\xe2\x98\x9d"
#define Black "\xf0\x9f\x8f\xbf"
#define NoHair "\xf0\x9f\xa6\xb2"
#define ShakeHand "\xf0\x9f\xa4\x9d"
#define Brother "\xf0\x9f\x91\xac"
#define ArabicHello "\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7"
#define ArabicWorld "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85"
#define Hebrew "\x5e\x25\xd5\x5d\xc5\xdd"
#define ChineseHelloWorld "\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c"
#define KeyConnector "\xef\xb8\x8f"
#define Key "\xe2\x83\xa3"
#define SnowMan "\xe2\x9b\x84"

// baisc emoji first plane
#define Watch "\u231A"
#define FastForwardButton "\u23E9"
#define AlartClock "\u23F0"
#define Umbrella "\u2614"
#define RedCircle "\u2B55"
#define BlackLargeSquare "\u2B1B"
#define CurlyLoop "\u27B0"
#define Plus "\u2795"
#define Snowman "\u26C4"
#define Voltage "\u26A1"

// basic emoji first plane with FEOF
#define EmojiVariant "\uFE0F"
#define Copyright "\u00A9" EmojiVariant
#define Registered "\u00AE" EmojiVariant
#define DoubleExclamationMark "\u203C" EmojiVariant
#define ExclamationQuestionMark "\u2049" EmojiVariant
#define WavyDash "\u3030" EmojiVariant
#define LeftArrow "\u2B05" EmojiVariant
#define RedHeart "\u2764" EmojiVariant
#define Pencil "\u270F" EmojiVariant
#define Airplane "\u2708" EmojiVariant
#define Atom "\u269B" EmojiVariant
#define Laptop "\U0001F4BB" EmojiVariant

// basic emoji second plane
#define RedDragon "\U0001F004"
#define Joker "\U0001F0CF"
#define Cyclone "\U0001F300"
#define NewMoonFace "\U0001F31A"
#define Cat "\U0001F408"
#define Dog "\U0001F415"
#define Boy "\U0001F466"
#define PostBox "\U0001F4EE"
#define GrinningFace "\U0001F600"
#define Rocket "\U0001F680"

// basic emoji second plane with FE0F
#define CloudWithRain "\U0001F327" EmojiVariant
#define Tornado "\U0001F32A" EmojiVariant
#define Fog "\U0001F32B" EmojiVariant
#define PersonGolfing "\U0001F3CC" EmojiVariant
#define Motorcycle "\U0001F3CD" EmojiVariant
#define Candle "\U0001F56F" EmojiVariant
#define Spider "\U0001F577" EmojiVariant
#define Bed "\U0001F6CF" EmojiVariant
#define WorldMap "\U0001F5FA" EmojiVariant
#define SpeakingHead "\U0001F5E3" EmojiVariant

// keyCap sequence
#define KeyCap "\u20E3"
#define Sharp "#" EmojiVariant KeyCap
#define Asterisk "*" EmojiVariant KeyCap
#define Zero "0" EmojiVariant KeyCap
#define One "1" EmojiVariant KeyCap
#define Two "2" EmojiVariant KeyCap
#define Three "3" EmojiVariant KeyCap
#define Four "4" EmojiVariant KeyCap
#define Five "5" EmojiVariant KeyCap
#define Six "6" EmojiVariant KeyCap
#define Seven "7" EmojiVariant KeyCap
#define Eight "8" EmojiVariant KeyCap
#define Nine "9" EmojiVariant KeyCap

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

#define China FLAG(C, N)
#define Australia FLAG(A, U)
#define Canada FLAG(C, A)
#define France FLAG(F, R)
#define Ireland FLAG(I, E)
#define India FLAG(I, N)
#define Iceland FLAG(I, S)
#define Italy FLAG(I, T)
#define Japan FLAG(J, P)
#define Russia FLAG(R, U)

// color emoji
#define Girl "\U0001F467"
#define Skin0 "\U0001F3FB"
#define Skin1 "\U0001F3FC"
#define Skin2 "\U0001F3FD"
#define Skin3 "\U0001F3FE"
#define Skin4 "\U0001F3FF"

#define Boy0  Boy  Skin0
#define Boy1  Boy  Skin1
#define Boy2  Boy  Skin2
#define Boy3  Boy  Skin3
#define Boy4  Boy  Skin4
#define Girl0 Girl Skin0
#define Girl1 Girl Skin1
#define Girl2 Girl Skin2
#define Girl3 Girl Skin3
#define Girl4 Girl Skin4

// combination emoji
#define ZWJ "\u200D"
#define Man "\U0001F468"
#define Woman "\U0001F469"
#define Person "\U0001F9D1"
#define ManSportVariant "\u2640"
#define WomanSportVariant "\u2642"
#define BouncingBall "\u26F9"
#define LiftingWeights "\U0001F3CB"
#define HairRed "\U0001F9B0"
#define HairCurly "\U0001F9B1"
#define HairBald "\U0001F9B2"
#define HairWhite "\U0001F9B3"
#define Flag "\U0001F3F3"
#define Transgender "\u26A7"

#endif // ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_TEXT_DEFINE_H
