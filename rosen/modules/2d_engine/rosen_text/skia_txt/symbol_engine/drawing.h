/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef LIB_TXT_SRC_SYMBOL_ENGINE__DRAWING_H
#define LIB_TXT_SRC_SYMBOL_ENGINE__DRAWING_H

#include "recording/recording_canvas.h"
#include "skia_adapter/skia_text_blob.h"
#include "draw/path.h"
#include "text/text_blob.h"
#include "text/hm_symbol.h"
#include "text/hm_symbol_config_ohos.h"

namespace RSDrawing = OHOS::Rosen::Drawing;
using RSBrush = RSDrawing::Brush;
using RSCanvas = RSDrawing::Canvas;
using RSColor = RSDrawing::Color;
using RSPath = RSDrawing::Path;
using RSPen = RSDrawing::Pen;
using RSPoint = RSDrawing::Point;
using RSTextBlob = RSDrawing::TextBlob;
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