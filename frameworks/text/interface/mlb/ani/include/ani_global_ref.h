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

class AniGlobalNamespace {
public:
    static AniGlobalNamespace& GetInstance()
    {
        static AniGlobalNamespace instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_namespace text;

private:
    AniGlobalNamespace() = default;
    ~AniGlobalNamespace() = default;
    AniGlobalNamespace(const AniGlobalNamespace&) = delete;
    AniGlobalNamespace& operator=(const AniGlobalNamespace&) = delete;
    AniGlobalNamespace(AniGlobalNamespace&&) = delete;
    AniGlobalNamespace& operator=(AniGlobalNamespace&&) = delete;
};

class AniGlobalClass {
public:
    static AniGlobalClass& GetInstance()
    {
        static AniGlobalClass instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_class aniString;
    ani_class aniArray;
    ani_class aniMap;
    ani_class aniDouble;
    ani_class aniInt;
    ani_class aniBoolean;
    ani_class businessError;
    ani_class globalResource;
    ani_class fontCollection;
    ani_class fontDescriptor;
    ani_class paragraphBuilder;
    ani_class paragraph;
    ani_class run;
    ani_class textLine;
    ani_class lineTypeSet;
    ani_class positionWithAffinity;
    ani_class lineMetrics;
    ani_class runMetrics;
    ani_class font;
    ani_class textBox;
    ani_class range;
    ani_class textStyle;
    ani_class textShadow;
    ani_class decoration;
    ani_class rectStyle;
    ani_class fontFeature;
    ani_class fontVariation;
    ani_class typographicBounds;
    ani_class cleaner;
    ani_class canvas;
    ani_class paragraphStyle;
    ani_class strutStyle;
    ani_class textTab;
    ani_class point;
    ani_class path;
    ani_class placeholderSpan;
    ani_class textLayoutResult;
    ani_class textRectSize;

private:
    AniGlobalClass() = default;
    ~AniGlobalClass() = default;
    AniGlobalClass(const AniGlobalClass&) = delete;
    AniGlobalClass& operator=(const AniGlobalClass&) = delete;
    AniGlobalClass(AniGlobalClass&&) = delete;
    AniGlobalClass& operator=(AniGlobalClass&&) = delete;
};

class AniGlobalEnum {
public:
    static AniGlobalEnum& GetInstance()
    {
        static AniGlobalEnum instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_enum fontWeight;
    ani_enum fontWidth;
    ani_enum fontEdging;
    ani_enum affinity;
    ani_enum textDirection;
    ani_enum fontStyle;
    ani_enum textBaseline;
    ani_enum ellipsisMode;
    ani_enum textDecorationType;
    ani_enum textDecorationStyle;
    ani_enum textBadgeType;
    ani_enum lineHeightStyle;

private:
    AniGlobalEnum() = default;
    ~AniGlobalEnum() = default;
    AniGlobalEnum(const AniGlobalEnum&) = delete;
    AniGlobalEnum& operator=(const AniGlobalEnum&) = delete;
    AniGlobalEnum(AniGlobalEnum&&) = delete;
    AniGlobalEnum& operator=(AniGlobalEnum&&) = delete;
};

class AniGlobalMethod {
public:
    static AniGlobalMethod& GetInstance()
    {
        static AniGlobalMethod instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_method map;
    ani_method mapSet;
    ani_method businessErrorCtor;
    ani_method arrayCtor;
    ani_method arraySet;
    ani_method doubleCtor;
    ani_method doubleGet;
    ani_method intCtor;
    ani_method intGet;
    ani_method booleanCtor;
    ani_method booleanGet;
    ani_method fontCollection;
    ani_method fontCollectionGetNative;
    ani_method paragraphBuilderGetNative;
    ani_method paragraph;
    ani_method paragraphGetNative;
    ani_method paragraphBuilderCtor;
    ani_method run;
    ani_method runGetNative;
    ani_method textLine;
    ani_method textLineGetNative;
    ani_method lineTypesetCtor;
    ani_method lineTypesetGetNative;
    ani_method fontDescriptorCtor;
    ani_method canvasGetNative;
    ani_method lineMetricsCtor;
    ani_method fontDescriptorGetPath;
    ani_method fontDescriptorGetPostScriptName;
    ani_method fontDescriptorGetFullName;
    ani_method fontDescriptorGetFontFamily;
    ani_method fontDescriptorGetFontSubfamily;
    ani_method fontDescriptorGetWeight;
    ani_method fontDescriptorGetWidth;
    ani_method fontDescriptorGetItalic;
    ani_method fontDescriptorGetMonoSpace;
    ani_method fontDescriptorGetSymbolic;
    ani_method positionWithAffinity;
    ani_method paragraphStyleMaxLines;
    ani_method paragraphStyleTextStyle;
    ani_method paragraphStyleTextDirection;
    ani_method paragraphStyleAlign;
    ani_method paragraphStyleWordBreak;
    ani_method paragraphStyleBreakStrategy;
    ani_method paragraphStyleTextHeightBehavior;
    ani_method paragraphStyleStrutStyle;
    ani_method paragraphStyleTab;
    ani_method paragraphStyleTrailingSpaceOptimized;
    ani_method paragraphStyleAutoSpace;
    ani_method paragraphStyleCompressHeadPunctuation;
    ani_method paragraphStyleVerticalAlign;
    ani_method paragraphStyleIncludeFontPadding;
    ani_method paragraphStyleFallbackLineSpacing;
    ani_method paragraphStyleLineSpacing;
    ani_method strutStyleFontStyle;
    ani_method strutStyleFontWidth;
    ani_method strutStyleFontWeight;
    ani_method strutStyleFontSize;
    ani_method strutStyleHeight;
    ani_method strutStyleLeading;
    ani_method strutStyleForceHeight;
    ani_method strutStyleEnabled;
    ani_method strutStyleHeightOverride;
    ani_method strutStyleHalfLeading;
    ani_method strutStyleFontFamilies;
    ani_method textTabAlignment;
    ani_method textTabLocation;
    ani_method textStyleCtor;
    ani_method textStyleColor;
    ani_method textStyleFontWeight;
    ani_method textStyleFontWidth;
    ani_method textStyleFontStyle;
    ani_method textStyleBaseline;
    ani_method textStyleFontFamilies;
    ani_method textStyleFontSize;
    ani_method textStyleLetterSpacing;
    ani_method textStyleWordSpacing;
    ani_method textStyleHeightScale;
    ani_method textStyleHalfLeading;
    ani_method textStyleHeightOnly;
    ani_method textStyleEllipsis;
    ani_method textStyleEllipsisMode;
    ani_method textStyleLocale;
    ani_method textStyleBaselineShift;
    ani_method textStyleDecoration;
    ani_method textStyleTextShadows;
    ani_method textStyleFontFeatures;
    ani_method textStyleFontVariations;
    ani_method textStyleBackgroundRect;
    ani_method textStyleBadgeType;
    ani_method textStyleMaxLineHeight;
    ani_method textStyleMinLineHeight;
    ani_method textStyleLineHeightStyle;
    ani_method textStyleFontEdging;
    ani_method decorationCtor;
    ani_method decorationDecorationType;
    ani_method decorationDecorationStyle;
    ani_method decorationDecorationThicknessScale;
    ani_method decorationDecorationColor;
    ani_method pointX;
    ani_method pointY;
    ani_method textShadowCtor;
    ani_method textShadowBlurRadius;
    ani_method textShadowColor;
    ani_method textShadowPoint;
    ani_method fontFeatureCtor;
    ani_method fontFeatureName;
    ani_method fontFeatureValue;
    ani_method fontVariationCtor;
    ani_method fontVariationAxis;
    ani_method fontVariationValue;
    ani_method rectStyleCtor;
    ani_method rectStyleColor;
    ani_method rectStyleLeftTopRadius;
    ani_method rectStyleRightTopRadius;
    ani_method rectStyleRightBottomRadius;
    ani_method rectStyleLeftBottomRadius;
    ani_method pathGetNative;
    ani_method placeholderSpanWidth;
    ani_method placeholderSpanHeight;
    ani_method placeholderSpanAlign;
    ani_method placeholderSpanBaseline;
    ani_method placeholderSpanBaselineOffset;
    ani_method globalResourceId;
    ani_method globalResourceBundleName;
    ani_method globalResourceModuleName;
    ani_method globalResourceParams;
    ani_method globalResourceType;
    ani_method runMetricsCtor;
    ani_method fontCtor;
    ani_method rangeCtor;
    ani_method rangeStart;
    ani_method rangeEnd;
    ani_method textRectSizeCtor;
    ani_method textRectSizeWidth;
    ani_method textRectSizeHeight;
    ani_method textBoxCtor;
    ani_method typographicBoundsCtor;
    ani_method textLayoutResultCtor;

private:
    AniGlobalMethod() = default;
    ~AniGlobalMethod() = default;
    AniGlobalMethod(const AniGlobalMethod&) = delete;
    AniGlobalMethod& operator=(const AniGlobalMethod&) = delete;
    AniGlobalMethod(AniGlobalMethod&&) = delete;
    AniGlobalMethod& operator=(AniGlobalMethod&&) = delete;

    void InitBaseMethod(ani_env* env);
    void InitCommonMethod(ani_env* env);
    void InitFontDescriptorMethod(ani_env* env);
    void InitParagraphStyleMethod(ani_env* env);
    void InitStrutStyleMethod(ani_env* env);
    void InitTextStyleMethod(ani_env* env);
    void InitDecorationMethod(ani_env* env);
    void InitTextShadowMethod(ani_env* env);
    void InitFontFeatureMethod(ani_env* env);
    void InitFontVariationMethod(ani_env* env);
    void InitRectStyleMethod(ani_env* env);
    void InitTextLayoutResultMethod(ani_env* env);
    void InitPlaceholderMethod(ani_env* env);
    void InitGlobalResourceMethod(ani_env* env);
    void InitRangeMethod(ani_env* env);
    void InitPointMethod(ani_env* env);
    void InitTextTabMethod(ani_env* env);
    void InitTextRectSizeMethod(ani_env* env);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_GLOBAL_REF_H