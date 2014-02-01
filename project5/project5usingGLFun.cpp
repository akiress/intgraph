#include "GLFundamentals.hpp"
#include <fstream>
#include <iostream>

#define WINDOW_W glutGet(GLUT_WINDOW_WIDTH)
#define WINDOW_H glutGet(GLUT_WINDOW_HEIGHT)

const int HEIGHT = 512;
const int WIDTH = 512;
const char *vert_source = "vshader5.glsl";
const char *frag_source = "fshader5.glsl";
const char *data_source = "Bunny.dat";
const float SHINY = 100.0f;

int n;
int &N = n;
int m;
int &M = m;

int k = 0;

gl::vec4 black = gl::vec4(0.0f, 0.0f, 0.0f, 1.0f);
gl::vec4 white = gl::vec4(1.0f, 1.0f, 1.0f, 1.0f);
gl::vec4 magenta = gl::vec4(1.0f, 0.0f, 1.0f, 1.0f);
gl::vec4 green = gl::vec4(0.0f, 1.0f, 0.0f, 1.0f);

GLsizei w = WIDTH, h = HEIGHT;
GLint color_loc;
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

gl::mat4 view_matrix ()
{
    return gl::translation(gl::vec3(0, 0, position_z))
    * gl::xrotation(rotation_x)
    * gl::yrotation(rotation_y);
}

void mouse_click (int button, int state, int x, int y)
{
    start_x = static_cast<float>(x);    // Removing MS warnings with cast
    start_y = static_cast<float>(y);    // Removing MS warnings with cast
    start_button = button;
    start_rotation_x = rotation_x;
    start_rotation_y = rotation_y;
    start_position_z = position_z;
}

void mouse_left_drag (int x, int y)
{
    rotation_x = start_rotation_x + (90.0f * (x - start_x) / WINDOW_W);
    rotation_y = start_rotation_y + (180.0f * (y - start_y) / WINDOW_H);

    if (rotation_x > 90.0f) rotation_x = 90.0f;
    if (rotation_x < -90.0f) rotation_x = -90.0f;
    if (rotation_y > 180.0f) rotation_y -= 360.0f;
    if (rotation_y < -180.0f) rotation_y += 360.0f;

}

void mouse_right_drag (int x, int y)
{
    position_z = start_position_z + (5.0f * (y - start_y) / WINDOW_H);
}

void mouse_drag (int x, int y)
{
    if (start_button == GLUT_LEFT_BUTTON) mouse_left_drag (x, y);
    if (start_button == GLUT_RIGHT_BUTTON) mouse_right_drag (x, y);
    glutPostRedisplay ();
}

void keyboard (unsigned char key, int mousex, int mousey)
{
    switch (key)
    {
        case 033:
        case 'q':
        case 'Q':
        exit (EXIT_SUCCESS);
        break;
    }
}

void reshape (GLsizei ww, GLsizei hh)
{
    glViewport(0,0, ww, hh);
    w = ww;
    h = hh;
}

struct Vertex
{
    gl::vec3 p;
    gl::vec3 n;
};

struct Triangle
{
    GLuint a;
    GLuint b;
    GLuint c;
};

struct Vertex *vertices;
struct Triangle *triangles;

void setup ()
{
    GLfloat vpx, vpy, vpz;
    int ti, tj, tk;
    FILE *dataFile;
    dataFile = fopen(data_source, "r");
    if (dataFile != NULL)
    {
        fscanf(dataFile, "%d %d", &N, &M);

        vertices = new Vertex[N];
        triangles = new Triangle[M];

        for (int i = 0 ; i < N ; i++)
        {
            fscanf(dataFile, " %f %f %f", &vpx, &vpy, &vpz);
            vertices[i].p[0] = vpx;
            vertices[i].p[1] = vpy;
            vertices[i].p[2] = vpz;
        }

        for (int t = 0 ; t < M ; t++)
        {
            fscanf(dataFile, " %d %d %d", &ti, &tj, &tk);
            triangles[t].a = ti;
            triangles[t].b = tj;
            triangles[t].c = tk;
        }
    }
    else
    {
        std::cout << "dataFile is empty." << std::endl;
    }

    /*gl::vec3 x, y, z;
    gl::vec3 n;
    for (int t = 0 ; t < N ; t++)
    {
       n = gl::normalize(gl::cross(b-a, c-b));
       vertices[triangles[t].a].n += n;
       vertices[triangles[t].b].n += n;
       vertices[triangles[t].c].n += n;
   }*/
}

void init ()
{
    GLuint vbo;
    GLuint tbo;

    if ((program = gl::init_program(vert_source, frag_source)))
    {
        vloc = glGetAttribLocation (program, "vPosition");
        Ploc = glGetUniformLocation(program, "projection");
        Mloc = glGetUniformLocation(program, "ModelView");
    }

    glUseProgram (program);

    vloc = glGetAttribLocation ( program, "vPosition" );
    vnorm = glGetAttribLocation ( program, "vNormal" );

    Mloc = glGetUniformLocation (program, "ModelView");
    Ploc = glGetUniformLocation (program, "projection");
    color_loc = glGetUniformLocation (program, "fcolor");

    GLfloat AmbientProduct[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat DiffuseProduct[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat SpecularProduct[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat LightPosition[] = {0.0f, 1.0f, 0.0f, 0.0f};

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, AmbientProduct);
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, DiffuseProduct);
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, SpecularProduct);
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, LightPosition);
    glUniform1f( glGetUniformLocation(program, "Shininess"), SHINY );

    glGenBuffers ( 1, &vbo );
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glBufferData ( GL_ARRAY_BUFFER, N * sizeof (Vertex), vertices, GL_STATIC_DRAW );

    glGenBuffers (1, &tbo);
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, tbo );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, M * sizeof (Triangle), triangles, GL_STATIC_DRAW );

    glEnableVertexAttribArray ( vloc );
    glVertexAttribPointer ( vloc, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid *) 0);

    glEnableVertexAttribArray ( vnorm );
    glVertexAttribPointer ( vnorm, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid *) 12);
}

void display ()
{
    gl::mat4 projection = gl::perspective ( 45.0f, 1.777f, 0.1f, 100.0f );
    gl::mat4 ModelView = view_matrix ();

    glUniformMatrix4fv ( Ploc, 1, GL_TRUE, projection );
    glUniformMatrix4fv ( Mloc, 1, GL_TRUE, ModelView );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDrawElements ( GL_TRIANGLES, 3 * M, GL_UNSIGNED_INT, 0 );

    glutSwapBuffers();

    glClearColor ( 0.3f, 0.3f, 0.3f, 0.45f );
}

int main (int argc, char *argv[])
{
    setup ();
    glutInitDisplayMode ( GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize ( HEIGHT, WIDTH );
    glutInitContextVersion ( 3,3 );
    glutInitContextProfile ( GLUT_CORE_PROFILE );
    glutInit ( &argc, argv );

    glutCreateWindow ( argv[0] );
    glutMouseFunc ( mouse_click );
    glutMotionFunc ( mouse_drag );
    glutDisplayFunc ( display );
    glutReshapeFunc ( reshape );
    glutKeyboardFunc ( keyboard );
    

    glewExperimental = GL_TRUE;
    if ( GLEW_OK != glewInit() )
    {
        return (EXIT_FAILURE);
    }

    FILE *outputFile;
    outputFile = fopen("output.txt", "w");
    for (int i = 0 ; i < N ; i++)
    {
        fprintf(outputFile, "%f --- %f --- %f\n", vertices[i].p[0], vertices[i].p[1], vertices[i].p[2]);
    }
    for (int i = 0 ; i < M ; i++)
    {
        fprintf(outputFile, "%d --- %d --- %d\n", triangles[i].a, triangles[i].b, triangles[i].c);
    }

    glewInit ();

    glEnable ( GL_DEPTH_TEST );
    glutMainLoop ();

    return (EXIT_SUCCESS);
}
