#include "Angel.h"
#include "GLShader.h"
#include <fstream>
#include <cstring>

#define WINDOW_W glutGet ( GLUT_WINDOW_WIDTH )
#define WINDOW_H glutGet ( GLUT_WINDOW_HEIGHT )

const char *vert_source = "vshader5.glsl";
const char *frag_source = "fshader5.glsl";
const float Shininess = 100.0f;

int N;
int M;

GLsizei w;
GLsizei h;
GLuint program;
GLuint vloc;
GLuint vnorm;
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

mat4 view_matrix ()
{
    return Translate ( vec3 ( 0, 0, position_z ) )
        * RotateX ( rotation_x )
        * RotateY ( rotation_y );
}

void mouse_click ( int button, int state, int x, int y )
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

    if ( rotation_x > 90.0f ) rotation_x = 90.0f;
    if ( rotation_x < -90.0f ) rotation_x = -90.0f;
    if ( rotation_y > 180.0f ) rotation_y -= 360.0f;
    if ( rotation_y < -180.0f ) rotation_y += 360.0f;

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
};

struct Triangle
{
    GLuint a;
    GLuint b;
    GLuint c;
};

struct Vertex *vertices;
struct Triangle *triangles;

void setup ( const char *data_source )
{
    GLfloat vpx;
    GLfloat vpy;
    GLfloat vpz;
    int ti;
    int tj;
    int tk;

    FILE *dataFile;
    dataFile = fopen ( data_source, "r" );
	FILE *out;
	out = fopen("output.txt", "w");
    if ( dataFile != NULL )
    {
        fscanf ( dataFile, "%d %d", &N, &M );

        vertices = new Vertex[N];
        triangles = new Triangle[M];

        for ( int i = 0 ; i < N ; i++ )
        {
            fscanf ( dataFile, " %f %f %f", &vpx, &vpy, &vpz );

            vertices[i].p[0] = vpx;
            vertices[i].p[1] = vpy;
            vertices[i].p[2] = vpz;
        }

        for ( int t = 0 ; t < M ; t++ )
        {
            fscanf ( dataFile, " %d %d %d", &ti, &tj, &tk );

            triangles[t].a = ti;
            triangles[t].b = tj;
            triangles[t].c = tk;
        }

        for ( int j = 0 ; j < M ; j++ )
        {
            vec3 norms;
            vec3 a;
            vec3 b;
            vec3 c;

            a = vertices[triangles[j].a].p;
            b = vertices[triangles[j].b].p;
            c = vertices[triangles[j].c].p;

            norms = normalize (  ( b - a ) * ( c - a ) );

            vertices[triangles[j].a].n += norms;
            vertices[triangles[j].b].n += norms;
            vertices[triangles[j].c].n += norms;
        }

        for ( int i = 0 ; i < N ; i++ )
        {
            vertices[i].n = normalize ( vertices[i].n );
        }

		fprintf(out, "Vertices\n");
		for (int i = 0; i < N; i++)
		{
			fprintf(out, "vertices[%d].p[0] = %f\t", i, vertices[i].p[0]);
			fprintf(out, "vertices[%d].p[1] = %f\t", i, vertices[i].p[1]);
			fprintf(out, "vertices[%d].p[2] = %f\n", i, vertices[i].p[2]);
		}

		fprintf(out, "Triangles\n");
		for (int i = 0; i < M; i++)
		{
			fprintf(out, "triangles[%d].a = %d\t", i, triangles[i].a);
			fprintf(out, "triangles[%d].b = %d\t", i, triangles[i].b);
			fprintf(out, "triangles[%d].c = %d\n", i, triangles[i].c);
		}

		fprintf(out, "Normals\n");
		for (int i = 0; i < N; i++)
		{
			fprintf(out, "vertices[%d].n[0] = %f\t", i, vertices[i].n[0]);
			fprintf(out, "vertices[%d].n[1] = %f\t", i, vertices[i].n[1]);
			fprintf(out, "vertices[%d].n[2] = %f\n", i, vertices[i].n[2]);
		}
    }
    
    program = LoadShader("vshader5.glsl", "fshader5.glsl");
    glUseProgram ( program );

    vloc = glGetAttribLocation ( program, "vPosition" );
    vnorm = glGetAttribLocation ( program, "vNormal" );

    Mloc = glGetUniformLocation ( program, "ModelView" );
    Ploc = glGetUniformLocation ( program, "projection" );

    GLfloat AmbientProduct[] = {0.0f, 0.0f, 0.7f, 1.0f};
    GLfloat DiffuseProduct[] = {0.0f, 0.0f, 0.68f, 1.0f};
    GLfloat SpecularProduct[] = {1.0f, 1.0f, 0.32f, 1.0f};
    GLfloat LightPosition[] = {0.0f, -2.0f, -1.0f, 0.0f};

    glUniform4fv ( glGetUniformLocation ( program, "AmbientProduct" ), 1, AmbientProduct );
    glUniform4fv ( glGetUniformLocation ( program, "DiffuseProduct" ), 1, DiffuseProduct );
    glUniform4fv ( glGetUniformLocation ( program, "SpecularProduct" ), 1, SpecularProduct );
    glUniform4fv ( glGetUniformLocation ( program, "LightPosition" ), 1, LightPosition );
    glUniform1f ( glGetUniformLocation ( program, "Shininess" ), Shininess );

    GLuint vao;
    GLuint vbo;
    GLuint tbo;

    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers ( 1, &vbo );
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glBufferData ( GL_ARRAY_BUFFER, N * sizeof ( Vertex ), vertices, GL_STATIC_DRAW );

    glGenBuffers ( 1, &tbo );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, tbo );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, M * sizeof ( Triangle ), triangles, GL_STATIC_DRAW );

    glEnableVertexAttribArray ( vloc );
    glVertexAttribPointer ( vloc, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 0 );

    glEnableVertexAttribArray ( vnorm );
    glVertexAttribPointer ( vnorm, 3, GL_FLOAT, GL_FALSE, sizeof ( Vertex ), ( GLvoid * ) 12 );
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

    glClearColor ( 0.3f, 0.3f, 0.3f, 0.45f );
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
    glutMouseFunc ( mouse_click );
    glutMotionFunc ( mouse_drag );
    glutDisplayFunc ( display );

    glutInitContextVersion ( 3,3 );
    glutInitContextProfile ( GLUT_CORE_PROFILE );

    glewExperimental = GL_TRUE;

    if ( GLEW_OK != glewInit () )
    {
        return ( EXIT_FAILURE );
    }

    setup ( "Dragon.dat" );
    
    glutMainLoop ();

    return ( EXIT_SUCCESS );
}
