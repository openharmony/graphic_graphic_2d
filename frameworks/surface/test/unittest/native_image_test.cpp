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
#include "graphic_common_c.h"
#include "surface_type.h"
#include "window.h"
#include "GLES/gl.h"
#include "buffer_log.h"

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

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* native_display, const EGLint* attrib_list)
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
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }

    return eglGetDisplay((EGLNativeDisplayType)native_display);
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
};

void NativeImageTest::OnFrameAvailable(void *context)
{
    (void) context;
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
    ASSERT_EQ(ret, SURFACE_ERROR_INIT);
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
    ASSERT_EQ(timeStamp, SURFACE_ERROR_ERROR);
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
        int32_t ret = OH_NativeImage_GetTransformMatrix(image, matrix);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        bool bRet = CheckMatricIsSame(matrix, matrixArr[i]);
        ASSERT_EQ(bRet, true);
    }
    delete region;
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
}