// Please see about_cb or license about license
// Harap lihat about_cb atau license tentang lisensi
// Last update 2016-05-11
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/x.H>
#include <X11/cursorfont.h>
#include <time.h>
#include <png.h>
//constants
#define APP "XLView"
#define VER "1.0.0"
#define MAX_BUFFER 2048
#define UNB "Unable to "
#define TMR "Timer is running\nPlease reset first"
//if your distro is Tiny Core Linux
#define TC 0
//globals
Window xroot,xwin;
uchar rb, gb, bb,rf,gf,bf;

void open_cb(Fl_Widget* wgt,void*d);
void openw_cb(Fl_Widget* wgt,void*d);
void rec_cb(Fl_Widget* wgt,void*d);
void stretch_cb( Fl_Widget* wgt, void* d );
void zoom_cb( Fl_Widget* wgt, void* d );
void prop_cb( Fl_Widget* wgt, void* d );
void scr_cb( Fl_Widget* wgt, void* d );
void tmr_cb( Fl_Widget* wgt, void* d );
void inv_cb( Fl_Widget* wgt, void* d );
void crop_cb( Fl_Widget* wgt, void* d );
void conv_cb( Fl_Widget* wgt, void* d );
void wall_cb( Fl_Widget* wgt, void* d );
void quit_cb(Fl_Widget*,void*);
void dummy_cb(Fl_Widget*,void*){}
//reset color
void col_cb(Fl_Widget*w,void*){
  Fl::background(rb,gb,bb);
  Fl::foreground(rf,gf,bf);
  w->redraw();
}
//set bg/fg color
void bg_cb(Fl_Widget*w,void*d){
  uchar r,g,b;
  if((int)d){
    Fl::get_color(w->labelcolor(), r, g, b);
  }else{
	Fl::get_color(w->color(), r, g, b);
  }
  int v=fl_color_chooser("Choose Color",r,g,b);
  if(v){
	if((int)d){
	  Fl::foreground(r,g,b); 
	}else{
	  Fl::background(r,g,b);
	} 
	w->redraw();
  }
}

void about_cb(Fl_Widget*,void*){
  fl_message(APP" version "VER
  "\nCopyright 2016 Albert.S  <nimdays[at]gmail.com>\n\n"
  APP" is free software: you can redistribute it and/or\n"
  "modify it under the terms of the GNU General Public License\n"
  "as published by the Free Software Foundation, version 2.\n\n"
  APP" is distributed in the hope that it will be useful,\n"
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
  "See http://www.gnu.org/licenses/ for more details."
  );
}

void visit_cb(Fl_Widget*,void*){
	fl_open_uri("https://sourceforge.net/projects/xlview/");
}
//fixed rec menu
Fl_Menu_Item recitm[10] = {
   {"",0,rec_cb,(void*)1},
   {"",0,rec_cb,(void*)2},
   {"",0,rec_cb,(void*)3},
   {"",0,rec_cb,(void*)4},
   {"",0,rec_cb,(void*)5},
   {"",0,rec_cb,(void*)6},
   {"",0,rec_cb,(void*)7},
   {0}
};  
//popup menu
Fl_Menu_Item viewitm[42] = {
   {"Open Image",FL_CTRL+'o',open_cb,0},
   {"Recent",0,dummy_cb,recitm,FL_SUBMENU_POINTER},
   {"Open With",0,openw_cb,0,FL_MENU_DIVIDER},
   {"Image Info",FL_CTRL+'p',prop_cb,0,FL_MENU_DIVIDER},
   {"Stretch",FL_COMMAND+'m',stretch_cb,0,FL_MENU_DIVIDER},
   {"Zoom 1x",FL_CTRL+'1',zoom_cb,(void*)1,0},
   {"Zoom 2x",FL_CTRL+'2',zoom_cb,(void*)2,0},
   {"Zoom 3x",FL_CTRL+'3',zoom_cb,(void*)3,0},
   {"Zoom 4x",FL_CTRL+'4',zoom_cb,(void*)4,0},
   {"Zoom 5x",FL_CTRL+'5',zoom_cb,(void*)5,0},
   {"Zoom 6x",FL_CTRL+'6',zoom_cb,(void*)6,0},
   {"Zoom 7x",FL_CTRL+'7',zoom_cb,(void*)7,FL_MENU_DIVIDER},
   {"Screenshot",0,dummy_cb,0,FL_SUBMENU|FL_MENU_DIVIDER},
     {"Screen",FL_CTRL+'s',scr_cb,0},
     {"Window",FL_CTRL+'w',scr_cb,(void*)1},
     {"Timer",FL_CTRL+'t',tmr_cb,0,0},
     {0},
   {"Black And White",FL_CTRL+'i',inv_cb,0,FL_MENU_TOGGLE},  
   {"Crop Image",FL_CTRL+'c',crop_cb,0,0},
   {"Convert Image To",0,dummy_cb,0,FL_SUBMENU|FL_MENU_DIVIDER},
     {"BMP", 0,conv_cb,(void*)1},
     {"JPG", 0,conv_cb,(void*)2},
     {"PNG", 0,conv_cb,(void*)3},
     {"PNM", 0,conv_cb,(void*)4},
     {0},
   {"Wallpaper",0,dummy_cb,0,FL_SUBMENU|FL_MENU_DIVIDER},
     {"Center", 0,wall_cb,0},
     {"Stretch", 0,wall_cb,(void*)1},
     {"Tile", 0,wall_cb,(void*)2},
     {0}, 
   {"Color",0,dummy_cb,0,FL_SUBMENU|FL_MENU_DIVIDER},  
     {"Reset Color",FL_CTRL+'r',col_cb,0,0},  
     {"Background",FL_CTRL+'b',bg_cb,0,0},
     {"Foreground",FL_CTRL+'f',bg_cb,(void*)1,0},
     {0},
   {"Help",0,dummy_cb,0,FL_SUBMENU|FL_MENU_DIVIDER},  
     {"About "APP,0,about_cb,0,0},
     {"Visit "APP,0,visit_cb,0,0},
     {0},
   {"Quit",FL_CTRL+'q',quit_cb,0,0},
   {0}
};  
 
class XL : public Fl_Double_Window {
public://this is open source after all
  char path[MAX_BUFFER]; //full filename
  char prev[MAX_BUFFER]; //prev full filename
  char fname[128]; //filename
  char tmr[6]; //timer 1 to ...
  int iw,ih,id; // image width ,heght,depth
  int tmode; //timer mode window=tmr
  int trun; //timer is running?
  int bw;//black and white 
  Fl_Box* bx;
  Fl_Scroll* scr;
  Fl_Browser* bro;
  Fl_Shared_Image *img;
  //ctor
  XL(int x, int y,int w, int h, const char*l = 0):Fl_Double_Window(x,y,w,h,l) {
    scr = new Fl_Scroll(0,0,10,10);
    bx = new Fl_Box(0,0,10,10);
    strcpy(tmr,"1"); //default timer 1 sec
  }
  //fix box size
  void reload(){
	  if(!img)return;
	  if(img->w() >=this->w() || img->h()>=this->h()){
	    bx->resize(0,0,img->w(),img->h());
	  }else {
		bx->resize((this->w()/2)-(img->w()/2),(this->h()/2)-(img->h()/2),img->w(),img->h());
	  }  
	  scr->resize(0,0,this->w(),this->h());
	  redraw();
  }
  
  void resize(int x, int y,int w, int h){
	Fl_Double_Window::resize(x, y, w, h);
	reload();
  }
  
  int handle(int e) {
	static int x1,y1;
	if(e==FL_KEYUP) {
	  if (Fl::event_ctrl()) {
		 switch(Fl::event_key()){
			case 'o': open_cb(this,0);break;
			case 'm':stretch_cb(this,0);break; 
			case '1':zoom_cb(this,(void*)1);break; 
			case '2':zoom_cb(this,(void*)2);break; 
			case '3':zoom_cb(this,(void*)3);break;
			case '4':zoom_cb(this,(void*)4);break;
			case '5':zoom_cb(this,(void*)5);break;
			case '6':zoom_cb(this,(void*)6);break; 
			case '7':zoom_cb(this,(void*)7);break;   
			case 'p':prop_cb(this,0);break;  
			case 's':scr_cb(this,0);break; 
			case 'w':scr_cb(this,(void*)1);break; 
			case 't':tmr_cb(this,0);break;
			case 'i':inv_cb(this,0);break;
			case 'c':crop_cb(this,0);break;
			case 'r':col_cb(this,0);break;
			case 'b':bg_cb(this,0);break;
			case 'f':bg_cb(this,(void*)1);break;
			default :break;
	     }
      }
      return 1;
    }else if(e==FL_PUSH){
	  x1 = bx->x() - Fl::event_x();
	  y1 = bx->x() - Fl::event_y();
	  //return 1;  
	}else if(e==FL_RELEASE) {
	  cursor(FL_CURSOR_DEFAULT);
	  if(Fl::event_button() == FL_RIGHT_MOUSE){ 
		 const Fl_Menu_Item *mi=viewitm->popup(Fl::event_x(), Fl::event_y());
		 if(mi)mi->do_callback(this,mi->user_data());
      }else if (Fl::event_button() == FL_LEFT_MOUSE && Fl::event_clicks() == 1) {
         int x1,y1;
		 Fl::get_mouse(x1,y1);
		 XImage *ximg = XGetImage(fl_display,xroot,x1,y1,1,1,0xffffffff,ZPixmap);
		 if(ximg==0){
			fl_alert(UNB"grab");
		    return 0;
		 }
		 uint pixel = XGetPixel(ximg, 0, 0);
		 XDestroyImage(ximg);
		 uchar r= pixel >> 16;
		 uchar g= (pixel& 0x00ff00)>>8;
		 uchar b= pixel & 0x0000ff;
		 Fl::background(r,g,b);
		 Fl::foreground(255-r,255-g,255-b);
		 fl_message("Value %d\nRGB %d %d %d\nHEX #%0.2x%0.2x%0.2x\n\nInverted \nRGB %d %d %d\nHEX #%0.2x%0.2x%0.2x\n",
		           pixel,r,g,b,r,g,b,255-r,255-g,255-b,255-r,255-g,255-b);
		 redraw();
		 
      }  
      return 1;
    }else if(e==FL_DRAG){
	  
	  cursor(FL_CURSOR_MOVE);
	  bx->position(x1+Fl::event_x(), y1+Fl::event_y());	
	  redraw();
	  return 1; 
    }
		
    return(Fl_Double_Window::handle(e)); 
  }
  
};

XL *xl;
//update recent
void update_rec(){
    if(strcmp(xl->prev,xl->path)==0)return;
    int found=0;
	for (int t=1; t<=xl->bro->size(); t++ ) {
	  if(strcmp(xl->prev,xl->bro->text(t))==0 || strcmp(xl->path,xl->bro->text(t))==0){
		 found=t;
		 break;
	  }
    }
    if(found)xl->bro->remove(found);
	xl->bro->insert(1,xl->prev);
	if(xl->bro->size()>=7)xl->bro->remove(xl->bro->size());
    Fl_Menu_Item *mi=recitm;
    for (int t=1; t<=xl->bro->size(); t++ ) {
	  mi->label(xl->bro->text(t)); 
	  mi=mi->next();
    }
}

void load_img(const char*fname,int z=1,int s=0,int update=1){
	if (xl->img) {xl->img->release();xl->img=0;}
	xl->img = Fl_Shared_Image::get(fname);
	if(xl->img==NULL){
	  fl_alert(UNB"load image");
	  return ;
	}
	//broken image ?
	if(xl->img->w()<0 || xl->img->h()<0){
	  fl_alert("Invalid image\nwidth : %d\nheight : %d",xl->img->w(),xl->img->h());
	  return ;
	}
	xl->iw=xl->img->w(); //save width
	xl->ih=xl->img->h(); //save height
	Fl_Image *temp;
	if(s){//strecth
	  temp=xl->img->copy(xl->w(),xl->h());
	}else{
	  temp=xl->img->copy(xl->img->w()*z,xl->img->h()*z);
	}
	xl->id=temp->d(); //save depth
	xl->img =(Fl_Shared_Image*)temp->copy(temp->w(),temp->h());
	if (temp)delete(temp);
	xl->bx->image(xl->img);
	xl->reload();
	if(!update)return;
	strcpy(xl->prev,xl->path);//save  prev
    strcpy(xl->path,fname);
    strcpy(xl->fname,fl_filename_name(xl->path));
    char buf[MAX_BUFFER];
    sprintf(buf,"%s - %s",APP,xl->fname);
    xl->copy_label(buf);
    update_rec();
}
//load image
void open_cb(Fl_Widget* wgt,void*d){
  const char *fil = fl_file_chooser("Open image", "*.{gif,bmp,jpg,png,xpm,pbm,pgm}", ".");
  if(fil==NULL)return;
  load_img(fil);
}
//open with
void openw_cb(Fl_Widget* wgt,void*d){
  if (!xl->img)return;
  char buf[MAX_BUFFER];
  sprintf(buf,"Open %s With",xl->fname);
  const char*val=fl_input(buf,"mtpaint");
  if(val==NULL)return;
  sprintf(buf,"%s %s 2>/dev/null & ",val,xl->path);
  if(system(buf)!=0)fl_alert(UNB"execute");  
}
//recent
void rec_cb(Fl_Widget* wgt,void*d){
	Fl_Menu_Item *mi = recitm;
	int opt=(int)d;
	if(!mi)return;
	if((mi->next(opt-1))->label()[0]==0)return;
	load_img((mi->next(opt-1))->label());
}
//stretch
void stretch_cb( Fl_Widget* wgt, void* d ){
	if (!xl->img)return;//or segfault
	load_img(xl->path,1,1,0);
}
//zoom 1 to ... 
void zoom_cb( Fl_Widget* wgt, void* d ) {
	if (!xl->img)return;//or segfault
	load_img(xl->path,(int)d,0,0);
}
//image info callback
void prop_cb( Fl_Widget* wgt, void* d ) {
	fl_message("File : %s\nWidth : %d\nHeight : %d\nDepth : %d\n\nWindow\nWidth : %d\nHeight : %d\n",
	           xl->path,xl->iw,xl->ih,xl->id,xl->w(),xl->h());
}
//save to png
int save_png(const char *file,int w,int h,XImage* ximg){
	FILE* fp;
	png_structp png;
	png_infop info;
    png_bytep png_row;
    uint col;
    fp = fopen (file, "wb");
    if (fp==NULL)return 1;
    png = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png==NULL){
	  fclose (fp);
      return 2;
    }
    info = png_create_info_struct (png);
    if (info==NULL || setjmp (png_jmpbuf (png))){
	  png_destroy_write_struct (&png, &info);
	  fclose (fp);
      return 3;
    }
    png_init_io (png, fp);
    png_set_IHDR (png,info,w,h,8,PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
    png_write_info (png, info);
    png_row = (png_bytep)png_malloc (png,3 * w * sizeof (png_byte));
    
    for(int y=0; y<h; y++){
	  for(int x=0; x<w; x++){
		 col = XGetPixel(ximg, x, y);
		 png_byte* pptr = &(png_row[x*3]);
		 pptr[0] = col >> 16 & 255;
         pptr[1] = col >> 8 & 255 ;
         pptr[2] = col & 255 ;
         if(xl->bw)pptr[1] =pptr[0];
         if(xl->bw)pptr[2] =pptr[0];
      }
      png_write_row (png, png_row);
    }
    png_write_end (png, NULL);
    //clean up
    png_free_data (png, info, PNG_FREE_ALL, -1);
    png_destroy_write_struct (&png, &info);
    png_free (png ,png_row);
    fclose (fp);
    return 0;
    
}
//bf save
void save_image(int x=0,int y=0,int w=0,int h=0){
	xl->show();//show me
    XWindowAttributes attr;
    XGetWindowAttributes(fl_display,xwin ,&attr);
    if(x)attr.x=x;
    if(y)attr.y=y;
    if(w)attr.width=w;
    if(h)attr.height=h;
    //now get the image
    XImage *ximg = XGetImage(fl_display,xroot,attr.x,attr.y,attr.width,attr.height,0xffffffff,ZPixmap);
    if(ximg==0){
	   fl_alert(UNB"grab\n\nX : %d\nY : %d\nWidth : %d\nHeight : %d\n",
	            attr.x,attr.y,attr.width,attr.height);
	   return;
	}
	char buf[42];
	time_t now = time(0);
    struct tm *ltm = localtime(&now);
    sprintf(buf,"/tmp/%d%.2d%.2d%.2d%.2d%.2d.png", ltm->tm_year+1900, ltm->tm_mon,
            ltm->tm_mday,ltm->tm_hour,ltm->tm_min,ltm->tm_sec);
	 
	int rc=save_png(buf,attr.width,attr.height,ximg);
	if(rc != 0){
	   fl_alert(UNB"save");
	   if(ximg)XDestroyImage(ximg);
	   return;
    }
    if(ximg)XDestroyImage(ximg);
	load_img(buf,1,0);
}
//the timer
static void timer_cb(void *data) {
   static int count = 0;
   count ++;
   xl->trun=1;
   //show the counter
   char buf[MAX_BUFFER];
   sprintf(buf,"%s - %d",APP,atoi(xl->tmr)-count);
   xl->copy_label(buf);  
   if(count +1 > atoi(xl->tmr)-xl->tmode){
	  count=0;
	  Fl::remove_timeout(timer_cb);
	  xl->trun=0; //reset run
      save_image(); 
   }else{Fl::repeat_timeout(1.0, timer_cb,data);}
}
//window from pointer
Window win_ptr(Window w){
	Cursor cur = XCreateFontCursor(fl_display,XC_crosshair);
	XSync (fl_display, 0);
	if(XGrabPointer(fl_display,w,0,ButtonPressMask,GrabModeSync,GrabModeAsync,
	None,cur,CurrentTime)!=GrabSuccess)return 0;
	Window twin;
	XEvent event;
	while(twin==0){
	  XAllowEvents (fl_display, SyncPointer, CurrentTime);  
      XWindowEvent (fl_display, w, ButtonPressMask, &event);
      if(event.type == ButtonPress){
		twin = event.xbutton.subwindow;
		if(twin == 0)twin = w;
		break;  
      }else{break;}
    }	
	XUngrabPointer(fl_display, CurrentTime);
    if (cur)XFreeCursor(fl_display,cur);
    XSync (fl_display, 0);
    return twin;
}

void scr_cb( Fl_Widget* wgt, void* d ) {
	if(xl->trun){
	  fl_alert(TMR);
	  return;
    }
	int val=(int)d;//fl_choice("Screenshot","Cancel","Screen","Window");
    xl->iconize();//minimize me
    xwin=xroot;
    xl->tmode=0;
	if(val){
	   xwin=win_ptr(xwin);
	   xl->tmode=atoi(xl->tmr);
	}
	Fl::add_timeout(1.0, timer_cb,(void*)xl);
}
//set the timer
void tmr_cb( Fl_Widget* wgt, void* d ) {
	const char* val=fl_input("Screenshot timer (sec)",xl->tmr);
	if(val==0)return;
	strcpy(xl->tmr,val);
	if(atoi(xl->tmr)<1){
	  xl->trun=0;
	  strcpy(xl->tmr,"1");
	}
}
//b &w
void inv_cb( Fl_Widget* wgt, void* d ){
	Fl_Menu_Item *mi=viewitm;
	mi=mi->next(13);
	if(mi->value()){
	  mi->clear();
	  xl->bw=0;	
    }else{
	  mi->set();
	  xl->bw=1;
    }
}
//timer2
static void timer1_cb(void *data) {
   static int count = 0;
   count ++;
   if(count  > 1){
	  count=0;
	  Fl::remove_timeout(timer1_cb);
	  int sw=0,sh=0;//scrollbar
	  if(xl->img){
	   if(xl->img->w()>xl->w())sw=Fl::scrollbar_size();
	   if(xl->img->h()>xl->h())sh=Fl::scrollbar_size();
      }
      save_image(xl->x(),xl->y(),xl->w()-sw,xl->h()-sh); 
   }else{Fl::repeat_timeout(1.0, timer1_cb,data);}
}
//crop
void crop_cb( Fl_Widget* wgt, void* d ){
	printf("%d\n",fl_xid(xl));
	xwin=fl_xid(xl);
	Fl::add_timeout(1.0, timer1_cb,(void*)xl);
}
//wallpaper
void wall_cb( Fl_Widget* wgt, void* d ){
	if (!xl->img)return;
	int opt=((int)d);
	char bt[10]="center";
	if(opt==1){
	   strcpy(bt,"fill");
    }else if(opt==2){
	   strcpy(bt,"tile");
	}
	uchar r,g,b;
	Fl::get_color(xl->color(), r, g, b);
	char buf[MAX_BUFFER];
	#if TC
	 sprintf(buf,"cp %s /opt/backgrounds/%s 2>/dev/null",xl->path,xl->fname);
	 if(system(buf)!=0){
		fl_alert(UNB"copy");
		return; 
	 }
	 sprintf(buf,"setbackground n %s /opt/backgrounds/%s 2>/dev/null",bt,xl->fname);
	#else
	 sprintf(buf,"hsetroot -solid \"#%0.2x%0.2x%0.2x\" -%s %s 2>/dev/null",r,g,b,bt,xl->path);
	#endif
	 if(system(buf)!=0){
		fl_alert(UNB"change wallpaper\nMaybe hsetroot is missing ?");
		return; 
	 }	 
}
//convert image via imlib2
void conv_cb( Fl_Widget* wgt, void* d){
	if (!xl->img)return;
	int opt=((int)d)-1;
	if (opt<0)return;
	char im[4][6]={".bmp",".jpg",".png",".pnm"};
	if(strcmp(fl_filename_ext(xl->path),im[opt])==0){
	  fl_alert("Please choose different extension");	
	  return;
    }
    char buf[MAX_BUFFER];
    char buf1[MAX_BUFFER];
    sprintf(buf,"%s",xl->path);
    buf[strlen(buf)-1]=im[opt][3];
    buf[strlen(buf)-2]=im[opt][2];
    buf[strlen(buf)-3]=im[opt][1];
    sprintf(buf1,"imlib2_conv %s %s 2>/dev/null",xl->path,buf);
    if(system(buf1)!=0){
	  fl_alert(UNB"convert or maybe imlib2_conv is missing ?");
	  return;
    }
    load_img(buf);
}

void quit_cb(Fl_Widget*,void*){
  exit(0);
}
//main
int main(int argc, char *argv[]) {
   fl_open_display();
   xroot=RootWindow(fl_display, fl_screen);
   Fl_File_Icon::load_system_icons();
   Fl::scheme(NULL);
   fl_message_title_default(APP);
   Fl_Color c = FL_BACKGROUND_COLOR;
   Fl::get_color(c, rb, gb, bb);
   c = FL_FOREGROUND_COLOR;
   Fl::get_color(c, rf, gf, bf);
   xl = new XL(100,100,400,300,APP);
   xl->resizable(xl);
   xl->show();
   xl->bro = new Fl_Browser(0,0,10,10);
   xl->bro->hide();
   if(argc >1){
	 load_img(argv[1]);
   }else{
	 fl_message("Right click for popup menu");
   }
   int rc = Fl::run();
   if(xroot)fl_close_display();
   return rc;
}
//EOF
