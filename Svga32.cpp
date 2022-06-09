#include "metagraphics.h"
#include "depreciate\raster2d.h"
#include "depreciate\context.h"


namespace g2d
{
	namespace raster
	{
		namespace svga32
		{
			enum
			{
				shiftcol=13,
				shiftred=19,
				shiftgreen=18,
				shiftblue=19
			};

			//typedef union{    DWORD argb;    struct {        BYTE b,g,r,a;    };}

			//putbmflat16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat32(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);

			void putbmflat32ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat16ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat8ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat32add(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat32sub(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflatgrey8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflatgrey16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflatgrey32(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat24(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat15(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflat4444(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
			void putbmflatrgb8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);

#define fillhigh(r)\
	_asm mov eax,r \
	_asm mov ebx,eax \
	_asm shl ebx,16 \
	_asm or eax,ebx \
	_asm mov r,eax; 

			tcolour get_pixel(PIXINT x,PIXINT y)
			{   
				if (!IC.viewport.is_inside(tmath::CPoint2i(x,y))) return 0;				
				return *(tcolour*)get_scanstart32(IC.scrptr,x,y,IC.offsd);
			}    

			void putpixel(register PIXINT x,register PIXINT y)
			{   
				if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))   
					*(UINT32*)get_scanstart32(SC.scrptr,x,y,SC.offsd)=t_col;	            
			}

#define _hline_32bit(adrs,length,col,op)\
	_asm mov edi,adrs \
	_asm mov ecx,length \
	_asm mov eax,col \
	_asm drawloop: \
	_asm   op [edi],eax \
	_asm   add edi,4 \
	_asm   dec ecx \
	_asm jnz drawloop;

			void hline(PIXINT x1,PIXINT x2,PIXINT y)
			{
				INT32 xlen;    
				if (x1>x2) swap(x2,x1);
				if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
				if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
				if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
				x2++;	
				xlen=x2-x1;	

				if (xlen<=0) return;  	
				sadrs=get_scanstart32(SC.scrptr,x1,y,SC.offsd);

				_hline_32bit(sadrs,xlen,t_col,mov);      
			}

#define _vline_32bit(adrs,offs,cy,col,op)\
	_asm mov edi,adrs \
	_asm mov edx,offs \
	_asm mov ecx,cy \
	_asm mov eax,col \
	_asm _yloop: \
	_asm    op [edi],eax \
	_asm    add edi,edx \
	_asm    dec ecx \
	_asm jnz _yloop;

			void vline(PIXINT x,PIXINT y1,PIXINT y2)
			{
				PIXINT ylen;
				if (y1>y2) swap(y2,y1);
				if ((x<SC.viewport.a.x)||(x>SC.viewport.b.x)) return;
				if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
				if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
				y2++;
				ylen=(y2-y1);
				if (ylen<=0) return;

				sadrs=get_scanstart32(SC.scrptr,x,y1,SC.offsd);
				_vline_32bit(sadrs,SC.offsd,ylen,t_col,mov);    
			}      

#define xmajorline_32bit(col) \
	_asm mov eax,col \
	_asm mov edx,addy \
	_asm mov ebx,edx \
	_asm shr ebx,1 \
	_asm mov esi,linexdiff \
	_asm mov edi,sadrs \
	_asm stosd \
	_asm xor ecx,ecx \
	_asm xloop1: \
	_asm   add bx,dx \
	_asm   jnc nodraw \
	_asm   rep stosd \
	_asm   add edi,offs \
	_asm nodraw: \
	_asm inc ecx \
	_asm dec esi \
	_asm jnz xloop1;

#define ymajorline1_32bit(col)\
	_asm mov eax,col \
	_asm mov edx,addy \
	_asm mov ebx,edx \
	_asm shr ebx,1 \
	_asm mov esi,offs \
	_asm mov edi,sadrs \
	_asm mov ecx,lineydiff \
	_asm yloop1: \
	_asm   mov [edi],eax \
	_asm   add bx,dx \
	_asm   jnc nooverflow1 \
	_asm     sub edi,4 \
	_asm   nooverflow1: \
	_asm   sub edi,esi \
	_asm   dec ecx \
	_asm jnz yloop1;

#define ymajorline2_32bit(col)\
	_asm mov eax,col \
	_asm mov edx,addy \
	_asm mov ebx,edx \
	_asm shr ebx,1 \
	_asm mov esi,offs \
	_asm mov edi,sadrs \
	_asm mov ecx,lineydiff \
	_asm yloop2: \
	_asm   mov [edi],eax \
	_asm   add bx,dx \
	_asm   jnc nooverflow2 \
	_asm     add edi,4 \
	_asm   nooverflow2: \
	_asm   add edi,esi \
	_asm   dec ecx \
	_asm jnz yloop2;

#define vert_or_diagline_32bit(col) \
	_asm mov eax,col \
	_asm mov esi,offs \
	_asm mov edi,sadrs \
	_asm mov ecx,linexdiff \
	_asm yloop3: \
	_asm   mov [edi],eax \
	_asm   add edi,esi \
	_asm dec ecx \
	_asm jnz yloop3;


			void line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
			{

				tmath::CLine2i l(x1,y1,x2,y2);
				if (y2==y1) {hline(x1,x2,y1);return;}
				if (!tmath::bAnd(l,SC.viewport)) return;
				x1=l.a.x;
				x2=l.b.x;
				y1=l.a.y;
				y2=l.b.y;

				fillhigh(t_col);
				INT32 offsd=SC.offsd;
				linexdiff=abs(x2-x1);
				lineydiff=abs(y2-y1);
				lineydiff++;
				linexdiff++;      
				if (linexdiff>lineydiff)
				{        
					if (x1>x2)
					{        
						swap(x1,x2);
						swap(y1,y2);
					}    
					sadrs=get_scanstart32(SC.scrptr,x1,y1,SC.offsd);        

					if (y2>y1)
						offs=offsd;
					else
						offs=-offsd;

					addy=(lineydiff << 16)/ linexdiff;
					xmajorline_32bit(t_col);              
				} else if (lineydiff>linexdiff)
				{


					if (y1>y2)
					{
						swap(x1,x2);
						swap(y1,y2);
					}
					sadrs=get_scanstart32(SC.scrptr,x1,y1,SC.offsd);        
					addy=(linexdiff << 16)/ lineydiff;

					if (x1>x2)
					{
						offs=-offsd;
						ymajorline1_32bit(t_col);               
					} else
					{            
						offs=offsd;
						ymajorline2_32bit(t_col);
					}
				}   
				else
				{
					//diagonals +vertical
					if (y1>y2)
					{
						swap(x1,x2);
						swap(y1,y2);
					}
					sadrs=get_scanstart32(SC.scrptr,x1,y1,SC.offsd);        
					if (x1==x2)
						offs=SC.offsd;
					else if (x2>x1)
						offs=SC.offsd+4;
					else 
						offs=SC.offsd-4;
					vert_or_diagline_32bit(t_col);                
				}
			}


			//this routine draws a bar onto the screen. It uses doubleword string writes and
			//doubleword aligns the data before writting. Thanks for that one Martin!
			//maybe the inner loop could be done with the adc rep stosw trick

#define _bar_32bit(adrs,length,cy)\
	_asm mov edi,adrs    \
	_asm mov edx,length  \
	_asm mov esi,cy      \
	_asm mov eax,dword ptr t_fillcol \
	_asm mov ebx,temp32  \
	_asm yloop:          \
	_asm   mov ecx,edx   \
	_asm   rep stosd     \
	_asm   add edi,ebx   \
	_asm   dec esi       \
	_asm jnz yloop       \

			void bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
			{
				PIXINT xlen;    
				if (y1>y2) swap(y2,y1);
				if (x1>x2) swap(x2,x1);

				if ((x1>SC.viewport.b.x)||(x2<SC.viewport.a.x)||
					(y1>SC.viewport.b.y)||(y2<SC.viewport.a.y))return;

				if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
				if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
				if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
				if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;

				sadrs=get_scanstart32(SC.scrptr,x1,y1,SC.offsd);

				xlen=1+x2-x1;
				y2-=(y1-1);
				temp32=SC.offsd-(xlen+xlen);
				//_bar_32bit(sadrs,xlen,y2);      
			}



			////////////////////////////////////////////////////////////////////////////////////////////
			//Set the drawing mode for the polygons.. only implemented in this driver as of 12/1/99

			/*	=0,
			PXFi1		=1,
			PXFi4		=2,
			PXFi8		=3,
			PXFpi4		=4,
			PXFpi8		=5,
			PXFr3g3b2	=6,
			PXFr5g6b5	=7,
			PXFx1r5g5b5	=8,
			PXFa1r5g5b5	=9,
			PXFx4r4g4b4	=10,					
			PXFa4r4g4b4	=11,	
			PXFr8g8b8 	=13,
			PXFx8r8g8b8 =12,
			PXFa8r8g8b8	=14,

			PXFl8		=15,
			PXFl16		=16,
			PXFl32		=17,
			PXFa8l8		=18,

			PXFuvla8888 =19,
			PXFu8vl		=20,
			PXFu8v8		=21,
			PXFu8v8l8	=22,

			PXFz16		=23,
			PXFz32		=24,
			PXFz15s1	=25,
			PXFs1z15	=26,
			PXFs8z24	=27,*/




			pixmap_func_type pixfuncs_32[]=
			{
				//normalput xorput andput orput addput subput blendput chromaput ndef ndef ndef ndef ndef ndef ndef ndef ndef 
				NULL,	      NULL,  NULL,NULL, NULL,  NULL,	NULL,	  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFunknown  
					NULL,	      NULL,  NULL,NULL, NULL,  NULL,	NULL,	  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi1
					NULL,       NULL,  NULL,NULL, NULL,  NULL,    NULL,     NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi4	  
					putbmflat8, NULL,  NULL,NULL, NULL,NULL,NULL,putbmflat8ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi8
					NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFpi4
					NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFpi8
					putbmflatrgb8,NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr3g3b2
					putbmflat16  ,NULL,NULL, NULL,NULL,NULL,NULL,putbmflat16ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr5g6b5
					putbmflat15,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFa1r5g5b5	  	  	  	  
					putbmflat15,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFx1r5g5b5
					putbmflat4444,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFx4g4b4a4
					putbmflat4444,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr4g4b4a4
					putbmflat24,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr8g8b8
					putbmflat32,NULL,NULL, NULL,putbmflat32add,putbmflat32sub,NULL,putbmflat32ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr8g8b8a8
					putbmflat32,NULL,NULL, NULL,putbmflat32add,putbmflat32sub,NULL,putbmflat32ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr8g8b8a8

					putbmflatgrey8,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //grey 8
					putbmflatgrey16,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //grey 16
					putbmflatgrey32,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //grey 32
					putbmflatgrey16,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL   //grey a8l8	  	  
			};




			pixmap_func_type pixmapmode(UINT32 mode)
			{
			
				if (mode!=t_currentpixmapmode)
				{
					t_currentpixmapmode=mode;
					t_renderpixmap=pixfuncs_32[mode];		
					if (!t_renderpixmap)
						t_renderpixmap=putbmflat32;

				}		
				return t_renderpixmap;
			}
			////////////////////////////////////////////////////////////////////////////////////////////
			//Bitmap pasting routines

#define _putbmflat16(xlen,ylen,offset,bmoffset)\
	_asm mov ecx,xlen \
	_asm mov edx,ylen \
	_asm mov ebx,offset \
	_asm mov eax,bmoffset \
	_asm mov esi,padrs \
	_asm mov edi,sadrs \
	_asm test edi,2 \
	_asm setnz byte ptr leadingbyte \
	_asm sub ecx,leadingbyte \
	_asm mov temp32,ecx \
	_asm add ecx,leadingbyte \
	_asm add ecx,ecx \
	_asm sub ebx,ecx \
	_asm sub eax,ecx \
	_asm xor ecx,ecx \
	_asm yloop: \
	_asm   mov cl,byte ptr leadingbyte \
	_asm   rep movsw \
	_asm   mov ecx,temp32 \
	_asm   shr ecx,1 \
	_asm   rep movsd \
	_asm   adc cl,cl \
	_asm   rep movsw \
	_asm   add edi,ebx \
	_asm   add esi,eax \
	_asm dec dx \
	_asm jnz yloop;


			void putbmflatrgb8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen);
				temp32=SC.offsd-(xlen<<2);  
				UINT8  pic;
				while (ylen>0)
				{	                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						pic=*(UINT8*)padrs;						
						*(UINT32*)sadrs=((pic << 5)& 0x00F90000)|((pic << 2)& 0x0000FC00)|((pic)& 0x000000F9);				
						padrs+=1;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflat16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen+xlen);
				temp32=SC.offsd-(xlen<<2);  
				UINT16 pic;
				while (ylen>0)
				{	                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						pic=*(UINT16*)padrs;						
						*(UINT32*)sadrs=((pic << 8)& 0x00F90000)|((pic << 6)& 0x0000FC00)|((pic << 3)& 0x000000F9);				
						padrs+=2;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflat15(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen+xlen);
				temp32=SC.offsd-(xlen<<2);  
				UINT16 pic;
				while (ylen>0)
				{	                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						pic=*(UINT16*)padrs;						
						*(UINT32*)sadrs=((pic << 9)& 0x00F90000)|((pic << 6)& 0x0000F900)|((pic << 3)& 0x000000F9);				

						padrs+=2;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflat4444(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen+xlen);
				temp32=SC.offsd-(xlen<<2);  
				UINT16 pic;
				while (ylen>0)
				{	                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						pic=*(UINT16*)padrs;						
						*(UINT32*)sadrs=((pic << 12)& 0x00FB0000)|((pic << 8)& 0x0000FB00)|((pic << 4)& 0x000000FB);				
						padrs+=2;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}
			//see bottom of file for annotated version of the above

			//uses native palette
			void putbmflat8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart8(tpadrs,x,y,bmoffset);
				bmoffset-=xlen;

				temp32=SC.offsd-(xlen<<2);  

				while (ylen>0)
				{     
					for (xlop=0;xlop<xlen;xlop++)
					{   
						*(UINT32*)sadrs=*(UINT32*)&g2d::raster::localpalette[(UINT8)*padrs];          
						padrs++;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflatgrey8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart8(tpadrs,x,y,bmoffset);
				bmoffset-=xlen;

				temp32=SC.offsd-(xlen<<2);  

				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						UINT8 pic=*padrs;
						*(UINT32*)sadrs=0xFF000000|pic<<16|pic<<8|pic;          
						padrs++;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}



			void putbmflatgrey16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen<<1);

				temp32=SC.offsd-(xlen<<2);  

				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						UINT16 pic=*(UINT16*)padrs;
						*(UINT32*)sadrs=0xFF000000|(pic<<8)&(0x00FF0000)|pic & (0x0000FF00)|(pic>>8);          
						padrs+=2;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}


			void putbmflatgrey32(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart32(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen<<2);

				temp32=SC.offsd-(xlen<<2);  

				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						UINT32 pic=*(UINT32*)padrs;
						*(UINT32*)sadrs=0xFF000000|(pic>>8)&0x00FF00000|(pic>>16)&0x0000FF00|(pic>>24);          
						padrs+=4;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflat24(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart24(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen*3);
				temp32=SC.offsd-(xlen<<2);  
				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						UINT32 pix=*(UINT32*)padrs;		  
						*(UINT32*)sadrs=(0xFF000000)|(pix>>8);
						padrs+=3;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}
			
			void putbmflat32(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart32(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen<<2);
				temp32=SC.offsd-(xlen<<2);  
				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						*(UINT32*)sadrs=*(UINT32*)padrs;          
						padrs+=4;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}

			//UINT8 satsubtable[512]={00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
			//UINT8 sataddtable[512]={00,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,00,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63};

			void putbmflat32sub(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				//INT32 xlop;
				padrs=get_scanstart32(tpadrs,x,y,bmoffset);
				//bmoffset;-=(xlen<<2);
				temp32=SC.offsd;//-(xlen<<2);  
				char* lsadrs=sadrs;
				while (ylen>0)
				{                  
					/*for (xlop=0;xlop<xlen;xlop++)
					{   
					UINT32 s=*(UINT32*)sadrs;
					UINT32 p=*(UINT32*)padrs;
					UINT32 r=0;
					UINT32 t;         		

					/*t=(((s & 0x00FF0000)-(p & 0x00FF0000)));		  		  
					r=(t>>((t&0x10000000)>>24))&0x00FF0000;	

					t=(((s & 0x0000FF00)-(p & 0x0000FF00)));		  
					r|=(t&~((t & 0x00FF0000)>>8))&0xFF00;

					t=(((s & 0x000000FF)-(p & 0x000000FF)));		  		  
					r|=(t&~((t & 0x0000FF00)>>8))&0xFF;


					//  r|=t;		  
					*(UINT32*)sadrs=r;

					padrs+=4;
					sadrs+=4;
					}*/

					_asm
					{
						//red
						mov esi,padrs
							mov edi,lsadrs
							mov ebx,xlen

							push ebp
							//mov ebp,ebx


xloop:
						mov ebp,[edi]
						mov eax,ebp
							mov ecx,[esi]
							and eax,0x00FF0000
								and ecx,0x00FF0000
								sub eax,ecx
								mov ecx,eax
								and ecx,0x10000000
								shr ecx,24
								shr eax,cl
								and eax,0x00FF0000
								mov edx,eax
								//green
								mov eax,ebp
								mov ecx,[esi]
								and eax,0x0000FF00
									and ecx,0x0000FF00
									sub eax,ecx
									mov ecx,eax
									and ecx,0x00FF0000
									shr ecx,8
									not ecx
									and eax,ecx
									and eax,0xFF00
									or  edx,eax

									//red
									mov eax,ebp
									mov ecx,[esi]
									and eax,0xFF
										and ecx,0xFF
										sub eax,ecx
										mov ecx,eax
										and ecx,0xFF00
										shr ecx,8
										not ecx
										and eax,ecx
										and eax,0xFF
										or  edx,eax
										mov [edi],edx
										add edi,4
										add esi,4

										dec ebx
										jnz xloop
										pop ebp
					}
					padrs+=bmoffset;
					lsadrs+=temp32;                                 
					ylen--;
				}
			}

			void putbmflat32add(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart32(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen<<2);
				temp32=SC.offsd-(xlen<<2);  
				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						UINT32 s=*(UINT32*)sadrs;
						UINT32 p=*(UINT32*)padrs;
						*(UINT32*)sadrs=((p & 0x00FF0000)+(s & 0x00FF0000) & (0x00FF0000))|
							((p & 0x0000FF00)+(s & 0x0000FF00) & (0x0000FF00))|
							((p & 0x000000FF)+(s & 0x000000FF) & (0x000000FF));          
						padrs+=4;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}


			void putbmflat32ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart32(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen<<2);
				temp32=SC.offsd-(xlen<<2);  
				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{   
						if (*(UINT32*)padrs !=t_chroma)
							*(UINT32*)sadrs=*(UINT32*)padrs;          
						padrs+=4;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}


			void putbmflat8ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart8(tpadrs,x,y,bmoffset);
				bmoffset-=xlen;
				temp32=SC.offsd-(xlen<<2);  
				UINT32 inchar;  
				while (ylen>0)
				{                  
					for (xlop=0;xlop<xlen;xlop++)
					{
						inchar=*(UINT32*)&g2d::raster::localpalette[(UINT8)*padrs];
						if (inchar!=t_chroma)
						{              
							*(UINT32*)sadrs=inchar;
						}                                  
						padrs++;            
						sadrs+=4;
					}
					padrs+=bmoffset;
					sadrs+=temp32;                                 
					ylen--;
				}
			}


			void putbmflat16ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
			{
				INT32 xlop;
				padrs=get_scanstart16(tpadrs,x,y,bmoffset);
				bmoffset-=(xlen+xlen);
				temp32=SC.offsd-(xlen<<2);  
				UINT32 pixin;
				while (ylen>0)
				{
					for (xlop=0;xlop<xlen;xlop++)
					{
						//do colour convertion and stippling

						pixin=*(UINT16*)padrs;
						pixin=((pixin<< 8)& 0x00FF0000)|((pixin<< 5)& 0x0000FF00)|((pixin<< 3)& 0x000000FF);				
						if (pixin!=t_chroma)
							*(UINT32*)sadrs=pixin;
						padrs+=2;
						sadrs+=4;                               
					}
					padrs+=bmoffset;
					sadrs+=temp32;
					ylen--;
				}
			}



			void putbitmapfast(INT32 x,INT32 y,tmath::CRect2i *srect,tsurface *pic,UINT32 attrib)
			{
				
				if (!pic) return;
				INT32 x2,y2,xlen,ylen;     
				INT32 pic_x,pic_y;
				if (!srect) srect=&pic->viewport;
				pic_x=srect->a.x;
				pic_y=srect->a.y;		
				UINT32 picoffset=pic->offsd;	
				x2=x+(srect->b.x-pic_x);//pic->size.x-1;
				y2=y+(srect->b.y-pic_y);//pic->size.y-1;

				if (x<SC.viewport.a.x)
				{
					if (x2<SC.viewport.a.x) return;
					pic_x+=SC.viewport.a.x-x; 
					x=SC.viewport.a.x;
				}
				if (y<SC.viewport.a.y)
				{
					if (y2<SC.viewport.a.y)return;        
					pic_y+=SC.viewport.a.y-y; 
					y=SC.viewport.a.y;
				}    
				if (x2>SC.viewport.b.x)
				{
					if (x>SC.viewport.b.x) return;
					x2=SC.viewport.b.x;
				}
				if (y2>SC.viewport.b.y)
				{
					if (y>SC.viewport.b.y) return;        
					y2=SC.viewport.b.y;
				}

				xlen=x2-x+1;
				ylen=y2-y+1;
				if (xlen<=0 || ylen <=0) return;
				sadrs=get_scanstart32(SC.scrptr,x,y,SC.offsd);
				tpadrs=pic->scrptr;    
				if (pic->isindexed())
				{
					if (pic->palette)	
					{

						g2d::setbitmappalette(pic->palette);
						pixmapmode((pic->pfd << 4)  + attrib);	
					}else  //draw as greyscale
						pixmapmode((PXFl8 << 4)  + attrib);	
				}else	
					pixmapmode((pic->pfd << 4)  + attrib);	
				t_chroma=pic->chroma;



				t_renderpixmap(pic_x,pic_y,xlen,ylen,picoffset);                
			}


			void putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,tmath::CRect2i *srect,tsurface *pic,UINT32 attrib)
			{
				
				INT32 x2,y2,end_pw,end_ph,cylen,cxlen,cx,yclip,xclip,picoffset;     
				INT32 pic_x,pic_y,pic_w,pic_h,cpic_x,cpic_y;
				tmath::bAnd(*srect,tmath::CRect2i(0,0,pic->size.x-1,pic->size.y-1));

				pic_x=srect->a.x;
				pic_y=srect->a.y;
				pic_w=1+srect->b.x-pic_x;
				pic_h=1+srect->b.y-pic_y;


				x2=x+dw-1;
				y2=y+dh-1;

				if (x<SC.viewport.a.x)
				{
					if (x2<SC.viewport.a.x) return;
					cpic_x=SC.viewport.a.x-x; 
					x=SC.viewport.a.x;
				}else
					cpic_x=0;
				if (y<SC.viewport.a.y)
				{
					if (y2<SC.viewport.a.y)return;        
					cpic_y=SC.viewport.a.y-y; 
					y=SC.viewport.a.y;
				}else
					cpic_y=0; 
				if (x2>SC.viewport.b.x)
				{
					if (x>SC.viewport.b.x) return;
					x2=SC.viewport.b.x;
				}
				if (y2>SC.viewport.b.y)
				{
					if (y>SC.viewport.b.y) return;        
					y2=SC.viewport.b.y;
				}

				if ((pic_h<=0) || (pic_w<=0)) return;

				//xlen=x2-x + 1;ylen=y2-y + 1;
				if ((x2-x)<0 || (y2-y)<0) return;				
				picoffset=pic->offsd;
				tpadrs=pic->scrptr;    
				t_chroma=pic->chroma;

				if (pic->isindexed())
				{
					if (pic->palette)	
					{
						g2d::setbitmappalette(pic->palette);
						pixmapmode((pic->pfd << 4)  + attrib);	
					}else  //draw as greyscale
						pixmapmode((PXFl8 << 4)  + attrib);	
				}else	
					pixmapmode((pic->pfd << 4)  + attrib);	

				cpic_x-=((cpic_x) / pic_w) *pic_w;
				cpic_y-=((cpic_y) / pic_h) *pic_h;

				//first row
				INT32 endx=x2-pic_w;
				INT32 endy=y2-pic_h;	

				cylen=pic_h-cpic_y;		
				if ((yclip=(y2-(y+cylen-1)))<0)
					cylen+=yclip;

				cxlen=pic_w-cpic_x;
				cpic_x+=pic_x;
				cpic_y+=pic_y;

				if ((xclip=(x2-(x+cxlen-1)))<=0)
				{
					cxlen+=xclip;
					sadrs=get_scanstart32(SC.scrptr,x,y,SC.offsd);
					t_renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		

					if (yclip>0)		
					{
						y+=cylen;
						while (y<=endy)
						{	
							sadrs=get_scanstart32(SC.scrptr,x,y,SC.offsd);	
							t_renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
							y+=pic_h;				
						}		
						sadrs=get_scanstart32(SC.scrptr,x,y,SC.offsd);	
						if ((end_ph=(1+y2-y))>0)								
							t_renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);
					}		
				}else
				{		
					cx=x;		
					sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
					t_renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		
					cx+=cxlen;						
					while (cx<=endx)
					{
						sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
						t_renderpixmap(pic_x,cpic_y,pic_w,cylen,picoffset);			
						cx+=pic_w;			
					}						
					if ((end_pw=(1+x2-cx))>0)				
					{		
						sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
						t_renderpixmap(pic_x,cpic_y,end_pw,cylen,picoffset);
					}
					if (yclip>0)
					{
						y+=cylen;
						while (y<=endy)
						{			
							cx=x;
							sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
							t_renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
							cx+=cxlen;									
							while (cx<=endx)
							{
								sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
								t_renderpixmap(pic_x,pic_y,pic_w,pic_h,picoffset);
								cx+=pic_w;					
							}							

							if (end_pw>0)										
							{
								sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
								t_renderpixmap(pic_x,pic_y,end_pw,pic_h,picoffset);
							}
							y+=pic_h;
						}

						if ((end_ph=(1+y2-y))>0)	
						{
							cx=x;
							sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
							t_renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);					
							cx+=cxlen;									
							while (cx<=endx)
							{
								sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);
								t_renderpixmap(pic_x,pic_y,pic_w,end_ph,picoffset);
								cx+=pic_w;					
							}				
							if (end_pw>0)				
							{		
								sadrs=get_scanstart32(SC.scrptr,cx,y,SC.offsd);	
								t_renderpixmap(pic_x,pic_y,end_pw,end_ph,picoffset);								
							}
						}
					}
				}
			}


#define drawbytes_1bit32(adrs,data,xlen,ylen,offs,mask,col)\
	_asm mov edi,dword ptr adrs \
	_asm mov esi,data \
	_asm mov cl,byte ptr xlen \
	_asm mov ch,byte ptr ylen \
	_asm mov edx,dword ptr offs \
	_asm mov ah,byte ptr mask \
	_asm mov ebx,dword ptr col \
	_asm mov byte ptr temp32,cl \
	_asm _yloop: \
	_asm    mov cl,byte ptr temp32 \
	_asm   _xloop: \
	_asm     mov al,[esi] \
	_asm     and al,ah \
	_asm     test al,128 \
	_asm     jz _nodraw1 \
	_asm       mov [edi],ebx \
	_asm     _nodraw1: \
	_asm     test al,64 \
	_asm     jz _nodraw2 \
	_asm       mov [edi+4],ebx \
	_asm     _nodraw2: \
	_asm     test al,32 \
	_asm     jz _nodraw3 \
	_asm       mov [edi+8],ebx \
	_asm     _nodraw3: \
	_asm     test al,16 \
	_asm     jz _nodraw4 \
	_asm       mov [edi+12],ebx \
	_asm     _nodraw4: \
	_asm     test al,8 \
	_asm     jz _nodraw5 \
	_asm       mov [edi+16],ebx \
	_asm     _nodraw5: \
	_asm     test al,4 \
	_asm     jz _nodraw6 \
	_asm       mov [edi+20],ebx \
	_asm     _nodraw6: \
	_asm     test al,2 \
	_asm     jz _nodraw7 \
	_asm       mov [edi+24],ebx \
	_asm     _nodraw7: \
	_asm     test al,1 \
	_asm     jz _nodraw8 \
	_asm       mov [edi+28],ebx \
	_asm     _nodraw8: \
	_asm     inc esi \
	_asm     add edi,edx \
	_asm     dec cl \
	_asm   jnz _xloop \
	_asm   dec ch \
	_asm   jnz _yloop;



#define drawbytes_1bit2(adrs,data,xlen,ylen,offs,mask,col)\
	_asm mov edi,dword ptr adrs \
	_asm mov esi,dword ptr data \
	_asm mov cl,byte ptr xlen \
	_asm mov ch,byte ptr ylen \
	_asm mov edx,dword ptr offs \
	_asm mov byte ptrmask, ah \
	_asm mov bx,word ptr col \
	_asm mov byte ptr temp32,cl \
	_asm _yloop: \
	_asm    mov cl,byte ptr temp32 \
	_asm    xor edx,edx \
	_asm   _xloop: \
	_asm     mov al,[esi] \
	_asm     and al,ah \
	_asm     mov [edi],ebx \
	_asm     mov [edi+4],ebx \
	_asm     mov [edi+8],ebx \
	_asm     mov [edi+12],ebx \
	_asm     mov [edi+16],ebx \
	_asm     mov [edi+20],ebx \
	_asm     mov [edi+24],ebx \
	_asm     mov [edi+28],ebx \
	_asm     inc esi \
	_asm     add edi,edx \
	_asm     dec cl \
	_asm   jnz _xloop \
	_asm   dec ch \
	_asm   jnz _yloop;

			void circle(INT32 cx,INT32 cy,INT32 radius)
			{
				tmath::CRect2i circrect=tmath::CRect2i(cx-radius,cy-radius,cx+radius,cy+radius);
				if (!tmath::is_inside(circrect,SC.viewport)) return;
				register UINT32 col=t_col;
				UINT32 halfoffsd=SC.offsd >>1;
				INT32 x,y,d,deltaE,deltaSE;
				INT32 yoff;
				INT32 xoff;


				x=xoff=0;
				y=radius;
				yoff=(y*SC.offsd) >> 1;
				d=1-radius;
				deltaE=3;
				deltaSE=5-(radius<<1);

				UINT8 *base=(UINT8*)get_scanstart32(SC.scrptr,cx,cy,SC.offsd);

				/**(UINT16*)(basey+cx+y)=t_col.c16;
				*(UINT16*)(basey+yoff+cx)=t_col.c16;
				*(UINT16*)(basey+yoff-cx)=t_col.c16;
				*(UINT16*)(basey+cx-y)=t_col.c16;
				*(UINT16*)(basey+cx-y)=t_col.c16;
				*(UINT16*)(basey-yoff+cx)=t_col.c16;
				*(UINT16*)(basey-yoff-cx)=t_col.c16;
				*(UINT16*)(basey-cx+y)=t_col.c16;*/

				*(UINT32*)( ((UINT32*)base)+y)=col;
				*(UINT32*)( ((UINT32*)base)-y)=col;
				*(UINT32*)( ((UINT32*)base)-y)=col;
				*(UINT32*)( ((UINT32*)base)+y)=col;     
				*(UINT32*)( ((UINT32*)base)+yoff)=col;     
				*(UINT32*)( ((UINT32*)base)-yoff)=col;     

				while(y>x)
				{
					if (d<0)
					{
						d+=deltaE;
						deltaE+=2;
						deltaSE+=2;
						x++;
						xoff+=halfoffsd;
					}
					else
					{
						d+=deltaSE;
						deltaE+=2;
						deltaSE+=4;
						x++;
						y--;
						xoff+=halfoffsd;
						yoff-=halfoffsd;
					}         
					*(UINT32*)( ((UINT32*)base)+xoff+y)=col;
					*(UINT32*)( ((UINT32*)base)+xoff-y)=col;
					*(UINT32*)( ((UINT32*)base)-xoff-y)=col;
					*(UINT32*)( ((UINT32*)base)-xoff+y)=col;

					*(UINT32*)( ((UINT32*)base)+yoff+x)=col;
					*(UINT32*)( ((UINT32*)base)+yoff-x)=col;
					*(UINT32*)( ((UINT32*)base)-yoff-x)=col;
					*(UINT32*)( ((UINT32*)base)-yoff+x)=col;

				}
			}

			//error in clipping, missing out last pixel!!
			void drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *databytes)
			{
				PIXINT y2,x2;

				PIXINT y_clip=0;
				unsigned char mask;

				x2=x+(w<<3);
				y2=y+h;
				if ((y>SC.viewport.b.y)||(y2<=SC.viewport.a.y)||(x>SC.viewport.b.x)||(x2<=SC.viewport.a.x))
					return;

				if (y<SC.viewport.a.y)
				{
					y_clip=abs(SC.viewport.a.y-y);
					y=SC.viewport.a.y;
					databytes+=y_clip*t_charset.xbytesize;
					h-=y_clip;
				}


				if (y2>SC.viewport.b.y)
				{
					y_clip=(y2-SC.viewport.b.y)-1;        
					h-=y_clip;
				}    

				mask=0xff;
				if (x<SC.viewport.a.x)
					mask&=(0xff>>(SC.viewport.a.x-x));
				if (x2>SC.viewport.b.x)                                
					mask&=(0xff<<(x2-SC.viewport.b.x-1));


				sadrs=get_scanstart32(SC.scrptr,x,y,SC.offsd);   

				drawbytes_1bit32(sadrs,databytes,w,h,SC.offsd,mask,t_col);            
			}

			void clearouttextxy(PIXINT cx,PIXINT cy,const char *txt,PIXINT slen)
			{
				//char *adrs;
				register tfontchar *ch;        
				INT32 schar=0;

				if (slen<0)
					slen=(INT32)strlen(txt);

				while (cx<SC.viewport.a.x)
				{

					cx+=FNTget_charwidth(*txt);
					txt++;
					schar++;

				}
				if (schar!=0)
				{
					schar--;
					txt--;
					cx-=FNTget_charwidth(*txt);
				}        

				while (cx<=SC.viewport.b.x)
				{
					if (schar>slen) return;
					ch=&t_charset.offsets[*txt];
					drawbytes(cx,cy,ch->bytewidth,ch->height,ch->data);
					cx+=FNTget_charwidth(ch);
					txt++;
					schar++;
				}        
			}


			void outtextxy(PIXINT cx,PIXINT cy,const char *txt,PIXINT slen)
			{
				//char *adrs;
				register tfontchar *ch;        
				INT32 schar=0;
				if (cx>SC.viewport.b.x) return;
				if (slen<0)
					slen=(INT32)strlen(txt);

				while (cx<SC.viewport.a.x)
				{        
					cx+=FNTget_charwidth(*txt);
					txt++;
					if (!*txt) return;
					schar++;        
				}
				if (schar!=0)
				{
					schar--;
					txt--;
					cx-=FNTget_charwidth(*txt);
				}        

				while (cx<=SC.viewport.b.x)
				{
					if (schar>slen) return;
					ch=&t_charset.offsets[*txt];
					drawbytes(cx,cy,ch->bytewidth,ch->height,ch->data);
					cx+=FNTget_charwidth(ch);
					txt++;
					schar++;
				}        
			}


#define _cleardevice32(screen,pagesize,col)\
	_asm mov edi,screen \
	_asm mov ecx,pagesize \
	_asm mov eax,col \
	_asm shr ecx,2 \
	_asm rep stosd;

			void cleardevice(void)
			{
				UINT32 pagesize=SC.pagesize();
				_cleardevice32(SC.scrptr,pagesize,t_fillcol);
			}

		}
	}

}