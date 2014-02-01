#include "GLFundamentals.hpp"
#include "Motion.h"
#include <cstdlib>

const float PI = 3.14159f;
const int BOUND = 256;
const int DATAX = 257;
const int DATAY = 257;
const int HEIGHT = 512;
const int WIDTH = 512;
const int HALF = 128;

Motion motion;

GLsizei w = WIDTH, h = HEIGHT;

GLuint buffers[1];
GLint color_loc;
GLuint program;
GLuint vloc;
GLint Ploc;
GLint Mloc;

gl::vec4 triangles [3*BOUND*2*BOUND];

gl::vec4 black = gl::vec4(0.0f, 0.0f, 0.0f, 1.0f);
gl::vec4 white = gl::vec4(1.0f, 1.0f, 1.0f, 1.0f);
gl::vec4 magenta = gl::vec4(1.0f, 0.0f, 1.0f, 1.0f);

GLfloat rotation_x = 0.0f;
GLfloat rotation_y = 0.0f;
GLfloat position_z = -5.0f;

int click_button;

GLfloat click_rotation_x;
GLfloat click_rotation_y;
GLfloat click_position_z;
GLfloat click_x;
GLfloat click_y;

void init ()
{
    GLuint vao;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    glGenBuffers (1, buffers);
    glBindBuffer (GL_ARRAY_BUFFER, buffers[0]);
    glBufferData (GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

    if ((program = gl::init_program("vshader47.glsl", "fshader47.glsl")))
    {
        vloc = glGetAttribLocation (program, "vPosition");
        Ploc = glGetUniformLocation(program, "projection");
        Mloc = glGetUniformLocation(program, "model_view");
    }

    glUseProgram (program);

    vloc = glGetAttribLocation (program, "vPosition");
    glEnableVertexAttribArray (vloc);
    glVertexAttribPointer (vloc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
    Mloc = glGetUniformLocation (program, "model_view");
    Ploc = glGetUniformLocation (program, "projection");
    color_loc = glGetUniformLocation (program, "fcolor");

    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1.0, 1.0);

    glClearColor (0.7f, 0.7f, 0.7f, 0.65f);
}

void display ()
{
    gl::vec4 t;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl::mat4 projection = gl::perspective(45.0f, 1.3333333f, 1.0f, 100.0f);
    gl::mat4 model_view = motion.view_matrix();

    glUniformMatrix4fv (Ploc, 1, GL_TRUE, projection);
    glUniformMatrix4fv (Mloc, 1, GL_TRUE, model_view);

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glUniform4fv (color_loc, 1, black);
    glDrawArrays (GL_TRIANGLES, 0, 3*BOUND*BOUND*2);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    glUniform4fv (color_loc, 1, magenta);
    glDrawArrays (GL_TRIANGLES, 0, 3*BOUND*BOUND*2);
    glutSwapBuffers ();
}

void reshape (GLsizei ww, GLsizei hh)
{
    glViewport(0,0, ww, hh);
    w = ww;
    h = hh;
}

int main (int argc, char** argv)
{
    Motion motion;
    int k = 0;
    int i, j;
    float z_0;
    float z_1;
    float x_0;
    float x_1;
    // FILE * file;
    // file = fopen ("output.txt", "w");
    for (i = 0 ; i < BOUND ; i++)
    {
        for (j = 0 ; j < BOUND ; j++)
        {
            z_0 = PI * (static_cast<float>(i) - HALF)/HALF;
            z_1 = PI * (static_cast<float>(i + 1) - HALF)/HALF;
            x_0 = PI * (static_cast<float>(j) - HALF)/HALF;
            x_1 = PI * (static_cast<float>(j + 1) - HALF)/HALF;

            triangles[k] = gl::vec4(x_0, sin(x_0)*sin(z_0), z_0, 1.0);
            k++;
            triangles[k] = gl::vec4(x_0, sin(x_0)*sin(z_1), z_1, 1.0);
            k++;
            triangles[k] = gl::vec4(x_1, sin(x_1)*sin(z_1), z_1, 1.0);
            k++;
            triangles[k] = gl::vec4(x_0, sin(x_0)*sin(z_0), z_0, 1.0);
            k++;
            triangles[k] = gl::vec4(x_1, sin(x_1)*sin(z_1), z_1, 1.0);
            k++;
            triangles[k] = gl::vec4(x_1, sin(x_1)*sin(z_0), z_0, 1.0);
            k++;

            // DEBUG statements
            // fprintf(file, "i: %5d\t\tj: %5d\t\tx_0: %5.4f\t\tx_1: %5.4f\t\tz_0: %5.4f\t\tz_1: %5.4f\n", i, j, x_0,
            //          x_1, z_0, z_1);
            // debug_print("i: %d --- j: %d --- X: %f --- Y: %f --- Z: %f\n", i, j, x, y, z);
        }
    }
    // fclose (file);

    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (512, 512);
    glutInitContextVersion (3,2);
    glutInitContextProfile (GLUT_CORE_PROFILE);
    glutCreateWindow ("Sine Wave");

    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutMouseFunc (motion.mouse_click);
    glutMotionFunc (motion.mouse_drag);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit() )
    {
        return (EXIT_FAILURE);
    }

    init ();

    glEnable (GL_DEPTH_TEST);
    glutMainLoop ();

    return (EXIT_SUCCESS);
}
