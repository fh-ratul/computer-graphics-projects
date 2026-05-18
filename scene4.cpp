#include <GL/glut.h>
#include <cmath>

#if !defined(_WIN32)
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

// ================= STATE =================
const float horizonY = -0.05f;

const float moonStartX = -0.75f;
const float moonStartY =  0.75f;
const float moonEndX   =  0.85f;
const float moonEndY   = -0.25f;
const float scene4_moonStep = 0.0005714f;

#if !defined(_WIN32)
static pid_t scene4_musicPid = -1;
static unsigned int scene4_musicSession = 0;
static const char* scene4_musicPath = "music/music4.mp3";

void scene4StopMusic();
void scene4MusicTimeout(int value);

void scene4StartMusic(){
    scene4StopMusic();
    ++scene4_musicSession;

    pid_t pid = fork();
    if (pid == 0) {
        execlp("cvlc",
               "cvlc",
               "--intf", "dummy",
               "--quiet",
               "--play-and-exit",
               "--no-video",
               scene4_musicPath,
               (char*)nullptr);
        _exit(1);
    }

    if (pid > 0) {
        scene4_musicPid = pid;
        glutTimerFunc(240000, scene4MusicTimeout, static_cast<int>(scene4_musicSession));
    }
}

void scene4StopMusic(){
    ++scene4_musicSession;

    if (scene4_musicPid <= 0) {
        return;
    }

    kill(scene4_musicPid, SIGTERM);

    for (int i = 0; i < 20; ++i) {
        int status = 0;
        pid_t result = waitpid(scene4_musicPid, &status, WNOHANG);
        if (result == scene4_musicPid) {
            scene4_musicPid = -1;
            return;
        }
        usleep(50000);
    }

    kill(scene4_musicPid, SIGKILL);
    waitpid(scene4_musicPid, nullptr, 0);
    scene4_musicPid = -1;
}

void scene4MusicTimeout(int value){
    if (value != static_cast<int>(scene4_musicSession)) {
        return;
    }
    scene4StopMusic();
}
#else
void scene4StartMusic() {}
void scene4StopMusic() {}
#endif

float moonX = moonStartX;
float moonY = moonStartY;
float t = 0.0f;
bool animate = false;
float morningFactor = 0.0f;
int messagePhase = 0;
int messageTimer = 0;

const int messageDelayFrames = 90;
const int messageFramesFirst = 180;
const int messageFramesSecond = 220;

// ================= BASIC =================
float clamp01(float v){ if(v<0) return 0; if(v>1) return 1; return v; }

void setColor(float r,float g,float b){ glColor3f(clamp01(r),clamp01(g),clamp01(b)); }

void setColorLit(float r,float g,float b){
    float lift = 0.18f * morningFactor;
    setColor(r + lift, g + lift, b + lift * 0.8f);
}

void drawRect(float x1,float y1,float x2,float y2,float r,float g,float b){
    setColor(r,g,b);
    glBegin(GL_QUADS);
    glVertex2f(x1,y1); glVertex2f(x2,y1);
    glVertex2f(x2,y2); glVertex2f(x1,y2);
    glEnd();
}

void drawRectLit(float x1,float y1,float x2,float y2,float r,float g,float b){
    setColorLit(r,g,b);
    glBegin(GL_QUADS);
    glVertex2f(x1,y1); glVertex2f(x2,y1);
    glVertex2f(x2,y2); glVertex2f(x1,y2);
    glEnd();
}

void drawCircle(float cx,float cy,float r,float cr,float cg,float cb){
    setColor(cr,cg,cb);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=50;i++){
        float a=2*3.1416f*i/50;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }
    glEnd();
}

void drawCircleLit(float cx,float cy,float r,float cr,float cg,float cb){
    setColorLit(cr,cg,cb);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=50;i++){
        float a=2*3.1416f*i/50;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }
    glEnd();
}

void drawRoundedRect(float x,float y,float w,float h,float radius){
    float r = radius;
    if(r < 0.0f) r = 0.0f;
    if(r > h * 0.5f) r = h * 0.5f;
    if(r > w * 0.5f) r = w * 0.5f;

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
    for(int i=0;i<=segments;i++){
        float a = 3.1416f + (3.1416f/2.0f) * (float)i / segments;
        glVertex2f(x + r + cos(a) * r, y + r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + w - r, y + r);
    for(int i=0;i<=segments;i++){
        float a = (3.1416f/2.0f) * (float)i / segments;
        glVertex2f(x + w - r + cos(a) * r, y + r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + w - r, y + h - r);
    for(int i=0;i<=segments;i++){
        float a = 0.0f + (3.1416f/2.0f) * (float)i / segments;
        glVertex2f(x + w - r + cos(a) * r, y + h - r + sin(a) * r);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + r, y + h - r);
    for(int i=0;i<=segments;i++){
        float a = 3.1416f/2.0f + (3.1416f/2.0f) * (float)i / segments;
        glVertex2f(x + r + cos(a) * r, y + h - r + sin(a) * r);
    }
    glEnd();
}

int textWidth(void* font, const char* text){
    int w = 0;
    for(const char* p = text; *p; ++p){
        w += glutBitmapWidth(font, *p);
    }
    return w;
}

void drawTextCentered(float x,float y,const char* text, void* font){
    int w = textWidth(font, text);
    float px = x - (float)w / 2.0f;
    glRasterPos2f(px, y);
    for(const char* p = text; *p; ++p){
        glutBitmapCharacter(font, *p);
    }
}

float strokeTextWidth(void* font, const char* text){
    float w = 0.0f;
    for(const char* p = text; *p; ++p){
        w += (float)glutStrokeWidth(font, *p);
    }
    return w;
}

void drawStrokeCentered(float x, float y, const char* text, void* font, float scale){
    float w = strokeTextWidth(font, text) * scale;
    glPushMatrix();
    glTranslatef(x - w * 0.5f, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(1.8f);
    for(const char* p = text; *p; ++p){
        glutStrokeCharacter(font, *p);
    }
    glLineWidth(1.0f);
    glPopMatrix();
}

void drawMessageBubble(const char* line1, const char* line2){
    const float bubbleW = 0.65f;
    const float bubbleH = 0.18f;
    const float bubbleX = -bubbleW * 0.5f;
    const float bubbleY = -0.05f;

    glColor3f(1.0f, 1.0f, 1.0f);
    drawRoundedRect(bubbleX, bubbleY, bubbleW, bubbleH, 0.035f);

    glColor3f(0.0f, 0.0f, 0.0f);
    drawStrokeCentered(0.0f, bubbleY + 0.115f, line1, GLUT_STROKE_ROMAN, 0.0003f);
    drawStrokeCentered(0.0f, bubbleY + 0.055f, line2, GLUT_STROKE_ROMAN, 0.0003f);
}

void drawStars(){
    float fade = 1.0f - morningFactor;
    if(fade<=0.0f) return;

    glPointSize(2.0f);
    setColor(0.92f*fade,0.92f*fade,0.98f*fade);
    glBegin(GL_POINTS);
    glVertex2f(-0.80f,0.82f);
    glVertex2f(-0.62f,0.88f);
    glVertex2f(-0.50f,0.76f);
    glVertex2f(-0.30f,0.86f);
    glVertex2f(-0.08f,0.80f);
    glVertex2f( 0.16f,0.88f);
    glVertex2f( 0.40f,0.82f);
    glVertex2f( 0.68f,0.86f);
    glVertex2f( 0.86f,0.76f);
    glVertex2f( 0.06f,0.70f);
    glVertex2f(-0.36f,0.70f);
    glVertex2f( 0.56f,0.68f);
    glEnd();
}

void startMoonAnimation(){
    animate = true;
    t = 0.0f;
    morningFactor = 0.0f;
    moonX = moonStartX;
    moonY = moonStartY;
    messagePhase = 0;
    messageTimer = 0;
}

// ================= MOON =================
void drawMoon(){
    drawCircle(0,0,0.12f,0.95f,0.93f,0.70f);
    drawCircle(0.05f,0.04f,0.12f,0.02f,0.02f,0.08f);
}

// ================= BUILDING =================
void drawBuilding(float x,float y,float w,float h){
    drawRectLit(x,y,x+w,y+h,0.05f,0.05f,0.05f);

    int rows = 5;
    int cols = 3;
    float marginX = 0.04f;
    float marginTop = 0.06f;
    float marginBottom = 0.06f;
    float gapX = 0.04f;
    float gapY = 0.05f;
    float winW = (w - 2.0f * marginX - (cols - 1) * gapX) / cols;
    float winH = (h - marginTop - marginBottom - (rows - 1) * gapY) / rows;

    if(winW > 0.0f && winH > 0.0f){
        for(int r=0;r<rows;r++){
            for(int c=0;c<cols;c++){
                float wx = x + marginX + c * (winW + gapX);
                float wy = y + marginBottom + r * (winH + gapY);

                float intensity = (1.0f - morningFactor) - (r*c%3)*0.05f;
                if(intensity<0) intensity=0;

                drawRect(wx,wy,wx+winW,wy+winH,
                         0.9f*intensity,0.8f*intensity,0.3f*intensity);
            }
        }
    }
}

// ================= SHADOW =================
void drawShadow(float x,float y,float w){
    if(morningFactor <= 0.0f) return;

    float dx = (0.0f - moonX);
    float dy = (-0.6f - moonY);

    float mag = sqrt(dx*dx + dy*dy);
    dx/=mag; dy/=mag;

    float len = 0.15f + (1.0f - moonY)*0.2f;

    float alpha = 0.08f + 0.12f * morningFactor;
    glColor4f(0.0f,0.0f,0.0f,alpha);
    glBegin(GL_QUADS);
    glVertex2f(x,y);
    glVertex2f(x+w,y);
    glVertex2f(x+w+dx*len,y+dy*len);
    glVertex2f(x+dx*len,y+dy*len);
    glEnd();
}

// ================= PERSON =================
void drawPerson(float x,float y){
    // head + hair
    drawCircle(x,y+0.28f,0.05f,0.95f,0.78f,0.65f);
    drawCircle(x,y+0.31f,0.055f,0.08f,0.08f,0.08f);

    // blue t-shirt
    drawRect(x-0.07f,y+0.14f,x+0.07f,y+0.26f,0.16f,0.28f,0.70f);
    // pants aligned with torso width
    drawRect(x-0.08f,y+0.08f,x+0.08f,y+0.15f,0.08f,0.12f,0.30f);

    // letter near hand
    drawRect(x+0.08f,y+0.16f,x+0.14f,y+0.20f,0.98f,0.98f,0.98f);
}

// ================= BENCH =================
void drawBench(float x,float y){
    // dark top plank
    drawRect(x-0.26f,y+0.16f,x+0.26f,y+0.26f,0.20f,0.12f,0.07f);
    // lighter front face
    drawRect(x-0.26f,y+0.08f,x+0.26f,y+0.16f,0.32f,0.20f,0.12f);

    // two legs
    drawRect(x-0.22f,y-0.04f,x-0.18f,y+0.08f,0.18f,0.12f,0.08f);
    drawRect(x+0.18f,y-0.04f,x+0.22f,y+0.08f,0.18f,0.12f,0.08f);
}

// ================= SKY (COLOR TEMP SHIFT) =================
void drawSky(){
    // night (blue)
    float nr=0.02f, ng=0.02f, nb=0.08f;

    // morning (warm)
    float mr=0.9f, mg=0.6f, mb=0.3f;

    float r = nr*(1-morningFactor) + mr*morningFactor;
    float g = ng*(1-morningFactor) + mg*morningFactor;
    float b = nb*(1-morningFactor) + mb*morningFactor;

    glBegin(GL_QUADS);
    glColor3f(r,g,b);
    glVertex2f(-1,-0.05f); glVertex2f(1,-0.05f);

    glColor3f(0.04f,0.05f,0.12f);
    glVertex2f(1,1); glVertex2f(-1,1);
    glEnd();
}

// ================= DISPLAY =================
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();

    drawStars();

    // moon transform (correct method)
    float moonScale = 1.0f;
    float scaleStart = 0.35f;
    if(moonY < scaleStart){
        float scaleEnd = moonEndY;
        float tScale = clamp01((moonY - scaleEnd) / (scaleStart - scaleEnd));
        moonScale = 0.20f + 0.80f * tScale;
    }
    glPushMatrix();
    glTranslatef(moonX,moonY,0);
    glScalef(moonScale,moonScale,1.0f);
    drawMoon();
    glPopMatrix();

    drawRectLit(-1,-1,1,horizonY,0.06f,0.06f,0.08f);

    drawBuilding(-0.95f,-0.05f,0.22f,0.50f);
    drawBuilding(-0.72f,-0.05f,0.24f,0.58f);
    drawBuilding(-0.48f,-0.05f,0.26f,0.72f);
    drawBuilding(-0.18f,-0.05f,0.22f,0.55f);
    drawBuilding( 0.08f,-0.05f,0.26f,0.68f);
    //drawBuilding( 0.38f,-0.05f,0.24f,0.60f);
    drawBuilding( 0.66f,-0.05f,0.24f,0.56f);

    drawShadow(-0.26f,-0.60f,0.52f);
    drawShadow(-0.08f,-0.60f,0.16f);
    drawBench(0,-0.6f);
    drawPerson(0,-0.6f);

    if(messagePhase == 1){
        drawMessageBubble("Why she did this", "to me?");
    } else if(messagePhase == 2){
        drawMessageBubble("Let success be", "the best REVENGE");
    }

    glutSwapBuffers();
}

// ================= UPDATE =================
void update(int v){
    if(animate){
        t += scene4_moonStep;
        if(t>1) t=1;

        float ease = t * t * (3.0f - 2.0f * t);

        moonX = moonStartX + (moonEndX - moonStartX) * ease;
        moonY = moonStartY + (moonEndY - moonStartY) * ease;

        if(t>0.55f){
            morningFactor = (t - 0.55f) / 0.45f;
            morningFactor = clamp01(morningFactor);
        }

        if(t>=1) animate = false;
    }

    messageTimer++;
    if(messagePhase == 0){
        if(messageTimer > messageDelayFrames){
            messagePhase = 1;
            messageTimer = 0;
        }
    } else if(messagePhase == 1){
        if(messageTimer > messageFramesFirst){
            messagePhase = 2;
            messageTimer = 0;
        }
    } else if(messagePhase == 2){
        if(messageTimer > messageFramesSecond){
            messagePhase = 0;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

// ================= INPUT =================
void keyboard(unsigned char key,int x,int y){
    if(key=='m'||key=='M'){
        startMoonAnimation();
    }
}

// ================= INIT =================
void init(){
    glClearColor(0,0,0,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
    scene4StartMusic();
    startMoonAnimation();
}

// ================= MAIN =================
int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(1000,600);
    glutCreateWindow("Moon Transition");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,update,0);

    glutMainLoop();
    return 0;
}