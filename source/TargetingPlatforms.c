// adapted from	the	original acube demo	by tkcne.

// enjoy

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <testmesh.h>

GXRModeObj	*screenMode;
static void	*frameBuffer;
static vu8	readyForCopy;
#define	FIFO_SIZE (256*1024)

float angle1 = 0.0F;    // Triangle 1 starts at the top
float angle2 = 120.0F;  // Triangle 2 starts at the left
float angle3 = 240.0F;  // Triangle 3 starts at the right

bool aPressed = false; 

static const float vertices[] = {
	-2.0 , -2.0 , 0.0 ,
	2.0 , -2.0 , 0.0 ,
	-2.0 , 2.0 , 0.0 ,
	2.0 , 2.0 , 0.0 ,
	-2.0 , -2.0 , 4.0 ,
	2.0 , -2.0 , 4.0 ,
	-2.0 , 2.0 , 4.0 ,
	2.0 , 2.0 , 4.0 ,
};

static const short faceList[] = {
	0 , 2 , 3 ,
	3 , 1 , 0 ,
	4 , 5 , 7 ,
	7 , 6 , 4 ,
	0 , 1 , 5 ,
	5 , 4 , 0 ,
	1 , 3 , 7 ,
	7 , 5 , 1 ,
	3 , 2 , 6 ,
	6 , 7 , 3 ,
	2 , 0 , 4 ,
	4 , 6 , 2 ,
};


static const float pyramidVertices[] = {
	0.0875181 , 0.175275 , 5.0 ,
	-2.41248 , -2.32473 , 0.0 ,
	2.58752 , -2.32473 , 0.0 ,
	2.58752 , 2.67527 , 0.0 ,
	-2.41248 , 2.67527 , 0.0 ,
	0.0875181 , 0.175275 , 0.0 ,
};

static const short pyramidFaceList[] = {
	0 , 1 , 2 ,
	0 , 2 , 3 ,
	0 , 3 , 4 ,
	0 , 4 , 1 ,
	1 , 5 , 2 ,
	2 , 5 , 3 ,
	3 , 5 , 4 ,
	4 , 5 , 1 ,
};





	s16	secondVertices[] ATTRIBUTE_ALIGN(32) = {
		0, 15, 0,
		-30, -15, 0,
		30,	-15, 0};

u8 colors[]	ATTRIBUTE_ALIGN(32)	= {
	255, 0,	0, 255,		// red
	0, 255,	0, 255,		// green
	0, 0, 255, 255};	// blue

    u8 blueColors[] = {
    0, 0, 255, 255,  // Blue
    0, 0, 255, 255,  // Blue
    0, 0, 255, 255   // Blue
};

u8 pinkColors[] = {
    255, 105, 180, 255,  // Pink
    255, 105, 180, 255,  // Pink
    255, 105, 180, 255   // Pink
};

    static float angle = 0.0F;
    static float angleY = 0.0f;
    float blueSquareX = 0.0F;  // Initial X position of the blue square
    float blueSquareY = 0.0F;  // Initial Y position of the blue square

    void update_screen(Mtx viewMatrix/* , float x1, float y1, float x2, float y2, float x3, float y3 */);

static void	copy_buffers(u32 unused);

int main(void) {
    Mtx view;
    Mtx44 projection;
    GXColor backgroundColor = {0, 0, 0, 255};
    void *fifoBuffer = NULL;

    VIDEO_Init();
    WPAD_Init();

    screenMode = VIDEO_GetPreferredMode(NULL);
    frameBuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));

    VIDEO_Configure(screenMode);
    VIDEO_SetNextFramebuffer(frameBuffer);
    VIDEO_SetPostRetraceCallback(copy_buffers);
    VIDEO_SetBlack(false);
    VIDEO_Flush();

    fifoBuffer = MEM_K0_TO_K1(memalign(32, FIFO_SIZE));
    memset(fifoBuffer, 0, FIFO_SIZE);

    GX_Init(fifoBuffer, FIFO_SIZE);
    GX_SetCopyClear(backgroundColor, 0x00ffffff);
    GX_SetViewport(0, 0, screenMode->fbWidth, screenMode->efbHeight, 0, 1);
    GX_SetDispCopyYScale((f32)screenMode->xfbHeight / (f32)screenMode->efbHeight);
    GX_SetScissor(0, 0, screenMode->fbWidth, screenMode->efbHeight);
    GX_SetDispCopySrc(0, 0, screenMode->fbWidth, screenMode->efbHeight);
    GX_SetDispCopyDst(screenMode->fbWidth, screenMode->xfbHeight);
    GX_SetCopyFilter(screenMode->aa, screenMode->sample_pattern, GX_TRUE, screenMode->vfilter);
    GX_SetFieldMode(screenMode->field_rendering, ((screenMode->viHeight == 2 * screenMode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(frameBuffer, GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    guVector camera = {0.0F, 0.0F, 0.0F};
    guVector up = {0.0F, 1.0F, 0.0F};
    guVector look = {0.0F, 0.0F, -1.0F};

    guPerspective(projection, 100, 1.33F, 10.0F, 300.0F);
    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetArray(GX_VA_POS, vertices, 3 * sizeof(s16));
    GX_SetArray(GX_VA_CLR0, colors, 4 * sizeof(u8));
    GX_SetArray(GX_VA_PTNMTXIDX, faceList, sizeof(s16)); 
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);


	while (1)
{
    // // Update the angles to make the triangles move
    // angle1 += 1.0F;
    // angle2 += 1.0F;
    // angle3 += 1.0F;

    // // Reset angles if they exceed 360° (for continuous circular movement)
    // if (angle1 >= 360.0F) angle1 = 0.0F;
    // if (angle2 >= 360.0F) angle2 = 0.0F;
    // if (angle3 >= 360.0F) angle3 = 0.0F;

    // // Define radius here before using it
    // float radius = 50.0F; // Radius of the circular path

    // // Calculate the new positions based on angles
    // float x1 = cos(angle1 * M_PI / 180.0F) * radius;
    // float y1 = sin(angle1 * M_PI / 180.0F) * radius;

    // float x2 = cos(angle2 * M_PI / 180.0F) * radius;
    // float y2 = sin(angle2 * M_PI / 180.0F) * radius;

    // float x3 = cos(angle3 * M_PI / 180.0F) * radius;
    // float y3 = sin(angle3 * M_PI / 180.0F) * radius;

    // Update the view matrix (camera transformation)
    guLookAt(view, &camera, &up, &look);
    GX_SetViewport(0, 0, screenMode->fbWidth, screenMode->efbHeight, 0, 1);
    GX_InvVtxCache();
    GX_InvalidateTexAll();

    // Now render the triangles with updated positions
    update_screen(view/*, x1, y1, x2, y2, x3, y3 */);

    // Handle button presses
    WPAD_ScanPads();
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
        if (!aPressed) {
            aPressed = true;  // Toggle to true when button is pressed
        }
        else {
            aPressed = false;  // Reset when the button is not pressed
        }
    } 

    angle += 1.0F; // Increment rotation
    angleY += 1.0f; 
    
}

	return 0;
}

void update_screen(Mtx viewMatrix/* , float x1, float y1, float x2, float y2, float x3, float y3 */) {
    Mtx modelView, rotation;
    guVector axisY = {0, 1, 0};  // Y-axis rotation
    guVector axisX = {1, 0, 0};  // X-axis rotation


    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) {
        blueSquareY += 1.0F;  // Move up
    }
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {
        blueSquareY -= 1.0F;  // Move down
    }
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT) {
        blueSquareX -= 1.0F;  // Move left
    }
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT) {
        blueSquareX += 1.0F;  // Move right
    }


    // // Apply transformations
    // guMtxIdentity(viewMatrix);
    // guMtxRotAxisDeg(rotation, &axisY, angle);  // Rotate around Y-axis
    // guMtxConcat(viewMatrix, rotation, modelView);

    // //guMtxRotAxisDeg(rotation, &axisX, angleY); // Rotate around X-axis
    // //guMtxConcat(modelView, rotation, modelView);

    // guMtxTransApply(modelView, modelView, blueSquareX, blueSquareY, -50.0F);
    // GX_LoadPosMtxImm(modelView, GX_PNMTX0);

    // // Set color array (optional, can be per-vertex)
     GX_SetArray(GX_VA_CLR0, blueColors, 4 * sizeof(u8));
    // GX_SetArray(GX_VA_POS, vertices, 3 * sizeof(s16));
    // // Begin rendering cube using faceList
    // if(aPressed != true)
    // {
    //     GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 36);  // 12 triangles = 36 indices
    //     for (int i = 0; i < 36; i++) {
    //         GX_Position1x8(faceList[i]);  
    //         GX_Color1x8(faceList[i] % 3);  // Assign color per face
    //     }
    //     GX_End();
    // }

    // guMtxIdentity(modelView);
    // guMtxRotAxisDeg(rotation, &axisY, angle);  // Rotate around Y-axis
    // guMtxConcat(viewMatrix, rotation, modelView);
    // guMtxRotAxisDeg(rotation, &axisX, angleY); // Rotate around X-axis
    // guMtxConcat(modelView, rotation, modelView);
    // guMtxTransApply(modelView, modelView, 20.0F, 0.0F, -50.0F); // Move second cube right
    // GX_LoadPosMtxImm(modelView, GX_PNMTX0);

    // // Draw second cube (same blue color)
    // GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 36);
    // for (int i = 0; i < 36; i++) {
    //     GX_Position1x8(faceList[i]);
    //     GX_Color1x8(faceList[i] % 3);
    // }
    // GX_End();
   
	GX_SetArray(GX_VA_POS, Teavertices, 3 * sizeof(float));
    guMtxIdentity(modelView);
guMtxRotAxisDeg(rotation, &axisY, angle);  // Rotate around Y-axis
guMtxConcat(viewMatrix, rotation, modelView);
guMtxRotAxisDeg(rotation, &axisX, angleY); // Rotate around X-axis
guMtxConcat(modelView, rotation, modelView);
guMtxTransApply(modelView, modelView, 20.0F, 0.0F, -50.0F); // Move right

GX_LoadPosMtxImm(modelView, GX_PNMTX0);

// Draw the mesh
GX_Begin(GX_TRIANGLES, GX_VTXFMT0, numFaces * 3);
for (int i = 0; i < numFaces * 3; i++) { 
	GX_Position1x8(teaFaceList[i]);
	GX_Color1x8(teaFaceList[i] % 3);
}
GX_End();

	
    // GX_SetArray(GX_VA_POS, pyramidVertices, 3 * sizeof(s16));
    // guMtxIdentity(viewMatrix);
    // guMtxRotAxisDeg(rotation, &axisY, angle);  // Rotate around Y-axis
    // guMtxConcat(viewMatrix, rotation, modelView);

    // //guMtxRotAxisDeg(rotation, &axisX, angleY); // Rotate around X-axis
    // guMtxConcat(modelView, rotation, modelView);

    // guMtxTransApply(modelView, modelView, 20.0F, 0.0F, -50.0F);
    // GX_LoadPosMtxImm(modelView, GX_PNMTX0);

    // // Draw second cube (same blue color)
    // GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 24);
    // for (int i = 0; i < 24; i++) {
    //     GX_Position1x8(pyramidFaceList[i]);
    //     GX_Color1x8(pyramidFaceList[i] % 3);
    // }
    // GX_End();


	// GX_SetArray(GX_VA_POS, TorusVertices, 3 * sizeof(s16));
    // guMtxIdentity(viewMatrix);
    // guMtxRotAxisDeg(rotation, &axisY, angle);  // Rotate around Y-axis
    // guMtxConcat(viewMatrix, rotation, modelView);

    // //guMtxRotAxisDeg(rotation, &axisX, angleY); // Rotate around X-axis
    // guMtxConcat(modelView, rotation, modelView);

    // guMtxTransApply(modelView, modelView, 20.0F, 0.0F, -50.0F);
    // GX_LoadPosMtxImm(modelView, GX_PNMTX0);

    // // Draw second cube (same blue color)
    // GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 1728);
    // for (int i = 0; i < 1728; i++) {
    //     GX_Position1x8(TorusFaceList[i]);
    //     GX_Color1x8(TorusFaceList[i] % 3);
    // }
    // GX_End();

   /*  if(aPressed == false)
    {
         // Second Triangle (Blue)
    guMtxIdentity(modelView);
    guMtxTransApply(modelView, modelView, x2, y2, -50.0F);  // Use x2, y2 for the second triangle
    guMtxConcat(viewMatrix, modelView, modelView);
    GX_LoadPosMtxImm(modelView, GX_PNMTX0);
    GX_SetArray(GX_VA_CLR0, blueColors, 4 * sizeof(u8));  // Set blue color for the second triangle
    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
    GX_Position1x8(0);  // Vertex 0
    GX_Color1x8(0);     // Blue color for vertex 0
    GX_Position1x8(1);  // Vertex 1
    GX_Color1x8(1);     // Blue color for vertex 1
    GX_Position1x8(2);  // Vertex 2
    GX_Color1x8(2);     // Blue color for vertex 2
    GX_End();

    }
   
    // Third Triangle (Pink)
    guMtxIdentity(modelView);
    guMtxTransApply(modelView, modelView, x3, y3, -50.0F);  // Use x3, y3 for the third triangle
    guMtxConcat(viewMatrix, modelView, modelView);
    GX_LoadPosMtxImm(modelView, GX_PNMTX0);
    GX_SetArray(GX_VA_CLR0, pinkColors, 4 * sizeof(u8));  // Set pink color for the third triangle
    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
    GX_Position1x8(0);  // Vertex 0
    GX_Color1x8(0);     // Pink color for vertex 0
    GX_Position1x8(1);  // Vertex 1
    GX_Color1x8(1);     // Pink color for vertex 1
    GX_Position1x8(2);  // Vertex 2
    GX_Color1x8(2);     // Pink color for vertex 2
    GX_End(); */

    GX_DrawDone();  // Ensure all triangles are drawn before swapping buffers
    GX_CopyDisp(frameBuffer, GX_TRUE);
    GX_Flush();

    readyForCopy = GX_TRUE;
    VIDEO_WaitVSync();
}





static void	copy_buffers(u32 count __attribute__ ((unused)))
{
	if (readyForCopy==GX_TRUE) {
		GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer,GX_TRUE);
		GX_Flush();
		readyForCopy = GX_FALSE;
	}
}