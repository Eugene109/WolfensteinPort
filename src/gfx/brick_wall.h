#ifndef brick_wall_include_file
#define brick_wall_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define brick_wall_width 64
#define brick_wall_height 64
#define brick_wall_size 4098
#define brick_wall ((gfx_sprite_t *)brick_wall_arr_data)
extern unsigned char brick_wall_arr_data[];

#ifdef __cplusplus
}
#endif

#endif