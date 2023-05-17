#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipf.h"

extern struct s_IPFheader    IPFheader;
extern struct s_IPFartheader IPFartheader;

extern char *b_dict,**p_dict,
            *b_toc, **p_toc;
extern int32 *b_slot;
extern struct s_IPFindex *b_index,**p_index;

int width;

int spacing,                    /* put a space after each word? */
    example,                    /* in an example? (1=mono, 2=lines) */
    align,                      /* example=2: alignment */
    hpos,                       /* current cursor position */
    blankl,                     /* number of blank lines already printed */
    leftmarg,defleft,rightmarg; /* margin settings */

int count[16];                  /* counter for chapter numbering */

char assembly[1024];

void print_prologue()
{
        int i;

        blankl=2;
        for(i=0;i<15;i++)       /* reset chapter counters */
          count[i]=0;
}

void print_epilogue()
{
}

void print_adjustpos(int pos)
{
        int marg=(pos<leftmarg)?leftmarg:pos;

        while(hpos<=marg) {
           putchar(' ');
           hpos++;
        }
}

void print_newline(int n)
{
        char *p,*q;
        int pos;

        for(p=assembly; *p==' '; p++)
           ;                      /* remove leading spaces */
        for(q=assembly+strlen(assembly)-1;q>=p && *q==' ';q--)
           ;                      /* remove trailing spaces */
        *(q+1)='\0';              /* cut off trailing spaces */
        if(*p) p=assembly;        /* not just spaces? print all */
        if(n!=-1 && *p) {         /* print word currently in the making */
           pos=0;                 /* default: no alignment */
           if(example==2) {       /* "lines" may be justified */
             switch (align) {     /* alignment */
             case 0x02:
                pos=width-rightmarg-strlen(assembly)+1;
                break;
             case 0x04:
                pos=(width-rightmarg-strlen(assembly)+2)/2;
                break;
             default: 
                pos=0;
             } /* endswitch */
           }
           print_adjustpos(pos);  /* do left margin */
           printf("%s",p);
           blankl=0;
        }
        if(n==-1)
           n=1;
        else
           *assembly='\0';        /* word was printed */
        while(n>blankl || n==0) {
           putchar('\n');
           blankl++;
           if(n==0) n=1;
        }
        hpos=1;
}

void print_title(int16 tocnr,int8 flags,char *title)
{
        int i;

        print_newline(3);

        if(!(flags & 0x40) && (flags & 15)) {
                                  /* not a hidden chapter */
          count[flags & 15]++;    /* count chapter */
          for(i=(flags & 15)+1;i<15;i++)
            count[i]=0;           /* reset subordinate chapter counters */
          *assembly='\0';         /* clear title string */
          for(i=1;i<=(flags & 15);i++)
            sprintf(assembly+strlen(assembly),"%d.",count[i]);
        }
        else
          strcpy(assembly,"<hidden>");
        printf("ÍÍÍ %s %s ÍÍÍ",assembly,title);

        *assembly='\0';           /* clear temporary buffer */
        blankl=0;
        spacing=1;
        example=0;
        leftmarg=0;
        defleft=0;
        rightmarg=1;
        print_newline(2);
}

void print_word(int32 word)
{
        char *s,*p;

        if(word==0xFFFFF) {             /* internal signal: new slot */
          spacing=(example==1?0:1);     /* reset spacing to default */
          return;
        }
        s=(word==0x10000)?" ":p_dict[word];
        if(!spacing || *s==' ' || example) {
           while(hpos+(int)strlen(assembly)<=leftmarg)
              strcat(assembly," ");
           strcat(assembly,s);
           if(spacing && *s!=' ') strcat(assembly," ");
        }
        else {
           p=assembly;
           if(hpos+(int)strlen(s)+(int)strlen(assembly)>width-rightmarg+1) {
              print_newline(-1);
              print_adjustpos(0);
              while(*p==' ') p++;
           }
           hpos+=printf("%s",p);
           print_adjustpos(0);
           hpos+=printf(spacing?"%s ":"%s",s);
           *assembly='\0';
           blankl=0;
        }
}

void print_special(char8 *p)
{
        char8 esclen,esccode;

        switch(*p) {
        case 0xFA:                /* end-of-paragraph */
           print_newline(2);
           leftmarg=defleft;      /* reset default left margin */
           spacing=(example==1?0:1);
           break;
        case 0xFC:
           spacing=!spacing;
           break;
        case 0xFD:
           print_newline(0);
           spacing=(example==1?0:1);
           break;
        case 0xFE:                /* space */
           print_word(0x10000);
           break;
        case 0xFF:
           p++;                   /* skip over Esc code */
           esclen=*(p++);
           esccode=*(p++);

           switch (esccode) {
           case 0x02:
           case 0x11:
           case 0x12:
              if(esccode==0x11) {
                 defleft=leftmarg=p[0];
                 print_newline(1);
              }
              else
                 defleft=leftmarg=p[0];
              break;
           case 0x1C:             /* set left margin to cursor posn */
              leftmarg=hpos+strlen(assembly)-1;
              break;              /* default not changed */

           case 0x0B:             /* start code example */
              example=1; spacing=0;
              print_newline(2);
              break;
           case 0x0C:             /* end code example */
              example=0; spacing=1;
              print_newline(1);
              break;
           case 0x1A:             /* start plain (lines) example */
              example=2; spacing=0;
              align=p[0];         /* store alignment setting */
              print_newline(2);
              break;
           case 0x1B:             /* end plain example */
              example=0; spacing=1;
              print_newline(1);
              break;
           } /* endswitch */
        } /* endswitch */
}

int print_parse(int argc,char **argv,int arg1)
{
        if(arg1<argc)
          sscanf(argv[arg1],"%d",&width);
        else
          width=79;
        return 0;
}

char *print_banner()
{
        fputs("\nipf2txt 1.02 -=- by Marcus Gr”ber\n"
                "Decompile OS/2 INF/HLP files to ASCII text\n\n",stderr);
        return " [<width>]";
}
