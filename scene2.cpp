#include <GL/glut.h>

#include <math.h>
#include <stdlib.h>

#if !defined(_WIN32)
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

// ---------------- GLOBAL VARIABLES ----------------
float rainOffset = 0.0f;
float cloudOffset = 0.0f;
float walkOffset = -1.0f;
float scaleFactor = 1.0f;

int rainMode = 1;

// ⚡ THUNDER
int thunderOn = 0;
int thunderTimer = 0;
float flashAlpha = 0.0f;
float lightningX = 0.0f;

// 🌂 NEW ROTATION VARIABLE
float umbrellaRotate = 0.0f;

int scene2_loopCount = 0;
const int scene2_loopMax = 2;
bool scene2_pause = false;

#if !defined(_WIN32)
static pid_t scene2_musicPid = -1;
static unsigned int scene2_musicSession = 0;
static const char* scene2_musicPath = "music/music2.mp3";

void scene2StopMusic();
void scene2MusicTimeout(int value);

void scene2StartMusic()
{
    scene2StopMusic();
    ++scene2_musicSession;

    pid_t pid = fork();
    if (pid == 0) {
        execlp("cvlc",
               "cvlc",
               "--intf", "dummy",
               "--quiet",
               "--play-and-exit",
               "--no-video",
               scene2_musicPath,
               (char*)nullptr);
        _exit(1);
    }

    if (pid > 0) {
        scene2_musicPid = pid;
        glutTimerFunc(240000, scene2MusicTimeout, static_cast<int>(scene2_musicSession));
    }
}

void scene2StopMusic()
{
    ++scene2_musicSession;

    if (scene2_musicPid <= 0) {
        return;
    }

    kill(scene2_musicPid, SIGTERM);

    for (int i = 0; i < 20; ++i) {
        int status = 0;
        pid_t result = waitpid(scene2_musicPid, &status, WNOHANG);
        if (result == scene2_musicPid) {
            scene2_musicPid = -1;
            return;
        }
        usleep(50000);
    }

    kill(scene2_musicPid, SIGKILL);
    waitpid(scene2_musicPid, nullptr, 0);
    scene2_musicPid = -1;
}

void scene2MusicTimeout(int value)
{
    if (value != static_cast<int>(scene2_musicSession)) {
        return;
    }
    scene2StopMusic();
}
#else
void scene2StartMusic() {}
void scene2StopMusic() {}
#endif

// ---------------- INIT ----------------
void init()
{
    rainOffset = 0.0f;
    cloudOffset = 0.0f;
    walkOffset = -1.0f;
    scaleFactor = 1.0f;
    rainMode = 1;
    thunderOn = 0;
    thunderTimer = 0;
    flashAlpha = 0.0f;
    lightningX = 0.0f;
    umbrellaRotate = 0.0f;
    scene2_loopCount = 0;
    scene2_pause = false;

    glClearColor(0.05,0.05,0.15,1);
    gluOrtho2D(-1,1,-1,1);
    glDisable(GL_BLEND);

    scene2StartMusic();
}

// ---------------- CIRCLE ----------------
void drawCircle(float cx,float cy,float r)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);

    for(int i=0;i<=100;i++)
    {
        float a = 2*3.1416*i/100;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }

    glEnd();
}

// ---------------- RAIN ----------------
void drawRain()
{
    glColor3f(0.6,0.7,0.9);

    glBegin(GL_LINES);

    for(float i=-1;i<=1;i+=0.08)
    {
        for(float j=1;j>=-1;j-=0.2)
        {
            float y = j - rainOffset;

            if(y < -1)
                y += 2;

            glVertex2f(i,y);
            glVertex2f(i+0.03,y-0.12);
        }
    }

    glEnd();
}

// ---------------- LAMP ----------------
void drawLamp(float x, float y)
{
    // pole
    glColor3f(0,0,0);

    glBegin(GL_QUADS);
    glVertex2f(x-0.012,y);
    glVertex2f(x+0.012,y);
    glVertex2f(x+0.012,y+0.32);
    glVertex2f(x-0.012,y+0.32);
    glEnd();

    float headY = y + 0.34;

    // head
    drawCircle(x, headY, 0.022);

    // glow
    for(float r=0.02;r<=0.08;r+=0.015)
    {
        float alpha = 0.09 - r*0.6;

        glColor4f(1.0,0.95,0.8,alpha);

        drawCircle(x, headY, r);
    }

    // reflection
    float startY = headY - 0.12f;
    float endY   = -0.75f;

    int layers = 25;

    for(int i=0;i<layers;i++)
    {
        float t1 = (float)i / layers;
        float t2 = (float)(i+1) / layers;

        float y1 = startY - t1 * (startY - endY);
        float y2 = startY - t2 * (startY - endY);

        float w1 = 0.02 + t1 * 0.12;
        float w2 = 0.02 + t2 * 0.12;

        float a1 = 0.35 * (1.0f - t1);
        float a2 = 0.35 * (1.0f - t2);

        glBegin(GL_QUADS);

        glColor4f(1.0,0.95,0.8,a1);
        glVertex2f(x - w1, y1);
        glVertex2f(x + w1, y1);

        glColor4f(1.0,0.95,0.8,a2);
        glVertex2f(x + w2, y2);
        glVertex2f(x - w2, y2);

        glEnd();
    }
}

// ---------------- BOY ----------------
void drawBoy()
{
    glColor3f(1,0.8,0.6);
    drawCircle(-0.03,-0.22,0.04);

    glColor3f(0,0,0);
    drawCircle(-0.03,-0.20,0.045);

    glColor3f(0.2,0.4,0.9);

    glBegin(GL_POLYGON);
    glVertex2f(-0.07,-0.26);
    glVertex2f(0.01,-0.26);
    glVertex2f(0.00,-0.52);
    glVertex2f(-0.08,-0.52);
    glEnd();

    glColor3f(0,0,0);

    glBegin(GL_QUADS);

    glVertex2f(-0.06,-0.52);
    glVertex2f(-0.03,-0.52);
    glVertex2f(-0.03,-0.75);
    glVertex2f(-0.06,-0.75);

    glVertex2f(-0.01,-0.52);
    glVertex2f(0.02,-0.52);
    glVertex2f(0.02,-0.75);
    glVertex2f(-0.01,-0.75);

    glEnd();
}

// ---------------- GIRL ----------------
void drawGirl()
{
    glColor3f(1,0.8,0.6);
    drawCircle(0.03,-0.22,0.04);

    glColor3f(0.2,0.05,0.05);

    glBegin(GL_POLYGON);
    glVertex2f(-0.01,-0.18);
    glVertex2f(0.07,-0.18);
    glVertex2f(0.08,-0.45);
    glVertex2f(-0.02,-0.45);
    glEnd();

    glColor3f(1,0.2,0.5);

    glBegin(GL_POLYGON);
    glVertex2f(-0.01,-0.26);
    glVertex2f(0.07,-0.26);
    glVertex2f(0.10,-0.60);
    glVertex2f(-0.04,-0.60);
    glEnd();

    glColor3f(0,0,0);

    glBegin(GL_QUADS);

    glVertex2f(0.02,-0.60);
    glVertex2f(0.05,-0.60);
    glVertex2f(0.05,-0.80);
    glVertex2f(0.02,-0.80);

    glVertex2f(0.07,-0.60);
    glVertex2f(0.10,-0.60);
    glVertex2f(0.10,-0.80);
    glVertex2f(0.07,-0.80);

    glEnd();
}

// ---------------- UMBRELLA WITH ROTATION ----------------
void drawUmbrella()
{
    float cx=0.0f;
    float cy=-0.15f;
    float r=0.20f;

    glPushMatrix();

    // rotation effect
    glTranslatef(cx, cy, 0);
    glRotatef(umbrellaRotate, 0, 0, 1);
    glTranslatef(-cx, -cy, 0);

    // umbrella top
    glColor3f(1,0,0);

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(cx,cy);

    for(int i=0;i<=100;i++)
    {
        float a=3.1416*i/100;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }

    glEnd();

    // handle
    glColor3f(0,0,0);

    glBegin(GL_LINES);
    glVertex2f(cx,cy);
    glVertex2f(cx,-0.22f);
    glEnd();

    glPopMatrix();
}

// ---------------- LIGHTNING ----------------
void drawLightning()
{
    if(!thunderOn)
        return;

    glColor3f(1,1,1);

    glBegin(GL_LINE_STRIP);

    float x = lightningX;
    float y = 1.0f;

    for(int i=0;i<8;i++)
    {
        float nx = x + ((rand()%100)/100.0f - 0.5f)*0.12f;
        float ny = y - 0.15f;

        glVertex2f(x,y);
        glVertex2f(nx,ny);

        x=nx;
        y=ny;
    }

    glEnd();
}

// ---------------- UPDATE ----------------
void update(int v)
{
    if (scene2_pause)
    {
        glutPostRedisplay();
        glutTimerFunc(16,update,0);
        return;
    }

    rainOffset+=0.02;

    // 🌂 umbrella rotation animation
    umbrellaRotate = sin(rainOffset * 2.0f) * 10.0f;

    if(rainOffset>2)
        rainOffset=0;

    cloudOffset+=0.0012f;

    if(cloudOffset>2)
        cloudOffset=0;

    walkOffset+=0.002f;

    if(walkOffset>0.2f)
    {
        walkOffset=-1.0f;
        scene2_loopCount++;
        if (scene2_loopCount >= scene2_loopMax)
        {
            scene2_pause = true;
        }
    }

    scaleFactor = 1.0f - ((walkOffset + 1.0f)*0.5f);

    if(thunderOn)
    {
        thunderTimer++;

        flashAlpha = (rand()%10 < 3) ? 0.6f : 0.0f;

        if(thunderTimer > 120)
        {
            thunderOn = 0;
            thunderTimer = 0;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

// ---------------- KEYBOARD ----------------
void keyboard(unsigned char key, int x, int y)
{
    if(key=='r')
        rainMode = !rainMode;

    if(key=='t' && rainMode==1)
    {
        thunderOn = 1;
        lightningX = ((rand()%200)/100.0f)-1.0f;
    }
}

// ---------------- DISPLAY ----------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float s = flashAlpha * 0.5f;

    // sky
    glBegin(GL_QUADS);

    glColor3f(0.05+s,0.05+s,0.15+s);
    glVertex2f(-1,1);
    glVertex2f(1,1);
    glVertex2f(1,0);
    glVertex2f(-1,0);

    glEnd();

    // moon
    if(!rainMode)
    {
        glColor3f(1,1,0.85);
        drawCircle(0.6,0.7,0.06);
    }

    // land
    glBegin(GL_QUADS);

    glColor3f(0.05,0.3,0.1);
    glVertex2f(-1,0);
    glVertex2f(1,0);
    glVertex2f(1,-1);
    glVertex2f(-1,-1);

    glEnd();

    // road
    glBegin(GL_QUADS);

    glColor3f(0.1,0.1,0.1);
    glVertex2f(-0.2,0);
    glVertex2f(0.2,0);
    glVertex2f(0.5,-1);
    glVertex2f(-0.5,-1);

    glEnd();

    // clouds
    if(rainMode)
    {
        glColor3f(0.25,0.25,0.3);

        for(int i=0;i<3;i++)
        {
            glPushMatrix();

            glTranslatef(cloudOffset - i*1.5f,0,0);

            drawCircle(-0.8,0.85,0.12);
            drawCircle(-0.7,0.85,0.16);
            drawCircle(-0.6,0.85,0.12);

            glPopMatrix();
        }
    }

    drawLightning();

    drawLamp(-0.45f,-0.5f);
    drawLamp(0.45f,-0.5f);

    // characters
    glPushMatrix();

    glTranslatef(0,walkOffset,0);
    glScalef(scaleFactor,scaleFactor,1);

    drawBoy();
    drawGirl();

    if(rainMode)
        drawUmbrella();

    glPopMatrix();

    // rain
    if(rainMode)
        drawRain();

    glFlush();
}

// ---------------- MAIN ----------------
int main(int argc,char** argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(720,480);

    glutCreateWindow("FINAL CINEMATIC SCENE");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,update,0);

    glutMainLoop();

    return 0;
}