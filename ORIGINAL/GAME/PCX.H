#define	NO_ERROR 0
#define FILE_NOT_OPENED 1
#define INSUFFICIENT_MEMORY 2
#define TOO_MANY_ARGUMENTS 3

struct pcx_header {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  int  xmin,ymin;
  int  xmax,ymax;
  int  hres;
  int  vres;
  char palette16[48];
  char reserved;
  char color_planes;
  int  bytes_per_line;
  int  palette_type;
  char filler[58];
};

struct pcx_struct {
  pcx_header header;
  unsigned char far *image;
  unsigned char palette[3*256];
  int clength;
  unsigned char far *cimage;
};

int  loadPCX(char far *filename,pcx_struct *pcx);
