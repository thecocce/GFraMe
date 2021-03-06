
static char sprVs[] = 
  "#version 330\n"
  "layout(location = 0) in vec2 vtx;\n"
  "out vec2 texCoord;\n"
  "uniform mat4 locToGL;\n"
  "uniform vec2 texDimensions;\n"
  "uniform vec2 rotation;\n"
  "uniform vec2 scale;\n"
  "uniform vec2 translation;\n"
  "uniform vec2 dimensions;\n"
  "uniform vec2 texOffset;\n"
  "void main() {\n"
  "  mat2 rot = mat2(rotation.x, -rotation.y,"
  "                  rotation.y, rotation.x);\n"
  "  vec2 pos = vtx;\n"
  "  pos *= dimensions;\n"
  "  pos *= scale;\n"
  //"  pos *= rot;\n"
  "  pos += dimensions*vec2(0.5f,"
  "                            0.5f);\n"
  "  pos += translation;\n"
  "  vec4 position = vec4(pos.x, pos.y,"
  "                     -1.0f, 1.0f);\n"
  "  gl_Position = position*locToGL;\n"
  
  "  vec2 _texCoord = vtx + vec2(0.5f,"
                             "0.5f);\n"
  "  _texCoord *= texDimensions;\n"
  "  _texCoord *= dimensions;\n"
  "  texCoord = texDimensions*texOffset"
  "                +_texCoord;\n"
  "}\n";

static char sprFs[] = 
  "#version 330\n"
  "in vec2 texCoord;\n"
  "uniform sampler2D gSampler;\n"
  "uniform float alpha;\n"
  "void main() {\n"
  "  gl_FragColor = texture2D(gSampler, texCoord.st);\n"
  "  gl_FragColor.a *= alpha;\n"
  "}\n";

static char bbVs[] = 
  "#version 330\n"
  "layout(location = 0) in vec2 vtx;\n"
  "out vec2 texCoord;\n"
  "void main() {\n"
  "  gl_Position = vec4(vtx, -1.0f, 1.0f);\n"
  "  texCoord = 0.5f * vtx + vec2(0.5f, 0.5f);\n"
  "}\n";

static char bbFs[] = 
  "#version 330\n"
  "in vec2 texCoord;\n"
  "uniform sampler2D gSampler;\n"
  "uniform vec2 texDimensions;\n"
  "void main() {\n"
  "  vec2 texPos = texCoord.st;\n"
  "  vec3 pixel = texture2D(gSampler, texPos).rgb;\n"
  
  "  texPos.y += texDimensions.y;\n"
  "  vec3 pixelBelow = texture2D(gSampler, texPos).rgb;\n"
  
  "  int y = int(gl_FragCoord.y - 0.5f);\n"
  "  y = (1 + y % 3) >> 1;\n"
  "  pixel = pixel * y + (pixel + pixelBelow) * 0.33f * (1 - y);\n"
  "  gl_FragColor = vec4(pixel, 1.0f);\n"
  "}\n";

static char bbFs_noSL[] = 
  "#version 330\n"
  "in vec2 texCoord;\n"
  "uniform sampler2D gSampler;\n"
  "uniform vec2 texDimensions;\n"
  "void main() {\n"
  "  gl_FragColor = texture2D(gSampler, texCoord.st);\n"
  "}\n";

