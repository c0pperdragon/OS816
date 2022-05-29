#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frotz.h"

char* storyfilename = "";
int currentstyle;

int fileexists(const char* name)
{
    FILE *f = fopen(name, "rb");
    if (!f) { return 0; }
    fclose(f);
    return 1;
}

void read_line_with_echo(char* buffer, int bsize, const char* preset)
{
    int l = 0;
    int c;
    int i;
    
    for (i=0; preset && preset[i] && l+1<bsize; i++)
    {   
        c = preset[i];
        if (c>=32 && c<=126) 
        {
            buffer[l] = c;
            l++;
            putchar (c);            
        }
    }
    for (;;)
    {
    	c = getchar();
        if (c=='\n' || c=='\r')
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
        else if (l+1<bsize && c>=32 && c<=126) 
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
    printf ("\033[0m\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
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
    printf("\033[%d;%dH", row, column);
}

void os_scroll_area(int row, int column, int height, int width, int upwards) 
{
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
    read_line_with_echo(buf,max, 0);
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

char *os_read_file_name (const char *default_name, int filetype)
{
    char x[110];    
    char* ext;
    switch (filetype) 
    {
        case FILE_RESTORE:  printf("Restore state from file"); ext=".sav"; break;
        case FILE_SAVE:     printf("Save state to file"); ext=".sav"; break;
        case FILE_SCRIPT:   printf("Write transcript to file"); ext=".txt"; break;
        case FILE_PLAYBACK: printf("Play back commands from file"); ext=".log"; break;
        case FILE_RECORD:   printf("Record commands to file"); ext=".log"; break;
        case FILE_LOAD_AUX: printf("Auxilary file load"); ext=".aux"; break;
        case FILE_SAVE_AUX: printf("Auxilary file save"); ext=".aux"; break;
        default:            printf("Unspecified file"); ext=""; break;
    }
    if (!default_name)
    {
        char *dot;
        strcpy (x, storyfilename);
        dot = strstr(x, ".");
        if (dot) 
        {
            strcpy (dot, ext);
            default_name = x;
        }
    }

    printf(": ");
    read_line_with_echo(x, 100, default_name);
    printf("\n");
    return strdup(x); 
}

void os_restart_game(int code) 
{
    os_reset_screen();
}

void os_process_arguments(int argc, char *argv[]) 
{
    char buffer[110];
    if (argc>1) 
    {
        storyfilename=argv[1]; 
    }
    else
    {
        printf("Which story do you want to load (zork1, zork2, zork3)? ");
        read_line_with_echo(buffer,100,0);
        putchar('\n');
        // if file does not exist try to guess an extension
        if (!fileexists(buffer))
        {
            int l = strlen(buffer);
            strcpy (buffer+l, ".z3");
            if (!fileexists(buffer)) 
            {
                strcpy (buffer+l, ".z5");
                if (!fileexists(buffer)) { strcpy(buffer+l, ""); }
            }
        }        
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


