/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <native_image.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <sys/time.h>
#include <securec.h>
#include "graphic_common_c.h"
#include "surface_type.h"
#include "window.h"
#include "GLES/gl.h"
#include "buffer_log.h"
#include "metadata_helper.h"
#include "surface.h"
#include "surface_image.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS::Rosen {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr int32_t MATRIX_SIZE = 16;

struct TEST_IMAGE {
    int a;
    bool b;
};

typedef struct OH_NativeImage_Tmp {
    OHOS::sptr<OHOS::SurfaceImage> consumer;
    OHOS::sptr<OHOS::IBufferProducer> producer;
    OHOS::sptr<OHOS::Surface> pSurface = nullptr;
    struct NativeWindow* nativeWindow = nullptr;
} OH_NativeImage_Tmp;

static bool CheckEglExtension(const char* extensions, const char* extension)
{
    size_t extlen = strlen(extension);
    const char* end = extensions + strlen(extensions);

    while (extensions < end) {
        size_t n = 0;
        /* Skip whitespaces, if any */
        if (*extensions == CHARACTER_WHITESPACE) {
            extensions++;
            continue;
        }
        n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);
        /* Compare strings */
        if (n == extlen && strncmp(extension, extensions, n) == 0) {
            return true; /* Found */
        }
        extensions += n;
    }
    /* Not found */
    return false;
}

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* nativeDisplay, const EGLint* attribList)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
                CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, nativeDisplay, attribList);
    }

    return eglGetDisplay((EGLNativeDisplayType)nativeDisplay);
}

class NativeImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static void InitEglContext();
    static void Deinit();

    static inline OH_NativeImage* image = nullptr;
    static inline OHNativeWindow* nativeWindow = nullptr;
    static inline GLuint textureId = 0;
    static inline GLuint textureId2 = 0;
    static inline EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    static inline EGLContext eglContext_ = EGL_NO_CONTEXT;
    static inline EGLConfig config_;
    static void OnFrameAvailable(void *context);
    static inline uint32_t availableCnt_ = 0;
};

void NativeImageTest::OnFrameAvailable(void *context)
{
    (void) context;
    availableCnt_++;
    cout << "OnFrameAvailable is called" << endl;
}

void NativeImageTest::SetUpTestCase()
{
    image = nullptr;
    nativeWindow = nullptr;
    glGenTextures(1, &textureId);
    glGenTextures(1, &textureId2);
}

void NativeImageTest::TearDownTestCase()
{
    image = nullptr;
    nativeWindow = nullptr;
    Deinit();
}

void NativeImageTest::InitEglContext()
{
    if (eglContext_ != EGL_NO_DISPLAY) {
        return;
    }

    BLOGI("Creating EGLContext!!!");
    eglDisplay_ = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        BLOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    EGLint major = 0;
    EGLint minor = 0;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        BLOGE("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        BLOGE("Failed to bind OpenGL ES API");
        return;
    }

    unsigned int ret;
    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        BLOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        BLOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }

    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_);

    BLOGW("Create EGL context successfully, version %{public}d.%{public}d", major, minor);
}

void NativeImageTest::Deinit()
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }
    eglDestroyContext(eglDisplay_, eglContext_);
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
}

/*
* Function: OH_NativeImage_Create
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_Create
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageCreate001, Function | MediumTest | Level1)
{
    image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(image, nullptr);
}

/*
* Function: OH_NativeImage_AcquireNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindow by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindow001, Function | MediumTest | Level2)
{
    nativeWindow = OH_NativeImage_AcquireNativeWindow(nullptr);
    ASSERT_EQ(nativeWindow, nullptr);
}

/*
* Function: OH_NativeImage_AcquireNativeWindow
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindow
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindow002, Function | MediumTest | Level1)
{
    nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
    ASSERT_NE(nativeWindow, nullptr);
}

/*
* Function: OH_NativeImage_AttachContext
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AttachContext by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAttachContext001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeImage_AttachContext(nullptr, textureId);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_DetachContext
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_DetachContext by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDetachContext001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeImage_DetachContext(nullptr);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_DetachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_DetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDetachContext002, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_DetachContext(image);
    ASSERT_EQ(ret, SURFACE_ERROR_EGL_STATE_UNKONW);
}

/*
* Function: OH_NativeImage_DetachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_DetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDetachContext003, Function | MediumTest | Level1)
{
    InitEglContext();
    int32_t ret = OH_NativeImage_DetachContext(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_AttachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AttachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAttachContext002, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_AttachContext(image, textureId);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_UpdateSurfaceImage by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeImage_UpdateSurfaceImage(nullptr);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_UpdateSurfaceImage
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage002, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_UpdateSurfaceImage(image);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowRequestBuffer
*                  2. call OH_NativeWindow_NativeWindowFlushBuffer
*                  3. OH_NativeImage_UpdateSurfaceImage
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage003, Function | MediumTest | Level1)
{
    int code = SET_USAGE;
    uint64_t usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    int32_t ret = NativeWindowHandleOpt(nativeWindow, code, usage);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_USAGE faile" << std::endl;
    }
    code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    ret = NativeWindowHandleOpt(nativeWindow, code, width, height);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_BUFFER_GEOMETRY failed" << std::endl;
    }
    code = SET_STRIDE;
    int32_t stride = 0x8;
    ret = NativeWindowHandleOpt(nativeWindow, code, stride);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_STRIDE failed" << std::endl;
    }
    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    ret = NativeWindowHandleOpt(nativeWindow, code, format);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_FORMAT failed" << std::endl;
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    for (int32_t i = 0; i < 2; i++) {
        ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);
        ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeImage_UpdateSurfaceImage(image);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);
        ASSERT_EQ(NativeWindowDisconnect(nativeWindow), SURFACE_ERROR_OK);
    }
    delete region;
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_UpdateSurfaceImage
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage006, Function | MediumTest | Level1)
{
    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    int32_t ret = OH_NativeImage_UpdateSurfaceImage(imageNew);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_GetTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTimestamp by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTimestamp001, Function | MediumTest | Level2)
{
    int64_t timeStamp = OH_NativeImage_GetTimestamp(nullptr);
    ASSERT_EQ(timeStamp, -1);
}

/*
* Function: OH_NativeImage_GetTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTimestamp
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTimestamp002, Function | MediumTest | Level1)
{
    int64_t timeStamp = OH_NativeImage_GetTimestamp(image);
    ASSERT_NE(timeStamp, SURFACE_ERROR_ERROR);
}

/*
* Function: OH_NativeImage_GetTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTimestamp
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTimestamp003, Function | MediumTest | Level1)
{
    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    int32_t ret = OH_NativeImage_GetTimestamp(imageNew);
    ASSERT_EQ(ret, -1);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_GetTransformMatrix
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTransformMatrix by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTransformMatrix001, Function | MediumTest | Level2)
{
    float matrix[MATRIX_SIZE];
    int32_t ret = OH_NativeImage_GetTransformMatrix(nullptr, matrix);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_GetTransformMatrix
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTransformMatrix
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTransformMatrix002, Function | MediumTest | Level1)
{
    float matrix[MATRIX_SIZE];
    int32_t ret = OH_NativeImage_GetTransformMatrix(image, matrix);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

bool CheckMatricIsSame(float matrixOld[MATRIX_SIZE], float matrixNew[MATRIX_SIZE])
{
    for (int32_t i = 0; i < MATRIX_SIZE; i++) {
        if (fabs(matrixOld[i] - matrixNew[i]) > 1e-6) {
            return false;
        }
    }
    return true;
}

int32_t testType[] = {
    GraphicTransformType::GRAPHIC_ROTATE_NONE, GraphicTransformType::GRAPHIC_ROTATE_90,
    GraphicTransformType::GRAPHIC_ROTATE_180, GraphicTransformType::GRAPHIC_ROTATE_270,
    GraphicTransformType::GRAPHIC_FLIP_H, GraphicTransformType::GRAPHIC_FLIP_V,
    GraphicTransformType::GRAPHIC_FLIP_H_ROT90, GraphicTransformType::GRAPHIC_FLIP_V_ROT90,
    GraphicTransformType::GRAPHIC_FLIP_H_ROT180, GraphicTransformType::GRAPHIC_FLIP_V_ROT180,
    GraphicTransformType::GRAPHIC_FLIP_H_ROT270, GraphicTransformType::GRAPHIC_FLIP_V_ROT270,
};
float matrixArr[][MATRIX_SIZE] = {
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
};

/*
* Function: OH_NativeImage_GetTransformMatrix
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTransformMatrix
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTransformMatrix003, Function | MediumTest | Level1)
{
    if (image == nullptr) {
        image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
        ASSERT_NE(image, nullptr);
    }

    if (nativeWindow == nullptr) {
        nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
        ASSERT_NE(nativeWindow, nullptr);
    }

    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = nullptr;
    int32_t ret = OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    listener.onFrameAvailable = NativeImageTest::OnFrameAvailable;
    ret = OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();

    for (int32_t i = 0; i < sizeof(testType) / sizeof(int32_t); i++) {
        int code = SET_TRANSFORM;
        ret = NativeWindowHandleOpt(nativeWindow, code, testType[i]);
        ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);

        rect->x = 0x100;
        rect->y = 0x100;
        rect->w = 0x100;
        rect->h = 0x100;
        region->rects = rect;
        ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeImage_UpdateSurfaceImage(image);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        float matrix[16];
        int32_t ret = OH_NativeImage_GetTransformMatrix(image, matrix);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        bool bRet = CheckMatricIsSame(matrix, matrixArr[i]);
        ASSERT_EQ(bRet, true);
    }
    delete region;
}

float matrixArrV2[][MATRIX_SIZE] = {
    {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},   // 单位矩阵
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},    // 90度矩阵
    {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},   // 180度矩阵
    {0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1},  // 270度矩阵
    {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1},  // 水平翻转
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},    // 垂直翻转
    {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},   // 水平*90
    {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},   // 垂直*90
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},    // 水平*180
    {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1},  // 垂直*180
    {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},   // 水平*270
    {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},   // 垂直*270
};

/*
* Function: OH_NativeImage_GetTransformMatrix
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTransformMatrix
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTransformMatrix004, Function | MediumTest | Level1)
{
    if (image == nullptr) {
        image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
        ASSERT_NE(image, nullptr);
    }

    if (nativeWindow == nullptr) {
        nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
        ASSERT_NE(nativeWindow, nullptr);
    }

    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageTest::OnFrameAvailable;
    int32_t ret = OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();

    for (int32_t i = 0; i < sizeof(testType) / sizeof(int32_t); i++) {
        int code = SET_TRANSFORM;
        ret = NativeWindowHandleOpt(nativeWindow, code, testType[i]);
        ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);

        rect->x = 0x100;
        rect->y = 0x100;
        rect->w = 0x100;
        rect->h = 0x100;
        region->rects = rect;
        ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeImage_UpdateSurfaceImage(image);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        float matrix[16];
        int32_t ret = OH_NativeImage_GetTransformMatrixV2(image, matrix);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        bool bRet = CheckMatricIsSame(matrix, matrixArrV2[i]);
        ASSERT_EQ(bRet, true);
    }
    delete region;
}

/*
 * Function: OH_NativeImage_GetBufferMatrix
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. test with different transform types and crop regions
 *                  2. verify matrix calculation with crop
 * @tc.require: issueI5KG61
 */
HWTEST_F(NativeImageTest, OHNativeImageGetBufferMatrix001, Function | MediumTest | Level1)
{
    if (image == nullptr) {
        image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
        ASSERT_NE(image, nullptr);
    }
    if (nativeWindow == nullptr) {
        nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
        ASSERT_NE(nativeWindow, nullptr);
    }

    // Setup frame available listener
    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageTest::OnFrameAvailable;
    int32_t ret = OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    ASSERT_EQ(ret, GSERROR_OK);

    // Setup buffer and region
    NativeWindowBuffer* buffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    int32_t width = 600;
    int32_t height = 800;
    ret = NativeWindowHandleOpt(nativeWindow, SET_BUFFER_GEOMETRY, width, height);
    float bufferMatrix[16] = {
        0, -0.5, 0, 0,
        -0.5, 0, 0, 0,
        0, 0, 1, 0,
        1, 1, 0, 1
    };
    EXPECT_EQ(ret, GSERROR_OK);

    // Set transform
    ret = NativeWindowHandleOpt(nativeWindow, SET_TRANSFORM, GraphicTransformType::GRAPHIC_ROTATE_90);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &buffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    using namespace OHOS::HDI::Display::Graphic::Common::V1_0;
    BufferHandleMetaRegion metaRegion{300, 400, 300, 400};
    vector<uint8_t> data;
    MetadataHelper::ConvertMetadataToVec(metaRegion, data);
    buffer->sfbuffer->SetMetadata(ATTRKEY_CROP_REGION, data);
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, buffer, -1, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    // Update surface to apply transform and crop
    ret = OH_NativeImage_UpdateSurfaceImage(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    // Get and verify buffer matrix
    float matrix[16];
    ret = OH_NativeImage_GetBufferMatrix(image, matrix);
    std::cout << "{\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "    {";
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i * 4 + j];
            if (j < 3) std::cout << ", ";
        }
        std::cout << "},\n";
    }
    std::cout << "}" << std::endl;
    ASSERT_EQ(ret, GSERROR_OK);
    BufferHandleMetaRegion crop;
    if (MetadataHelper::GetCropRectMetadata(buffer->sfbuffer, crop) != GSERROR_OK) {
        const float initMatrix[] = {
            0, -1, 0, 0,
            -1, 0, 0, 0,
            0, 0, 1, 0,
            1, 1, 0, 1
        };
        std::copy(std::begin(initMatrix), std::end(initMatrix), bufferMatrix);
    }

    ASSERT_EQ(CheckMatricIsSame(matrix, bufferMatrix), true);
    EXPECT_EQ(SURFACE_ERROR_INVALID_PARAM, OH_NativeImage_GetBufferMatrix(nullptr, matrix));
    EXPECT_EQ(SURFACE_ERROR_INVALID_PARAM, OH_NativeImage_GetBufferMatrix(image, nullptr));
    delete region;
}

/*
* Function: OH_NativeImage_GetTransformMatrix
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_GetTransformMatrix
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageGetTransformMatrix005, Function | MediumTest | Level1)
{
    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    int32_t ret = OH_NativeImage_GetTransformMatrixV2(nullptr, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ret = OH_NativeImage_GetTransformMatrix(imageNew, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ret = OH_NativeImage_GetTransformMatrixV2(imageNew, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    float matrix[16];
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    ret = OH_NativeImage_GetTransformMatrix(imageNew, matrix);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ret = OH_NativeImage_GetTransformMatrixV2(imageNew, matrix);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_AttachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AttachContext with another texture
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAttachContext003, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_AttachContext(image, textureId2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowRequestBuffer
*                  2. call OH_NativeWindow_NativeWindowFlushBuffer
*                  3. OH_NativeImage_UpdateSurfaceImage after the bound OPENGL ES texture changed
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage004, Function | MediumTest | Level1)
{
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    int32_t ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    delete region;

    ret = OH_NativeImage_UpdateSurfaceImage(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_DetachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_DetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDetachContext004, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_DetachContext(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_AttachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AttachContext after OH_NativeImage_DetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAttachContext004, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_AttachContext(image, textureId2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_AttachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AttachContext after OH_NativeImage_DetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAttachContext005, Function | MediumTest | Level1)
{
    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    int32_t ret = OH_NativeImage_AttachContext(imageNew, 0);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ret = OH_NativeImage_DetachContext(imageNew);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_UpdateSurfaceImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowRequestBuffer
*                  2. call OH_NativeWindow_NativeWindowFlushBuffer
*                  3. OH_NativeImage_UpdateSurfaceImage again
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageUpdateSurfaceImage005, Function | MediumTest | Level1)
{
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    int32_t ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    delete region;

    ret = OH_NativeImage_UpdateSurfaceImage(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_GetSurfaceId
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. create image
*                  2. GetSurfaceId
*                  2. check ret
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageGetSurfaceId001, Function | MediumTest | Level1)
{
    if (image == nullptr) {
        image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
        ASSERT_NE(image, nullptr);
    }

    uint64_t surfaceId;
    int32_t ret = OH_NativeImage_GetSurfaceId(image, &surfaceId);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_GetSurfaceId
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. create image
*                  2. GetSurfaceId
*                  2. check ret
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageGetSurfaceId002, Function | MediumTest | Level1)
{
    uint64_t surfaceId;
    int32_t ret = OH_NativeImage_GetSurfaceId(nullptr, &surfaceId);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);

    ret = OH_NativeImage_GetSurfaceId(imageNew, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    ret = OH_NativeImage_GetSurfaceId(imageNew, &surfaceId);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_SetOnFrameAvailableListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. check image and nativeWindow
*                  2. call OH_NativeImage_SetOnFrameAvailableListener
*                  3. call OH_NativeWindow_NativeWindowFlushBuffer
*                  4. check OnFrameAvailable is called
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageSetOnFrameAvailableListener001, Function | MediumTest | Level1)
{
    if (image == nullptr) {
        image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
        ASSERT_NE(image, nullptr);
    }

    if (nativeWindow == nullptr) {
        nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
        ASSERT_NE(nativeWindow, nullptr);
    }

    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageTest::OnFrameAvailable;
    int32_t ret = OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    delete region;

    ret = OH_NativeImage_UpdateSurfaceImage(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_SetOnFrameAvailableListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. check image and nativeWindow
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageSetOnFrameAvailableListener002, Function | MediumTest | Level1)
{
    OH_OnFrameAvailableListener listener;
    int32_t ret = OH_NativeImage_SetOnFrameAvailableListener(nullptr, listener);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    ret = OH_NativeImage_SetOnFrameAvailableListener(imageNew, listener);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_UnsetOnFrameAvailableListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_UnsetOnFrameAvailableListener
*                  2. check ret
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageUnsetOnFrameAvailableListener001, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_UnsetOnFrameAvailableListener(image);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

/*
* Function: OH_NativeImage_UnsetOnFrameAvailableListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_UnsetOnFrameAvailableListener
*                  2. check ret
* @tc.require: issueI86VH2
*/
HWTEST_F(NativeImageTest, OHNativeImageUnsetOnFrameAvailableListener002, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_UnsetOnFrameAvailableListener(nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage* imageNew = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(imageNew, nullptr);
    memset_s(imageNew, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    ret = OH_NativeImage_UnsetOnFrameAvailableListener(imageNew);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&imageNew);
}

/*
* Function: OH_NativeImage_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_Destroy by abnormal input
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDestroy001, Function | MediumTest | Level2)
{
    OH_NativeImage_Destroy(nullptr);
    ASSERT_NE(image, nullptr);
}

/*
* Function: OH_NativeImage_Destroy
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_Destroy
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageDestroy002, Function | MediumTest | Level1)
{
    OH_NativeImage_Destroy(&image);
    ASSERT_EQ(image, nullptr);
}

/*
* Function: OH_NativeImage_AcquireNativeWindowBuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindowBuffer
*                  2. check ret
*                  3. call OH_NativeImage_ReleaseNativeWindowBuffer
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindowBuffer001, Function | MediumTest | Level1)
{
    int32_t ret = OH_NativeImage_AcquireNativeWindowBuffer(nullptr, nullptr, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    OH_NativeImage* newImage1 = OH_NativeImage_Create(0, 0);
    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage1, nullptr, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage1, &nativeWindowBuffer, nullptr);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    ret = OH_NativeImage_ReleaseNativeWindowBuffer(nullptr, nullptr, 0);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage1, nullptr, 0);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    int32_t fenceFd;
    memset_s(newImage1, sizeof(OH_NativeImage_Tmp), 0, sizeof(OH_NativeImage_Tmp));
    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage1, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    NativeWindowBuffer nativeWindowBufferNew;
    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage1, &nativeWindowBufferNew, 0);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    OH_NativeImage_Destroy(&newImage1);
}

/*
* Function: OH_NativeImage_AcquireNativeWindowBuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindowBuffer
*                  2. check ret
*                  3. call OH_NativeImage_ReleaseNativeWindowBuffer
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindowBuffer002, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* newNativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    ASSERT_NE(newNativeWindow, nullptr);

    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    int32_t ret = NativeWindowHandleOpt(newNativeWindow, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    for (int32_t i = 0; i < 100; i++) {
        ret = OH_NativeWindow_NativeWindowRequestBuffer(newNativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeWindow_NativeWindowFlushBuffer(newNativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        nativeWindowBuffer = nullptr;
        ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);
        ASSERT_NE(nativeWindowBuffer, nullptr);

        ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);
    }

    delete rect;
    delete region;
    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OH_NativeImage_AcquireNativeWindowBuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindowBuffer
*                  2. check ret
*                  3. call OH_NativeImage_ReleaseNativeWindowBuffer
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindowBuffer003, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* newNativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    ASSERT_NE(newNativeWindow, nullptr);

    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    int32_t ret = NativeWindowHandleOpt(newNativeWindow, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;

    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, SURFACE_ERROR_NO_BUFFER);

    ret = OH_NativeWindow_NativeWindowRequestBuffer(newNativeWindow, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = OH_NativeWindow_NativeWindowFlushBuffer(newNativeWindow, nativeWindowBuffer, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
    ASSERT_EQ(ret, SURFACE_ERROR_BUFFER_STATE_INVALID);

    OH_NativeImage* newImage1 = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage1, nullptr);
    OHNativeWindow* newNativeWindow1 = OH_NativeImage_AcquireNativeWindow(newImage1);
    ASSERT_NE(newNativeWindow1, nullptr);
    code = SET_BUFFER_GEOMETRY;
    width = 0x100;
    height = 0x100;
    ret = NativeWindowHandleOpt(newNativeWindow1, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer1 = nullptr;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(newNativeWindow1, &nativeWindowBuffer1, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeWindow_NativeWindowFlushBuffer(newNativeWindow1, nativeWindowBuffer1, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage1, &nativeWindowBuffer1, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer1, fenceFd);
    ASSERT_EQ(ret, SURFACE_ERROR_BUFFER_NOT_INCACHE);

    delete rect;
    delete region;
    OH_NativeImage_Destroy(&newImage);
    OH_NativeImage_Destroy(&newImage1);
}

/*
* Function: OH_NativeImage_AcquireNativeWindowBuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_AcquireNativeWindowBuffer
*                  2. check ret
*                  3. call OH_NativeImage_ReleaseNativeWindowBuffer
*                  4. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OHNativeImageAcquireNativeWindowBuffer004, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* newNativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    ASSERT_NE(newNativeWindow, nullptr);

    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    int32_t ret = NativeWindowHandleOpt(newNativeWindow, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    struct timeval acquireStartTime;
    struct timeval acquireEndTime;
    struct timeval releaseStartTime;
    struct timeval releaseEndTime;
    uint64_t acquireTotalTime = 0;
    uint64_t releaseTotalTime = 0;
    for (int32_t i = 0; i < 10000; i++) {
        ret = OH_NativeWindow_NativeWindowRequestBuffer(newNativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeWindow_NativeWindowFlushBuffer(newNativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        nativeWindowBuffer = nullptr;
        gettimeofday(&acquireStartTime, nullptr);
        ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
        gettimeofday(&acquireEndTime, nullptr);
        acquireTotalTime += (1000000 * (acquireEndTime.tv_sec - acquireStartTime.tv_sec) +
            (acquireEndTime.tv_usec - acquireStartTime.tv_usec));
        ASSERT_EQ(ret, GSERROR_OK);
        ASSERT_NE(nativeWindowBuffer, nullptr);

        gettimeofday(&releaseStartTime, nullptr);
        ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
        gettimeofday(&releaseEndTime, nullptr);
        releaseTotalTime += (1000000 * (releaseEndTime.tv_sec - releaseStartTime.tv_sec) +
            (releaseEndTime.tv_usec - releaseStartTime.tv_usec));
        ASSERT_EQ(ret, GSERROR_OK);
    }
    std::cout << "10000 count total time, OH_NativeImage_AcquireNativeWindowBuffer: " << acquireTotalTime << " us" <<
        " OH_NativeImage_ReleaseNativeWindowBuffer: " << releaseTotalTime << " us" << std::endl;

    delete rect;
    delete region;
    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OH_ConsumerSurface_Create
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_ConsumerSurface_Create
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_ConsumerSurface_Create001, Function | MediumTest | Level1)
{
    OH_NativeImage* consumerSurface = OH_ConsumerSurface_Create();
    ASSERT_NE(consumerSurface, nullptr);
    OHNativeWindow* newNativeWindow = OH_NativeImage_AcquireNativeWindow(consumerSurface);
    ASSERT_NE(newNativeWindow, nullptr);

    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    int32_t ret = NativeWindowHandleOpt(newNativeWindow, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    for (int32_t i = 0; i < 10000; i++) {
        ret = OH_NativeWindow_NativeWindowRequestBuffer(newNativeWindow, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);

        ret = OH_NativeWindow_NativeWindowFlushBuffer(newNativeWindow, nativeWindowBuffer, fenceFd, *region);
        ASSERT_EQ(ret, GSERROR_OK);

        nativeWindowBuffer = nullptr;
        ret = OH_NativeImage_AcquireNativeWindowBuffer(consumerSurface, &nativeWindowBuffer, &fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);
        ASSERT_NE(nativeWindowBuffer, nullptr);

        ret = OH_NativeImage_ReleaseNativeWindowBuffer(consumerSurface, nativeWindowBuffer, fenceFd);
        ASSERT_EQ(ret, GSERROR_OK);
    }

    delete rect;
    delete region;
    OH_NativeImage_Destroy(&consumerSurface);
}

/*
* Function: OH_ConsumerSurface_Create
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_ConsumerSurface_Create
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_ConsumerSurface_Create002, Function | MediumTest | Level1)
{
    struct timeval startTime;
    struct timeval endTime;
    uint64_t totalTime = 0;
    OH_NativeImage* consumerSurface;
    for (int32_t i = 0; i < 10000; i++) {
        gettimeofday(&startTime, nullptr);
        consumerSurface = OH_ConsumerSurface_Create();
        gettimeofday(&endTime, nullptr);
        totalTime += (1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec));
        ASSERT_NE(consumerSurface, nullptr);
        OH_NativeImage_Destroy(&consumerSurface);
    }
    std::cout << "10000 count total time, OH_ConsumerSurface_Create: " << totalTime << " us" << std::endl;
}

/*
* Function: OH_ConsumerSurface_SetDefaultUsage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_ConsumerSurface_SetDefaultUsage
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_ConsumerSurface_SetDefaultUsage001, Function | MediumTest | Level1)
{
    uint64_t usage = BUFFER_USAGE_CPU_READ;
    OH_NativeImage* consumerSurface = OH_ConsumerSurface_Create();
    ASSERT_NE(consumerSurface, nullptr);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultUsage(nullptr, usage), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultUsage(consumerSurface, usage), GSERROR_OK);
    OH_NativeImage_Destroy(&consumerSurface);
}

/*
* Function: OH_ConsumerSurface_SetDefaultSize
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_ConsumerSurface_SetDefaultSize
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_ConsumerSurface_SetDefaultSize001, Function | MediumTest | Level1)
{
    int32_t width = 100;
    int32_t height = 100;
    OH_NativeImage* consumerSurface = OH_ConsumerSurface_Create();
    ASSERT_NE(consumerSurface, nullptr);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultSize(nullptr, 1, 1), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultSize(consumerSurface, 0, -1), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultSize(consumerSurface, 1, -1), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(OH_ConsumerSurface_SetDefaultSize(consumerSurface, width, height), GSERROR_OK);
    OH_NativeImage_Destroy(&consumerSurface);
}

/*
* Function: OH_NativeImage_SetDropBufferMode001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_SetDropBufferMode
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_NativeImage_SetDropBufferMode001, Function | MediumTest | Level1)
{
    OH_NativeImage* consumerSurface = OH_ConsumerSurface_Create();
    ASSERT_NE(consumerSurface, nullptr);
    ASSERT_EQ(OH_NativeImage_SetDropBufferMode(nullptr, true), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(OH_NativeImage_SetDropBufferMode(consumerSurface, true), GSERROR_OK);
    ASSERT_EQ(OH_NativeImage_SetDropBufferMode(consumerSurface, false), GSERROR_OK);
    OH_NativeImage_Destroy(&consumerSurface);
}

/*
* Function: OH_NativeImage_SetDropBufferMode002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeImage_SetDropBufferMode002
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageTest, OH_NativeImage_SetDropBufferMode002, Function | MediumTest | Level1)
{
    OH_NativeImage* consumerSurfaceTest = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(consumerSurfaceTest, nullptr);

    ASSERT_EQ(OH_NativeImage_SetDropBufferMode(consumerSurfaceTest, true), GSERROR_OK);

    OHNativeWindow* nativeWindowTest = OH_NativeImage_AcquireNativeWindow(consumerSurfaceTest);
    ASSERT_NE(nativeWindowTest, nullptr);
    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    int32_t ret = NativeWindowHandleOpt(nativeWindowTest, code, width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageTest::OnFrameAvailable;
    ret = OH_NativeImage_SetOnFrameAvailableListener(consumerSurfaceTest, listener);
    ASSERT_EQ(ret, GSERROR_OK);
    availableCnt_ = 0;
    NativeWindowBuffer* NativeWindowBuffer1 = nullptr;
    NativeWindowBuffer* NativeWindowBuffer2 = nullptr;
    NativeWindowBuffer* NativeWindowBuffer3 = nullptr;
    int fenceFd = -1;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindowTest, &NativeWindowBuffer1, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindowTest, &NativeWindowBuffer2, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindowTest, &NativeWindowBuffer3, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindowTest, NativeWindowBuffer1, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindowTest, NativeWindowBuffer2, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindowTest, NativeWindowBuffer3, fenceFd, *region);
    ASSERT_EQ(ret, GSERROR_OK);
    delete region;
    ASSERT_EQ(availableCnt_, 1);
    ret = OH_NativeImage_UpdateSurfaceImage(consumerSurfaceTest);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}
}
