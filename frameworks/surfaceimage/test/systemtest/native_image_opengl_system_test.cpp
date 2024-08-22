/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

class NativeImageSystemOpenGLTest : public testing::Test {
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
    static inline bool isOnFrameAvailabled_ = false;
};

void NativeImageSystemOpenGLTest::OnFrameAvailable(void *context)
{
    (void) context;
    isOnFrameAvailabled_ = true;
}

void NativeImageSystemOpenGLTest::SetUpTestCase()
{
    glGenTextures(1, &textureId);
    glGenTextures(1, &textureId2);
    InitEglContext();
}

void NativeImageSystemOpenGLTest::TearDownTestCase()
{
    Deinit();
}

void NativeImageSystemOpenGLTest::InitEglContext()
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

void NativeImageSystemOpenGLTest::Deinit()
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

void ProducerThreadOpenGL(OHNativeWindow* nativeWindow)
{
    if (nativeWindow == nullptr) {
        return;
    }
    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    auto ret = NativeWindowHandleOpt(nativeWindow, code, width, height);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != GSERROR_OK) {
        return;
    }
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    delete rect;
    delete region;
    return;
}

/*
* Function: OHNativeImageUpdateImage
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageUpdateImage and check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageUpdateImage, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    std::thread producerThread(ProducerThreadOpenGL, nativeWindow);
    producerThread.join();

    auto ret = OH_NativeImage_UpdateSurfaceImage(newImage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OHNativeImageUpdateImageWithListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageUpdateImageWithListener
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageUpdateImageWithListener, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageSystemOpenGLTest::OnFrameAvailable;
    auto ret = OH_NativeImage_SetOnFrameAvailableListener(newImage, listener);
    ASSERT_EQ(ret, GSERROR_OK);


    isOnFrameAvailabled_ = false;
    std::thread producerThread(ProducerThreadOpenGL, nativeWindow);
    producerThread.join();
    EXPECT_TRUE(isOnFrameAvailabled_);
    ret = OH_NativeImage_UpdateSurfaceImage(newImage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OHNativeImageUpdateImageWithAttachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageUpdateImageWithAttachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageUpdateImageWithAttachContext, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    int32_t ret = OH_NativeImage_AttachContext(newImage, textureId);
    ASSERT_EQ(ret, GSERROR_OK);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);

    std::thread producerThread(ProducerThreadOpenGL, nativeWindow);
    producerThread.join();
    ret = OH_NativeImage_UpdateSurfaceImage(newImage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OHNativeImageUpdateImageWithDetachContext
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageUpdateImageWithDetachContext
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageUpdateImageWithDetachContext, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    int32_t ret;
    ret = OH_NativeImage_DetachContext(newImage);
    ASSERT_EQ(ret, GSERROR_OK);

    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    std::thread producerThread(ProducerThreadOpenGL, nativeWindow);
    producerThread.join();
    
    ret = OH_NativeImage_UpdateSurfaceImage(newImage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OHNativeImageUpdateImageWithGetTimeStamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageUpdateImageWithGetTimeStamp
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageUpdateImageWithGetTimeStamp, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);

    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    std::thread producerThread(ProducerThreadOpenGL, nativeWindow);
    producerThread.join();
    int64_t timeStamp = OH_NativeImage_GetTimestamp(newImage);
    ASSERT_NE(timeStamp, SURFACE_ERROR_ERROR);
    auto ret = OH_NativeImage_UpdateSurfaceImage(newImage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    OH_NativeImage_Destroy(&newImage);
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

bool CheckMatricIsSame(float matrixOld[MATRIX_SIZE], float matrixNew[MATRIX_SIZE])
{
    for (int32_t i = 0; i < MATRIX_SIZE; i++) {
        if (fabs(matrixOld[i] - matrixNew[i]) > 1e-6) {
            return false;
        }
    }
    return true;
}

void ProducerThreadWithMatrix(OHNativeWindow* nativeWindow, int32_t matrixIndex)
{
    if (nativeWindow == nullptr) {
        return;
    }
    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    auto ret = NativeWindowHandleOpt(nativeWindow, code, width, height);
    
    code = SET_TRANSFORM;
    ret = NativeWindowHandleOpt(nativeWindow, code, testType[matrixIndex]);
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != GSERROR_OK) {
        return;
    }
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    delete rect;
    delete region;
    return;
}

/*
* Function: OHNativeImageSTWithMatrix
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageSTWithMatrix
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageSTWithMatrix, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);

    uint32_t ret;

    for (int32_t i = 0; i < sizeof(testType) / sizeof(int32_t); i++) {
        std::thread producerThread(ProducerThreadWithMatrix, nativeWindow, i);
        producerThread.join();
        ret = OH_NativeImage_UpdateSurfaceImage(newImage);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);
        float matrix[16];
        int32_t ret = OH_NativeImage_GetTransformMatrix(newImage, matrix);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        bool bRet = CheckMatricIsSame(matrix, matrixArr[i]);
        ASSERT_EQ(bRet, true);
    }
    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: OHNativeImageSTWithMatrixV2
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run OHNativeImageSTWithMatrixV2
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemOpenGLTest, OHNativeImageSTWithMatrixV2, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);

    uint32_t ret;

    for (int32_t i = 0; i < sizeof(testType) / sizeof(int32_t); i++) {
        std::thread producerThread(ProducerThreadWithMatrix, nativeWindow, i);
        producerThread.join();
        ret = OH_NativeImage_UpdateSurfaceImage(newImage);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);
        float matrix[16];
        int32_t ret = OH_NativeImage_GetTransformMatrixV2(newImage, matrix);
        ASSERT_EQ(ret, SURFACE_ERROR_OK);

        bool bRet = CheckMatricIsSame(matrix, matrixArrV2[i]);
        ASSERT_EQ(bRet, true);
    }
    OH_NativeImage_Destroy(&newImage);
}
}