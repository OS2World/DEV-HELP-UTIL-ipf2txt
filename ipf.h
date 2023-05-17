typedef unsigned char  char8;
typedef unsigned char  int8;
typedef unsigned short int16;
typedef unsigned long  int32;

#pragma pack(1)
struct s_IPFheader {
            int16 ID;           /* ID magic word (5348h = "HS") */
            int16 unknown1;     /* unknown purpose */
            int16 hdrsize;      /* total size of header */
            int16 unknown2;     /* unknown purpose */
            int16 ntoc;         /* 16 bit number of entries in the tocarray */
            int32 tocstrtablestart;  /* 32 bit file offset of the start of the */
                                     /* strings for the table-of-contents */
            int32 tocstrlen;    /* number of bytes in file occupied by the */
                                /* table-of-contents strings */
            int32 tocstart;     /* 32 bit file offset of the start of the tocarray */
            int16 nres;         /* number of panels with ressource numbers */
            int32 resstart;     /* 32 bit file offset of ressource number table */
            int16 nname;        /* number of panels with textual name */
            int32 namestart;    /* 32 bit file offset to panel name table */
            int16 nindex;       /* number of index entries */
            int32 indexstart;   /* 32 bit file offset to index table */
            int32 indexlen;     /* size of index table */
            int8  unknown3[10]; /* unknown purpose */
            int32 searchstart;  /* 32 bit file offset of full text search table */
            int32 searchlen;    /* size of full text search table */
            int16 nslots;       /* number of "slots" */
            int32 slotsstart;   /* file offset of the slots array */
            int32 dictlen;      /* number of bytes occupied by the "dictionary" */
            int16 ndict;        /* number of entries in the dictionary */
            int32 dictstart;    /* file offset of the start of the dictionary */
            int8  unknown4[5];  /* unknown purpose */
            int32 xxxstart;     /* 32 bit file offset of ??? */
            int32 xxxlen;       /* size of ??? */
            int32 yyystart;     /* 32 bit file offset of ??? (font related?) */
            int8  unknown5[12]; /* unknown purpose */
            char8 title[48];    /* ASCII title of database */
};

struct s_IPFartheader {
             int8       stuff;          /* ?? */
             int32      localdictpos;   /* file offset of the local dictionary */
             int8       nlocaldict;     /* number of entries in the local dictionary */
             int16      ntext;          /* number of bytes in the text */
} IPFartheader;

struct s_IPFindex {
              int8  nword;              /* size of name */
              int8  level;              /* indent level */
              int8  stuff;
              int16 toc;                /* toc entry number of article */
              char8 word[1];            /* index word [not zero-terminated] */
};
#pragma pack()
