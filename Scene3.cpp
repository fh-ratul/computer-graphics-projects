#include<windows.h>
#include <GL/glut.h>
#define PI 3.1416
#include<math.h>


// GLOBAL
float trainX = -2.4f;
float cloudX = 0.0f;
float boyX = -0.85f;
float sunY = 0.84f;
float clockAngle = 0;
float moonY = 0.3f;
float moonX = 0.50f;
float sunAngle = 0;
float breakOffset = 0;
float legAngle = 0;
bool sunrise = false;
bool moonDown = false;



// states
bool trainMove = false;
bool trainGone = false;
bool boyRun = false;
bool sunStart = false;

float minuteAngle = 0;
float hourAngle = 0;

bool heartShow = false;
int heartTimer = 0;

bool heartBreak = false;
float heartOffset = 0;

bool night = false;
float dayMix = 0;

bool nightTrain = false;
float nightTrainX = -1.5f;



void circle(float cx,float cy,float r){
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=100;i++){
        float a=2*PI*i/100;
        glVertex2f(cx+cos(a)*r,cy+sin(a)*r);
    }
    glEnd();
}

// SKY
void drawSky(){
    float t = dayMix;

    glColor3ub(160*(1-t)+20*t, 185*(1-t)+20*t, 210*(1-t)+60*t);

    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 0.3f);
    glVertex2f( 1.0f, 0.3f);
    glVertex2f( 1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
}

//  CLOUD

    void drawCloud(float x,float y){

    // shadow layer
    glColor3ub(210,210,210);

    circle(x-0.18,y,0.06);
    circle(x-0.10,y+0.04,0.08);
    circle(x,y+0.06,0.10);

    circle(x+0.10,y+0.04,0.08);
    circle(x+0.18,y,0.06);

    // main cloud
    glColor3ub(255,255,255);

    circle(x-0.20,y+0.01,0.07);
    circle(x-0.12,y+0.035,0.075);

    circle(x,y+0.045,0.09);

    circle(x+0.12,y+0.035,0.075);
    circle(x+0.20,y+0.01,0.07);

    // lower fluffy layer
    circle(x-0.08,y-0.01,0.07);
    circle(x+0.08,y-0.01,0.07);

    // extra tiny cloud puffs
    circle(x-0.15,y+0.02,0.03);
   circle(x-0.05,y+0.06,0.022);

    circle(x+0.05,y+0.065,0.025);
    circle(x+0.16,y+0.03,0.025);

    circle(x-0.02,y+0.02,0.02);
    circle(x+0.02,y+0.05,0.018);

    // bottom smooth base
    glBegin(GL_QUADS);

    glVertex2f(x-0.22,y-0.05);
    glVertex2f(x+0.22,y-0.05);

    glVertex2f(x+0.22,y+0.03);
    glVertex2f(x-0.22,y+0.03);

    glEnd();
}
// SUN

void drawSun(){

    if(sunY > 0.35f){

        float sunSize = 0.10f * (sunY / 0.85f);

        if(sunSize < 0.02f)
            sunSize = 0.02f;

        // gradient sun
        glBegin(GL_TRIANGLE_FAN);

        // center bright
        glColor3ub(255,255,180);
        glVertex2f(0.35,sunY);

        // outer orange
        for(int i=0;i<=100;i++){

            float a = 2*PI*i/100;

            float x = 0.35 + cos(a)*sunSize;
            float y = sin(a)*sunSize + sunY;

            glColor3ub(255,140,0);
            glVertex2f(x,y);
        }

        glEnd();
    }

    // rotating rays
    if(!heartBreak && sunY > 0.5){

        glColor3ub(255,200,0);

        for(int i=0;i<12;i++){

            float angle = sunAngle + i*(PI/6);

            float x1 = 0.35 + cos(angle)*0.12;
            float y1 = sin(angle)*0.12 + sunY;

            float x2 = 0.35 + cos(angle)*0.18;
            float y2 = sin(angle)*0.18 + sunY;

            glBegin(GL_LINES);
            glVertex2f(x1,y1);
            glVertex2f(x2,y2);
            glEnd();
        }
    }
}

void drawBuildings(){

    // LEFT SIDE (moon side ছোট buildings)

    float h[9] = {
        0.55,0.65,0.78,
        0.92,0.98,
        0.82,0.70,
        0.58,0.50
    };

    float x = -1.0f;

    for(int i=0;i<9;i++){

        float w = 0.12f;

        // different colors
        if(i%3==0) glColor3ub(70,90,130);
        if(i%3==1) glColor3ub(90,100,140);
        if(i%3==2) glColor3ub(80,110,150);

        glBegin(GL_QUADS);

        glVertex2f(x,0.30);
        glVertex2f(x+w,0.30);

        glVertex2f(x+w,h[i]);
        glVertex2f(x,h[i]);

        glEnd();

        // windows
        for(float wy=0.36; wy<h[i]-0.05; wy+=0.08){

            for(float wx=x+0.02; wx<x+w-0.04; wx+=0.05){

                glColor3ub(255,240,120);

                glBegin(GL_QUADS);

                glVertex2f(wx,wy);
                glVertex2f(wx+0.025,wy);

                glVertex2f(wx+0.025,wy+0.04);
                glVertex2f(wx,wy+0.04);

                glEnd();
            }
        }

        x += 0.13f;
    }
}

//Clock
void drawClock(){

    float cx = -0.20f;
    float cy = 0.30f;

    // shadow
    glColor3ub(120,120,120);
    circle(cx+0.01f,cy-0.01f,0.075f);

    // outer border
    glColor3ub(30,30,30);
    circle(cx,cy,0.075f);

    // inner border
    glColor3ub(200,180,120);
    circle(cx,cy,0.070f);

    // clock body
    glColor3ub(245,245,245);
    circle(cx,cy,0.062f);

    // numbers/dots
    for(int i=0;i<12;i++){

        float a = 2*PI*i/12;

        float x = cx + sin(a)*0.050f;
        float y = cy + cos(a)*0.050f;

        // main dots
        glColor3ub(0,0,0);
        circle(x,y,0.004f);
    }

    // center
    glColor3ub(0,0,0);
    circle(cx,cy,0.006f);

    // minute hand
    float mx = cx + sin(minuteAngle)*0.040f;
    float my = cy + cos(minuteAngle)*0.040f;

    glLineWidth(3);

    glBegin(GL_LINES);
    glVertex2f(cx,cy);
    glVertex2f(mx,my);
    glEnd();

    // hour hand
    float hx = cx + sin(hourAngle)*0.026f;
    float hy = cy + cos(hourAngle)*0.026f;

    glLineWidth(4);

    glBegin(GL_LINES);
    glVertex2f(cx,cy);
    glVertex2f(hx,hy);
    glEnd();

    // hanger
    glColor3ub(60,60,60);

    glBegin(GL_QUADS);

    glVertex2f(cx-0.006f,cy+0.075f);
    glVertex2f(cx+0.006f,cy+0.075f);

    glVertex2f(cx+0.006f,cy+0.11f);
    glVertex2f(cx-0.006f,cy+0.11f);

    glEnd();
}

//Draw Moon
void drawMoon(){

    if(night){

        // moon size grows while rising
        float moonSize;

        if(moonY < 0.85f){

            moonSize = 0.02f + (moonY + 0.2f) * 0.08f;
        }
        else{

            moonSize = 0.09f;
        }

        // glow
        glColor4f(1.0f,1.0f,1.0f,0.18f);
        circle(moonX,moonY,moonSize + 0.03f);

        // moon
        glColor3ub(245,245,230);
        circle(moonX,moonY,moonSize);
    }
}


//Star
void drawStars(){

    if(night){

        //  GLOW STARS
        glColor4f(1.0f,1.0f,1.0f,0.18f);

        circle(-0.60,0.97,0.014);
        circle(0.24,0.97,0.014);
        circle(0.84,0.97,0.014);

        circle(-0.24,0.95,0.012);
        circle(0.48,0.96,0.012);
        circle(0.00,0.90,0.012);

        // MAIN BRIGHT STARS
        glColor3ub(255,255,220);

        // upper sky
        circle(-0.96,0.96,0.0055);
        circle(-0.90,0.90,0.004);
        circle(-0.84,0.98,0.0055);
        circle(-0.78,0.86,0.004);
        circle(-0.72,0.94,0.0055);

        circle(-0.66,0.82,0.004);
        circle(-0.60,0.97,0.0055);
        circle(-0.54,0.88,0.004);
        circle(-0.48,0.93,0.0055);
        circle(-0.42,0.80,0.004);

        circle(-0.36,0.99,0.0055);
        circle(-0.30,0.86,0.004);
        circle(-0.24,0.95,0.0055);
        circle(-0.18,0.83,0.004);
        circle(-0.12,0.91,0.0055);

        circle(-0.06,0.78,0.004);
        circle(0.00,0.98,0.0055);
        circle(0.06,0.88,0.004);
        circle(0.12,0.94,0.0055);
        circle(0.18,0.81,0.004);

        circle(0.24,0.97,0.0055);
        circle(0.30,0.86,0.004);
        circle(0.36,0.93,0.0055);
        circle(0.42,0.79,0.004);
        circle(0.48,0.96,0.0055);

        circle(0.54,0.84,0.004);
        circle(0.60,0.99,0.0055);
        circle(0.66,0.88,0.004);
        circle(0.72,0.95,0.0055);
        circle(0.78,0.82,0.004);

        circle(0.84,0.97,0.0055);
        circle(0.90,0.89,0.004);
        circle(0.96,0.94,0.0055);

        // middle sky
        circle(-0.92,0.72,0.004);
        circle(-0.84,0.76,0.003);
        circle(-0.76,0.70,0.004);

        circle(-0.68,0.74,0.003);
        circle(-0.60,0.68,0.004);
        circle(-0.52,0.77,0.003);

        circle(-0.44,0.71,0.004);
        circle(-0.36,0.75,0.003);
        circle(-0.28,0.69,0.004);

        circle(-0.20,0.73,0.003);
        circle(-0.12,0.67,0.004);
        circle(-0.04,0.76,0.003);

        circle(0.04,0.70,0.004);
        circle(0.12,0.74,0.003);
        circle(0.20,0.68,0.004);

        circle(0.28,0.77,0.003);
        circle(0.36,0.71,0.004);
        circle(0.44,0.75,0.003);

        circle(0.52,0.69,0.004);
        circle(0.60,0.73,0.003);
        circle(0.68,0.67,0.004);

        circle(0.76,0.76,0.003);
        circle(0.84,0.70,0.004);
        circle(0.92,0.74,0.003);
    }
}

//  HEART
void drawHeart(){

    float cx = boyX;
    float cy = -0.3f;

    glColor3ub(255,0,0);

    glBegin(GL_TRIANGLE_FAN);

    // center point
    glVertex2f(cx, cy);

    // heart curve
    for(float t = 0; t <= 2*PI; t += 0.01f){
        float x = 0.003f * (16 * pow(sin(t),3));
        float y = 0.003f * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));

        glVertex2f(cx + x, cy + y);
    }

    glEnd();

    //  crack line
    glColor3ub(255,255,255);
    glLineWidth(2);

    glBegin(GL_LINE_STRIP);
    glVertex2f(cx, cy+0.05);
    glVertex2f(cx-0.01, cy+0.02);
    glVertex2f(cx+0.01, cy);
    glVertex2f(cx-0.01, cy-0.03);
    glVertex2f(cx+0.01, cy-0.06);
    glEnd();

    glLineWidth(1);
}


// - STATION
void drawStation(){

    // main building
    glColor3ub(210,210,210);
    glBegin(GL_QUADS);
    glVertex2f(-1,0.0);
    glVertex2f(1,0.0);
    glVertex2f(1,0.3);
    glVertex2f(-1,0.3);
    glEnd();



    // top border
    glColor3ub(60,60,60);
    glBegin(GL_QUADS);
    glVertex2f(-1,0.25);
    glVertex2f(1,0.25);
    glVertex2f(1,0.3);
    glVertex2f(-1,0.3);
    glEnd();

    //  STATION SHADE

// top surface
glColor3ub(95,95,95);

glBegin(GL_QUADS);
glVertex2f(-1.0f,0.42f);
glVertex2f( 1.0f,0.42f);

glVertex2f( 0.88f,0.50f);
glVertex2f(-0.88f,0.50f);



glEnd();


// front depth surface
glColor3ub(55,55,55);

glBegin(GL_QUADS);

glVertex2f(-1.0f,0.30f);
glVertex2f( 1.0f,0.30f);

glVertex2f( 1.0f,0.34f);
glVertex2f(-1.0f,0.34f);

glEnd();


// inner lighter roof
glColor3ub(175,175,175);

glBegin(GL_QUADS);
glVertex2f(-0.90f,0.34f);
glVertex2f( 0.90f,0.34f);

glVertex2f( 0.82f,0.42f);
glVertex2f(-0.82f,0.42f);


glEnd();


// support pillars
for(float i=-0.82f;i<=0.82f;i+=0.28f){

    // front pillar
    glColor3ub(120,120,120);

    glBegin(GL_QUADS);

    glVertex2f(i,0.0f);
    glVertex2f(i+0.018f,0.0f);

   glVertex2f(i+0.018f,0.34f);
   glVertex2f(i,0.34f);

    glEnd();

    // side shadow
    glColor3ub(80,80,80);

    glBegin(GL_QUADS);

    glVertex2f(i+0.018f,0.0f);
    glVertex2f(i+0.028f,0.0f);

    glVertex2f(i+0.028f,0.26f);
    glVertex2f(i+0.018f,0.26f);

    glEnd();
}

    // windows
    for(float i=-0.9;i<0.9;i+=0.22){
        glColor3ub(140,170,210);

        glBegin(GL_QUADS);
        glVertex2f(i,0.08);
        glVertex2f(i+0.14,0.08);
        glVertex2f(i+0.14,0.22);
        glVertex2f(i,0.22);
        glEnd();
    }

    // clock
    drawClock();

}

//Bench
void drawBench(float x,float y){

    //  seat
    glColor3ub(145,90,40);

    glBegin(GL_QUADS);

    glVertex2f(x,y);
    glVertex2f(x+0.22f,y);

    glVertex2f(x+0.22f,y+0.03f);
    glVertex2f(x,y+0.03f);

    glEnd();


    // back support
    glColor3ub(160,100,45);

    glBegin(GL_QUADS);

    glVertex2f(x,y+0.03f);
    glVertex2f(x+0.22f,y+0.03f);

    glVertex2f(x+0.22f,y+0.10f);
    glVertex2f(x,y+0.10f);

    glEnd();


    //  wooden lines
    glColor3ub(100,60,25);

    for(float yy=y+0.045f; yy<y+0.095f; yy+=0.018f){

        glBegin(GL_LINES);

        glVertex2f(x+0.01f,yy);
        glVertex2f(x+0.21f,yy);

        glEnd();
    }


    //left leg
    glColor3ub(50,50,50);

    glBegin(GL_QUADS);

    glVertex2f(x+0.03f,y-0.07f);
    glVertex2f(x+0.045f,y-0.07f);

    glVertex2f(x+0.045f,y);
    glVertex2f(x+0.03f,y);

    glEnd();


    //  right leg
    glBegin(GL_QUADS);

    glVertex2f(x+0.17f,y-0.07f);
    glVertex2f(x+0.185f,y-0.07f);

    glVertex2f(x+0.185f,y);
    glVertex2f(x+0.17f,y);

    glEnd();


    //  shadow
    glColor4f(0,0,0,0.15);

    glBegin(GL_QUADS);

    glVertex2f(x+0.01f,y-0.08f);
    glVertex2f(x+0.23f,y-0.08f);

    glVertex2f(x+0.23f,y-0.07f);
    glVertex2f(x+0.01f,y-0.07f);

    glEnd();
}


// TRACK
void drawTrack(){

    // ground
    glColor3ub(170,170,170);
    glBegin(GL_QUADS);
    glVertex2f(-1,-1);
    glVertex2f(1,-1);
    glVertex2f(1,-0.2);
    glVertex2f(-1,-0.2);
    glEnd();

    // yellow line
    glColor3ub(240,220,0);
    glBegin(GL_QUADS);
    glVertex2f(-1,-0.25);
    glVertex2f(1,-0.25);
    glVertex2f(1,-0.22);
    glVertex2f(-1,-0.22);
    glEnd();

    // rails (upper)
    glColor3ub(40,40,40);
    glBegin(GL_QUADS);
    glVertex2f(-1,-0.55);
    glVertex2f(1,-0.55);
    glVertex2f(1,-0.52);
    glVertex2f(-1,-0.52);
    glEnd();

    // rails (lower)
    glBegin(GL_QUADS);
    glVertex2f(-1,-0.65);
    glVertex2f(1,-0.65);
    glVertex2f(1,-0.62);
    glVertex2f(-1,-0.62);
    glEnd();

    // sleepers
    for(float i=-1;i<1;i+=0.15){
        glColor3ub(110,70,40);

        glBegin(GL_QUADS);
        glVertex2f(i,-0.65);
        glVertex2f(i+0.08,-0.65);
        glVertex2f(i+0.08,-0.52);
        glVertex2f(i,-0.52);
        glEnd();
    }
}
//  TRAIN

void drawTrain(){

    float y = -0.58f;

    // 8 bogies
    for(int b=0; b<8; b++){

        // less gap between bogies
        float bx = trainX + b*0.28f;

        // 🚆 main body
        glColor3ub(200,20,20);

        glBegin(GL_QUADS);
        glVertex2f(bx, y);
        glVertex2f(bx+0.27f, y);

        glVertex2f(bx+0.27f, y+0.28f);
        glVertex2f(bx, y+0.28f);
        glEnd();

        // roof
        glColor3ub(40,40,40);

        glBegin(GL_QUADS);
        glVertex2f(bx-0.005f, y+0.28f);
        glVertex2f(bx+0.275f, y+0.28f);

        glVertex2f(bx+0.26f, y+0.33f);
        glVertex2f(bx+0.01f, y+0.33f);
        glEnd();

        // yellow stripe
        glColor3ub(240,220,0);

        glBegin(GL_QUADS);
        glVertex2f(bx, y+0.02f);
        glVertex2f(bx+0.27f, y+0.02f);

        glVertex2f(bx+0.27f, y+0.05f);
        glVertex2f(bx, y+0.05f);
        glEnd();

        // windows
        for(int i=0;i<3;i++){

            float wx = bx + 0.04f + i*0.075f;

            glColor3ub(140,170,210);

            glBegin(GL_QUADS);
            glVertex2f(wx, y+0.12f);
            glVertex2f(wx+0.055f, y+0.12f);

            glVertex2f(wx+0.055f, y+0.22f);
            glVertex2f(wx, y+0.22f);
            glEnd();
        }

        // wheels
        glColor3ub(0,0,0);

        circle(bx+0.05f, y-0.01f, 0.03f);
        circle(bx+0.22f, y-0.01f, 0.03f);

        //  FULL CONNECTION between bogies
        if(b < 7){

            glColor3ub(90,90,90);

            glBegin(GL_QUADS);

            glVertex2f(bx+0.27f, y+0.10f);
            glVertex2f(bx+0.29f, y+0.10f);

            glVertex2f(bx+0.29f, y+0.14f);
            glVertex2f(bx+0.27f, y+0.14f);

            glEnd();
        }
    }


    if(!night){

        float gx = trainX + 1.825f;

        // head
        glColor3ub(0,0,0);
        circle(gx, y+0.19f, 0.018f);

        // dress
        glColor3ub(255,0,0);

        glBegin(GL_QUADS);
        glVertex2f(gx-0.02f, y+0.12f);
        glVertex2f(gx+0.02f, y+0.12f);

        glVertex2f(gx+0.02f, y+0.18f);
        glVertex2f(gx-0.02f, y+0.18f);

        glEnd();
    }
}

//  BOY
void drawBoy(){
    float x = boyX;

    // head
    glColor3ub(0,0,0);
    circle(x,-0.40,0.055);

    // NECK
glColor3ub(0,0,0);
glBegin(GL_QUADS);
glVertex2f(x-0.008,-0.50);
glVertex2f(x+0.008,-0.50);
glVertex2f(x+0.008,-0.55);
glVertex2f(x-0.008,-0.55);
glEnd();

    // body
    glColor3ub(40,90,200);
    glBegin(GL_QUADS);
    glVertex2f(x-0.06,-0.6);
    glVertex2f(x+0.06,-0.6);
    glVertex2f(x+0.06,-0.45);
    glVertex2f(x-0.06,-0.45);
    glEnd();

    // upper leg
    glColor3ub(30,30,30);
    glBegin(GL_QUADS);
    glVertex2f(x-0.055,-0.70);
    glVertex2f(x+0.055,-0.70);
    glVertex2f(x+0.055,-0.60);
    glVertex2f(x-0.055,-0.60);
    glEnd();

// RUNNING LOWER LEGS
 float swing;

if(boyRun){
    swing = sin(legAngle) * 0.02f;
}
else{
    swing = 0;
}

    // left leg
    glBegin(GL_QUADS);
    glVertex2f(x-0.04 + swing,-0.85);
    glVertex2f(x-0.015 + swing,-0.85);
    glVertex2f(x-0.015,-0.70);
    glVertex2f(x-0.04,-0.70);
    glEnd();

    // right leg
    glBegin(GL_QUADS);
    glVertex2f(x+0.015 - swing,-0.85);
    glVertex2f(x+0.04 - swing,-0.85);
    glVertex2f(x+0.04,-0.70);
    glVertex2f(x+0.015,-0.70);
    glEnd();

    //  SHOES (NEW)
    glColor3ub(0,0,0);

    glBegin(GL_QUADS);
    glVertex2f(x-0.045 + swing,-0.87);
    glVertex2f(x-0.015 + swing,-0.87);
    glVertex2f(x-0.015 + swing,-0.85);
    glVertex2f(x-0.045 + swing,-0.85);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x+0.015 - swing,-0.87);
    glVertex2f(x+0.045 - swing,-0.87);
    glVertex2f(x+0.045 - swing,-0.85);
    glVertex2f(x+0.015 - swing,-0.85);
    glEnd();

    // hands
    glColor3ub(40,90,200);

    glBegin(GL_QUADS);
    glVertex2f(x-0.09,-0.55);
    glVertex2f(x-0.06,-0.55);
    glVertex2f(x-0.06,-0.50);
    glVertex2f(x-0.09,-0.50);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x+0.06,-0.55);
    glVertex2f(x+0.09,-0.55);
    glVertex2f(x+0.09,-0.50);
    glVertex2f(x+0.06,-0.50);
    glEnd();
}

//  DISPLAY
void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawStars();
if(!night){

    drawCloud(-0.75 + cloudX, 0.82);
    drawCloud(0.65 + cloudX, 0.74);
}
drawBuildings();


    drawSun();

    if(night){

    glColor4f(0.0f,0.0f,0.1f,0.45f);

    glBegin(GL_QUADS);

    glVertex2f(-1,-1);
    glVertex2f( 1,-1);
    glVertex2f( 1, 1);
    glVertex2f(-1, 1);

    glEnd();
    drawMoon();

}

    drawStation();

    // left bench
drawBench(-0.72f,0.03f);

// middle bench
drawBench(-0.10f,0.03f);

// right bench
drawBench(0.50f,0.03f);


    drawTrack();
    drawTrain();
    drawBoy();

   if(heartBreak && heartTimer < 60)
    drawHeart();


    glFlush();
}

// KEYBOARD
void keyboard(unsigned char key,int x,int y){
    if(key=='t'||key=='T'){
        trainMove=true;
        boyRun=true;
        sunStart=true;
    }
}
//UPDATE
void update(int v){

    //  TRAIN MOVEMENT
    if(trainMove && !trainGone){

        trainX += 0.006f;

        // first train leaves
        if(trainX > 1.2f){

            trainGone = true;
        }
    }

    // NIGHT TRAIN
    if(nightTrain){

        trainX += 0.006f;

        // continuous loop
        if(trainX > 3.5f){

            trainX = -3.0f;
        }
    }

    //  BOY RUN
    if(boyRun && boyX < 0.72f){

        boyX += 0.004f;

        // leg animation
        legAngle += 0.2f;
    }

    // stop boy at corner
    if(boyX >= 0.72f){

        boyRun = false;
    }

    //  HEART BREAK
    if(trainGone && !heartBreak){

        heartBreak = true;
        heartTimer = 0;

        // boy stops
        boyRun = false;
    }

    // SUNSET
    if(heartBreak && !night){

        dayMix += 0.003f;

        // sun slowly down
        if(sunY > 0.35f){

            sunY -= 0.003f;
        }

        // night starts
        if(sunY <= 0.35f){

            night = true;

            // hide sun
            sunY = -0.25f;

            // restart train from left
            trainX = -3.0f;

            nightTrain = true;
        }
    }

    //  SUN ROTATION
    if(!heartBreak){

        sunAngle += 0.03f;
    }

    // CLOCK
    minuteAngle -= 0.01f;
    hourAngle   -= 0.002f;

    //  MOON RISING
    if(night && moonY < 0.85f){

        moonY += 0.0015f;
    }

    //  CLOUD MOVE
    cloudX += 0.001f;

    if(cloudX > 1.0f)
        cloudX = -1.0f;

    // CLOCK ANIMATION
    clockAngle += 0.03f;

    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}


//  MAIN
int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(1000,550);
    glutCreateWindow("Scene 3");

    glClearColor(1,1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,update,0);

    glutMainLoop();
}
