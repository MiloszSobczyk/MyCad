#include "VertexLayoutTypes.h"

#include "GL/glew.h"

void PositionVertexLayout::SetupVAO() const
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionVertex), (void*)0);
}

void PositionNormalVertexLayout::SetupVAO() const
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertex), (void*)(sizeof(Algebra::Vector4)));
}
