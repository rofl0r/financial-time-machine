#ifndef EZSDL_H
#define EZSDL_H

#include <SDL/SDL.h>
#include <assert.h>
#include <stdlib.h>

//RcB: LINK "-lSDL"

#define SDL_RGB_LSHIFT 8
#define RGB_RED(X) 0 | ((X) << (16+SDL_RGB_LSHIFT))
#define RGB_GREEN(X) 0 | ((X) << (8+SDL_RGB_LSHIFT))
#define RGB_BLUE(X) (X << (0+SDL_RGB_LSHIFT))
#define RGB(R,G,B) (RGB_RED(R) | RGB_GREEN(G) | RGB_BLUE(B))

#define ARGB_RED(X) 0 | ((X) << 16)
#define ARGB_GREEN(X) 0 | ((X) << 8)
#define ARGB_BLUE(X) (X << 0)
#define ARGB(R,G,B) (ARGB_RED(R) | ARGB_GREEN(G) | ARGB_BLUE(B))

static inline unsigned rgba_to_argb(unsigned col) { return col >> 8; }
static inline unsigned argb_to_rgba(unsigned col) { return col << 8; }

typedef struct bmp4 {
	unsigned width, height;
	unsigned *data;
} bmp4;

static inline bmp4* bmp4_new(unsigned width, unsigned height) {
	bmp4* res = malloc((sizeof *res)+(width*height*sizeof(*res->data)));
	if(!res) return 0;
	res->width=width;
	res->height=height;
	res->data=(void*)(res+1);
	return res;
}

static inline bmp4* bmp4_from_file(const char* fn) {
	FILE*f=fopen(fn, "r");
	if(!f) return 0;
	unsigned w, h;
	fread(&w, 4, 1, f);
	fread(&h, 4, 1, f);
	bmp4* b = bmp4_new(w, h);
	if(!b) goto ext;
	size_t i; unsigned *p = b->data;
	for(i=0; i<w*h; i++, p++) fread(p, 4, 1, f);
	ext:
	fclose(f);
	return b;
}

static inline void bmp4_fill(bmp4 *b, unsigned color) {
	size_t i = 0;
	size_t max = b->width*b->height;
	unsigned *d = b->data;
	for(;i<max;i++) *(d++) = color;
}

static inline bmp4* bmp4_new_filled(unsigned width, unsigned height, unsigned color) {
	bmp4 *b = bmp4_new(width, height);
	if(b) bmp4_fill(b, color);
	return b;
}

static inline unsigned* bmp4_scanline(bmp4* b, int y) {
	return &b->data[y * b->width];
}

typedef struct bmp3 {
	unsigned width, height;
	unsigned char *data;
} bmp3;

static inline bmp3* bmp3_new(unsigned width, unsigned height) {
	bmp3* res = malloc((sizeof *res)+(width*height*3));
	res->width=width;
	res->height=height;
	res->data=(void*)(res+1);
	return res;
}

static inline unsigned char* bmp3_scanline(bmp3* b, int y) {
	return &b->data[3*y*b->width];
}

static inline void bmp3_to_bmp4(bmp3* in, bmp4 *out) {
	assert(in->width == out->width && in->height == out->height);
        size_t i = 0;
        size_t max = in->width*in->height;
	unsigned *d = out->data;
	unsigned char* id = in->data;
	#define ALPHA (0xff << 24)
	for(;i<max;i++,id+=3) *(d++) = ALPHA | (id[0] << 16) | (id[1] << 8) | id[2];
	#undef ALPHA
}

static inline void bmp4_to_bmp3(bmp4* in, bmp3 *out) {
	assert(in->width == out->width && in->height == out->height);
        size_t i = 0;
        size_t max = in->width*in->height;
	unsigned char *d = out->data;
	unsigned* id = in->data;
	for(;i<max;i++,d+=3,id++) {
		d[0] = (*id >> 16) & 255;
		d[1] = (*id >> 8 ) & 255;
		d[2] = (*id >> 0 ) & 255;
	}
}

typedef struct bmp1 {
	unsigned width, height;
	unsigned char *data;
} bmp1;

static inline bmp1* bmp1_new(unsigned width, unsigned height) {
	bmp1 *res = malloc((sizeof *res)+(width*height*1));
	res->width=width;
	res->height=height;
	res->data=(void*)(res+1);
	return res;
}

static inline unsigned char* bmp1_scanline(bmp1* b, int y) {
	return &b->data[y*b->width];
}

static inline unsigned colget8(unsigned char col, int which) {
	switch (which) {
		case 0: /* red */
			return (col >> 5) * 36;
		case 1: /* green */
			return ((col >> 3) & 3) * 85;
		case 2: /* blue */
			return (col & 7) * 36;
	}
}

static inline void bmp1_to_bmp4(bmp1* in, bmp4 *out, unsigned palette[256]) {
	assert(in->width == out->width && in->height == out->height);
        size_t i = 0;
        size_t max = in->width*in->height;
	unsigned *d = out->data;
	unsigned char* id = in->data;
	#define ALPHA (0xff << 24)
	if(!palette)
		for(;i<max;i++,id++) *(d++) = ALPHA | (colget8(*id, 0) << 16) | (colget8(*id, 1) << 8) | colget8(*id, 2);
	else
		for(;i<max;i++,id++) *(d++) = ALPHA | palette[*id];
	#undef ALPHA
}

typedef struct display {
	unsigned width, height;
	SDL_Surface *surface;
	int fs;
} display;

static inline void display_init(display *d, unsigned width, unsigned height) {
	SDL_Init(SDL_INIT_VIDEO);
	d->surface = SDL_SetVideoMode(width, height, 32, SDL_HWPALETTE);
	d->width = width;
	d->height = height;
	d->fs = 0;
}

static inline int display_get_pitch(display *d) {
	return d->surface->pitch;
}

static inline unsigned *display_get_vram(display *d) {
	return d->surface->pixels;
}

static inline void ezsdl_set_title(const char* text) {
	SDL_WM_SetCaption(text, 0);
}

static inline bmp4 *display_get_screenshot(display *d) {
	bmp4 *r = bmp4_new(d->width, d->height);
	if(!r) return 0;
	unsigned *out = r->data, *in = (void*)d->surface->pixels;
	size_t i, l = d->width*d->height;
	for(i=0;i<l;i++) *(out++)=argb_to_rgba(*(in++));
	return r;
}

struct spritesheet {
	unsigned sprite_count, sprites_per_row;
	unsigned sprite_w, sprite_h;
	bmp4 *bitmap;
};

static inline int spritesheet_init(struct spritesheet *result, bmp4 *b, unsigned sprite_w, unsigned sprite_h) {
	result->bitmap = b;
	result->sprite_w = sprite_w;
	result->sprite_h = sprite_h;
	result->sprite_count = (b->height * b->width) / (sprite_h * sprite_w);
	result->sprites_per_row = b->width / sprite_w;
	if((b->height * b->width) % (sprite_h * sprite_w)) return 0; /* picture dimensions are no multiple of the sprite dimensions*/
	return 1;
}

static inline unsigned spritesheet_getspritestart(struct spritesheet *ss, unsigned sprite_nr, unsigned row_nr) {
	unsigned sprite_row = sprite_nr / ss->sprites_per_row;
	unsigned row_off = (row_nr * ss->sprite_w * ss->sprites_per_row);
	unsigned res = (sprite_row * ss->sprite_w * ss->sprite_h * ss->sprites_per_row) + row_off
		       + ((sprite_nr % ss->sprites_per_row) * ss->sprite_w);
	return res;
}

static inline void display_draw_sprite(display *d, struct spritesheet* ss, unsigned sprite_no, unsigned sx, unsigned sy, unsigned scale) {
	if(!scale) scale = 1;
	assert(d->width >= sx+ss->sprite_w*scale && d->height >= sy+ss->sprite_h*scale);
	unsigned x,xx,y,yd,ys,pitch=d->surface->pitch/sizeof(unsigned);
	unsigned xscale,yscale;
	unsigned transp_col = ss->bitmap->data[0];
	unsigned sprite_pitch = ss->sprites_per_row * ss->sprite_w;
	for(y=0,yd=sy*pitch, ys=spritesheet_getspritestart(ss, sprite_no, y); y < ss->sprite_h; y++,ys+=sprite_pitch)
		for(yscale=0; yscale<scale; yscale++,yd+=pitch)
			for(x=0, xx=sx; x < ss->sprite_w; x++)
				for(xscale = 0; xscale < scale; xscale++,xx++)
					if(ss->bitmap->data[ys+x] != transp_col)
					((unsigned*)d->surface->pixels)[yd + xx] = rgba_to_argb(ss->bitmap->data[ys+x]);
}

static inline void display_draw(display *d, bmp4* b, unsigned sx, unsigned sy, unsigned scale) {
	if(!scale) scale = 1;
	assert(d->width >= sx+b->width*scale && d->height >= sy+b->height*scale);
	unsigned x,xx,y,yd,ys,pitch=d->surface->pitch/sizeof(unsigned);
	unsigned xscale,yscale;
	for(y=0,yd=sy*pitch, ys=0; y < b->height; y++,ys+=b->width)
		for(yscale=0; yscale<scale; yscale++,yd+=pitch)
			for(x=0, xx=sx; x < b->width; x++)
				for(xscale = 0; xscale < scale; xscale++,xx++)
					((unsigned*)d->surface->pixels)[yd + xx] = rgba_to_argb(b->data[ys+x]);
	//SDL_UpdateRect(d->surface, sx, sy, b->width*scale, b->height*scale);
}

static inline void display_draw_vline(display *d, unsigned sx, unsigned sy, unsigned height, unsigned color, unsigned scale) {
	if(!scale) scale = 1;
	assert(d->width >= sx && d->height >= sy+height*scale);
	unsigned y,yd,yscale,xscale,pitch=d->surface->pitch/sizeof(unsigned);
	for(y = sy, yd=sy*pitch; y < sy+height; y++) for(yscale=0;yscale<scale;yscale++,yd+=pitch)
	for(xscale=0; xscale<scale; xscale++)
		((unsigned*)d->surface->pixels)[yd + sx + xscale] = rgba_to_argb(color);
}

static inline void display_draw_hline(display *d, unsigned sx, unsigned sy, unsigned width, unsigned color, unsigned scale) {
	if(!scale) scale = 1;
	assert(d->width >= sx+width*scale && d->height >= sy);
	unsigned x,yd,yscale,pitch=d->surface->pitch/sizeof(unsigned);
	for(yscale = 0, yd=sy*pitch; yscale < scale; yscale++,yd+=pitch)
	for(x=sx;x<sx+width*scale;x++)
		((unsigned*)d->surface->pixels)[yd + x] = rgba_to_argb(color);
}

static inline void display_fill_rect(display *d, unsigned sx, unsigned sy, unsigned width, unsigned height, unsigned color, unsigned scale) {
	if(!scale) scale = 1;
	assert(d->width >= sx+width*scale && d->height >= sy+height*scale);
	unsigned x,xx,y,yd,pitch=d->surface->pitch/sizeof(unsigned);
	unsigned xscale,yscale;
	for(y=0,yd=sy*pitch; y < height; y++)
		for(yscale=0; yscale<scale; yscale++,yd+=pitch)
			for(x=0, xx=sx; x < width; x++)
				for(xscale = 0; xscale < scale; xscale++,xx++)
					((unsigned*)d->surface->pixels)[yd + xx] = rgba_to_argb(color);
}

static inline void display_refresh(display *d) {
	SDL_UpdateRect(d->surface, 0, 0, d->width, d->height);
}

static inline void display_clear(display *d) {
        unsigned *ptr = (unsigned *) d->surface->pixels;
        unsigned pitch = d->surface->pitch/4;
        unsigned x, y;
        for(y = 0; y < d->height; y++) for (x = 0; x < d->width; x++)
                ptr[y*pitch + x] = 0;
}

static inline void display_toggle_fullscreen(display *d) {
	d->fs = !d->fs;
	SDL_WM_ToggleFullScreen(d->surface);
	SDL_Delay(1);
	//display_clear(d);
	SDL_UpdateRect(d->surface,0,0,d->width,d->height);
	SDL_Delay(1);
}

#if 0
static inline void display_hline(display *d, unsigned color, unsigned x, unsigned y, unsigned len, unsigned scale) {
	unsigned y, x;
}
#endif

enum eventtypes {
	EV_NONE = 0,
	EV_KEYDOWN,
	EV_KEYUP,
	EV_MOUSEMOVE,
	EV_MOUSEDOWN,
	EV_MOUSEUP,
	EV_MOUSEWHEEL,
	EV_JOYMOVE,
	EV_JOYDOWN,
	EV_JOYUP,
	EV_QUIT,
	EV_NEEDREDRAW,
	EV_MAX
};

enum cbtypes {
	CB_GAMETICK = 0,
	CB_KEYDOWN,
	CB_KEYUP,
	CB_MOUSEMOVE,
	CB_MOUSEDOWN,
	CB_MOUSEUP,
	CB_MOUSEWHEEL,
	CB_JOYMOVE,
	CB_JOYDOWN,
	CB_JOYUP,
	CB_TIMER,
	CB_MAX
};

typedef struct event {
	int which;
	int xval, yval;
} ezevent;

typedef int(*eventcallbackfunc)(void*data, struct event* ev);
struct callback {
	void* data;
	eventcallbackfunc cb;
};

typedef struct inp {
	struct callback callbacks[CB_MAX];
	struct { int x, y; } mouse;
} inp;

static inline void inp_setcb(struct inp *in, enum cbtypes type, eventcallbackfunc cb, void* data) {
	in->callbacks[type].data = data;
	in->callbacks[type].cb = cb;
}

typedef void (*event_process_func)(struct inp* inp, SDL_Event* sdl_event, struct event *myevent);
static void event_process_mousemove(struct inp* inp, SDL_Event* sdl_event, struct event *myevent) {
	myevent->xval = sdl_event->motion.x;
	myevent->yval = sdl_event->motion.y;
	inp->mouse.x = sdl_event->motion.x;
	inp->mouse.y = sdl_event->motion.y;
}
static void event_process_mousedown(struct inp* inp, SDL_Event* sdl_event, struct event *myevent) {
	myevent->xval = sdl_event->button.x;
	myevent->yval = sdl_event->button.y;
	inp->mouse.x = sdl_event->motion.x;
	inp->mouse.y = sdl_event->motion.y;
}
static void event_process_mouseup(struct inp* inp, SDL_Event* sdl_event, struct event *myevent) {
	myevent->xval = sdl_event->button.x;
	myevent->yval = sdl_event->button.y;
	inp->mouse.x = sdl_event->motion.x;
	inp->mouse.y = sdl_event->motion.y;
}

static void event_process_keydown(struct inp* inp, SDL_Event* sdl_event, struct event *myevent) {
	myevent->which = sdl_event->key.keysym.sym;
}

static long long ezsdl_getutime64(void) {
        struct timeval t;
        gettimeofday(&t, NULL);
        return (t.tv_sec * 1000LL * 1000LL) + t.tv_usec;
}

static struct ezsdl {
	struct display disp;
	struct inp inp;
} ezsdl;

static inline void ezsdl_init(unsigned width, unsigned height) {
	display_init(&ezsdl.disp, width, height);
}

static inline void ezsdl_draw(bmp4*b, unsigned x, unsigned y, int scale) {
	display_draw(&ezsdl.disp, b, x, y, scale);
}

static inline void ezsdl_draw_vline(unsigned sx, unsigned sy, unsigned height, unsigned color, unsigned scale) {
	display_draw_vline(&ezsdl.disp, sx, sy, height, color, scale);
}

static inline void ezsdl_draw_hline(unsigned sx, unsigned sy, unsigned width, unsigned color, unsigned scale) {
	display_draw_hline(&ezsdl.disp, sx, sy, width, color, scale);
}

static inline void ezsdl_fill_rect(unsigned sx, unsigned sy, unsigned width, unsigned height, unsigned color, unsigned scale) {
	display_fill_rect(&ezsdl.disp, sx, sy, width, height, color, scale);
}

static inline void ezsdl_draw_sprite(struct spritesheet* ss, unsigned sprite_no, unsigned x, unsigned y, unsigned scale) {
	display_draw_sprite(&ezsdl.disp, ss, sprite_no, x, y, scale);
}

static inline bmp4* ezsdl_get_screenshot(void) {
	return display_get_screenshot(&ezsdl.disp);
}

static inline void ezsdl_clear(void) {
	display_clear(&ezsdl.disp);
}

static inline void ezsdl_refresh(void) {
	display_refresh(&ezsdl.disp);
}

static inline void ezsdl_setcb(enum cbtypes type, eventcallbackfunc cb, void* data) {
	inp_setcb(&ezsdl.inp, type, cb, data);
}

static inline void ezsdl_sleep(unsigned ms) {
	SDL_Delay(ms);
}

static inline int ezsdl_get_pitch(void) {
        return display_get_pitch(&ezsdl.disp);
}

static inline unsigned *ezsdl_get_vram(void) {
        return display_get_vram(&ezsdl.disp);
}

/* return event without blocking, call callbacks if so defined. */
static inline enum eventtypes ezsdl_getevent(struct event *myevent) {
	static const event_process_func event_processors[CB_MAX] = {
		[CB_MOUSEMOVE] = event_process_mousemove,
		[CB_MOUSEDOWN] = event_process_mousedown,
		[CB_MOUSEUP] = event_process_mousedown,
		[CB_KEYDOWN] = event_process_keydown,
		[CB_KEYUP] = event_process_keydown,
	};
	struct inp *in = &ezsdl.inp;
	SDL_Event sdl_event;
	enum eventtypes e = EV_NONE;
	if (SDL_PollEvent(&sdl_event)) {
		enum cbtypes t = CB_MAX;
		switch (sdl_event.type) {
			case SDL_MOUSEMOTION:
				e = EV_MOUSEMOVE;
				t = CB_MOUSEMOVE; break;
			case SDL_MOUSEBUTTONDOWN:
				e = EV_MOUSEDOWN;
				t = CB_MOUSEDOWN; break;
			case SDL_MOUSEBUTTONUP:
				e = EV_MOUSEUP;
				t = CB_MOUSEUP; break;
			case SDL_QUIT:
				return EV_QUIT;
			case SDL_KEYDOWN:
				if((sdl_event.key.keysym.mod & KMOD_LALT) ||
				   (sdl_event.key.keysym.mod & KMOD_RALT)) {
					display_toggle_fullscreen(&ezsdl.disp);
					SDL_Delay(1);
					e = EV_NEEDREDRAW;
				} else {
					e = EV_KEYDOWN;
					t = CB_KEYDOWN;
				}
				break;
			case SDL_KEYUP:
				e = EV_KEYUP;
				t = CB_KEYUP; break;
		}
		if(t != CB_MAX) {
			if(event_processors[t]) event_processors[t](in, &sdl_event, myevent);
			if(in->callbacks[t].cb && in->callbacks[t].cb(in->callbacks[t].data, myevent))
				e = EV_NEEDREDRAW;
		}
	}
	return e;
}

static inline void ezsdl_start(void(*game_tick)(int)) {
	struct event myevent;
	while(1) {
		unsigned need_redraw = 0;
		unsigned ms_passed = 0;
		do {
			enum eventtypes e;
			while((e = ezsdl_getevent(&myevent)) != EV_NONE)
				if(e == EV_QUIT) return;
				else if(e == EV_NEEDREDRAW) need_redraw = 1;
			SDL_Delay(1);
		} while (++ms_passed < 20);

		long long tm = ezsdl_getutime64();
		game_tick(need_redraw);
		ms_passed = (ezsdl_getutime64() - tm)/1000;
	}
}

#endif
