#include "glad/glad.h"
#include "glad/khrplatform.h"
#include <GLFW/glfw3.h>

#include "../common.h"
#include "cbmchargen.h"

void makeTextShaderObject(struct GlObjectDataSet *vds) {
    makeGlObject(vds);
  
  const size_t floatsInVertex = FLOATS_IN_POINT + 2 * FLOATS_IN_COLOR;
  
  // Position attribute (location 0)
  glVertexAttribPointer(0, FLOATS_IN_POINT, GL_FLOAT, GL_FALSE, floatsInVertex * sizeof(GLfloat), (void *)0);
  glEnableVertexAttribArray(0);
  
  // Text color attribute (location 1)
  glVertexAttribPointer(1, FLOATS_IN_COLOR, GL_FLOAT, GL_TRUE, floatsInVertex * sizeof(GLfloat), (void *)(FLOATS_IN_POINT * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  
  // Background color attribute (location 2)
  glVertexAttribPointer(2, FLOATS_IN_COLOR, GL_FLOAT, GL_TRUE, floatsInVertex * sizeof(GLfloat), (void *)((FLOATS_IN_POINT + FLOATS_IN_COLOR) * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);
}