#ifndef UTIL_H
#define UTIL_H

point_f s_coord_to_point(s_coord* s);
void draw_line(int x0, int y0, int x1, int y1, char render_char);
void draw_circle(int x_offset, int y_offset, int r, char c);
void draw_filled_circle(int x_offset, int y_offset, int radius, char c);

#endif