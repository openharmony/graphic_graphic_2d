/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_DRAWING_CACHE_UTILS_H
#define OHOS_ANI_DRAWING_CACHE_UTILS_H

#include <ani.h>
#include <string>
#include <string_view>

namespace OHOS {
namespace Rosen {
namespace Drawing {

ani_status InitAniGlobalRef(ani_env* env);

struct DescriptorConfig {
    std::string_view methodDes;
    std::string_view paramDes;
};

ani_class AniFindClass(ani_env* env, const char* descriptor);

ani_method AniFindMethod(ani_env* env, ani_class cls, const DescriptorConfig& config);

ani_enum AniFindEnum(ani_env* env, const char* descriptor);

class AniGlobalClass {
public:
    static AniGlobalClass& GetInstance()
    {
        static AniGlobalClass instance;
        return instance;
    }

    void Init(ani_env* env);
    ani_class intCls;
    ani_class doubleCls;
    ani_class booleanCls;
    ani_class strCls;
    ani_class arrayCls;
    ani_class businessErrorCls;
    ani_class resourceCls;

    ani_class brush;
    ani_class colorSpaceManager;
    ani_class samplingOptions;
    ani_class canvas;
    ani_class colorFilter;
    ani_class pen;
    ani_class typeface;
    ani_class font;
    ani_class lattice;
    ani_class matrix;
    ani_class maskFilter;
    ani_class pathEffect;
    ani_class imageFilter;
    ani_class pathIterator;
    ani_class path;
    ani_class shaderEffect;
    ani_class shadowLayer;
    ani_class textBlob;
    ani_class rectUtils;
    ani_class region;
    ani_class roundRect;
    ani_class tool;
    ani_class typefaceArguments;
    ani_class fontMetrics;
    ani_class point;
    ani_class rect;
    ani_class color;
    ani_class colorInterface;
    ani_class color4f;
    ani_class color4fInterface;
    ani_class pointInterface;
    ani_class fontFeatureInterface;
    ani_class rectInterface;
    ani_class point3dInterface;
    ani_class runBufferInterface;

private:
    AniGlobalClass() = default;
    ~AniGlobalClass() = default;
    AniGlobalClass(const AniGlobalClass&) = delete;
    AniGlobalClass& operator=(const AniGlobalClass&) = delete;
    AniGlobalClass(AniGlobalClass&&) = delete;
    AniGlobalClass& operator=(AniGlobalClass&&) = delete;

    void InitBaseClass(ani_env* env);
    void InitDrawingClass(ani_env* env);
    void InitInterfaceClass(ani_env* env);
};

class AniGlobalEnum {
public:
    static AniGlobalEnum& GetInstance()
    {
        static AniGlobalEnum instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_enum pathIteratorVerb;
    ani_enum fontHinting;
    ani_enum fontEdging;
    ani_enum capStyle;
    ani_enum joinStyle;
    ani_enum colorEnum;
    ani_enum pathFillType;

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

    ani_method intCtor;
    ani_method doubleCtor;
    ani_method canvasCtor;
    ani_method colorFilterCtor;
    ani_method matrixCtor;
    ani_method pathCtor;
    ani_method pathIteratorCtor;
    ani_method roundRectCtor;
    ani_method typefaceCtor;
    ani_method imageFilterCtor;
    ani_method latticeCtor;
    ani_method maskFilterCtor;
    ani_method pathEffectCtor;
    ani_method shaderEffectCtor;
    ani_method shadowLayerCtor;
    ani_method textBlobCtor;
    ani_method fontMetricsCtor;
    ani_method pointCtor;
    ani_method rectCtor;
    ani_method colorCtor;
    ani_method color4fCtor;

    ani_method colorFilterBindNative;
    ani_method typefaceBindNative;
    ani_method latticeBindNative;
    ani_method maskFilterBindNative;
    ani_method pathEffectBindNative;
    ani_method imageFilterBindNative;
    ani_method shaderEffectBindNative;
    ani_method shadowLayerBindNative;
    ani_method textBlobBindNative;

    ani_method intGet;
    ani_method doubleGet;
    ani_method booleanGet;
    ani_method resourceGetId;
    ani_method resourceGetParams;
    ani_method resourceGetType;

    ani_method colorGetAlpha;
    ani_method colorGetRed;
    ani_method colorGetGreen;
    ani_method colorGetBlue;
    ani_method color4fGetAlpha;
    ani_method color4fGetRed;
    ani_method color4fGetGreen;
    ani_method color4fGetBlue;
    ani_method fontFeatureGetName;
    ani_method fontFeatureGetValue;
    ani_method rectGetLeft;
    ani_method rectGetTop;
    ani_method rectGetRight;
    ani_method rectGetBottom;
    ani_method rectSetLeft;
    ani_method rectSetTop;
    ani_method rectSetRight;
    ani_method rectSetBottom;
    ani_method pointGetX;
    ani_method pointGetY;
    ani_method pointSetX;
    ani_method pointSetY;
    ani_method point3dGetX;
    ani_method point3dGetY;
    ani_method point3dGetZ;
    ani_method runBufferGetGlyph;
    ani_method runBufferGetPositionX;
    ani_method runBufferGetPositionY;

private:
    AniGlobalMethod() = default;
    ~AniGlobalMethod() = default;
    AniGlobalMethod(const AniGlobalMethod&) = delete;
    AniGlobalMethod& operator=(const AniGlobalMethod&) = delete;
    AniGlobalMethod(AniGlobalMethod&&) = delete;
    AniGlobalMethod& operator=(AniGlobalMethod&&) = delete;

    void InitCtorMethod(ani_env* env);
    void InitBindNativeMethod(ani_env* env);
    void InitGetSetMethod(ani_env* env);
};

class AniGlobalField {
public:
    static AniGlobalField& GetInstance()
    {
        static AniGlobalField instance;
        return instance;
    }

    void Init(ani_env* env);

    ani_field brushNativeObj;
    ani_field colorSpaceManagerNativeobj;
    ani_field samplingOptionsNativeObj;
    ani_field canvasNativeObj;
    ani_field colorFilterNativeObj;
    ani_field penNativeObj;
    ani_field typefaceNativeObj;
    ani_field fontNativeObj;
    ani_field latticeNativeObj;
    ani_field matrixNativeObj;
    ani_field maskFilterNativeObj;
    ani_field pathEffectNativeObj;
    ani_field imageFilterNativeObj;
    ani_field pathIteratorNativeObj;
    ani_field pathNativeObj;
    ani_field shaderEffectNativeObj;
    ani_field shadowLayerNativeObj;
    ani_field textBlobNativeObj;
    ani_field rectUtilsNativeObj;
    ani_field regionNativeObj;
    ani_field roundRectNativeObj;
    ani_field toolNativeObj;
    ani_field typefaceArgumentsNativeObj;

private:
    AniGlobalField() = default;
    ~AniGlobalField() = default;
    AniGlobalField(const AniGlobalField&) = delete;
    AniGlobalField& operator=(const AniGlobalField&) = delete;
    AniGlobalField(AniGlobalField&&) = delete;
    AniGlobalField& operator=(AniGlobalField&&) = delete;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_CACHE_UTILS_H