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

#ifndef OHOS_TEXT_GLOBAL_REF_H
#define OHOS_TEXT_GLOBAL_REF_H

#include <ani.h>

namespace OHOS::Text::ANI {
ani_status InitAniGlobalRef(ani_vm* vm);

struct AniGlobalNamespace
{
    static ani_namespace text;
};

struct AniGlobalClass
{
    static ani_class aniString;
    static ani_class aniArray;
    static ani_class aniMap;
    static ani_class aniDouble;
    static ani_class aniInt;
    static ani_class aniBoolean;
    static ani_class businessError;
    static ani_class globalResource;
    static ani_class fontCollection;
    static ani_class fontDescriptor;
    static ani_class paragraphBuilder;
    static ani_class paragraph;
    static ani_class run;
    static ani_class textLine;
    static ani_class lineTypeSet;
    static ani_class positionWithAffinity;
    static ani_class lineMetrics;
    static ani_class runMetrics;
    static ani_class font;
    static ani_class textBox;
    static ani_class range;
    static ani_class textStyle;
    static ani_class textShadow;
    static ani_class decoration;
    static ani_class rectStyle;
    static ani_class fontFeature;
    static ani_class fontVariation;
    static ani_class typographicBounds;
    static ani_class cleaner;
};

struct AniGlobalEnum
{
    static ani_enum fontWeight;
    static ani_enum affinity;
    static ani_enum textDirection;
    static ani_enum fontStyle;
    static ani_enum textBaseline;
    static ani_enum ellipsisMode;
    static ani_enum textDecorationType;
    static ani_enum textDecorationStyle;
};

struct AniGlobalMethod
{
    static ani_method map;
    static ani_method mapSet;
    static ani_method businessErrorCtor;
    static ani_method arrayCtor;
    static ani_method arraySet;
    static ani_method doubleCtor;
    static ani_method doubleGet;
    static ani_method intCtor;
    static ani_method intGet;
    static ani_method booleanCtor;
    static ani_method booleanGet;
    static ani_method fontCollection;
    static ani_method fontCollectionGetNative;
    static ani_method paragraphBuilderGetNative;
    static ani_method paragraph;
    static ani_method paragraphGetNative;
    static ani_method paragraphBuilderCtor;
    static ani_method run;
    static ani_method runGetNative;
    static ani_method textLine;
    static ani_method textLineGetNative;
    static ani_method lineTypesetCtor;
    static ani_method lineTypesetGetNative;
    static ani_method fontDescriptorCtor;
    static ani_method canvasGetNative;
    static ani_method lineMetricsCtor;
    static ani_method fontDescriptorGetPostScriptName;
    static ani_method fontDescriptorGetFullName;
    static ani_method fontDescriptorGetFontFamily;
    static ani_method fontDescriptorGetFontSubfamily;
    static ani_method fontDescriptorGetWidth;
    static ani_method fontDescriptorGetItalic;
    static ani_method fontDescriptorGetMonoSpace;
    static ani_method fontDescriptorGetSymbolic;
    static ani_method positionWithAffinity;
    static ani_method paragraphStyleMaxLines;
    static ani_method paragraphStyleTextStyle;
    static ani_method paragraphStyleTextDirection;
    static ani_method paragraphStyleAlign;
    static ani_method paragraphStyleWordBreak;
    static ani_method paragraphStyleBreakStrategy;
    static ani_method paragraphStyleTextHeightBehavior;
    static ani_method paragraphStyleStrutStyle;
    static ani_method paragraphStyleTab;
    static ani_method strutStyleFontStyle;
    static ani_method strutStyleFontWidth;
    static ani_method strutStyleFontWeight;
    static ani_method strutStyleFontSize;
    static ani_method strutStyleHeight;
    static ani_method strutStyleLeading;
    static ani_method strutStyleForceHeight;
    static ani_method strutStyleEnabled;
    static ani_method strutStyleHeightOverride;
    static ani_method strutStyleHalfLeading;
    static ani_method strutStyleFontFamilies;
    static ani_method textTabAlignment;
    static ani_method textTabLocation;
    static ani_method textStyleCtor;
    static ani_method textStyleRColor;
    static ani_method textStyleRFontWeight;
    static ani_method textStyleRFontStyle;
    static ani_method textStyleRBaseline;
    static ani_method textStyleRFontFamilies;
    static ani_method textStyleRFontSize;
    static ani_method textStyleRLetterSpacing;
    static ani_method textStyleRWordSpacing;
    static ani_method textStyleRHeightScale;
    static ani_method textStyleRHalfLeading;
    static ani_method textStyleRHeightOnly;
    static ani_method textStyleREllipsis;
    static ani_method textStyleREllipsisMode;
    static ani_method textStyleRLocale;
    static ani_method textStyleRBaselineShift;
    static ani_method textStyleRDecoration;
    static ani_method textStyleRTextShadows;
    static ani_method textStyleRFontFeatures;
    static ani_method textStyleRFontVariations;
    static ani_method textStyleRBackgroundRect;
    static ani_method decorationCtor;
    static ani_method decorationDecorationType;
    static ani_method decorationDecorationStyle;
    static ani_method decorationDecorationThicknessScale;
    static ani_method decorationDecorationColor;
    static ani_method pointX;
    static ani_method pointY;
    static ani_method textShadowCtor;
    static ani_method textShadowBlurRadius;
    static ani_method textShadowColor;
    static ani_method textShadowPoint;
    static ani_method fontFeatureCtor;
    static ani_method fontFeatureName;
    static ani_method fontFeatureValue;
    static ani_method fontVariationCtor;
    static ani_method fontVariationAxis;
    static ani_method fontVariationValue;
    static ani_method rectStyleCtor;
    static ani_method rectStyleColor;
    static ani_method rectStyleLeftTopRadius;
    static ani_method rectStyleRightTopRadius;
    static ani_method rectStyleRightBottomRadius;
    static ani_method rectStyleLeftBottomRadius;
    static ani_method pathGetNative;
    static ani_method placeholderSpanWidth;
    static ani_method placeholderSpanHeight;
    static ani_method placeholderSpanAlign;
    static ani_method placeholderSpanBaseline;
    static ani_method placeholderSpanBaselineOffset;
    static ani_method globalResourceId;
    static ani_method globalResourceBundleName;
    static ani_method globalResourceModuleName;
    static ani_method globalResourceParams;
    static ani_method globalResourceType;
    static ani_method runMetricsCtor;
    static ani_method fontCtor;
    static ani_method rangeCtor;
    static ani_method rangeStart;
    static ani_method rangeEnd;
    static ani_method textBoxCtor;
    static ani_method typographicBoundsCtor;
};

struct AniGlobalFunction
{
    static ani_function transToRequired;
};
    
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_GLOBAL_REF_H