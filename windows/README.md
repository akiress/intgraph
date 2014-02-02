# Freeglut & GLEW Installation

## Downloads

[Freeglut](http://files.transmissionzero.co.uk/software/development/GLUT/freeglut-MSVC.zip)

[GLEW](http://sourceforge.net/projects/glew/files/latest/download)

1. Unzip both files to anywhere.
2. Open Visual Studio 2012
	- create an empty C++ application
	- I placed the header files in Headers
	- I placed the rest of the files (.cpp/.glsl) in Source
3.  Freeglut
	- copy <path-to-freeglut>\bin\freeglut.dll <path-to-application-executable> ( Likely to be the Debug folder of the app )
	- copy <path-to-freeglut>\include\GL <path-to-VS-includes> ( ex: C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\ )
	- copy <path-to-freeglut>\lib\freeglut.dll <path-to-VS-lib> ( ex: C:\Program files (x86)\Microsoft Visual Studio 11.0\VC\lib\ )
4. GLEW
	- copy <path-to-glew>\bin\Release\Win32\glew32.dll C:\Windows\System32
	- copy <path-to-glew>\include\GL <path-to-VS-includes> ( ex: C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\ )
	- copy <path-to-glew>\lib\Release\Win32\\* ( ex: C:\Program files (x86)\Microsoft Visual Studio 11.0\VC\lib\ )
5. Code changes
	- Depending on if you want to copy the glew32.dll in the folder with the executable, I show two snippets of code change.
		- The first snippet links against the static library but issues a warning when built
		- The second snippet links against the dynamic library and issues no warning. It is important to note that 
		  glew32.dll has to be in the folder of the executable to work in this case
	- Note:
		- For some reason, I had to have two copies of the *.glsl files. One set was in the source location when I created the
		  project, which allows for running the program in VS and getting output. Trying to run from the Example1.exe from a
		  folder would not work until I copied the *.glsl files into the same Debug folder as the executable.
	- Angel.h
```c++
//----------------------------------------------------------------------------
//
// --- Include OpenGL header files and helpers ---
//
//   The location of these files vary by operating system.  We've included
//     copies of open-soruce project headers in the "GL" directory local
//     this this "include" directory.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#pragma comment(lib, "glew32s.lib")
```
#### OR IF YOU COPY glew32.dll into the Debug folder where Example1.exe is located

```c++
//----------------------------------------------------------------------------
//
// --- Include OpenGL header files and helpers ---
//
//   The location of these files vary by operating system.  We've included
//     copies of open-soruce project headers in the "GL" directory local
//     this this "include" directory.
//

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#pragma comment(lib, "glew32")
```