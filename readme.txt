                                ipf2txt 1.02
                  Convert OS/2 INF/HLP files to ASCII text

                          by Marcus Groeber 1993/94
                      Internet: mgroeber@compuseve.com

"During the last months, I have often read questions from people asking how
they can get the OS/2 online help files into ASCII format for printing etc.
When I came across Carl Hauser's analysis of the OS/2 help file format, I
decided to start writing a "decompiler" for these files.

ipf2txt is the first result of my attempts. It creates fairly well-formatted
ASCII listings from INF and HLP files that can be redirected to a file (or to
a printer destination). Maximum line width can be chosen freely (default is 79
characters)."

This was how I introduced to 1.00 version. Meanwhile, there are a number of
programs doing the same thing, but diversity is never a bad idea, so this
version contains a few minor improvements over the previous release...


ipf2txt can be started by entering

        ipf2txt inf_filename [width]

at the OS/2 or DOS command line, optionally specifying ">file" to redirect the
output into a file, e.g.

        ipf2txt \os2\book\cmdref.inf >cmdref.txt

Other than the previous version, the 1.02 version can be used in an OS/2 DOS
box and probably even on sub-386 machines, because its DOS part is completely
independant from the OS/2 module, although they are generated from the
same source files.

The OS/2 version was compiled using emx 0.8h, while the DOS version was
compiled with Microsoft C 7.00 (OS/2 hosted version). Both programs were
linked together into a single EXE (using the STUB command of the linker's .DEF
file, for those who might want to know). Depending on the environment you
start the program in, one of the two versions is selected.

DOS and OS/2 version should normally produce identical results, though for
large files it is possible that the DOS version "gives up" because of
structures exceeding the 64k limit (as a very crude rule of thumb, this should
be expected for files exceeding 500-600k in size). So far, no attempt has been
made to work around this, as the DOS version is mainly intended for reading
things like installation notes etc.


Future plans
------------
Any suggestions for features/improvements are welcome, bug reports if possible
with a description of the INF file that caused the problem.

I'm still thinking about a true IPFC decompiler which would create source code
suitable for re-compilation by IPFC. This has turned out to be more diffcult
than I thought because the translation process of IPFC actually looses
information from the original source file (especially with table formatting,
itemized lists etc.) which can only be "guessed" by some kind of
pattern-detection routines. Anybody interested in such a thing?

<later:> This question is even more interesting as I lost the first attempts
on an IPFC uncompiler because of hard disk trouble. Any votes for a rewrite?


History
-------

Version 1.0:

        Initial release

Version 1.01:

        Fixed incorrect handling of ":p." inside monospace examples. Thanx
                to Christian Zangl for finding this problem.

Version 1.02:

        Added chapter numbering.
        Converted program into "Janus style fat binary" (two versions, one
                for native OS/2 and one for native DOS and DOS sessions,
                included in one EXE).
        Program does not require emx.exe/emx.dll any more.

