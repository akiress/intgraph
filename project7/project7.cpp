#include "Angel.h"
#include "GLShader.h"
#include "TGA.hpp"
#include <fstream>
#include <pthread.h>

/** For Ubuntu only. Will not compile without **/
void junk()
{
    int i;
    i=pthread_getconcurrency();
}

#define WINDOW_W glutGet ( GLUT_WINDOW_WIDTH )
#define WINDOW_H glutGet ( GLUT_WINDOW_HEIGHT )

const char *vert_source = "vshader7.glsl";
const char *frag_source = "fshader7.glsl";

int tgaWidth = 2048;
int tgaHeight = 2048;
int tgaDepth = 32;
int previous_time = 0;

GLsizei w;
GLsizei h;
GLuint program;
GLuint vPositionLocation;
GLuint vNormalLocation;
GLuint vTexCoordLocation;
GLint ProjectionLocation;
GLint ModelViewLocation;

vec2 view_rotation = vec2 ( 0, 0 );
vec2 start_rotation = vec2 ( 0, 0 );
vec2 start = vec2 ( 0, 0 );
vec3 view_position = vec3 ( 0, 2, 5 );
vec4 view_motion = vec4 ( 0, 0, 0 );

mat4 P;
mat4 M;

mat4 view_matrix ()
{
    return RotateX ( -view_rotation.x ) * RotateY ( -view_rotation.y ) * Translate ( -view_position );
}

void mouse_click ( int button, int state, int x, int y )
{
    start.x = static_cast<float> ( x );
    start.y = static_cast<float> ( y );
    start_rotation = view_rotation;
}

void mouse_drag ( int x , int y )
{
    view_rotation.x = start_rotation.x + ( 90.0f * ( y - start.y ) / WINDOW_H );
    view_rotation.y = start_rotation.y + ( 180.0f * ( x - start.x ) / WINDOW_W );

    if (view_rotation.x > 90.0) view_rotation.x = -90.0;
    if (view_rotation.x < -90.0) view_rotation.x = 90.0;
    if (view_rotation.y > 180.0) view_rotation.y += 360.0;
    if (view_rotation.y < -180.0) view_rotation.y -= 360.0;
}

void keyboard_down ( unsigned char key, int x, int y )
{
    switch ( key )
    {
        case 'a':
            view_motion.x = view_motion.x - 1;
            break;
        case 'd':
            view_motion.x = view_motion.x + 1;
            break;
        case 's':
            view_motion.z = view_motion.z + 1;
            break;
        case 'w':
            view_motion.z = view_motion.z - 1;
            break;
    }
}

void keyboard_up ( unsigned char key, int x, int y )
{
    switch ( key )
    {
        case 'a':
            view_motion.x = view_motion.x + 1;
            break;
        case 'd':
            view_motion.x = view_motion.x - 1;
            break;
        case 's':
            view_motion.z = view_motion.z - 1;
            break;
        case 'w':
            view_motion.z = view_motion.z + 1;
            break;
    }
}

void idle ()
{
    int current_time;
    GLfloat delta_t;

    mat4 R = RotateY ( view_rotation.y ) * RotateX ( view_rotation.x );
    vec4 v = R * view_motion;

    current_time = glutGet ( GLUT_ELAPSED_TIME );
    delta_t = ( current_time - previous_time ) / 1000.0f;
    previous_time = current_time;

    view_position = view_position + ( vec3 ( v.x, v.y, v.z ) * delta_t );

    glutPostRedisplay ();
}

struct Model
{
    int n;
    int m;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint tex;
};

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
struct Model desert;
struct Model chest;
struct Model palm1;
struct Model palm2;
struct Model palm3;
struct Model palm4;
struct Model palm5;
struct Model tropical_plant;
struct Model fern;
struct Model Chest;

void model_load ( struct Model *models, const char *data_source )
{
    FILE *dataFile;
    dataFile = fopen ( data_source, "r" );
    if ( dataFile != NULL )
    {
        char tga_source[256];

        fscanf ( dataFile, "%d %d %s", &models->n, &models->m, tga_source );

        vertices = new Vertex[models->n];
        triangles = new Triangle[models->m];

        for ( int i = 0 ; i < models->n ; i++ )
        {
            fscanf(dataFile, " %f %f %f %f %f %f %f %f ",
                    &vertices[i].p.x, &vertices[i].p.y, &vertices[i].p.z,
                    &vertices[i].n.x, &vertices[i].n.y, &vertices[i].n.z,
                    &vertices[i].t.x, &vertices[i].t.y);
        }

        for ( int j = 0 ; j < models->m ; j++ )
        {
            fscanf(dataFile, " %u %u %u ",
                    &triangles[j].a, &triangles[j].b, &triangles[j].c);
        }

        glGenVertexArrays ( 1, &models->vao );
        glBindVertexArray ( models->vao );

        glGenBuffers ( 1, &models->vbo );
        glGenBuffers ( 1, &models->ebo );

        glBindBuffer ( GL_ARRAY_BUFFER, models->vbo );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, models->ebo );

        glBufferData ( GL_ARRAY_BUFFER, models->n * sizeof ( Vertex ), vertices, GL_STATIC_DRAW );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, models->m * sizeof ( Triangle ), triangles, GL_STATIC_DRAW );

        delete [] vertices;
        delete [] triangles;

        void *p = load_tga ( tga_source, tgaWidth, tgaHeight, tgaDepth );
        if ( p != NULL )
        {
            glGenTextures ( 1, &models->tex );
            glBindTexture ( GL_TEXTURE_2D, models->tex );
            glTexParameteri ( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
            if ( tgaDepth == 32 )
                glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, tgaWidth, tgaHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, p );
            if ( tgaDepth == 24 )
                glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, tgaWidth, tgaHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, p );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            free ( p );
        }
    }
}

void model_draw ( struct Model *models, vec3 position )
{
    M = view_matrix() * Translate ( position );
    glUniformMatrix4fv ( ModelViewLocation, 1, GL_TRUE, M );

    glBindBuffer ( GL_ARRAY_BUFFER, models->vbo );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, models->ebo );
    glBindTexture ( GL_TEXTURE_2D, models->tex );

    glEnableVertexAttribArray ( vPositionLocation );
    glEnableVertexAttribArray ( vNormalLocation );
    glEnableVertexAttribArray ( vTexCoordLocation );

    glVertexAttribPointer ( vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 0 );
    glVertexAttribPointer ( vNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 12 );
    glVertexAttribPointer ( vTexCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 24 );

    glDrawElements ( GL_TRIANGLES, 3 * models->m, GL_UNSIGNED_INT, 0 );
}

void setup ()
{
    if (  ( program = LoadShader ( vert_source, frag_source ) ) )
    {
        vPositionLocation = glGetAttribLocation ( program, "vPosition" );
        vNormalLocation = glGetAttribLocation ( program, "vNormal" );
        vTexCoordLocation = glGetAttribLocation ( program, "vTexCoord");

        ModelViewLocation = glGetUniformLocation ( program, "ModelView" );
        ProjectionLocation = glGetUniformLocation ( program, "Projection" );
    }

    glUseProgram ( program );

    GLfloat AmbientProduct[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat DiffuseProduct[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat LightPosition[] = {0.0f, 1.0f, 0.0f, 0.0f};

    glUniform4fv ( glGetUniformLocation ( program, "AmbientProduct" ), 1, AmbientProduct );
    glUniform4fv ( glGetUniformLocation ( program, "DiffuseProduct" ), 1, DiffuseProduct );
    glUniform4fv ( glGetUniformLocation ( program, "LightPosition" ), 1, LightPosition );

    model_load ( &desert, "desert.dat" );
    model_load ( &palm1, "palm1.dat" );
    model_load ( &palm2, "palm2.dat" );
    model_load ( &palm3, "palm3.dat" );
    model_load ( &palm4, "palm4.dat" );
    model_load ( &palm5, "palm5.dat" );
    model_load ( &fern, "fern.dat" );

    glEnable ( GL_DEPTH_TEST );
}

void display ()
{
    P = Perspective ( 45.0f, 1.777f, 0.1f, 100.0f );

    glUniformMatrix4fv ( ProjectionLocation, 1, GL_TRUE, P );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    model_draw ( &desert, vec3 ( 1.5f, 0.f, 0.5f ) );
    model_draw ( &palm1, vec3 ( 0.1f, 0.0f, 1.3f ) );
    model_draw ( &palm2, vec3 ( 1.2f, 0.0f, 0.5f ) );
    model_draw ( &palm3, vec3 ( 0.4f, 0.0f, 1.2f ) );
    model_draw ( &palm4, vec3 ( 2.3f, 0.0f, 0.8f ) );
    model_draw ( &palm5, vec3 ( 0.8f, 0.0f, 0.2f ) );
    model_draw ( &fern, vec3 ( 1.15f, 0.0f, 0.34f ) );
    model_draw ( &fern, vec3 ( 0.25f, 0.0f, 0.54f ) );
    model_draw ( &fern, vec3 ( 1.45f, 0.0f, 0.24f ) );
    model_draw ( &fern, vec3 ( 2.35f, 0.0f, 1.84f ) );
    model_draw ( &fern, vec3 ( 0.85f, 0.0f, 0.24f ) );
    model_draw ( &palm1, vec3 ( 1.94f, 0.0f, 2.9f ) );
    model_draw ( &palm2, vec3 ( 3.24f, 0.0f, 2.8f ) );
    model_draw ( &palm3, vec3 ( 1.14f, 0.0f, 2.7f ) );
    model_draw ( &palm4, vec3 ( 2.39f, 0.0f, 3.6f ) );
    model_draw ( &palm5, vec3 ( 1.87f, 0.0f, 3.5f ) );
    model_draw ( &fern, vec3 ( 1.12f, 0.0f, 3.20f ) );
    model_draw ( &fern, vec3 ( 1.29f, 0.0f, 1.89f ) );
    model_draw ( &fern, vec3 ( 1.68f, 0.0f, 2.31f ) );
    model_draw ( &fern, vec3 ( 1.41f, 0.0f, 0.27f ) );
    model_draw ( &fern, vec3 ( 1.72f, 0.0f, 2.48f ) );
    model_draw ( &palm3, vec3 ( 1.84f, 0.0f, 1.2f ) );
    model_draw ( &palm3, vec3 ( 1.94f, 0.0f, 1.3f ) );
    model_draw ( &palm3, vec3 ( 2.04f, 0.0f, 2.4f ) );
    model_draw ( &palm3, vec3 ( 3.24f, 0.0f, 1.8f ) );
    model_draw ( &palm3, vec3 ( 2.34f, 0.0f, 1.2f ) );
    model_draw ( &palm3, vec3 ( 1.44f, 0.0f, 2.3f ) );
    model_draw ( &palm3, vec3 ( 3.54f, 0.0f, 3.7f ) );

    glutSwapBuffers ();

    glClearColor ( 0.2f, 0.4f, 0.6f, 1.0f );
}

int main ( int argc, char *argv[] )
{
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize ( 960, 540 );
    glutInit ( &argc, argv );
    glutCreateWindow ( argv[0] );

    printf ( "OpenGL %s GLSL %s\n", glGetString ( GL_VERSION ),
            glGetString ( GL_SHADING_LANGUAGE_VERSION ) );

    glutMouseFunc ( mouse_click );
    glutMotionFunc ( mouse_drag );
    glutKeyboardFunc ( keyboard_down );
    glutKeyboardUpFunc ( keyboard_up );
    glutDisplayFunc ( display );
    glutIdleFunc ( idle );

    glutIgnoreKeyRepeat ( 1 );

    glutInitContextVersion ( 3, 3 );
    glutInitContextProfile ( GLUT_CORE_PROFILE );

    glewExperimental = GL_TRUE;

    if ( GLEW_OK != glewInit () )
    {
        return ( EXIT_FAILURE );
    }

    setup ();
    glutMainLoop ();

    return ( EXIT_SUCCESS );
}
