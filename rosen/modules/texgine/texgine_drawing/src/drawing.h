/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_DRAWING_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_DRAWING_H

#include "recording/recording_canvas.h"
#include "skia_adapter/skia_text_blob.h"
#include "draw/path.h"
#include "effect/path_effect.h"
#include "effect/mask_filter.h"
#include "text/text_blob.h"
#include "text/text_blob_builder.h"
#include "text/font.h"
#include "text/font_metrics.h"
#include "text/font_mgr.h"
#include "text/font_style.h"
#include "text/font_style_set.h"
#include "text/hm_symbol.h"
#include "text/hm_symbol_config_ohos.h"
#include "text/typeface.h"

namespace RSDrawing = OHOS::Rosen::Drawing;
using RSBlurType = RSDrawing::BlurType;
using RSBrush = RSDrawing::Brush;
using RSMemoryStream = RSDrawing::MemoryStream;
using RSCanvas = RSDrawing::Canvas;
using RSColor = RSDrawing::Color;
using RSData = RSDrawing::Data;
using RSMaskFilter = RSDrawing::MaskFilter;
using RSFont = RSDrawing::Font;
using RSFilter = RSDrawing::Filter;
using RSFont = RSDrawing::Font;
using RSFontEdging = RSDrawing::FontEdging;
using RSFontHinting = RSDrawing::FontHinting;
using RSFontMetrics = RSDrawing::FontMetrics;
using RSFontMgr = RSDrawing::FontMgr;
using RSFontStyle = RSDrawing::FontStyle;
using RSFontStyleSet = RSDrawing::FontStyleSet;
using RSPath = RSDrawing::Path;
using RSPen = RSDrawing::Pen;
using RSPoint = RSDrawing::Point;
using RSPicture = RSDrawing::Picture;
using RSPathEffect = RSDrawing::PathEffect;
using RSPathDashStyle = RSDrawing::PathDashStyle;
using RSRecordingPathEffect = RSDrawing::PathEffect;
using RSRecordingMaskFilter = RSDrawing::MaskFilter;
using RSRect = RSDrawing::Rect;
using RSRoundRect = RSDrawing::RoundRect;
using RSScalar = RSDrawing::scalar;
using RSTextBlob = RSDrawing::TextBlob;
using RSTextBlobBuilder = RSDrawing::TextBlobBuilder;
using RSTypeface = RSDrawing::Typeface;
using RSSColor = RSDrawing::DrawingSColor;
using RSRenderGroup = RSDrawing::DrawingRenderGroup;
using RSSymbolLayers = RSDrawing::DrawingSymbolLayers;
using RSSymbolLayersGroups = RSDrawing::DrawingSymbolLayersGroups;
using RSSymbolRenderingStrategy = RSDrawing::DrawingSymbolRenderingStrategy;
using RSEffectStrategy = RSDrawing::DrawingEffectStrategy;
using RSHMSymbolData = RSDrawing::DrawingHMSymbolData;
using RSHmSymbolConfig_OHOS = RSDrawing::HmSymbolConfigOhos;
using RSAnimationSetting = RSDrawing::DrawingAnimationSetting;
using RSAnimationType = RSDrawing::DrawingAnimationType;
using RSGroupInfo = RSDrawing::DrawingGroupInfo;
using RSPathOp = RSDrawing::PathOp;
using RSHMSymbol = RSDrawing::DrawingHMSymbol;
using RSGroupSetting = RSDrawing::DrawingGroupSetting;

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_DRAWING_H