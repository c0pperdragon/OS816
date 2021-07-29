#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frotz.h"

char* storyfilename = "";



void os_process_arguments(int argc, char *argv[]) 
{
    storyfilename=argc>1 ? argv[1] : "unknown"; 
}

void os_init_screen(void) 
{
    z_header.screen_rows = 24;
    z_header.screen_cols = 80;
    z_header.interpreter_version = 'F';
    z_header.interpreter_number = 1;
    z_header.screen_height = z_header.screen_rows;
    z_header.screen_width = z_header.screen_cols;
    z_header.font_width = 1; 
    z_header.font_height = 1;
};


void os_display_char(zchar c) 
{ 
    switch (c)
    {
    	case ZC_RETURN:
            putchar ('\n');
            break;
        case ZC_GAP: 
            putchar(' ');
            putchar(' ');
            break; 
        case ZC_INDENT: 
            putchar(' ');
            putchar(' ');
            putchar(' ');
            break;
        default:
            if (c>=32 && c<=255) { putchar(c); }
            else printf ("<%d>",c);
    }      
}

void os_display_string(const zchar *s) 
{ 
    int i; 
    zchar c;
    for(i=0; (c=s[i])!=0; i++) 
    { 
    	if (c == ZC_NEW_FONT || c == ZC_NEW_STYLE)
    	{
            if (s[i+1]!=0) { i++; };
        }
        else
        {
            os_display_char(c); 
        }
    }
}

void os_more_prompt(void) 
{
    zchar buf[10];
    printf("<MORE>");
    os_read_line(10, buf, 0,0,0); 
}

void os_set_cursor(int row, int column) 
{ 
    printf("\033[%d;%dH", row, column);
}

void os_scroll_area(int a, int b, int c, int d, int e) 
{
    putchar('\n');
//    printf("<%d,%d,%d,%d,%d>", a,b,c,d,e);
}



void os_fatal(const char *format, ...) 
{ 
	printf("Fatal: %s\n",format); 
	os_quit(1); 
}

int os_font_data(int font, int *height, int *width)
{
	if (font == TEXT_FONT) {
		*height = 1;
		*width = 1;
		return 1;
	}
	return 0;
}


char *os_read_file_name(const char *prompt, int i) 
{
    return storyfilename; 
}


zchar os_read_key(int timeout, int showcursor) 
{ 
    return getchar(); 
}

zchar os_read_line(int max, zchar *buf, int timeout, int width, int continued) 
{  
    int l = 0;
    zchar c;
    for (;;)
    {
    	c = getchar();
    	if (c=='\n') 
    	{ 
    	    buf[l] = 0; 
    	    return ZC_RETURN; 
    	}
    	if (c=='\b')
    	{
    	    if (l>0) { l--; }	
	}
	else if (l+1<max && c>=32 && c<=255) 
	{
	    buf[l] = c;
	    l++;
	}
    }
}

void os_reset_screen(void) 
{ 
    printf ("n\n\n");
}

void os_restart_game(int code) 
{
    printf ("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

FILE *os_load_story(void) 
{
    printf ("Loading story file...\n");
    return fopen(storyfilename, "rb"); 
};

int os_storyfile_seek(FILE *f, long offset, int whence)
{
    return fseek(f,offset,whence); 
}

int os_storyfile_tell(FILE *f) 
{ 
    return ftell(f); 
} 

int os_string_width(const zchar *s) 
{
    int w = 0;
    int i; 
    zchar c;
    for(i=0; (c=s[i])!=0; i++) 
    { 
    	if (c == ZC_NEW_FONT || c == ZC_NEW_STYLE)
    	{
	    if (s[i+1]!=0) { i++; };
	}
	else if (c>=32)
	{
           w++; 
        }
    }
    return w;
}

void os_warn(const char *formatstring, ...) 
{
    printf("Warning: %s\n", formatstring); 
}

void os_quit(int code) 
{ 
    exit(code); 
}


/**
 * Called regularly by the interpreter, at least every few instructions
 * (only when interpreting: e.g., not when waiting for input).
 */
void    os_tick(void) {}

/* This is callable only from resize_screen. */
bool os_repaint_window(int win, int ypos_old, int ypos_new, int xpos,
  int ysize, int xsize) { return FALSE; }


void os_beep(int duration) {}
int os_char_width(zchar c) { return 1; }
int os_check_unicode(int font, zchar c) { return 1; }
zword os_to_true_colour (int c)  { return 0; }
void os_init_setup(void) {}
void os_set_colour(int a, int b) { }
int os_from_true_colour(zword c) { return 0; } 
void os_set_font(int font) { } 
void os_set_text_style(int s) { } 
void os_start_sample(int a, int b, int c, zword d) {}
void os_stop_sample(int a) {}
int os_random_seed(void) { return 214; }  // looks random to me
void os_init_sound(void) {};
int os_peek_colour(void) { return BLACK_COLOUR; }
bool os_picture_data(int num, int *height, int *width) { return TRUE; }
void os_prepare_sample(int a) { }
void os_draw_picture(int a, int b, int c) {}
void os_erase_area(int x, int y, int w, int h, int b) { }
void os_finish_with_sample(int s) { } 


