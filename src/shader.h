
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform vec2 offset;\n"
"uniform float scale;\n"
"void main()\n"
"{\n"
"   vec2 pos=(aPos * scale ) + offset;\n"
"   gl_Position = vec4(pos.x,pos.y,0.0, 1.0);\n"
"   TexCoord=aTexCoord;\n"
"}\n";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D spriteTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(spriteTexture,TexCoord);\n"
"}\n";



