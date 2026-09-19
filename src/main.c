#define DB_IMPLEMENTATION
#include "db.h"
#include "input.h"
#include "platform/platform.h"

#include "../vendor/glad/glad.h"

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

void shader_create(db_arena *arena, shader *shader);

void shader_use(shader *shader);
void shader_destroy(shader *shader);

b8 opengl_startup(db_arena *main_arena)
{
    b8 success = platform_startup(main_arena, "slug", 0, 0, 600, 400);
    if (!success)
        return false;
    input_initialize(main_arena);
    return true;
}

// clang-format off
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};
// clang-format on

int main()
{
    // startup the opengl context;
    db_arena_params params     = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};
    db_arena        main_arena = db_arena_init_with_size(&params, MB(10));
    b8              success    = opengl_startup(&main_arena);
    if (!success)
        return 0;

    while (true)
    {
        platform_pump_messages();

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        platform_swap_buffers();
    }
}

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

void get_shaders(db_string *vertex_shader_source, db_string *fragment_shader_source)
{
}

void shader_create(db_arena *arena, shader *shader)
{
    printf("Creating vertex and fragment shaders\n");
    db_string vertex_shader_source   = db_string_make(arena, "vertex.glsl");
    db_string fragment_shader_source = db_string_make(arena, "fragment.glsl");

    get_shaders(&vertex_shader_source, &fragment_shader_source);

    printf("Compiling vertex shader\n");
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char *vert_c_str = db_string_get_cstr(arena, &vertex_shader_source);

    glShaderSource(vertex_shader, 1, &vert_c_str, NULL);
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

    u32         fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *frag_c_str      = db_string_get_cstr(arena, &fragment_shader_source);

    glShaderSource(fragment_shader, 1, &frag_c_str, NULL);
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
}

void shader_use(shader *shader)
{
    glUseProgram(shader->program);
}
void shader_destroy(shader *shader)
{
    glDeleteProgram(shader->program);
}
