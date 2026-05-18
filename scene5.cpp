#include <cmath>
#include <iostream>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#if !defined(_WIN32)
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
typedef const char* LPCSTR;
#define MB_OK 0x00000000
#define MB_ICONINFORMATION 0x00000040
static int MessageBoxA(void*, LPCSTR text, LPCSTR caption, unsigned int) {
    std::cout << caption << ": " << text << std::endl;
    return 0;
}
#endif

static double glfwGetTime() {
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

struct GLFWwindow {};
static int glfwInit() { return 0; }
static GLFWwindow* glfwCreateWindow(int, int, const char*, void*, void*) { return nullptr; }
static void glfwMakeContextCurrent(GLFWwindow*) {}
static int glfwWindowShouldClose(GLFWwindow*) { return 1; }
static void glfwSwapBuffers(GLFWwindow*) {}
static void glfwPollEvents() {}
static void glfwTerminate() {}

using namespace std;

const float PI = 3.14159265358979323846f;
const float scene5_speed = 1.25f;
const float scene5_popupTrigger = 0.65f;

const int scene5_popupDelayFrames = 30;
const int scene5_popupFramesFirst = 180;
const int scene5_popupFramesSecond = 240;
const double scene5_titleDuration = 10.0;
double scene5_titleStart = -1.0;

#if !defined(_WIN32)
static pid_t scene5_musicPid = -1;
static unsigned int scene5_musicSession = 0;
static const char* scene5_musicPath = "music/music5.mp3";

void scene5StopMusic();
void scene5MusicTimeout(int value);

void scene5StartMusic()
{
    scene5StopMusic();
    ++scene5_musicSession;

    pid_t pid = fork();
    if (pid == 0) {
        execlp("cvlc",
               "cvlc",
               "--intf", "dummy",
               "--quiet",
               "--play-and-exit",
               "--no-video",
               scene5_musicPath,
               (char*)nullptr);
        _exit(1);
    }

    if (pid > 0) {
        scene5_musicPid = pid;
        glutTimerFunc(240000, scene5MusicTimeout, static_cast<int>(scene5_musicSession));
    }
}

void scene5StopMusic()
{
    ++scene5_musicSession;

    if (scene5_musicPid <= 0) {
        return;
    }

    kill(scene5_musicPid, SIGTERM);

    for (int i = 0; i < 20; ++i) {
        int status = 0;
        pid_t result = waitpid(scene5_musicPid, &status, WNOHANG);
        if (result == scene5_musicPid) {
            scene5_musicPid = -1;
            return;
        }
        usleep(50000);
    }

    kill(scene5_musicPid, SIGKILL);
    waitpid(scene5_musicPid, nullptr, 0);
    scene5_musicPid = -1;
}

void scene5MusicTimeout(int value)
{
    if (value != static_cast<int>(scene5_musicSession)) {
        return;
    }
    scene5StopMusic();
}
#else
void scene5StartMusic() {}
void scene5StopMusic() {}
#endif

float cloudMove = 0.0f;

// ---------------- ANIMATION ----------------

float coupleMove = -1.2f;

float boyMoveX = 0.0f;
float boyMoveY = 0.0f;

float carMove = 0.0f;

float wheelRotation = 0.0f;
float legSwing = 0.0f;


bool boyEnteredCar = false;
bool carStarted = false;
bool firstMessageShown = false;
bool secondMessageShown = false;
bool dialogueFinished = false;
bool scene5_popupStarted = false;
bool scene5_popupCompleted = false;
int scene5_popupPhase = 0;
int scene5_popupTimer = 0;
// ---------------- BASIC DRAW ----------------

void drawRect(float x1, float y1, float x2, float y2, int r, int g, int b)
{
    glColor3ub(r, g, b);

    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glEnd();
}

void drawCircle(float cx, float cy, float r, int R, int G, int B)
{
    glColor3ub(R, G, B);

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(cx, cy);

    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * PI * i / 60.0f;

        glVertex2f(
            cx + cos(angle) * r,
            cy + sin(angle) * r
        );
    }

    glEnd();
}

void drawEllipse(float cx, float cy,
                 float rx, float ry,
                 int R, int G, int B)
{
    glColor3ub(R, G, B);

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(cx, cy);

    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * PI * i / 60.0f;

        glVertex2f(
            cx + cos(angle) * rx,
            cy + sin(angle) * ry
        );
    }

    glEnd();
}

void drawLine(float x1, float y1,
              float x2, float y2,
              int R, int G, int B,
              float w = 1.0f)
{
    glColor3ub(R, G, B);

    glLineWidth(w);

    glBegin(GL_LINES);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);

    glEnd();

    glLineWidth(1.0f);
}

void drawQuad(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4,
              int R, int G, int B)
{
    glColor3ub(R, G, B);

    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);

    glEnd();
}

void scene5_drawRoundedRect(float x, float y, float w, float h, float radius)
{
    float r = radius;
    if (r < 0.0f) r = 0.0f;
    if (r > h * 0.5f) r = h * 0.5f;
    if (r > w * 0.5f) r = w * 0.5f;

    glBegin(GL_QUADS);
    glVertex2f(x + r, y);
    glVertex2f(x + w - r, y);
    glVertex2f(x + w - r, y + h);
    glVertex2f(x + r, y + h);

    glVertex2f(x, y + r);
    glVertex2f(x + r, y + r);
    glVertex2f(x + r, y + h - r);
    glVertex2f(x, y + h - r);

    glVertex2f(x + w - r, y + r);
    glVertex2f(x + w, y + r);
    glVertex2f(x + w, y + h - r);
    glVertex2f(x + w - r, y + h - r);
    glEnd();

    const int segments = 18;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + r, y + r);
    for (int i = 0; i <= segments; i++)
    {
        float a = 3.1416f + (3.1416f / 2.0f) * (float)i / segments;
        glVertex2f(x + r + cos(a) * r, y + r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + w - r, y + r);
    for (int i = 0; i <= segments; i++)
    {
        float a = (3.1416f / 2.0f) * (float)i / segments;
        glVertex2f(x + w - r + cos(a) * r, y + r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + w - r, y + h - r);
    for (int i = 0; i <= segments; i++)
    {
        float a = 0.0f + (3.1416f / 2.0f) * (float)i / segments;
        glVertex2f(x + w - r + cos(a) * r, y + h - r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + r, y + h - r);
    for (int i = 0; i <= segments; i++)
    {
        float a = 3.1416f / 2.0f + (3.1416f / 2.0f) * (float)i / segments;
        glVertex2f(x + r + cos(a) * r, y + h - r + sin(a) * r);
    }
    glEnd();
}

float scene5_strokeTextWidth(void* font, const char* text)
{
    float w = 0.0f;
    for (const char* p = text; *p; ++p)
    {
        w += (float)glutStrokeWidth(font, *p);
    }
    return w;
}

void scene5_drawStrokeCentered(float x, float y, const char* text, void* font, float scale)
{
    float w = scene5_strokeTextWidth(font, text) * scale;
    glPushMatrix();
    glTranslatef(x - w * 0.5f, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(1.8f);
    for (const char* p = text; *p; ++p)
    {
        glutStrokeCharacter(font, *p);
    }
    glLineWidth(1.0f);
    glPopMatrix();
}

void scene5_drawStrokeLeft(float x, float y, const char* text, void* font, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(2.2f);
    for (const char* p = text; *p; ++p)
    {
        glutStrokeCharacter(font, *p);
    }
    glLineWidth(1.0f);
    glPopMatrix();
}

void scene5_drawMessageBubble(float centerX, float baseY,
                              const char* line1,
                              const char* line2,
                              const char* line3)
{
    const float bubbleW = 0.72f;
    const float bubbleH = (line3 != nullptr) ? 0.23f : 0.19f;
    const float bubbleX = centerX - bubbleW * 0.5f;
    const float bubbleY = baseY;

    glColor3f(1.0f, 1.0f, 1.0f);
    scene5_drawRoundedRect(bubbleX, bubbleY, bubbleW, bubbleH, 0.035f);

    glColor3f(0.0f, 0.0f, 0.0f);
    if (line3 != nullptr)
    {
        scene5_drawStrokeCentered(centerX, bubbleY + 0.165f, line1, GLUT_STROKE_ROMAN, 0.0003f);
        scene5_drawStrokeCentered(centerX, bubbleY + 0.105f, line2, GLUT_STROKE_ROMAN, 0.0003f);
        scene5_drawStrokeCentered(centerX, bubbleY + 0.045f, line3, GLUT_STROKE_ROMAN, 0.0003f);
    }
    else
    {
        scene5_drawStrokeCentered(centerX, bubbleY + 0.125f, line1, GLUT_STROKE_ROMAN, 0.0003f);
        scene5_drawStrokeCentered(centerX, bubbleY + 0.055f, line2, GLUT_STROKE_ROMAN, 0.0003f);
    }
}



// ---------------- SKY ----------------

void drawSky()
{
    glBegin(GL_QUADS);

    glColor3ub(110, 175, 245);
    glVertex2f(-1, 1);

    glColor3ub(110, 175, 245);
    glVertex2f(1, 1);

    glColor3ub(230, 242, 255);
    glVertex2f(1, -0.1f);

    glColor3ub(230, 242, 255);
    glVertex2f(-1, -0.1f);

    glEnd();
}

void drawSun()
{
    drawCircle(
        0.10f,
        0.75f,
        0.09f,
        255, 235, 60
    );
}

// ---------------- CLOUDS ----------------

void drawCloud(float x, float y)
{
    drawCircle(
        x - 0.09f,
        y - 0.015f,
        0.050f,
        220, 220, 220
    );

    drawCircle(
        x,
        y - 0.020f,
        0.065f,
        225, 225, 225
    );

    drawCircle(
        x + 0.09f,
        y - 0.015f,
        0.050f,
        220, 220, 220
    );

    drawCircle(
        x - 0.12f,
        y,
        0.045f,
        250, 250, 250
    );

    drawCircle(
        x - 0.05f,
        y + 0.03f,
        0.070f,
        255, 255, 255
    );

    drawCircle(
        x + 0.03f,
        y + 0.04f,
        0.085f,
        255, 255, 255
    );

    drawCircle(
        x + 0.12f,
        y + 0.01f,
        0.060f,
        250, 250, 250
    );

    drawCircle(
        x + 0.19f,
        y - 0.005f,
        0.040f,
        245, 245, 245
    );

    drawCircle(
        x - 0.02f,
        y - 0.03f,
        0.055f,
        245, 245, 245
    );

    drawCircle(
        x + 0.08f,
        y - 0.025f,
        0.050f,
        245, 245, 245
    );
}

void drawRiver()
{
    drawRect(
        -1.0f,
        -0.20f,
         1.0f,
         0.10f,
        135, 185, 225
    );
}

// ---------------- BRIDGE ----------------

void drawBridge()
{
    drawRect(
        -1.0f,
        -1.0f,
         1.0f,
        -0.55f,
        195, 145, 95
    );

    for (int i = 0; i < 26; i++)
    {
        float x = -1.0f + i * (2.0f / 26.0f);

        drawRect(
            x,
            -1.0f,
            x + 0.002f,
            -0.55f,
            145, 105, 65
        );
    }

    drawRect(
        -1.0f,
        -0.33f,
         1.0f,
        -0.31f,
        220, 35, 35
    );

    for (int i = 0; i < 7; i++)
    {
        float px = -0.95f + i * 0.32f;

        drawRect(
            px,
            -0.55f,
            px + 0.03f,
            -0.31f,
            225, 225, 230
        );
    }

    for (int i = 0; i < 6; i++)
    {
        float x1 = -0.92f + i * 0.32f;
        float x2 = x1 + 0.26f;

        drawRect(
            x1,
            -0.52f,
            x2,
            -0.36f,
            210, 210, 215
        );

        for (int k = 0; k < 7; k++)
        {
            float yy = -0.52f + k * (0.16f / 7.0f);

            drawRect(
                x1,
                yy,
                x2,
                yy + 0.0015f,
                180, 180, 185
            );
        }
    }

    drawRect(
        -0.98f,
        -0.10f,
        -0.84f,
         0.55f,
        145, 135, 125
    );

    drawRect(
        -0.985f,
         0.55f,
        -0.835f,
         0.60f,
        120, 115, 110
    );

    drawQuad(
        -0.84f, 0.22f,
        -0.10f, 0.07f,
        -0.08f, 0.11f,
        -0.84f, 0.26f,
        55, 55, 60
    );

    drawLine(
        -0.90f, 0.56f,
        -0.05f, 0.20f,
        70, 70, 75,
        2.5f
    );

    drawLine(
        -0.90f, 0.52f,
        -0.05f, 0.17f,
        70, 70, 75,
        2.5f
    );

    for (int i = 0; i < 10; i++)
    {
        float t = i / 9.0f;

        float cx = -0.80f + t * 0.70f;
        float cy = 0.50f + (0.18f - 0.50f) * t;
        float dy = 0.24f + (0.10f - 0.24f) * t;

        drawLine(
            cx,
            cy,
            cx,
            dy,
            90, 90, 95,
            1.5f
        );
    }
}

// ---------------- PEOPLE ----------------

void drawBoy(float x, float y,
             int r, int g, int b)
{
    float s = 0.70f;

    drawEllipse(x, y + 0.22f * s,
                0.06f * s,
                0.08f * s,
                237, 200, 165);

    drawEllipse(x, y + 0.26f * s,
                0.065f * s,
                0.045f * s,
                25, 25, 25);

    drawRect(x - 0.07f * s,
             y + 0.06f * s,
             x + 0.07f * s,
             y + 0.18f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.10f * s,
             x - 0.07f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x + 0.07f * s,
             y + 0.10f * s,
             x + 0.10f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.08f * s,
             x - 0.06f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x + 0.06f * s,
             y + 0.08f * s,
             x + 0.10f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x - 0.06f * s,
             y - 0.08f * s,
             x + 0.06f * s,
             y + 0.06f * s,
             30, 30, 40);

    drawRect(x - 0.055f * s,
             y - 0.22f * s,
             x - 0.015f * s,
             y - 0.08f * s,
             30, 30, 40);

    drawRect(x + 0.015f * s,
             y - 0.22f * s,
             x + 0.055f * s,
             y - 0.08f * s,
             30, 30, 40);

    drawRect(x - 0.060f * s,
             y - 0.24f * s,
             x - 0.005f * s,
             y - 0.22f * s,
             10, 10, 10);

    drawRect(x + 0.005f * s,
             y - 0.24f * s,
             x + 0.060f * s,
             y - 0.22f * s,
             10, 10, 10);
}

void drawBoyNoLegs(float x, float y,
                   int r, int g, int b)
{
    float s = 0.70f;

    drawEllipse(x, y + 0.22f * s,
                0.06f * s,
                0.08f * s,
                237, 200, 165);

    drawEllipse(x, y + 0.26f * s,
                0.065f * s,
                0.045f * s,
                25, 25, 25);

    drawRect(x - 0.07f * s,
             y + 0.06f * s,
             x + 0.07f * s,
             y + 0.18f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.10f * s,
             x - 0.07f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x + 0.07f * s,
             y + 0.10f * s,
             x + 0.10f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.08f * s,
             x - 0.06f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x + 0.06f * s,
             y + 0.08f * s,
             x + 0.10f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x - 0.06f * s,
             y - 0.08f * s,
             x + 0.06f * s,
             y + 0.06f * s,
             30, 30, 40);

             
}

void drawGirl(float x, float y)
{
    float s = 0.70f;

    drawEllipse(x, y + 0.22f * s,
                0.06f * s,
                0.08f * s,
                237, 200, 165);

    drawEllipse(x, y + 0.24f * s,
                0.070f * s,
                0.055f * s,
                20, 15, 15);

    drawRect(x - 0.070f * s,
             y + 0.12f * s,
             x - 0.030f * s,
             y + 0.24f * s,
             20, 15, 15);

    drawRect(x + 0.030f * s,
             y + 0.12f * s,
             x + 0.070f * s,
             y + 0.24f * s,
             20, 15, 15);

    drawRect(x - 0.065f * s,
             y + 0.10f * s,
             x + 0.065f * s,
             y + 0.18f * s,
             220, 70, 120);

    drawQuad(
        x - 0.09f * s,
        y + 0.10f * s,
        x + 0.09f * s,
        y + 0.10f * s,
        x + 0.06f * s,
        y - 0.10f * s,
        x - 0.06f * s,
        y - 0.10f * s,
        170, 25, 70
    );

    drawRect(x - 0.10f * s,
             y + 0.10f * s,
             x - 0.06f * s,
             y + 0.14f * s,
             237, 200, 165);

    drawRect(x + 0.06f * s,
             y + 0.10f * s,
             x + 0.10f * s,
             y + 0.14f * s,
             237, 200, 165);

  
}

void drawWalkingLegsBoy(float x, float y)
{
    float s = 0.70f;

    glPushMatrix();

    glTranslatef(
        x - 0.02f * s,
        y - 0.08f * s,
        0.0f
    );

    glRotatef(
        legSwing,
        0.0f,
        0.0f,
        1.0f
    );

    drawRect(
        -0.012f,
        -0.12f,
         0.012f,
         0.0f,
        30,30,40
    );

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        x + 0.02f * s,
        y - 0.08f * s,
        0.0f
    );

    glRotatef(
        -legSwing,
        0.0f,
        0.0f,
        1.0f
    );

    drawRect(
        -0.012f,
        -0.12f,
         0.012f,
         0.0f,
        30,30,40
    );

    glPopMatrix();
}


void drawWalkingBoyFull(float x, float y,
                        int r, int g, int b)
{
    drawBoyNoLegs(x, y, r, g, b);

    drawWalkingLegsBoy(x, y);
}

void drawWalkingLegsGirl(float x, float y)
{
    float s = 0.70f;

    glPushMatrix();

    glTranslatef(
        x - 0.015f * s,
        y - 0.10f * s,
        0.0f
    );

    glRotatef(
        legSwing,
        0.0f,
        0.0f,
        1.0f
    );

    drawRect(
        -0.008f,
        -0.11f,
         0.008f,
         0.0f,
        237,200,165
    );

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        x + 0.015f * s,
        y - 0.10f * s,
        0.0f
    );

    glRotatef(
        -legSwing,
        0.0f,
        0.0f,
        1.0f
    );

    drawRect(
        -0.008f,
        -0.11f,
         0.008f,
         0.0f,
        237,200,165
    );

    glPopMatrix();
}

// ---------------- CAR ----------------

void drawCar(float x, float y)
{
    float s = 1.45f;

    drawEllipse(
        x,
        y - 0.04f,
        0.26f * s,
        0.055f * s,
        0, 0, 0
    );

    drawRect(
        x - 0.26f * s,
        y,
        x + 0.26f * s,
        y + 0.12f * s,
        10, 95, 230
    );

    drawQuad(
        x - 0.14f * s, y + 0.12f * s,
        x + 0.12f * s, y + 0.12f * s,
        x + 0.06f * s, y + 0.22f * s,
        x - 0.09f * s, y + 0.22f * s,
        10, 80, 205
    );

    drawQuad(
        x - 0.105f * s, y + 0.135f * s,
        x - 0.005f * s, y + 0.135f * s,
        x - 0.015f * s, y + 0.205f * s,
        x - 0.085f * s, y + 0.205f * s,
        185, 230, 255
    );

    drawQuad(
        x + 0.025f * s, y + 0.135f * s,
        x + 0.105f * s, y + 0.135f * s,
        x + 0.055f * s, y + 0.205f * s,
        x + 0.010f * s, y + 0.205f * s,
        185, 230, 255
    );

    drawRect(
        x + 0.005f * s,
        y + 0.13f * s,
        x + 0.015f * s,
        y + 0.215f * s,
        20, 40, 70
    );

    drawLine(
        x + 0.02f * s,
        y + 0.01f * s,
        x + 0.02f * s,
        y + 0.12f * s,
        20, 40, 70,
        2.2f
    );

    drawRect(
        x + 0.055f * s,
        y + 0.072f * s,
        x + 0.10f * s,
        y + 0.078f * s,
        255, 255, 255
    );

    // FRONT WHEEL
    glPushMatrix();

    glTranslatef(x - 0.17f * s, y, 0.0f);

    glRotatef(wheelRotation, 0.0f, 0.0f, 1.0f);

    drawCircle(0, 0, 0.060f * s, 25, 25, 30);

    drawCircle(0, 0, 0.028f * s, 195, 195, 195);

    drawLine(
        -0.05f * s,
        0,
        0.05f * s,
        0,
        255,255,255,
        2
    );

    drawLine(
        0,
        -0.05f * s,
        0,
        0.05f * s,
        255,255,255,
        2
    );

    glPopMatrix();

    // BACK WHEEL
    glPushMatrix();

    glTranslatef(x + 0.17f * s, y, 0.0f);

    glRotatef(wheelRotation, 0.0f, 0.0f, 1.0f);

    drawCircle(0, 0, 0.060f * s, 25, 25, 30);

    drawCircle(0, 0, 0.028f * s, 195, 195, 195);

    drawLine(
        -0.05f * s,
        0,
        0.05f * s,
        0,
        255,255,255,
        2
    );

    drawLine(
        0,
        -0.05f * s,
        0,
        0.05f * s,
        255,255,255,
        2
    );

    glPopMatrix();

    drawCircle(
        x + 0.26f * s,
        y + 0.07f * s,
        0.022f * s,
        255, 240, 120
    );

    drawRect(
        x - 0.27f * s,
        y + 0.03f * s,
        x - 0.24f * s,
        y + 0.08f * s,
        255, 60, 60
    );
}

// ---------------- ANIMATION ----------------

void updateAnimation()
{
    cloudMove += 0.0035f * scene5_speed;

    if (!scene5_popupCompleted && coupleMove >= scene5_popupTrigger)
    {
        scene5_popupStarted = true;
    }

    if (scene5_popupStarted && !scene5_popupCompleted)
    {
        scene5_popupTimer++;
        if (scene5_popupPhase == 0)
        {
            if (scene5_popupTimer > scene5_popupDelayFrames)
            {
                scene5_popupPhase = 1;
                scene5_popupTimer = 0;
            }
        }
        else if (scene5_popupPhase == 1)
        {
            if (scene5_popupTimer > scene5_popupFramesFirst)
            {
                scene5_popupPhase = 2;
                scene5_popupTimer = 0;
            }
        }
        else if (scene5_popupPhase == 2)
        {
            if (scene5_popupTimer > scene5_popupFramesSecond)
            {
                scene5_popupPhase = 0;
                scene5_popupCompleted = true;
                dialogueFinished = true;
            }
        }
    }

    if(coupleMove < 0.80f)
{
    coupleMove += 0.0008f * scene5_speed;

    legSwing = sin(glfwGetTime() * 7.0f * scene5_speed) * 18.0f;
}
   else
{
    // lonely boy walks AFTER dialogue
    if(dialogueFinished)
    {
        if(!boyEnteredCar)
        {
            legSwing =
                sin(glfwGetTime() * 8.0f * scene5_speed) * 18.0f;

            boyMoveX -= 0.00055f * scene5_speed;
            boyMoveY -= 0.00042f * scene5_speed;

            if(boyMoveX <= -0.35f)
            {
                boyEnteredCar = true;
                carStarted = true;
            }
        }
    }
}

    if(carStarted)
    {
        carMove += 0.0036f * scene5_speed;

        wheelRotation -= 2.5f * scene5_speed;

        if(wheelRotation <= -360.0f)
        {
            wheelRotation = 0.0f;
        }
    }
}

// ---------------- DISPLAY ----------------

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();

    drawSun();

    drawCloud(
        -0.75f + fmod(cloudMove * 0.05f, 2.4f),
        0.70f
    );

    drawCloud(
        -0.05f + fmod(cloudMove * 0.035f, 2.4f),
        0.52f
    );

    drawCloud(
         0.55f + fmod(cloudMove * 0.045f, 2.4f),
         0.67f
    );

    drawRiver();

    drawBridge();

    // MOVING COUPLE
    glPushMatrix();

    glTranslatef(coupleMove, 0.0f, 0.0f);

   drawGirl(-0.65f, -0.62f);

drawWalkingLegsGirl(
    -0.65f,
    -0.62f
);

drawBoyNoLegs(
    -0.45f,
    -0.62f,
    45, 145, 60
);

drawWalkingLegsBoy(
    -0.45f,
    -0.62f
);

    glPopMatrix();

    // ALONE BOY
    // ALONE BOY
if(!boyEnteredCar)
{
    glPushMatrix();

    glTranslatef(boyMoveX, boyMoveY, 0.0f);

    // walking animation while moving
    if(coupleMove >= 0.80f)
    {
        legSwing = sin(glfwGetTime() * 8.0f * scene5_speed) * 18.0f;

        drawWalkingBoyFull(
             0.72f,
            -0.48f,
            30, 90, 190
        );
    }
    else
    {
        drawBoy(
             0.72f,
            -0.48f,
            30, 90, 190
        );
    }

    glPopMatrix();
}

    // MOVING CAR
    glPushMatrix();

    glTranslatef(carMove, 0.0f, 0.0f);

    drawCar(
         0.02f,
        -0.83f
    );

    glPopMatrix();

    double scene5_now = glfwGetTime();
    if (scene5_titleStart < 0.0)
    {
        scene5_titleStart = scene5_now;
    }

    if (scene5_now - scene5_titleStart <= scene5_titleDuration)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        scene5_drawStrokeLeft(-0.95f, 0.88f, "After 15 years...", GLUT_STROKE_ROMAN, 0.0008f);
    }

    if (scene5_popupPhase == 1)
    {
        scene5_drawMessageBubble(
            coupleMove - 0.55f,
            -0.32f,
            "Hey isn't he",
            "your ex....",
            nullptr
        );
    }
    else if (scene5_popupPhase == 2)
    {
        scene5_drawMessageBubble(
            coupleMove - 0.55f,
            -0.34f,
            "Yeah, but he was poor,",
            "that's why I moved to you,",
            "how he became so rich, I regret"
        );
    }

    glFlush();
}

// ---------------- MAIN ----------------

int main()
{
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window;

    window = glfwCreateWindow(
        1200,
        800,
        "Bridge Scene",
        NULL,
        NULL
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glOrtho(
        -1,
         1,
        -1,
         1,
        -1,
         1
    );

    glMatrixMode(GL_MODELVIEW);

    while (!glfwWindowShouldClose(window))
    {
        updateAnimation();

        display();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}