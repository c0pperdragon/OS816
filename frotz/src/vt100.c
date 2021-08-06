#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frotz.h"

char* storyfilename = "";
int currentstyle;


void read_line_with_echo(char* buffer, int bsize)
{
    int l = 0;
    int c;
    for (;;)
    {
    	c = getchar();
        if (c=='\n')
        {            
            buffer[l] = 0; 
            return; 
        }
        else if (c=='\b')
        {
            if (l>0) 
            { 
                l--; 
                printf("\b\033[0K");
            }	
        }
        else if (l+1<bsize && c>=32 && c<=255) 
        {
            buffer[l] = c;
            l++;
            putchar (c);
        }
    }
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

void os_reset_screen(void) 
{ 
    currentstyle = NORMAL_STYLE;
    printf ("\033[0m");
}

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

void os_set_text_style(int s) 
{ 
    int on = s & ~currentstyle;
    int off = currentstyle & ~s;
    currentstyle = s;
    //printf ("<%d>",s);
    
    if (on & REVERSE_STYLE)
    {
        printf ("\033[7m");
    }
    if (off & REVERSE_STYLE)
    {
        printf ("\033[0m");
    }
} 

void os_set_cursor(int row, int column) 
{ 
//    printf("<%d,%d>", row, column);
    printf("\033[%d;%dH", row, column);
}

void os_scroll_area(int row, int column, int height, int width, int upwards) 
{
//   printf("[%d,%d,%d,%d,%d]", a,b,c,d,e);
    if (row==2 && column==1 && height==24 && width==80)
    {
        int i;
        os_set_cursor(80,24);
        for (i=0; i<upwards; i++) { putchar('\n'); }
    }   
}
 

void os_display_string(const zchar *s) 
{ 
    int i; 
    zchar c;
    for(i=0; (c=s[i])!=0; i++) 
    { 
        if (c == ZC_NEW_FONT) 
        {
            if (s[i+1]!=0) { i++; };
        }    
    	else if (c == ZC_NEW_STYLE)
    	{
            if (s[i+1]!=0) 
            {
                os_set_text_style(s[i+1]);
                i++; 
            };
        }
        else
        {
            os_display_char(c); 
        }
    }
}


zchar os_read_key(int timeout, int showcursor) 
{ 
    return getchar(); 
}

void os_more_prompt(void) 
{
    zchar buf[10];
    printf("<MORE>");
    os_read_key(0,0);
    printf("\b\b\b\b\b\b\033[0K");
}

zchar os_read_line(int max, zchar *buf, int timeout, int width, int continued) 
{  
    os_set_text_style(NORMAL_STYLE);
    read_line_with_echo(buf,max);
    return ZC_RETURN; 
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


char *os_read_file_name (const char *default_name, int flag)
{
    char x[100];    
    strcpy (x,"story.sav");
    printf("FILENAME: '%s'\n", x);
    return strdup(x); 
}


void os_restart_game(int code) 
{
    os_reset_screen();
}

void os_process_arguments(int argc, char *argv[]) 
{
    char buffer[50];
    if (argc>1) 
    {
        storyfilename=argv[1]; 
    }
    else
    {
        printf("Which story do you want to load? ");
        read_line_with_echo(buffer,50);
        putchar('\n');
        storyfilename=strdup(buffer);
    }
}

FILE *os_load_story(void) 
{
    printf ("Loading story file '%s'...\n",storyfilename);
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


