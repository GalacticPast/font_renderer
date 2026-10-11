#include "renderer/gl.h"
#include "text/text.h"

typedef struct
{
    SSBO curves_ssbo;
    SSBO horizontal_bands_ssbo;
    SSBO vertical_bands_ssbo;

    VAO  b_vao;
    VBO  b_vbo;
    EBO  b_ebo;
    SSBO b_ssbo;
    VBO  instanced_vbo;

    shader shader;

    u32 projection_loc;
    u32 model_loc;
    u32 view_loc;

    // frame specific
    s_size frame_data_size;
} text_gl_state;

text_gl_state *t_gl_state;

// clang-format off
f32 vertices[] =
{ //     COORDINATES
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
};
u32 indices[] =
{
	0, 1, 2,
	0, 2, 3,
};
// clang-format on

void __text_gl_init(db_arena *arena)
{
    t_gl_state = db_arena_alloc(arena, sizeof(text_gl_state));
    ASSERT(t_gl_state);

    shader *shader = &t_gl_state->shader;
    b8 a = shader_create(arena, shader, "../assets/shaders/text_vertex.glsl", "../assets/shaders/text_fragment.glsl");
    ASSERT_WITH_MSG(a, "Text shader creation failed");

    t_gl_state->projection_loc = glGetUniformLocation(shader->program, "projection");
    t_gl_state->model_loc      = glGetUniformLocation(shader->program, "model");
    t_gl_state->view_loc       = glGetUniformLocation(shader->program, "view");

    VAO  *b_vao         = &t_gl_state->b_vao;
    VBO  *b_vbo         = &t_gl_state->b_vbo;
    EBO  *b_ebo         = &t_gl_state->b_ebo;
    SSBO *b_ssbo        = &t_gl_state->b_ssbo;
    VBO  *instanced_vbo = &t_gl_state->instanced_vbo;

    vao_create(b_vao);
    vao_bind(b_vao);

    vbo_create(b_vbo, vertices, sizeof(vertices));
    vao_link_vbo_attribs(b_vao, b_vbo, 0, 3, GL_FLOAT, 3 * sizeof(f32), (void *)0);

    vbo_create_dynamic(instanced_vbo, MAX_GLYPHS * TEXT_VEC4S_PER_GLYPH * sizeof(db_vector4));

    s_size stride = 2 * sizeof(db_vector2) + 13 * sizeof(db_vector4);

    vao_link_vbo_attribs(b_vao, instanced_vbo, 1, 2, GL_FLOAT, stride, (void *)0);
    vao_link_vbo_attribs(b_vao, instanced_vbo, 2, 2, GL_FLOAT, stride, (void *)(2 * sizeof(f32)));

    // mat4 takes locations 4..6, one vec4 column each
    vao_link_vbo_attribs(b_vao, instanced_vbo, 3, 4, GL_FLOAT, stride, (void *)(4 * sizeof(f32)));

    s_size offset                = 8 * sizeof(f32);
    s32    first_matrix_location = 4;
    s32    matrix_column_count   = 12;

    for (s32 i = 0; i < matrix_column_count; i++)
    {
        vao_link_vbo_attribs(b_vao, instanced_vbo, first_matrix_location + i, 4, GL_FLOAT, stride,
                             (void *)(offset + i * sizeof(db_vector4)));
    }
    // every instanced attribute: 1 .. 15
    for (s32 i = 1; i < first_matrix_location + matrix_column_count; i++)
    {
        glVertexAttribDivisor(i, 1);
    }

    ebo_create(b_ebo, indices, sizeof(indices));
    ebo_bind(b_ebo);
}

// needs the GL context, so it runs before renderer_shutdown
void __text_gl_deinit()
{
    ssbo_delete(&t_gl_state->curves_ssbo);
    ssbo_delete(&t_gl_state->horizontal_bands_ssbo);
    ssbo_delete(&t_gl_state->vertical_bands_ssbo);

    vbo_delete(&t_gl_state->instanced_vbo);
    vbo_delete(&t_gl_state->b_vbo);
    ebo_delete(&t_gl_state->b_ebo);
    vao_delete(&t_gl_state->b_vao);

    shader_destroy(&t_gl_state->shader);
}

void __text_upload_to_gpu(glyphs *g)
{
    ssbo_create(&(t_gl_state->curves_ssbo), 1, g->curves.data, sizeof(curve), g->curves.length);
    ssbo_bind(&(t_gl_state->curves_ssbo));

    ssbo_create(&(t_gl_state->horizontal_bands_ssbo), 2, g->horizontal_bands.data, sizeof(s32),
                g->horizontal_bands.length);
    ssbo_bind(&(t_gl_state->horizontal_bands_ssbo));

    ssbo_create(&(t_gl_state->vertical_bands_ssbo), 3, g->vertical_bands.data, sizeof(s32), g->vertical_bands.length);
    ssbo_bind(&(t_gl_state->vertical_bands_ssbo));
}

void __text_update_gpu_buffer(void *data, s_size size)
{
    ASSERT_WITH_MSG(size <= MAX_GLYPHS * TEXT_VEC4S_PER_GLYPH * sizeof(db_vector4),
                    "Insufficient size on the gpu to hold glyph data");
    t_gl_state->frame_data_size = size;
    vbo_update(&t_gl_state->instanced_vbo, data, size);
}

void __text_set_shader_uniforms(shader *shader, u32 physical_width, u32 physical_height)
{
    db_matrix4 ortho;
    // the projection works in physical pixels, matching the buffer and font_px
    db_matrix4_ortho2d(&ortho, 0, (f32)physical_width, (f32)physical_height, 0);

    db_matrix4 view;
    db_matrix4_identity(&view);

    db_matrix4 model;
    db_matrix4_identity(&model);

    glUniformMatrix4fv(t_gl_state->view_loc, 1, GL_FALSE, view.data);
    glUniformMatrix4fv(t_gl_state->projection_loc, 1, GL_FALSE, ortho.data);
    glUniformMatrix4fv(t_gl_state->model_loc, 1, GL_FALSE, model.data);
}

void __text_render(u32 physical_width, u32 physical_height)
{
    shader *shader = &t_gl_state->shader;

    shader_use(shader);

    __text_set_shader_uniforms(shader, physical_width, physical_height);

    vao_bind(&t_gl_state->b_vao);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            t_gl_state->frame_data_size / (TEXT_VEC4S_PER_GLYPH * sizeof(db_vector4)));
}
