/* 
 * Ardesia -- a program for painting on the screen
 * with this program you can play, draw, learn and teach
 * This program has been written such as a freedom sonet
 * We believe in the freedom and in the freedom of education
 *
 * Copyright (C) 2009 Pilolli Pietro <pilolli@fbk.eu>
 *
 * Ardesia is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Ardesia is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cursors.h>
#include <utils.h>


/* The image surface that will contain the pen icon. */
static cairo_surface_t *pen_image_surface         = (cairo_surface_t*) NULL;

/* The image surface that will contain the highlighter icon. */
static cairo_surface_t *highlighter_image_surface = (cairo_surface_t*) NULL;

/* The image surface that will contain the eraser icon. */
static cairo_surface_t *eraser_image_surface      = (cairo_surface_t*) NULL;

/* The image surface that will contain the arrow icon. */
static cairo_surface_t *arrow_image_surface      = (cairo_surface_t*) NULL;


/*
 * Create pixmap and mask for the invisible cursor;
 * this is used to hide the cursor.
 */
static void
get_invisible_pixmaps (gint size,
		       GdkPixmap **pixmap,
                       GdkPixmap **mask)
{
  cairo_t *invisible_cr = (cairo_t *) NULL;
  cairo_t *invisible_shape_cr = (cairo_t *) NULL;

  *pixmap = gdk_pixmap_new ((GdkDrawable *) NULL, size, size, 1);
  *mask =  gdk_pixmap_new ((GdkDrawable *) NULL, size, size, 1);

  invisible_cr = gdk_cairo_create (*pixmap);

  cairo_set_source_rgb (invisible_cr, 1, 1, 1);
  cairo_paint (invisible_cr);
  cairo_stroke (invisible_cr);
  cairo_destroy (invisible_cr);

  invisible_shape_cr = gdk_cairo_create (*mask);

  clear_cairo_context (invisible_shape_cr);
  cairo_stroke (invisible_shape_cr);
  cairo_destroy (invisible_shape_cr);
}


/* Get the eraser image surface. */
static cairo_surface_t *
get_eraser_image_surface ()
{
  if (eraser_image_surface)
    {
      return eraser_image_surface;
    }

  eraser_image_surface = cairo_image_surface_create_from_png (ERASER_ICON);
  return eraser_image_surface;
}


/* Get the highlighter image surface. */
static cairo_surface_t *
get_highlighter_image_surface ()
{
  if (highlighter_image_surface)
    {
      return highlighter_image_surface;
    }

  highlighter_image_surface = cairo_image_surface_create_from_png (HIGHLIGHTER_ICON);
  return highlighter_image_surface;
}


/* Get the arrow image surface. */
static cairo_surface_t *
get_arrow_image_surface ()
{
  if (arrow_image_surface)
    {
      return arrow_image_surface;
    }

  arrow_image_surface = cairo_image_surface_create_from_png (ARROW_ICON);
  return arrow_image_surface;
}


/* Get the pen image surface. */
static cairo_surface_t *
get_pen_image_surface ()
{
  if (pen_image_surface)
    {
      return pen_image_surface;
    }

  pen_image_surface = cairo_image_surface_create_from_png (PENCIL_ICON);
  return pen_image_surface;
}


/* Destroy the eraser image surface. */
static
void destroy_eraser_image_surface ()
{
  if (eraser_image_surface)
    {
      cairo_surface_destroy (eraser_image_surface);
    }
}


/* Destroy the highlighter image surface. */
static
void destroy_highlighter_image_surface ()
{
  if (highlighter_image_surface)
    {
      cairo_surface_destroy (highlighter_image_surface);
    }
}


/* Destroy the pen image surface. */
static
void destroy_pen_image_surface ()
{
  if (pen_image_surface)
    {
      cairo_surface_destroy (pen_image_surface);
    }
}


/* Swap blue with red in pibxbuf. */
static void
gdk_pixbuf_swap_blue_with_red (GdkPixbuf **pixbuf)
{
  gint  n_channels = gdk_pixbuf_get_n_channels (*pixbuf);

  gint pixbuf_width = gdk_pixbuf_get_width(*pixbuf);
  gint pixbuf_height = gdk_pixbuf_get_height(*pixbuf);
  gint rowstride = gdk_pixbuf_get_rowstride (*pixbuf);
  guchar *pixels = gdk_pixbuf_get_pixels (*pixbuf);

  gint x =0;

  for( x = 0; x < pixbuf_height; x++ )
    {
      gint y =0;

      for( y = 0; y < pixbuf_width; y++ )
	{
	  guchar* p = pixels + y * rowstride + x * n_channels;
          /* swap the pixel red value with the blue */
	  guchar p0 = p[0];

	  p[0] = p[2];
	  p[2] = p0;
	}
    }

}


/* Create pixmap and mask for the eraser cursor. */
static void
get_eraser_pixbuf (gdouble size,
		   GdkPixbuf **pixbuf)
{
  gdouble circle_width = 2.0;
  cairo_t *eraser_cr = (cairo_t *) NULL;
  cairo_surface_t *image_surface = get_eraser_image_surface ();

  gint width = size + cairo_image_surface_get_width (image_surface);
  gint height = size + cairo_image_surface_get_height (image_surface);

  cairo_surface_t *surface = (cairo_surface_t *) NULL;

  *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, width, height);

  surface = cairo_image_surface_create_for_data (gdk_pixbuf_get_pixels (*pixbuf),
						 CAIRO_FORMAT_RGB24,
						 width,
						 height,
						 gdk_pixbuf_get_rowstride (*pixbuf));

  eraser_cr = cairo_create (surface);

  clear_cairo_context (eraser_cr);

  cairo_set_line_width (eraser_cr, circle_width);

  cairo_set_source_surface (eraser_cr, image_surface, 0, 0);

  cairo_paint (eraser_cr);
  cairo_stroke (eraser_cr);

  /* Add a circle with the desired width. */
  cairo_set_source_rgba (eraser_cr, 0, 0, 1, 1);
  cairo_arc (eraser_cr, width/2, height/2, (size/2)-circle_width, 0, 2 * M_PI);
  cairo_stroke (eraser_cr);

  cairo_surface_destroy (surface);
  cairo_destroy (eraser_cr);

  /* The pixbuf created by cairo has the r and b color inverted. */
  gdk_pixbuf_swap_blue_with_red (pixbuf);
}


/* Create pixmap and mask for the pen cursor. */
static void
get_pen_pixbuf (GdkPixbuf **pixbuf,
		gdouble size,
		gchar *color,
		gdouble thickness,
                gdouble arrow)
{
  cairo_t *pen_cr = (cairo_t *) NULL;
  cairo_surface_t *surface = (cairo_surface_t *) NULL;
  gdouble circle_width = 2.0;
  gdouble side_lenght = (size*3) + thickness;

  *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, (gint) side_lenght, (gint) side_lenght);


  surface = cairo_image_surface_create_for_data (gdk_pixbuf_get_pixels (*pixbuf),
						 CAIRO_FORMAT_RGB24,
						 gdk_pixbuf_get_width (*pixbuf),
						 gdk_pixbuf_get_height (*pixbuf),
						 gdk_pixbuf_get_rowstride (*pixbuf));


  pen_cr = cairo_create (surface);

  clear_cairo_context (pen_cr);

  cairo_set_operator (pen_cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgb (pen_cr, 1, 1, 1);
  cairo_paint (pen_cr);
  cairo_stroke (pen_cr);

  pen_cr = cairo_create (surface);

  clear_cairo_context (pen_cr);

  cairo_set_line_width (pen_cr, circle_width);

  cairo_surface_t *image_surface = (cairo_surface_t *) NULL;


  /* Add a circle that respect the width and the selected colour. */
  cairo_set_source_color_from_string (pen_cr, color);
 
  cairo_arc (pen_cr,
	     size/2 + thickness/2,
	     5 * size/2,
	     thickness/2,
	     0,
	     2 * M_PI);

  cairo_stroke (pen_cr);

  if (arrow)
    {/* load the arrow icon. */
      image_surface = get_arrow_image_surface ();
      cairo_set_source_surface (pen_cr, image_surface, size/2 + thickness/2, size/2);
    }
  else
    {
      /* Take the opacity. */
      gchar* alpha = g_substr (color, 6, 8);
  
      if (g_strcmp0 (alpha, "FF") == 0)
	{
	  /* load the pencil icon. */
	  image_surface = get_pen_image_surface ();
	}
      else
	{
	  /* load the highlighter icon. */
	  image_surface = get_highlighter_image_surface ();
	}
      cairo_set_source_surface (pen_cr, image_surface, thickness/2, size);
    }

  cairo_paint (pen_cr);
  cairo_stroke (pen_cr);

  cairo_surface_destroy (surface);
  cairo_destroy (pen_cr);

  /* The pixbuf created by cairo has the r and b color inverted. */
  gdk_pixbuf_swap_blue_with_red (pixbuf);
}


/* Destroy the image surfaces used for the cursors. */
static void
destroy_cached_image_surfaces ()
{
  destroy_eraser_image_surface ();
  destroy_pen_image_surface ();
  destroy_highlighter_image_surface ();
}


/* Initialize the cursors variables. */
void 
cursors_main ()
{
  // The data will be loaded on runtime.
}


/* Allocate a invisible cursor that can be used to hide the cursor icon. */
void
allocate_invisible_cursor (GdkCursor **cursor)
{
  GdkPixmap *pixmap = (GdkPixmap *) NULL;
  GdkPixmap *mask = (GdkPixmap *) NULL;

  GdkColor *background_color_p = rgba_to_gdkcolor (BLACK);
  GdkColor *foreground_color_p = rgba_to_gdkcolor (WHITE);

  get_invisible_pixmaps (1, &pixmap, &mask);
  
  *cursor = gdk_cursor_new_from_pixmap (pixmap,
					mask,
					foreground_color_p,
					background_color_p,
					0,
                                        0);

  g_object_unref (pixmap);
  g_object_unref (mask);
  g_free (foreground_color_p);
  g_free (background_color_p);			
}


/* Set the pen cursor. */
void
set_pen_cursor (GdkCursor **cursor,
		gdouble thickness,
		gchar* color,
                gboolean arrow)
{
  GdkPixbuf *pixbuf = (GdkPixbuf *) NULL;
  gint size = 12;

  get_pen_pixbuf (&pixbuf, (gdouble) size, color, thickness, arrow);

  *cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (),
					pixbuf,
					size/2 + thickness/2,
					5* size/2);

  g_object_unref (pixbuf);
}


/* Set the eraser cursor. */
void
set_eraser_cursor (GdkCursor **cursor,
		   gint size)
{
  GdkPixbuf *pixbuf = (GdkPixbuf *) NULL;

  get_eraser_pixbuf (size, &pixbuf);

  *cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (),
					pixbuf,
					gdk_pixbuf_get_width (pixbuf)/2,
					gdk_pixbuf_get_height (pixbuf)/2);

  g_object_unref (pixbuf);
}


/* Quit the cursors and free the inners variables. */
void
cursors_main_quit ()
{
  destroy_cached_image_surfaces ();
}

