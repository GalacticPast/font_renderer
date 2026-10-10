#include "gl.h"
#include "platform/platform.h"
#include "text.h"

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

void text_init(db_arena *arena)
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

    s_size stride = 18 * sizeof(db_vector2) + 4 * sizeof(db_vector4);
    vao_link_vbo_attribs(b_vao, instanced_vbo, 1, 2, GL_FLOAT, stride, (void *)0);
    vao_link_vbo_attribs(b_vao, instanced_vbo, 2, 2, GL_FLOAT, stride, (void *)(2 * sizeof(f32)));

    // mat4 takes locations 3..6, one vec4 column each
    vao_link_vbo_attribs(b_vao, instanced_vbo, 3, 4, GL_FLOAT, stride, (void *)(4 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 4, 4, GL_FLOAT, stride, (void *)(8 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 5, 4, GL_FLOAT, stride, (void *)(12 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 6, 4, GL_FLOAT, stride, (void *)(16 * sizeof(f32)));

    // h_band_loc mat4 takes locations 7..10, packing bands_loc[0..7] (horizontal) two bands per column
    vao_link_vbo_attribs(b_vao, instanced_vbo, 7, 4, GL_FLOAT, stride, (void *)(20 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 8, 4, GL_FLOAT, stride, (void *)(24 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 9, 4, GL_FLOAT, stride, (void *)(28 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 10, 4, GL_FLOAT, stride, (void *)(32 * sizeof(f32)));

    // v_band_loc mat4 takes locations 11..14, packing bands_loc[8..15] (vertical) two bands per column
    vao_link_vbo_attribs(b_vao, instanced_vbo, 11, 4, GL_FLOAT, stride, (void *)(36 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 12, 4, GL_FLOAT, stride, (void *)(40 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 13, 4, GL_FLOAT, stride, (void *)(44 * sizeof(f32)));
    vao_link_vbo_attribs(b_vao, instanced_vbo, 14, 4, GL_FLOAT, stride, (void *)(48 * sizeof(f32)));

    for (int i = 1; i <= 14; i++)
    {
        glVertexAttribDivisor(i, 1);
    }

    ebo_create(b_ebo, indices, sizeof(indices));
    ebo_bind(b_ebo);
}

void text_deinit()
{
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

void __text_set_shader_uniforms(shader *shader)
{
    u32 width, height;
    platform_get_window_dimensions(&width, &height);
    f32 display_scale = platform_get_display_scale();

    db_matrix4 ortho;
    // the projection works in physical pixels, matching the buffer and font_px
    db_matrix4_ortho2d(&ortho, 0, width * display_scale, height * display_scale, 0);

    db_matrix4 view;
    db_matrix4_identity(&view);

    db_matrix4 model;
    db_matrix4_identity(&model);

    glUniformMatrix4fv(t_gl_state->view_loc, 1, GL_FALSE, view.data);
    glUniformMatrix4fv(t_gl_state->projection_loc, 1, GL_FALSE, ortho.data);
    glUniformMatrix4fv(t_gl_state->model_loc, 1, GL_FALSE, model.data);
}

void text_render(f32 font_size)
{
    shader *shader = &t_gl_state->shader;

    shader_use(shader);
    // this is kinda terrible fuuuu
    glUniform1f(glGetUniformLocation(shader->program, "font_px"), font_size);

    __text_set_shader_uniforms(shader);

    vao_bind(&t_gl_state->b_vao);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            t_gl_state->frame_data_size / (TEXT_VEC4S_PER_GLYPH * sizeof(db_vector4)));
}
