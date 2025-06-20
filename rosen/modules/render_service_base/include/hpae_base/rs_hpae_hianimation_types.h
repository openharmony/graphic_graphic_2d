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

#ifndef RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H
#define RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H

#include <cstdint>

const uint32_t HAE_COLOR_MATRIX_COEF_COUNT = 20;
namespace OHOS::Rosen {
struct HaeRect {
    int l;
    int t;
    int r;
    int b;

    HaeRect(int left, int top, int right, int bottom) {
        l = left;
        t = top;
        r = right;
        b = bottom;
    };

    HaeRect() {
        l = 0;
        t = 0;
        r = 0;
        b = 0;
    };
};

struct HaeImage {
    void *handle;
    struct HaeRect rect;
};

struct HaePixel {
    uint16_t a;
    uint16_t r;
    uint16_t g;
    uint16_t b;
};

struct HaeNoiseValue {
    float noiseRatio;
    float noiseValue;
    float noiseUpClip;
    float noiseDnClip;
};

struct BlurImgParam {
    uint32_t width;
    uint32_t height;
    float sigmaNum;
};

struct HaeBlurBasicAttr {
    struct HaeImage *srcLayer;
    struct HaeImage *dstLayer;
    uint32_t perfLevel;
    uint32_t timeoutMs = 0;
    int32_t expectRunTime = -1;
    float sigmaNum;
    bool enablePremult;
};

enum BlppAbility : uint32_t {
    BLPP_CANCEL_PREMULT_EN = 1 << 0,
    BLPP_NOISE_EN = 1 << 1,
    BLPP_ALPHA_REPLACE_EN = 1 << 2,
    BLPP_COLOR_MATRIX_EN = 1 << 3,
    BLPP_PREMULT_EN = 1 << 4,
    BLPP_COLOR_MASK_EN = 1 << 5,
    BLPP_CLMASK_PREMULT_EN = 1 << 6,
    BLPP_CLMASK_CXL_PREMULT_EN = 1 << 7,
};

struct HaeBlurEffectAttr {
    uint16_t effectCaps;
    struct HaeNoiseValue noisePara;
    uint32_t alphaReplaceVal;
    float colorMatrixCoef[HAE_COLOR_MATRIX_COEF_COUNT];
    struct HaePixel colorMaskPara;
};

struct hianimation_algo_device_t {
    /*
     * 函 数 名    ：hianimationInputCheck
     * 功能描述    ：校验芯片能力是否支持当前模糊任务
     * 输入参数    ：struct BlurImgParam *imgInfo, 输入有效模糊尺寸及最大模糊半径
     *              struct HaeNoiseValue *noisePara, 杂色相关参数；
     * 输出参数    ：None
     * 返 回 值    ：成功返回true，失败返回false
    */
    bool (* hianimationInputCheck)(const struct BlurImgParam *imgInfo, const struct HaeNoiseValue *noisePara);

    /*
     * 函 数 名    ：hianimationAlgoInit
     * 功能描述    ：模糊初始化，创建中间缓存Buffer资源，尺寸和模糊图源相关
     * 输入参数    ：模糊原图有效区宽高，用于确认申请中间Buffer尺寸;
     *              maxSigma，需要支持的最大模糊半径
     *              format，图源格式，用于缓存buffer格式配置
     * 输出参数    ：None
     * 返 回 值    ：成功返回0，失败返回小于0
    */
    int32_t (* hianimationAlgoInit)(uint32_t imgWeight, uint32_t imgHeight, float maxSigma, uint32_t format);

    /*
     * 函 数 名    ：hianimationAlgoDeInit
     * 功能描述    ：模糊反初始化，释放中间缓存buffer资源
     * 输入参数    ：NA
     * 输出参数    ：NA
     * 返 回 值    ：成功返回0，失败返回小于0
    */
    int32_t (* hianimationAlgoDeInit)();

    /*
     * 函 数 名    ：hianimationBuildTask
     * 功能描述    ：通知HAE完成srcImage模糊，输出到dstImage中
     * 输入参数    ：const struct HaeBlurBasicAttr *basicInfo，调用者传入的基础通路信息，
     *              包括输入/输出buffer; 图层坐标; AAE工作频点; 模糊半径; 预乘使能开关
     *              const struct HaeBlurEffectAttr *effectInfo，调用者传入的效果参数信息
     *              包括BLPP中所有效果开关及配置参数。先判断效果对应bit位是否拉高，再配置对应
     *              效果参数，即if (effectInfo->effectCaps & HAE_BLPP_XXX) {config}
     * 输出参数    ：void **task_ptr返回配置信息地址，提供给调用者，用于构建FFTS任务
     *               uint32_t *taskId返回该次模糊任务taskId，用于传递给destroy任务
     * 返 回 值    ：成功返回0，失败返回小于0
    */
    int32_t (* hianimationBuildTask)(const struct HaeBlurBasicAttr *basicInfo,
        const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr);

    /*
     * 函 数 名    ：hianimationSyncProcess
     * 功能描述    ：同步处理接口，在函数内部完成配置生成及任务提交，供调试使用
     * 输入参数    ：const struct HaeBlurBasicAttr *basicInfo，调用者传入的基础通路信息，
     *              包括输入/输出buffer; 图层坐标; AAE工作频点; 模糊半径; 预乘使能开关
     *              const struct HaeBlurEffectAttr *effectInfo，调用者传入的效果参数信息
     *              包括BLPP中所有效果开关及配置参数。先判断效果对应bit位是否拉高，再配置对应
     *              效果参数，即if (effectInfo->effectCaps & HAE_BLPP_XXX) {config}
     * 输出参数    ：无
     * 返 回 值    ：成功返回0，失败返回小于0
    */
    int32_t (* hianimationSyncProcess) (const struct HaeBlurBasicAttr *basicInfo,
        const struct HaeBlurEffectAttr *effectInfo);

    /*
     * 函 数 名    ：hianimationDestroyTask
     * 功能描述    ：释放模糊任务，回收资源
     * 输入参数    ：uint32_t taskId, 用于资源释放
     * 输出参数    ：无
     * 返 回 值    ：成功返回0，失败返回小于0
    */
    int32_t (* hianimationDestroyTask)(uint32_t taskId);
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_TYPES_H