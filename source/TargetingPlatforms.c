/// @file
/// @brief Wii rendering demo with triangle and pyramid rendering, including user interaction via Wii Remote.
/// @author
/// Adapted from the original acube demo by tkcne.

/*
Naming Conventions:
- PascalCase for structs and types (e.g., GXRModeObj, GXColor)
- camelCase for functions and local variables (e.g., updateScreen, viewMatrix)
- g_ prefix for global variables (e.g., g_frameBuffer, g_angle)
- k_ prefix for constants/macros (e.g., kFifoSize)
*/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "C:/devkitPro/examples/wii/graphics/gx/TargetingTriangles/testmesh.h"

/// @brief Screen mode object (global)
GXRModeObj *g_screenMode;
/// @brief Frame buffer pointer (global)
static void *g_frameBuffer;
/// @brief Flag to indicate frame buffer is ready to copy
static vu8 g_readyForCopy;
/// @brief Size of the FIFO buffer (constant)
#define kFifoSize (256*1024)

/// @brief Rotation angles for different triangles
float g_angle1 = 0.0F;
float g_angle2 = 120.0F;
float g_angle3 = 240.0F;

/// @brief A button pressed flag
bool g_aPressed = false;

/// @brief Vertex list for cube
static const float g_vertices[] = {
    -2.0 , -2.0 , 0.0 ,
    2.0 , -2.0 , 0.0 ,
    -2.0 , 2.0 , 0.0 ,
    2.0 , 2.0 , 0.0 ,
    -2.0 , -2.0 , 4.0 ,
    2.0 , -2.0 , 4.0 ,
    -2.0 , 2.0 , 4.0 ,
    2.0 , 2.0 , 4.0 ,
};

/// @brief Face list for cube
static const short g_faceList[] = {
    0 , 2 , 3 , 3 , 1 , 0 ,
    4 , 5 , 7 , 7 , 6 , 4 ,
    0 , 1 , 5 , 5 , 4 , 0 ,
    1 , 3 , 7 , 7 , 5 , 1 ,
    3 , 2 , 6 , 6 , 7 , 3 ,
    2 , 0 , 4 , 4 , 6 , 2 ,
};

/// @brief Vertex list for pyramid
static const float g_pyramidVertices[] = {
    0.0875181 , 0.175275 , 5.0 ,
    -2.41248 , -2.32473 , 0.0 ,
    2.58752 , -2.32473 , 0.0 ,
    2.58752 , 2.67527 , 0.0 ,
    -2.41248 , 2.67527 , 0.0 ,
    0.0875181 , 0.175275 , 0.0 ,
};

/// @brief Face list for pyramid
static const short g_pyramidFaceList[] = {
    0 , 1 , 2 , 0 , 2 , 3 ,
    0 , 3 , 4 , 0 , 4 , 1 ,
    1 , 5 , 2 , 2 , 5 , 3 ,
    3 , 5 , 4 , 4 , 5 , 1 ,
};

/// @brief Second object vertices
s16 g_secondVertices[] ATTRIBUTE_ALIGN(32) = {
    0, 15, 0,
    -30, -15, 0,
    30, -15, 0
};

/// @brief RGB colors for triangle
u8 g_colors[] ATTRIBUTE_ALIGN(32) = {
    255, 0, 0, 255,
    0, 255, 0, 255,
    0, 0, 255, 255
};

/// @brief Blue color array
u8 g_blueColors[] = {
    0, 0, 255, 255,
    0, 0, 255, 255,
    0, 0, 255, 255
};

/// @brief Pink color array
u8 g_pinkColors[] = {
    255, 105, 180, 255,
    255, 105, 180, 255,
    255, 105, 180, 255
};

static float g_angle = 0.0F;
static float g_angleY = 0.0f;
float g_objectPositionX = 0.0F;
float g_objectPositionY = 0.0F;

/// @brief Update screen rendering
/// @param viewMatrix The view (camera) transformation matrix
void updateScreen(Mtx viewMatrix);

/// @brief Copy buffers on vertical sync
/// @param unused Unused frame count
static void copyBuffers(u32 unused);


/// @brief Copy buffers on vertical sync
/// @param unused Unused frame count
int main(void) {
    Mtx view;
    Mtx44 projection;
    GXColor backgroundColor = {0, 0, 0, 255};
    void *fifoBuffer = NULL;

    VIDEO_Init();
    WPAD_Init();

    g_screenMode = VIDEO_GetPreferredMode(NULL);
    g_frameBuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(g_screenMode));

    VIDEO_Configure(g_screenMode);
    VIDEO_SetNextFramebuffer(g_frameBuffer);
    VIDEO_SetPostRetraceCallback(copyBuffers);
    VIDEO_SetBlack(false);
    VIDEO_Flush();

    fifoBuffer = MEM_K0_TO_K1(memalign(32, kFifoSize));
    memset(fifoBuffer, 0, kFifoSize);

    GX_Init(fifoBuffer, kFifoSize);
    GX_SetCopyClear(backgroundColor, 0x00ffffff);
    GX_SetViewport(0, 0, g_screenMode->fbWidth, g_screenMode->efbHeight, 0, 1);
    GX_SetDispCopyYScale((f32)g_screenMode->xfbHeight / (f32)g_screenMode->efbHeight);
    GX_SetScissor(0, 0, g_screenMode->fbWidth, g_screenMode->efbHeight);
    GX_SetDispCopySrc(0, 0, g_screenMode->fbWidth, g_screenMode->efbHeight);
    GX_SetDispCopyDst(g_screenMode->fbWidth, g_screenMode->xfbHeight);
    GX_SetCopyFilter(g_screenMode->aa, g_screenMode->sample_pattern, GX_TRUE, g_screenMode->vfilter);
    GX_SetFieldMode(g_screenMode->field_rendering, ((g_screenMode->viHeight == 2 * g_screenMode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(g_frameBuffer, GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    guVector camera = {0.0F, 0.0F, 0.0F};
    guVector up = {0.0F, 1.0F, 0.0F};
    guVector look = {0.0F, 0.0F, -1.0F};

    guPerspective(projection, 100, 1.33F, 10.0F, 300.0F);
    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
    GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetArray(GX_VA_POS, g_vertices, 3 * sizeof(s16));
    GX_SetArray(GX_VA_CLR0, g_colors, 4 * sizeof(u8));
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    while (1) {
        guLookAt(view, &camera, &up, &look);
        GX_SetViewport(0, 0, g_screenMode->fbWidth, g_screenMode->efbHeight, 0, 1);
        GX_InvVtxCache();
        GX_InvalidateTexAll();

        updateScreen(view);

        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
            g_aPressed = !g_aPressed;
        }

        g_angle += 1.0F;
        g_angleY += 1.0f;
    }

    return 0;
}

void updateScreen(Mtx viewMatrix) {
    Mtx modelView, rotation;
    guVector axisY = {0, 1, 0};
    guVector axisX = {1, 0, 0};

    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) g_objectPositionY += 1.0F;
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) g_objectPositionX -= 1.0F;
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT) g_objectPositionY -= 1.0F;
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT) g_objectPositionX += 1.0F;

    GX_SetArray(GX_VA_CLR0, g_pinkColors, 4 * sizeof(u8));
    GX_SetArray(GX_VA_POS, teaVertices, 3 * sizeof(float));

    guMtxIdentity(modelView);
    guMtxRotAxisDeg(rotation, &axisY, g_angle);
    guMtxConcat(viewMatrix, rotation, modelView);
    guMtxRotAxisDeg(rotation, &axisX, g_angleY);
    guMtxConcat(modelView, rotation, modelView);
    guMtxTransApply(modelView, modelView, g_objectPositionX, g_objectPositionY, -30.0F);

    GX_LoadPosMtxImm(modelView, GX_PNMTX0);

    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, numFaces * 3);
    for (int i = 0; i < numFaces * 3; i++) {
        GX_Position1x16(teaFaceList[i]);
        GX_Color1x8(teaFaceList[i] % 3);
    }
    GX_End();

    GX_DrawDone();
    GX_CopyDisp(g_frameBuffer, GX_TRUE);
    GX_Flush();
    g_readyForCopy = GX_TRUE;
    VIDEO_WaitVSync();
}

static void copyBuffers(u32 count __attribute__((unused))) {
    if (g_readyForCopy == GX_TRUE) {
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GX_SetColorUpdate(GX_TRUE);
        GX_CopyDisp(g_frameBuffer, GX_TRUE);
        GX_Flush();
        g_readyForCopy = GX_FALSE;
    }
}
