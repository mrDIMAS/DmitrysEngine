typedef struct de_point_t
{
	float x;
	float y;
	uint8_t flags;
} de_point_t;

typedef struct de_line2_t
{
	de_point_t begin;
	de_point_t end;
} de_line2_t;

typedef DE_ARRAY_DECLARE(de_line2_t, line_array_t);
typedef DE_ARRAY_DECLARE(de_point_t, point_array_t);

typedef struct de_bitmap_t
{
	unsigned char* pixels;
	int width;
	int height;
} de_bitmap_t;

typedef struct de_polygon_t
{
	DE_ARRAY_DECLARE(de_point_t, points);
} de_polygon_t;

void de_bitmap_create(de_bitmap_t * bitmap, int width, int height);
void de_bitmap_set_pixel(de_bitmap_t* bitmap, int x, int y, unsigned char pixel);
point_array_t de_vg_eval_quad_bezier(const de_point_t * p0, const de_point_t * p1, const de_point_t * p2, int steps);
bool de_vg_line_line_intersection(de_line2_t* a, de_line2_t* b, de_point_t *out);
line_array_t de_vg_polys_to_scanlines(de_polygon_t* polys, size_t poly_count, float width, float height, float scale);
de_bitmap_t de_bitmap_downscale4_box_filter(de_bitmap_t *src);
de_bitmap_t de_vg_raster_scanlines(de_bitmap_t* bitmap, line_array_t lines);