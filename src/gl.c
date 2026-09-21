#include "gl.h"

void vao_create(VAO *vao_object)
{
    glGenVertexArrays(1, &vao_object->id);
}
void vao_link_vbo_attribs(VAO *vao_object, VBO *vbo, GLuint layout, GLuint num_components, GLenum type,
                          GLsizeiptr stride, void *offset)
{
    vbo_bind(vbo);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo_unbind();
}

void vao_bind(VAO *vao_object)
{
    glBindVertexArray(vao_object->id);
}

void vao_unbind()
{
    glBindVertexArray(0);
}
void vao_delete(VAO *vao_object)
{
    glDeleteVertexArrays(1, &vao_object->id);
}

void vbo_create(VBO *vbo_object, GLfloat *vertices, GLsizeiptr size)
{
    glGenBuffers(1, &vbo_object->id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_object->id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void vbo_bind(VBO *vbo_object)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_object->id);
}

void vbo_unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void vbo_delete(VBO *vbo_object)
{
    glDeleteBuffers(1, &vbo_object->id);
}

void ebo_create(EBO *ebo_object, GLuint *indices, GLsizeiptr size)
{

    glGenBuffers(1, &ebo_object->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_object->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void ebo_bind(EBO *ebo_object)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_object->id);
}
void ebo_unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void ebo_delete(EBO *ebo_object)
{
    glDeleteBuffers(1, &ebo_object->id);
}

b8 shader_create(db_arena *arena, shader *shader)
{
    printf("Creating vertex and fragment shaders\n");

    db_file_contents vertex_shader_source =
        db_file_read_contents(arena, db_file_mode_read, 1, "../assets/shaders/vertex.glsl");
    if (vertex_shader_source.size == 0)
    {
        printf("shader copying error\n");
        return false;
    }
    db_file_contents fragment_shader_source =
        db_file_read_contents(arena, db_file_mode_read, 1, "../assets/shaders/fragment.glsl");
    if (fragment_shader_source.size == 0)
    {
        printf("shader copying error\n");
        return false;
    }

    printf("Compiling vertex shader\n");
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char *const vert_src = vertex_shader_source.data;
    glShaderSource(vertex_shader, 1, &vert_src, NULL);
    glCompileShader(vertex_shader);

    s32  success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);

        printf("Vertex shader compilation failed. %s\n", info_log);
        DEBUG_BREAK;
    }
    printf("Vertex shader compiled succesfully\n");

    printf("Compiling fragment shader\n");

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *const frag_src = fragment_shader_source.data;
    glShaderSource(fragment_shader, 1, &frag_src, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);

        printf("Fragment shader compilation failed. %s\n", info_log);
        DEBUG_BREAK;
    }

    shader->program = glCreateProgram();
    glAttachShader(shader->program, vertex_shader);
    glAttachShader(shader->program, fragment_shader);
    glLinkProgram(shader->program);

    glGetShaderiv(shader->program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shader->program, 512, NULL, info_log);

        printf("Shader progam linking failed. %s\n", info_log);
        DEBUG_BREAK;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    printf("Succsefully created vertex and fragment shaders\n");
    return true;
}

void shader_use(shader *shader)
{
    glUseProgram(shader->program);
}
void shader_destroy(shader *shader)
{
    glDeleteProgram(shader->program);
}
