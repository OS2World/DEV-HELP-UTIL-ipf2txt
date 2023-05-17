#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipf.h"

struct s_IPFheader    IPFheader;
struct s_IPFartheader IPFartheader;

FILE *ipf;

/* macros to get structures from file quickly */
#define ipf_getl(item,len) fread(item,(len),1,ipf)
#define ipf_get(item) ipf_getl(item,sizeof(*(item)))
#define ipf_getat(pos,item) {fseek(ipf,pos,SEEK_SET);ipf_get(item);}
#define ipf_getatl(pos,item,len) {fseek(ipf,pos,SEEK_SET);ipf_getl(item,len);}

/* macros to read variable length structures in memory */
#define get_int8(p) (*(((int8 *)p)++))
#define get_int16(p) (*(((int16 *)p)++))
#define get_int32(p) (*(((int32 *)p)++))

char *b_dict,**p_dict,
     *b_toc, **p_toc;
int32 *b_slot;
struct s_IPFindex *b_index,**p_index;


void print_prologue();
void print_epilogue();
void print_title(int16 tocnr,int8 flags,char *title);
void print_word(int32 word);
void print_special(char8 *p);
int print_parse(int argc,char **argv,int arg1);
char *print_banner();


#ifdef _MSDOS                   /* DOS version - restrict sizes to 64k */
int16 _32to16(int32 l)
{
        if(l>0xFFFF) {
          fputs("A table exceeds the limit of the 16 bit version.\n",stderr);
          exit(255);
        }
        return (int16)l;
}
#else
#define _32to16(l) l            /* for OS/2 native version: no effect */
#endif

char *strncpy2(char *d,char *s,size_t l) 
{
        strncpy(d, s, l);       /* get string (not terminated) */
        d[l]='\0';              /* terminate */
        return d;               /* return dest string */
}

int get_dictionary(void)
{
        char *p;
        int16 i,j;

        b_dict=malloc(_32to16(IPFheader.dictlen));
        p_dict=calloc(IPFheader.ndict,sizeof(char *));
        ipf_getatl(IPFheader.dictstart,b_dict,_32to16(IPFheader.dictlen));
                                        /* get dictionary into mem */

        p=b_dict;                       /* start at the beginning */
        for (i=0; i<IPFheader.ndict; i++) {
          p_dict[i]=p;                  /* pointer to i-th entry */
          for (j=*p-1; j>0; j--,p++) {  /* shift as many characters as */
             *p=*(p+1);                 /* indicated by length byte */
          } /* endfor */
          *(p++)='\0';                  /* terminate and advance */
        }
        return 0;                       /* no errors */
}

int get_toc(void)
{
        int16 i;
        char *p;

        b_toc=malloc(_32to16(IPFheader.tocstrlen));
        p_toc=calloc(IPFheader.ntoc,sizeof(char *));
        ipf_getatl(IPFheader.tocstrtablestart,b_toc,_32to16(IPFheader.tocstrlen));
                                        /* get toc into mem */

        p=b_toc;                        /* start of toc */
        for (i=0; i<IPFheader.ntoc; i++) {
           p_toc[i]=p;                  /* pointer to i-th tocentry */
           p+=*p;                       /* advance by one entry */
        } /* endfor */

        b_slot=calloc(IPFheader.nslots,sizeof(*b_slot));
        ipf_getatl(IPFheader.slotsstart,b_slot,IPFheader.nslots*sizeof(int32));
                                        /* get slot array into mem */

        return 0;                       /* no errors */
}

int get_index(void)
{
        struct s_IPFindex *p;
        int16 i;

        b_index=malloc(_32to16(IPFheader.indexlen));
        p_index=calloc(IPFheader.nindex,sizeof(struct s_IPFindex *));
        ipf_getatl(IPFheader.indexstart,b_index,_32to16(IPFheader.indexlen));
                                        /* get toc into mem */

        p=b_index;                      /* start of toc */
        for (i=0; i<IPFheader.nindex; i++) {
           p_index[i]=p;                /* pointer to i-th tocentry */
           ((char *)p)+=(p->nword)+sizeof(*p)-sizeof(p->word);
                                        /* advance by one entry */
        } /* endfor */

        return 0;                       /* no errors */
}

int get_slot(int32 pos)
{
        char8 *b_text;                  /* text buffer */
        int16 b_localwords[256];        /* local dictionary */
        unsigned i;                     /* position in text */

        ipf_getat(pos,&IPFartheader);   /* get slot header */
        b_text=malloc(IPFartheader.ntext); 
                                        /* allocate space for slot text */
        ipf_getl(b_text,IPFartheader.ntext);
                                        /* get slot text */
        ipf_getatl(IPFartheader.localdictpos, b_localwords,
                   IPFartheader.nlocaldict * sizeof(int16));
                                        /* get local dictionary */

        print_word(0xFFFFF);            /* set defaults for new slot */
        i=0;                            /* start of text */
        for(i=0;i<IPFartheader.ntext;i++) {   
                                        /* until text is through */
           if(b_text[i]<IPFartheader.nlocaldict)
              print_word(b_localwords[b_text[i]]);
                                        /* is it a word? print it! */
           else {
              print_special(b_text+i);  /* no: special character */
              if(b_text[i]==0xFF)       /* Escape sequence? */
                i+=b_text[i+1];         /* skip over sequence */
           }
        }
        free(b_text);                   /* free text buffer */
        return 0;
}

int get_article(int16 tocnr,int8 *tocentry)
{
        int8 *p=tocentry,               /* running pointer in struct */
             len,flags,ntocslots,tlen,w1,w2;
        char title[256],*tptr;          /* title of article */

        len=get_int8(p);                /* get entry length */
        flags=get_int8(p);              /* get flag byte */
        ntocslots=get_int8(p);          /* get number of slots */

        if(flags & 0x20) {              /* extended? */
          w1=get_int8(p);               /* get header */
          w2=get_int8(p);
          if (w1 & 0x8) { p += 2; }
          if (w1 & 0x1) { p += 5; }
          if (w1 & 0x2) { p += 5; }
          if (w2 & 0x4) { p += 2; }
        }

        tptr=p+ntocslots*sizeof(int16); /* title after slot list */
        tlen=len-(int8)(tptr-(char *)tocentry);
                                        /* length of title */
        strncpy2(title,tptr,tlen);      /* get title string */

        print_title(tocnr,flags,title); /* output tile */

        for (; ntocslots>0 ;ntocslots-- ) {
           get_slot(b_slot[get_int16(p)]);
                                        /* get 16 bit slot number */ 
                                        /* and get slot's contents */
        } /* endfor */
        return 0;
}

int main(int argc,char *argv[])
{
        int16 i;
        char *syntax;

        syntax=print_banner();
        if(argc<2 || print_parse(argc,argv,2)) {
           fprintf(stderr,"Syntax: %s <IPF_file>%s\n",argv[0],syntax);
           return 1;
        }

        if( (ipf=fopen(argv[1],"rb")) ) {
           ipf_getat(0,&IPFheader);     /* get header */
           get_dictionary();            /* build dictionary array */
           get_toc();                   /* build toc and slot list */
           get_index();                 /* build index array */
           print_prologue();
           for (i=0; i<IPFheader.ntoc; i++) {
              get_article(i,p_toc[i]);  /* display i-th article */
           } /* endfor */
           print_epilogue();
           fclose(ipf);
        }
        else
          fputs("Can't open file.\n",stderr);
        return 0;
}
