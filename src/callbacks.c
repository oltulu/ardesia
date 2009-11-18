/* 
 * Ardesia -- a program for painting on the screen
 * with this program you can play, draw, learn and teach
 * This program has been written such as a freedom sonet
 * We believe in the freedom and in the freedom of education
 *
 * Copyright (C) 2009 Pilolli Pietro <pilolli@fbk.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
 
/* This is the file where put the callback of the events generated by the ardesia interface
 * If you are trying to add some functionality and you have already add a button in interface
 * you are in the right place. In any case good reading!
 * Pay attentioni; all the things are delegated to the annotate commandline tool
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "recorder.h"
#include "saver.h"
#include "color_selector.h"
#include "preference_dialog.h"

#include "annotate.h"

#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include <string.h> 

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <math.h>
#include <utils.h>
#include <pngutils.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


#define COLORSIZE 9

/* annotation is visible */
gboolean     visible = TRUE;

/* pencil is selected */
gboolean     pencil = TRUE;

/* selected color in RGBA format */
gchar*       color = NULL;

/* selected line width */
int          tickness = 15;

gboolean     highlighter = FALSE;

/* arrow=0 mean no arrow, arrow=1 mean normal arrow, arrow=2 mean double arrow */
int        arrow = 0;

/* Default folder where store images and videos */
gchar*       workspace_dir = NULL;




/* Called when close the program */
gboolean  quit()
{
  gboolean ret=FALSE;
  quit_recorder();
  annotate_quit();
  remove_background();
  /* Disalloc */
  g_object_unref ( G_OBJECT(gtkBuilder) ); 

  gtk_main_quit();
  exit(ret);
}


/* Add alpha channel to build the RGBA string */
void add_alpha(char *color)
{
  if (highlighter)
    {
      color[6]='8';  
      color[7]='8';  
    }
  else
    {
      color[6]='F';  
      color[7]='F';  
    }
   color[8]=0;  
}


/* Start to annotate calling annotate */
void annotate()
{
  pencil=TRUE;
  if (color==NULL)
    {
      color = malloc(COLORSIZE);
      strcpy(color,"FF0000");
    }
  
  add_alpha(color);
  
  annotate_set_color(color);

  annotate_set_width(tickness);

  annotate_set_arrow(arrow);

  annotate_toggle_grab();
}


/* Start to erase calling annotate */
void erase()
{
  pencil=FALSE;

  annotate_set_width(tickness);
  annotate_eraser_grab ();  
}


/* 
 * This function is called when the thick property is changed;
 * start annotate with pen or eraser depending on the selected tool
 */
void thick()
{
  if (pencil)
    {
      annotate();
    }
  else
    {
      erase();
    }
}


/* Start event handler section */


gboolean on_quit                          (GtkWidget       *widget,
                                           GdkEvent        *event,
                                           gpointer         user_data)
{
  return quit();
}


gboolean on_winMain_delete_event          (GtkWidget       *widget,
                                           GdkEvent        *event,
                                           gpointer         user_data)
{
  return quit();
}


void on_toolsEraser_activate              (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  erase();
}


void
on_toolsHighlighter_activate          (GtkToolButton   *toolbutton,
				      gpointer         user_data)
{
  if (gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(toolbutton)))
    {
       highlighter = TRUE;
    }
  else
    {
       highlighter = FALSE;
    }
  if (pencil)
    {
      annotate();
    }
  else
    {
      erase();
    }
}


void on_toolsArrow_activate               (GtkToolButton   *toolbutton,
				           gpointer         user_data)
{
  if (gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(toolbutton)))
    {
      /* if single arrow is active release it */
      GtkToggleToolButton* doubleArrowToolButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(gtkBuilder,"buttonDoubleArrow"));
      if (gtk_toggle_tool_button_get_active(doubleArrowToolButton))
        {
	  gtk_toggle_tool_button_set_active(doubleArrowToolButton, FALSE); 
        }
      arrow=1;
    }
  else
    {
      arrow=0;
    }
  if (pencil)
    {
      annotate();
    }
  else
    {
      erase();
    }
}


void on_toolsDoubleArrow_activate         (GtkToolButton   *toolbutton,
					   gpointer         user_data)
{

  if (gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(toolbutton)))
    {
      /* if single arrow is active release it */
      GtkToggleToolButton* arrowToolButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(gtkBuilder,"buttonArrow"));
      if (gtk_toggle_tool_button_get_active(arrowToolButton))
        {
	  gtk_toggle_tool_button_set_active(arrowToolButton, FALSE); 
        }
      arrow=2;
    }
  else
    {
      arrow=0;
    }
  if (pencil)
    {
      annotate();
    }
  else
    {
      erase();
    }

}


void on_toolsVisible_activate             (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  if (visible)
    {
      annotate_hide_window();
      visible=FALSE;
      /* set tooltip to unhide */
      gtk_tool_item_set_tooltip_text((GtkToolItem *) toolbutton,"Unhide");
    }
  else
    {
      annotate_show_window();
      visible=TRUE;
      /* set tooltip to hide */
      gtk_tool_item_set_tooltip_text((GtkToolItem *) toolbutton,"Hide");
    }
}


void on_toolsScreenShot_activate	  (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  start_save_image_dialog(toolbutton, get_annotation_window(), workspace_dir);
}


void on_toolsRecorder_activate            (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{ 
  if(is_recording())
    {
      quit_recorder();
      /* set stop tooltip */ 
      gtk_tool_item_set_tooltip_text((GtkToolItem *) toolbutton,"Record");
      /* put icon to record */
      gtk_tool_button_set_stock_id (toolbutton, "gtk-media-record");
    }
  else
    {      
      /* Release grab */
      annotate_release_grab ();
      /* the recording is not active */ 
      gboolean status = start_save_video_dialog(toolbutton, get_annotation_window(), workspace_dir);
      if (status)
        {
          /* set stop tooltip */ 
          gtk_tool_item_set_tooltip_text((GtkToolItem *) toolbutton,"Stop");
          /* put icon to stop */
          gtk_tool_button_set_stock_id (toolbutton, "gtk-media-stop");
        }
    }
  annotate();
}


void on_thickScale_value_changed          (GtkHScale   *hScale,
					   gpointer         user_data)
{
  tickness=gtk_range_get_value(&hScale->scale.range);
  thick();
}


void on_toolsPreferences_activate	  (GtkToolButton   *toolbutton,
					   gpointer         user_data)
{
  /* Release grab */
  annotate_release_grab ();
  start_preference_dialog(toolbutton, get_annotation_window(), 
                          PACKAGE_DATA_DIR G_DIR_SEPARATOR_S PACKAGE G_DIR_SEPARATOR_S);
  /* This is a workaround to reput the annotation window over the background window */
  annotate_hide_window ();
  annotate(); 
}


void on_buttonClear_activate              (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  annotate_clear_screen (); 
}


void on_buttonColor_activate	          (GtkToolButton   *toolbutton,
					   gpointer         user_data)
{
  /* Release grab */
  annotate_release_grab ();
  color = start_color_selector_dialog(toolbutton, get_annotation_window(), workspace_dir, color);
  annotate(); 
}


/* Start color handlers */

void on_colorBlack_activate               (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"000000");
  annotate();
}


void on_colorBlue_activate                (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"3333CC");
  annotate();
}


void on_colorRed_activate                 (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"FF0000");
  annotate();
}


void on_colorGreen_activate               (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"008000");
  annotate();
}


void on_colorLightBlue_activate           (GtkToolButton   *toolbutton,
                                           gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"00C0FF");
  annotate();
}


void on_colorLightGreen_activate            (GtkToolButton   *toolbutton,
                                             gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"00FF00");
  annotate();
}


void on_colorMagenta_activate               (GtkToolButton   *toolbutton,
                                             gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"FF00FF");
  annotate();
}


void on_colorOrange_activate                (GtkToolButton   *toolbutton,
                                             gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"FF8000");
  annotate();
}


void on_colorYellow_activate                (GtkToolButton   *toolbutton,
                                             gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"FFFF00");
  annotate();
}


void on_colorWhite_activate                (GtkToolButton   *toolbutton,
	       			            gpointer         user_data)
{
  color = malloc(COLORSIZE);
  strcpy(color,"FFFFFF");
  annotate();
}


