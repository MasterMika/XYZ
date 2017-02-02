#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MPSIZE 10
#define SPEED 10
#define BALLRAD 0.025
#define BLOCKSIZE 0.1


typedef struct a3d{
      float x;
      float y;
      float z; 
}_3d;
           //koordinate lopte                                                                     
static _3d ball  = {.x = 0,                        
                    .y = -0.95 },
           //pocetne koordinate lopte        
           ball_s_poz = {.x = 0,             
                         .y = -0.95,
                         .z = 0 },
           //vektor lopte            
           ballvec = {.x = 0,                   
                      .y = 1,
                      .z = 0 };
             //brojaci pomeraja lopte          
static float ball_parameter_x = 0,
             ball_parameter_y = 0,
             
             paddle_poz = 0;

static int animation_active,
           //aktivatori rotiranja
           rot_ver,
           rot_hor,
           //ugao rotiranja
           ang_ver = 0,
           ang_hor = 0,
           //flagovi pomeranja lopte
           ball_start = 0,
           ball_move = 0,
           //flag pomeranja paddlea
           paddle_mov = 0,
           //selektovanje ravni
           color_sel = 0,
           col_fix = 0;

static int map[3*MPSIZE][MPSIZE] = {{0}};



static void on_keyboard(unsigned char key, int x, int y);
static void up_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);

static void draw_ball(void);
static void draw_paddle(void);
static void draw_blocks(void);
static void draw_blocks_bg(void); //crtanje fiksne mape
static void coloring( float r, float g, float b, int sh);

static void collision_pw(void); //paddle wall
static void collision_bw(void); //ball wall
static void collision_bp(void); //ball paddle
static void collision_bb(void); //ball blocks

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(400, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    glutKeyboardUpFunc(up_keyboard);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display); 
     
    
    animation_active = 1;
    glutTimerFunc(SPEED, on_timer, 0);

    
    glClearColor(0.6, 0.6, 0, 0);
    glEnable(GL_DEPTH_TEST);

    
    glutMainLoop();

    return 0;
}
static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        exit(0);
        break;
        
    case 'd':
    case 'D':
        //pomeranje paddle a u desno
        if(!paddle_mov)
            paddle_mov = 1;
        break;
        
    case 'a':
    case 'A':
        //pomeranje paddle a u levo
        if(!paddle_mov)
            paddle_mov = -1;
        break;

    }
}

static void up_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'd':
    case 'D':
        //prekid pomeranja paddle a u desno
        paddle_mov = 0;
        break;
        
    case 'a':
    case 'A':
        //prekid pomeranja paddle a u levo
        paddle_mov = 0;
        break;
        
    case 'q':
    case 'Q':
        //biranje crvene ravni
        if(rot_hor == 0 && ball.y < -0.5){
            
            if(ang_hor == 0 && ang_ver == 90)
                rot_ver = -1;
                
            if(ang_hor == 90 && ang_ver == 0)
                rot_hor = -1;
                
            if((ang_hor == 0 && ang_ver == 90)
                || (ang_hor == 90 && ang_ver == 0)){
                //biranje boje
                color_sel = 0;
                //zaustavljanje lopte
                ball_move = 0;
                
                ball_s_poz.x = ball.x;
                ball_s_poz.y = ball.y;
                ball_s_poz.z = 0;
                
                ball_parameter_x = 0;
                ball_parameter_y = 0;
            }
        }
        break;

    case 'w':
    case 'W':
        //biranje zelene ravni
        if(rot_hor == 0 && ball.y < -0.5){
            
            if(ang_hor == 0 && ang_ver == 0)                                    
                rot_ver = 1;
                
            if(ang_hor == 90 && ang_ver == 0){
                rot_hor = -1;
                rot_ver = 1;
            }
            
            if((ang_hor == 0 && ang_ver == 0) || 
               (ang_hor == 90 && ang_ver == 0)){
                color_sel = 1;
                
                ball_move = 0;
                
                ball_s_poz.x = ball.x;
                ball_s_poz.y = ball.y;
                ball_s_poz.z = 0;
                
                ball_parameter_x = 0;
                ball_parameter_y = 0;
            }
        }
        break;
        
    case 'e':
    case 'E':
        //biranje plave ravni
        if(rot_ver == 0 && ball.y < -0.5){
            
            if(ang_ver == 0 && ang_hor == 0)
                rot_hor = 1;
                
            if(ang_ver == 90 && ang_hor == 0){
                rot_ver = -1;
                rot_hor = 1;
            }
            
            if((ang_ver == 0 && ang_hor == 0) ||
               (ang_ver == 90 && ang_hor == 0)){
                color_sel = 2;
                
                ball_move = 0;  
                        
                ball_s_poz.x = ball.x;
                ball_s_poz.y = ball.y;
                ball_s_poz.z = 0;
                
                ball_parameter_x = 0;
                ball_parameter_y = 0;
            }
        }
        break;
        
    case 'p':
    case 'P'://pauza
        {
        animation_active = !animation_active;
        glutTimerFunc(SPEED, on_timer, 0);
        }
        break;
        
    case 32:
        //aktiviranje lopte
        if(!ball_start){
            ball_start = 1;
            ball_move = 1;    
        //pocetna poz lopte iznad paddle a          
            ball_s_poz.x = paddle_poz;
            ball_s_poz.y = -0.95;
            ball_s_poz.z = 0;
        }
        break;
    }
}

static void on_timer(int value)
{
    if (value != 0)
        return;
    //pomeranje paddle a
    if(paddle_mov){
        paddle_poz += paddle_mov*0.01;
        collision_pw();
    }
    
    if(ball_move&&ball_start){
        //pomeranje brojaca lopte
        ball_parameter_x = ball_parameter_x + 0.006;
        ball_parameter_y = ball_parameter_y + 0.006;
        //racunanje koordinata lopte
        ball.x = ball_s_poz.x + ball_parameter_x * ballvec.x;
        ball.y = ball_s_poz.y + ball_parameter_y * ballvec.y;
        ball.z = 0;
        //kolizije
        collision_bw();
        collision_bp();
        collision_bb();
        
    }
    //racunanje ugla rotacije vertikalno
    if(!(rot_ver == 0)){
        //promena ugla
        ang_ver = ang_ver + 2*rot_ver;
        //zaustavljanje rotiranja i restart lopte
        if (!(ang_ver % 90)){ 
            rot_ver = 0;
            ball_move = 1;
            }
    }
    //racunanje ugla rotacije horizontalno
    if(!(rot_hor == 0)){
        
        ang_hor = ang_hor + 2*rot_hor;
        
        if (!(ang_hor % 90)){  
            rot_hor = 0;
            ball_move = 1;
        }
    }
    
    glutPostRedisplay();

    if (animation_active)
        glutTimerFunc(SPEED, on_timer, 0);
}

static void on_reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

static void on_display(void)
{
    GLfloat light_position[] = { 1, 1, 1, 0 };
    
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };
    
    GLfloat light_specular[] = { 1, 1, 1, 0 };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 2, 0., 0, 0, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //crtanje
    draw_ball();
    draw_paddle();
    draw_blocks();
    draw_blocks_bg();
    
    glutSwapBuffers();
}
//bojenje 
void coloring( float r, float g, float b, int sh){
    GLfloat ambient_coeffs[] = { 0.1, 0.1, 0.1, 1 };

    GLfloat diffuse_coeffs[] = { r, g, b, 1 };
    
    GLfloat specular_coeffs[] = { 0, 0, 0, 1 };
    
    if(sh){specular_coeffs[0]=1;specular_coeffs[1]=1;specular_coeffs[2]=1;}

    GLfloat shininess = sh;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

static void draw_ball(void)
{
    glPushMatrix();
    
    if(!ball_start)
        glTranslatef(paddle_poz, -0.95, 0);
    else
        //pomeranje lopte
        glTranslatef(ball.x, ball.y, ball.z);
        
    coloring(0.8, 0.8, 0.9, 20);
    glutSolidSphere(BALLRAD, 50, 50);
    
    glPopMatrix();
}

static void draw_paddle(void)
{
    glPushMatrix();
    
    glTranslatef(paddle_poz, -1.0, 0.0);
    coloring(1.0, 0, 1.0, 20);
    //crtanje ivica paddle a
    glBegin(GL_POLYGON);
    
        glNormal3f(0, 1, 0);
        glVertex3f(0.075, 0.025, 0.025);
        glVertex3f(0.075, 0.025, -0.025);
        glVertex3f(0.125, 0.025, 0);
        
    glEnd();
    glBegin(GL_POLYGON);
    
        glVertex3f(-0.075, 0.025, 0.025);
        glVertex3f(-0.075, 0.025, -0.025);
        glVertex3f(-0.125, 0.025, 0);
        
    glEnd();
    glBegin(GL_POLYGON);
        
        glNormal3f(0.1, 0, 1);
        glVertex3f(0.075, 0.025, 0.025);
        glVertex3f(0.125, 0.025, 0);
        glVertex3f(0.125, -0.025, 0);
        glVertex3f(0.075, -0.025, 0.025);
        
    glEnd();
    glBegin(GL_POLYGON);
    
        glNormal3f(-0.5, 0, 1);
        glVertex3f(-0.075, 0.025, 0.025);
        glVertex3f(-0.125, 0.025, 0);
        glVertex3f(-0.125, -0.025, 0);
        glVertex3f(-0.075, -0.025, 0.025);
        
    glEnd();
    
    glScalef(3.0, 1.0, 1.0);
    
    glutSolidCube(0.05);
    
    glPopMatrix();

}

static void draw_blocks(void)
{
    int i,j;
    glPushMatrix();
    //rotiranje vertikalno
    glRotated(ang_ver, 1, 0, 0);
    //rotiranje horizontalno
    glRotated(ang_hor, 0, 1, 0);
    //crtanje crvene ravni
    for (i = 0; i < MPSIZE; i++)
        for(j = 0; j < MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
                
                coloring(1, 0, 0, 0);
                glTranslatef(-0.45 + i * BLOCKSIZE, 0.45 - j * BLOCKSIZE, 0);
                glScalef(0.85, 0.85, 0.2);
                glutSolidCube(BLOCKSIZE);
                
                glPopMatrix();
            }
        }
    //crtanje zelene ravni
    for (i = MPSIZE; i < 2 * MPSIZE; i++)
        for(j = 0; j < MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
            
                coloring(0, 1, 0, 0);
                glTranslatef(0.45 - (9 - (i - MPSIZE)) * BLOCKSIZE,
                             0,
                             -0.45 + j * BLOCKSIZE);
                glScalef(0.85, 0.2, 0.85);
                glutSolidCube(BLOCKSIZE);
            
                glPopMatrix();
            }
        }
    //crtanje plave ravni
    for (i=2*MPSIZE; i < 3*MPSIZE; i++)
        for(j=0; j<MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
                
                coloring(0, 0, 1, 0);
                glTranslatef(0,
                             -0.45 + (9-j)*BLOCKSIZE,
                             0.45 - (9-(i-2*MPSIZE))*BLOCKSIZE);
                glScalef(0.2, 0.85, 0.85);
                glutSolidCube(BLOCKSIZE);
            
                glPopMatrix();
            }
        }
        glPopMatrix();
}
//crtanje fiksirane mape
static void draw_blocks_bg(void)
{

    int i,j;
    glPushMatrix();
    //pozicioniranje i rotiranje
    glTranslatef(0, 0.85, 0);
    glScalef(0.3, 0.3, 0.3);
    glRotated(60, 1, 0, 0);
    glRotated(45, 0, 1, 0);
    //crvena
    for (i=0; i<MPSIZE; i++)
        for(j=0; j<MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
                
                coloring(1, 0, 0, 0);
                glTranslatef(-0.45 + i*BLOCKSIZE, 0.45 - j*BLOCKSIZE, 0);
                glScalef(0.85, 0.85, 0.2);
                glutSolidCube(BLOCKSIZE);
                
                glPopMatrix();
            }
        }
    //zelena
    for (i=MPSIZE; i<2*MPSIZE; i++)
        for(j=0; j<MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
            
                coloring(0, 0.8, 0, 0);
                glTranslatef(0.45 - (9-(i-MPSIZE))*BLOCKSIZE,
                             0,
                             -0.45 + j*BLOCKSIZE);
                glScalef(0.85, 0.2, 0.85);
                glutSolidCube(BLOCKSIZE);
            
                glPopMatrix();
            }
        }
    //plava
    for (i=2*MPSIZE; i<3*MPSIZE; i++)
        for(j=0; j<MPSIZE; j++){
            if(!map[i][j]){
                glPushMatrix();
                
                coloring(0.1, 0.1, 1, 0);
                glTranslatef(0,
                             -0.45 + (9-j)*BLOCKSIZE,
                             0.45 - (9-(i-2*MPSIZE))*BLOCKSIZE);
                glScalef(0.2, 0.85, 0.85);
                glutSolidCube(BLOCKSIZE);
            
                glPopMatrix();
            }
        }
    glPopMatrix();
}


//paddle wall
static void collision_pw(void)
{
    //desno
    if(paddle_poz >= 0.40)
        paddle_poz = 0.40;
    //levo
    if(paddle_poz <= -0.40)
        paddle_poz = -0.40;    
}
//ball wall
static void collision_bw(void)
{   
    //desno
    if(ball.x >= 0.47){
        ball_s_poz.x = 0.47;
        //promena vektora
        ballvec.x = -ballvec.x;
        //reset brojaca
        ball_parameter_x = 0;
        }
    //levo
    if(ball.x < -0.47){
        ball_s_poz.x = -0.47;
        
        ballvec.x = -ballvec.x;
        
        ball_parameter_x = 0;
        }
    //gore      
    if(ball.y > 0.47){
        ball_s_poz.y = 0.47;
        
        ballvec.y = -ballvec.y;
        
        ball_parameter_y = 0;
        }
    //restartovanje promasaja
    if(ball.y <= -1.3){
        //zaustavljanje lopte
        ball_start = 0;
        ball_move = 0;
        //reset brojaca
        ball_parameter_x = 0;
        ball_parameter_y = 0;
        //reset vektora
        ballvec.x = 0;
        ballvec.y = 1;
        }
}
//ball paddle
static void collision_bp(void)
{   //okvir paddle
    if(ball.y <= -0.95 && ball.x <= paddle_poz + 0.125 &&
       ball.x >= paddle_poz - 0.125 && ball.y >= -1){
        //pocetna pozicija
        ball_s_poz.y = -0.95;
        ball_s_poz.x = ball.x;
        //promena vektora
        ballvec.y = -ballvec.y;
        ballvec.x = ballvec.x + (ball.x - paddle_poz)*2;
        //reset brojaca
        ball_parameter_y = 0;
        ball_parameter_x = 0;
        }
}

//ball blocks
static void collision_bb(void)
{      
    int i,j;
    //racucanje samo unutar matrice
    if(ball.y >= -0.5 - BALLRAD){
        //racunanje mozicije u matrici
        if(ballvec.y >= 0)
        j = ceil(-(ball.y + BALLRAD) * 10) + 4;
        else
        j = ceil(-(ball.y - BALLRAD) * 10) + 4;
        if(ballvec.x >= 0)
        i = ceil((ball.x - BALLRAD) * 10) + 4;
        else
        i = ceil((ball.x + BALLRAD) * 10) + 4;
        if(ballvec.y > 0){
            //donja kolizija
            //provera da li postoji kvadrat za koliziju i popravka buga 
            if(col_fix==0 && j < MPSIZE && !map[i + color_sel * MPSIZE][j]){
                if(ball.y + BALLRAD >= -(j - 4) * BLOCKSIZE){
                    
                    ball_s_poz.y = -(j - 4) * BLOCKSIZE - BALLRAD;
                    
                    ballvec.y = -ballvec.y;
                    ball_parameter_y = 0;
                    //nestajanje kvadrata
                    map[i + color_sel * MPSIZE][j] = 1;
                    col_fix = 1;
                }
            }
            else col_fix = 0;}
        else
            //gornja kolizija
            //provera da li postoji kvadrat za koliziju i popravka buga
            if(col_fix==0 && j < MPSIZE && !map[i + color_sel * MPSIZE][j]){
                if(ball.y - BALLRAD <= -(j - 5) * BLOCKSIZE){
                    
                    ball_s_poz.y = -(j - 5) * BLOCKSIZE + BALLRAD;
                    
                    ballvec.y = -ballvec.y;
                    ball_parameter_y = 0;
                    //nestajanje kvadrata
                    map[i + color_sel * MPSIZE][j] = 1;
                    col_fix = 1;
                }
            }
            else col_fix = 0;
        if(ballvec.x > 0){
            //desna kolizija
            //provera da li postoji kvadrat za koliziju i popravka buga
            if(col_fix==0 && ball.y >= -0.5 && j < MPSIZE && !map[i + 1 + color_sel * MPSIZE][j]){                       
                if(ball.x + BALLRAD >= (i - 4) * BLOCKSIZE){
                    
                    ball_s_poz.x = (i - 4) * BLOCKSIZE - BALLRAD;
                    
                    ballvec.x = -ballvec.x;
                    ball_parameter_x = 0;
                    //nestajanje kvadrata
                    map[i + 1 + color_sel * MPSIZE][j] = 1;
                    col_fix = 1;
                }
            }
            else col_fix = 0;
        }
        else
            //leva kolizija
            //provera da li postoji kvadrat za koliziju i popravka buga
            if(col_fix==0 && ball.y >= -0.5 && j < MPSIZE && !map[i - 1 + color_sel * MPSIZE][j]){
                if(ball.x - BALLRAD <= (i - 5) * BLOCKSIZE){
                    
                    ball_s_poz.x = (i - 5) * BLOCKSIZE + BALLRAD;
                    
                    ballvec.x = -ballvec.x;
                    ball_parameter_x = 0;
                    //nestajanje kvadrata
                    map[i - 1 + color_sel * MPSIZE][j] = 1;
                    col_fix = 1;
                }
            }
            else col_fix = 0;
        

    }
}
