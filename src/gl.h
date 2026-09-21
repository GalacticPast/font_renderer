#include "../vendor/glad/glad.h"
#include "db.h"

typedef struct shader
{
    GLuint program;
} shader;

struct vertex_array_object
{
    GLuint id;
} typedef VAO;

struct vertex_buffer_object
{
    GLuint id;
} typedef VBO;

struct element_buffer_object
{
    GLuint id;
} typedef EBO;

void vao_create(VAO *vao_object);
void vao_bind(VAO *vao_object);
void vao_link_vbo_attribs(VAO *vao_object, VBO *vbo, GLuint layout, GLuint num_components, GLenum type,
                          GLsizeiptr stride, void *offset);
void vao_unbind();
void vao_delete(VAO *vao_object);

void vbo_create(VBO *vbo_object, GLfloat *vertices, GLsizeiptr size);
void vbo_bind(VBO *vbo_object);
void vbo_unbind();
void vbo_delete(VBO *vbo_object);

void ebo_create(EBO *ebo_object, GLuint *indices, GLsizeiptr size);
void ebo_bind(EBO *ebo_object);
void ebo_unbind();
void ebo_delete(EBO *ebo_object);

b8 shader_create(db_arena *arena, shader *shader);

void shader_use(shader *shader);
void shader_destroy(shader *shader);
