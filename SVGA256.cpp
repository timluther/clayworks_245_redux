
//renderpixmap=putbmflat8;

#define fillhigh_8(r)\
    _asm mov bl,byte ptr r\
	_asm mov ax,bx \
    _asm shl ax,8 \
    _asm or ax,bx \
	_asm mov ebx,eax \
	_asm shl ebx,16 \
	_asm or eax,ebx \
	_asm mov r,eax;

//takes an address and works out how much you should shift right by
//to affect 4 byte alignment
//you would then read the aligned data from the display memory,
//or with the shifted data and then write back to the screen
//ch is filled with the actual shift value, cl is filled with the amount of bytes to shift by

#define calcalignmentshiftvalue(adrs)\
_asm mov ecx,adrs \
_asm and ecx,3 \
_asm mov ch,1 \
_asm shl ch,cl;

//_asm bswap cx ;


//stores alignment mask in ebx
#define calcalignmentmask(adrs)\
calcalignmentshiftvalue(adrs)\
_asm mov ebx,0xFFFFFFFF \
_asm shr ebx,cl;

#define aligndata(data)\
_asm and data,0xFFFFFFFC ;

#define filleaxwithcolour(col)\
	_asm mov bl,byte ptr col \
	_asm mov ax,bx \
    _asm shl ax,8 \
    _asm or ax,bx \
	_asm mov ebx,eax \
	_asm shl ebx,16 \
	_asm or eax,ebx ;

#define drawalignedhline(writeop,readop,adrs,count,col)\
_asm mov edi,adrs \
calcalignmentmask(edi) \
_asm test ch,0xFF \
_asm jz noalignmentneeded \
    _asm xor edx,edx \
    _asm mov dl,ch \
	_asm sub dword ptr count,edx  \
	aligndata(edi) \
    _asm mov edx,dword ptr col \
    _asm mov eax,edx \
    _asm and edx,ebx \
    _asm mov esi,[edi] \
	_asm not ebx \
	_asm and esi,ebx \
	_asm writeop ecx,edx \
    _asm mov [edi],esi \
	_asm add edi,4 \
_asm noalignmentneeded: \
_asm mov ecx,dword ptr count \
_asm shr ecx,2 \
_asm xloop: \
   _asm writeop [edi],eax \
   _asm add edi,4 \
   _asm dec ecx \
_asm jnz xloop \
_asm test ebx,0 \
_asm jz notrailingbytesneeded \
	_asm mov ecx,[edi] \
	_asm and ecx,ebx \
	_asm not ebx \
	_asm and eax,ebx \
	_asm writeop ecx,eax \
	_asm mov [edi],ecx \
_asm notrailingbytesneeded: ;

     
tcolour svgai8_getpixel(PIXINT x,PIXINT y)
{   
   if (!IC.viewport.rcontains(x,y)) return 0;
   return *(tcolour*)getscanstart8(IC.scrptr,x,y,IC.offsd);
}    

void svgai8_putpixel(register PIXINT x,register PIXINT y)
{   
   if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))   
	   *(UINT8*)getscanstart8(SC.scrptr,x,y,SC.offsd)=t_col;	            
}


#define _hline_8bit(adrs,length,col,op)\
_asm mov edi,adrs \
_asm mov ecx,length \
_asm mov eax,col \
_asm drawloop: \
_asm   op [edi],al \
_asm   inc edi \
_asm   dec ecx \
_asm jnz drawloop;
  
void svgai8_hline(INT32 x1,INT32 x2,INT32 y)
{
    INT32 xlen;    
    if (x1>x2) swap(x2,x1);
    if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    x2++;	
    xlen=x2-x1;
	fillhigh_8(t_col);
	
    if (xlen<=0) return;  	
	sadrs=getscanstart8(SC.scrptr,x1,y,SC.offsd);	
	drawalignedhline(mov,and,sadrs,xlen,t_col);     
}

#define _cleardevice8(screen,pagesize,col)\
_asm mov edi,screen \
_asm mov ecx,pagesize \
_asm mov ax,word ptr col \
_asm mov bx,ax \
_asm shl eax,16 \
_asm mov ax,bx \
_asm shr ecx,2 \
_asm rep stosd;
       
void svgai8_cleardevice(void)
{
	UINT32 pagesize=SC.pagesize();
    _cleardevice8(SC.scrptr,pagesize,t_fillcol);
}
