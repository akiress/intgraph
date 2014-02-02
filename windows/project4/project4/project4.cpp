#include "Angel.h"
#include "GLShader.h"
#include <iostream>

#define WINDOW_W glutGet(GLUT_WINDOW_WIDTH)
#define WINDOW_H glutGet(GLUT_WINDOW_HEIGHT)

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int HEIGHT = 512;
const int WIDTH = 512;
const int N = 64;
const int M = 64;
const float R = 2.0f;
const float r = 0.5f;
const float D_PI = 2.0f * M_PI;

int k = 0;

point4 triangles [3*2*N*M];
vec3 normals [3*2*N*M];

point4 black = vec4(0.0f, 0.0f, 0.0f, 1.0f);
point4 white = vec4(1.0f, 1.0f, 1.0f, 1.0f);
point4 magenta = vec4(1.0f, 0.0f, 1.0f, 1.0f);
point4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);

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

int click_button;

GLfloat click_rotation_x;
GLfloat click_rotation_y;
GLfloat click_position_z;
GLfloat click_x;
GLfloat click_y;

mat4 modelview()
{
    return Translate(vec3(0, 0, position_z))
        * RotateX(rotation_x)
        * RotateY(rotation_y);
}

void init ()
{
    GLuint vao;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(triangles) + sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(triangles), triangles );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(triangles), sizeof(normals), normals );

    if ((program = LoadShader("vshader4.glsl", "fshader4.glsl")))
    {
        vloc = glGetAttribLocation (program, "vPosition");
        Ploc = glGetUniformLocation(program, "projection");
        Mloc = glGetUniformLocation(program, "model_view");
    }

    glUseProgram (program);

    vloc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vloc );
    glVertexAttribPointer( vloc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 );

    vnorm = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vnorm );
    glVertexAttribPointer( vnorm, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(triangles) );

    Mloc = glGetUniformLocation (program, "model_view");
    Ploc = glGetUniformLocation (program, "projection");
    color_loc = glGetUniformLocation (program, "fcolor");

    // Initialize shader lighting parameters
    point4 light_position( 0.0, 0.0, 4.0, 0.0 );
    color4 light_ambient( 0.0, 0.2, 0.0, 1.0 );
    color4 light_diffuse( 0.0, 0.6, 0.0, 1.0 );
    color4 light_specular( 0.0, 1.0, 0.0, 1.0 );

    color4 material_ambient( 0.0, 0.2, 0.0, 1.0 );
    color4 material_diffuse( 0.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 0.0, 1.0, 0.0, 1.0 );
    float  material_shininess = 5.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );
    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );

    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1.0, 1.0);

    glClearColor (1.0f, 0.0f, 1.0f, 0.15f);
}

void display ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 projection = Perspective(45.0f, 1.3333333f, 1.0f, 100.0f);
    mat4 model_view = modelview();

    glUniformMatrix4fv (Ploc, 1, GL_TRUE, projection);
    glUniformMatrix4fv (Mloc, 1, GL_TRUE, model_view);
    glDrawArrays( GL_TRIANGLES, 0, 3*2*N*M );
    glutSwapBuffers();
}

void mouse (int button, int state, int x, int y)
{
    click_x = static_cast<float>(x);    // Removing MS warnings with cast
    click_y = static_cast<float>(y);    // Removing MS warnings with cast
    click_button = button;
    click_rotation_x = rotation_x;
    click_rotation_y = rotation_y;
    click_position_z = position_z;
}

void motion (int x, int y)
{
    GLfloat dx = GLfloat (x - click_x) / WINDOW_W;
    GLfloat dy = GLfloat (y - click_y) / WINDOW_H;

    if (click_button == GLUT_LEFT_BUTTON)
    {
        rotation_x = click_rotation_x + 90.0f * dy;
        rotation_y = click_rotation_y + 180.0f * dx;
        if (rotation_x > 90.0) rotation_x = 90.0;
        if (rotation_x < -90.0) rotation_x = -90.0;
        if (rotation_y > 180.0) rotation_y -= 360.0;
        if (rotation_y < -180.0) rotation_y += 360.0;
    }

    else if (click_button == GLUT_RIGHT_BUTTON)
    {
        position_z = click_position_z + 5.0f * dy;
    }

    else
    {
        position_z = click_position_z + 5.0f * dy;
    }

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

vec4 createVertex(float a, float b)
{
    float x,y,z;
    point4 vertices;
    for (int i = 0 ; i < N ; i++)
    {
        for (int j = 0 ; j < M ; j++)
        {
            x = (R + r * cos(D_PI * a)) * cos(D_PI * b);
            y = (R + r * cos(D_PI * a)) * sin(D_PI * b);
            z = r * sin(D_PI * a);

            vertices = vec4(x, y, z, 1.0);

            return vertices;
        }
    }

    return vertices;
}

vec3 createNormal(float a, float b)
{
    float x,y,z,l;
    vec3 normal;
    for (int i = 0 ; i < N ; i++)
    {
        for (int j = 0 ; j < M ; j++)
        {
            x = cos(D_PI * a) * cos(D_PI * b);
            y = cos(D_PI * a) * sin(D_PI * b);
            z = sin(D_PI * a);

            l = sqrt(x*x + y*y + z*z);
            normal = vec3(x/l, y/l, z/l);

            return normal;
        }
    }

    return normal;
}

void drawTorus()
{
    for (int i = 0 ; i < N ; i++)
    {
        for (int j = 0 ; j < M ; j++)
        {
            triangles[k] = createVertex(float(i)/N, float(j)/M);
            normals[k] = createNormal(float(i)/N, float(j)/M);
            k++;
            triangles[k] = createVertex(float(i+1)/N, float(j + 1)/M);
            normals[k] = createNormal(float(i+1)/N, float(j + 1)/M);
            k++;
            triangles[k] = createVertex(float(i)/N, float(j + 1)/M);
            normals[k] = createNormal(float(i)/N, float(j + 1)/M);
            k++;
            triangles[k] = createVertex(float(i)/N, float(j)/M);
            normals[k] = createNormal(float(i)/N, float(j)/M);
            k++;
            triangles[k] = createVertex(float(i + 1)/N, float(j + 1)/M);
            normals[k] = createNormal(float(i + 1)/N, float(j + 1)/M);
            k++;
            triangles[k] = createVertex(float(i + 1)/N, float(j)/M);
            normals[k] = createNormal(float(i + 1)/N, float(j)/M);
            k++;        
        }
    }
}

int main (int argc, char** argv)
{
    drawTorus();
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (HEIGHT, WIDTH);
    glutInitContextVersion (3,2);
    glutInitContextProfile (GLUT_CORE_PROFILE);
    glutCreateWindow ("Torus with Lighting");

    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutKeyboardFunc (keyboard);
    glutMouseFunc (mouse);
    glutMotionFunc (motion);

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
