#include "Angel.h"
#include "GLShader.h"
#include "TGA.h"
#include <fstream>
#include <pthread.h>

#define WINDOW_W glutGet ( GLUT_WINDOW_WIDTH )
#define WINDOW_H glutGet ( GLUT_WINDOW_HEIGHT )

const char *vshader_source = "vshader6.glsl";
const char *fshader_source = "fshader6.glsl";

int N;
int M;
int tgaWidth = 1024;
int tgaHeight = 1024;
int tgaDepth = 24;

GLsizei w;
GLsizei h;
GLuint program;
GLuint vloc;
GLuint vnorm;
GLuint vtex;
GLint Ploc;
GLint Mloc;

GLfloat rotation_x = 0.0f;
GLfloat rotation_y = 0.0f;
GLfloat position_z = -5.0f;

int start_button;

GLfloat start_rotation_x;
GLfloat start_rotation_y;
GLfloat start_position_z;
GLfloat start_x;
GLfloat start_y;

int i = pthread_getconcurrency();

mat4 view_matrix ()
{
    return Translate ( vec3 ( 0, 0, position_z ) )
    * RotateX ( rotation_x )
    * RotateY ( rotation_y );
}

void mouse_start ( int button, int state, int x, int y )
{
    start_x = static_cast<float> ( x );
    start_y = static_cast<float> ( y );
    start_button = button;
    start_rotation_x = rotation_x;
    start_rotation_y = rotation_y;
    start_position_z = position_z;
}

void mouse_left_drag ( int x, int y )
{

    GLfloat dx = GLfloat ( x - start_x ) / WINDOW_W;
    GLfloat dy = GLfloat ( y - start_y ) / WINDOW_H;

    rotation_x = start_rotation_x + 90.0f * dy;
    rotation_y = start_rotation_y + 180.0f * dx;

    if (rotation_x > 90.0) rotation_x = 90.0;
    if (rotation_x < -90.0) rotation_x = -90.0;
    if (rotation_y > 180.0) rotation_y -= 360.0;
    if (rotation_y < -180.0) rotation_y += 360.0;
}

void mouse_right_drag ( int x, int y )
{
    position_z = start_position_z + ( position_z * ( y - start_y ) / WINDOW_H );
}

void mouse_drag ( int x, int y )
{
    if ( start_button == GLUT_LEFT_BUTTON ) mouse_left_drag ( x, y );
    if ( start_button == GLUT_RIGHT_BUTTON ) mouse_right_drag ( x, y );
    glutPostRedisplay ();
}

void keyboard ( unsigned char key, int mousex, int mousey )
{
    switch ( key )
    {
        case 033:
        case 'q':
        case 'Q':
        exit ( EXIT_SUCCESS );
        break;
    }
}

void reshape ( GLsizei ww, GLsizei hh )
{
    glViewport ( 0,0, ww, hh );
    w = ww;
    h = hh;
}

struct Vertex
{
    vec3 p;
    vec3 n;
    vec2 t;
};

struct Triangle
{
    GLuint a;
    GLuint b;
    GLuint c;
};

struct Vertex *vertices;
struct Triangle *triangles;

void setup ( const char *data_source, const char *tga_source )
{
    GLfloat vpx;
    GLfloat vpy;
    GLfloat vpz;
    GLfloat vnx;
    GLfloat vny;
    GLfloat vnz;
    GLfloat vtx;
    GLfloat vty;
    int ta;
    int tb;
    int tc;

    FILE *dataFile;
    dataFile = fopen ( data_source, "r" );
    if ( dataFile != NULL )
    {
        fscanf ( dataFile, "%d %d", &N, &M );

        vertices = new Vertex[N];
        triangles = new Triangle[M];

        for ( int i = 0 ; i < N ; i++ )
        {
            fscanf ( dataFile, " %f %f %f %f %f %f %f %f ", &vpx, &vpy, &vpz, &vnx, &vny, &vnz, &vtx, &vty );

            vertices[i].p.x = vpx;
            vertices[i].p.y = vpy;
            vertices[i].p.z = vpz;

            vertices[i].n.x = vnx;
            vertices[i].n.y = vny;
            vertices[i].n.z = vnz;

            vertices[i].t.x = vtx;
            vertices[i].t.y = vty;
        }

        for ( int t = 0 ; t < M ; t++ )
        {
            fscanf ( dataFile, " %d %d %d ", &ta, &tb, &tc );

            triangles[t].a = ta;
            triangles[t].b = tb;
            triangles[t].c = tc;
        }
    }

    if (  ( program = LoadShader ( vshader_source, fshader_source ) ) )
    {
        vloc = glGetAttribLocation ( program, "vPosition" );
        vnorm = glGetAttribLocation ( program, "vNormal" );
        vtex = glGetAttribLocation ( program, "vTexCoord");

        Mloc = glGetUniformLocation ( program, "ModelView" );
        Ploc = glGetUniformLocation ( program, "projection" );
    }

    glUseProgram ( program );

    GLfloat AmbientProduct[] = {0.3f, 0.1f, 0.3f, 1.0f};
    GLfloat DiffuseProduct[] = {0.75f, 0.31f, 0.76f, 1.0f};

    glUniform4fv ( glGetUniformLocation ( program, "AmbientProduct" ), 1, AmbientProduct );
    glUniform4fv ( glGetUniformLocation ( program, "DiffuseProduct" ), 1, DiffuseProduct );

    GLuint vao;
    GLuint vbo;
    GLuint tbo;
    GLuint tex;

    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers ( 1, &vbo );
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glBufferData ( GL_ARRAY_BUFFER, N * sizeof ( Vertex ), vertices, GL_STATIC_DRAW );

    glGenBuffers ( 1, &tbo );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, tbo );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, M * sizeof ( Triangle ), triangles, GL_STATIC_DRAW );

    glGenTextures ( 1, &tex );
    glBindTexture ( GL_TEXTURE_2D, tex );

    glEnableVertexAttribArray ( vloc );
    glVertexAttribPointer ( vloc, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 0 );

    glEnableVertexAttribArray ( vnorm );
    glVertexAttribPointer ( vnorm, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 12 );

    glEnableVertexAttribArray ( vtex );
    glVertexAttribPointer ( vtex, 2, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 24 );

    void *p = load_tga ( tga_source, tgaWidth, tgaHeight, tgaDepth );
    glTexParameteri ( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
    if ( tgaDepth == 32 )
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, tgaWidth, tgaHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, p );
    if ( tgaDepth == 24 )
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, tgaWidth, tgaHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, p );
    free ( p );
}

void display ()
{
    mat4 projection = Perspective ( 45.0f, 1.3333333f, 1.0f, 100.0f );
    mat4 ModelView = view_matrix ();

    glUniformMatrix4fv ( Ploc, 1, GL_TRUE, projection );
    glUniformMatrix4fv ( Mloc, 1, GL_TRUE, ModelView );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDrawElements ( GL_TRIANGLES, 3 * M, GL_UNSIGNED_INT, 0 );

    glutSwapBuffers ();

    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
}

int main ( int argc, char *argv[] )
{
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize ( 960, 540 );
    glutInit ( &argc, argv );
    glutCreateWindow ( argv[0] );

    printf ( "OpenGL %s GLSL %s\n", glGetString ( GL_VERSION ),
        glGetString ( GL_SHADING_LANGUAGE_VERSION ) );

    glutKeyboardFunc ( keyboard );
    glutMouseFunc ( mouse_start );
    glutMotionFunc ( mouse_drag );
    glutDisplayFunc ( display );

    glutInitContextVersion ( 3,3 );
    glutInitContextProfile ( GLUT_CORE_PROFILE );

    glewExperimental = GL_TRUE;

    if ( GLEW_OK != glewInit () )
    {
        return ( EXIT_FAILURE );
    }

    setup ( argv[1], argv[2] );

    glEnable (GL_DEPTH_TEST);
    glutMainLoop ();

    return ( EXIT_SUCCESS );
}
