/*
 * Copyright (c) 1993-1995 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and the Network Research Group at
 *      Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) $Header: renderer.h,v 1.22 96/04/06 01:49:38 van Exp $ (LBL)
 */

#ifndef vic_renderer_h
#define vic_renderer_h

#include <sys/types.h>
#include "Tcl.h"
#include "timer.h"
#include "module.h"

/*
 * Base class for objects that render video onto an output device.
 * Output devices may be X windows, external video ports, etc.
 * The input stream may either be uncompressed YUV video or
 * a compressed bit stream like JPEG.
 */
class Renderer : public Module, public Timer {
    public:
	Renderer(int ft);
	virtual int command(int argc, const char*const* argv);
	virtual void setcolor(int) {}
	inline void now(u_int v) { now_ = v; }

	inline int update_interval() const { return (update_interval_); }
	inline int need_update() const { return (need_update_); }

	Renderer* next_;	/* linked list for decoders */
    protected:
	void setinterval(u_int v);
	virtual void timeout();
	u_int now_;
	u_int update_interval_;	/* update interval */
	int need_update_;	/* true to update on next frame */
};

class BlockRenderer : public Renderer {
    public:
	virtual void sync() const = 0;
	virtual int command(int argc, const char*const* argv);
    protected:
	inline BlockRenderer(int ft) : Renderer(ft), irthresh_(150) {}
	virtual int consume(const VideoFrame*);
	virtual void resize(int w, int h) = 0;
	virtual void render(const u_char* frm, int off, int x,
			    int w, int h) = 0;
	virtual void push(const u_char* frm, int miny, int maxy,
			  int minx, int maxx) const = 0;
	u_int irthresh_;	/* 'immediate render' threshhold --
				 * if less than this number of blocks
				 * are marked to be rendered, only the
				 * marked blocks are sent to the x server.
				 * otherwise all blocks between the min
				 * & max y changes are sent to the server.
				 * This value wants to be fairly small
				 * (something in the range 100-200).
				 * Diagnostic tools such as h261_play
				 * can set this to a large number when
				 * they want to see every marked block
				 * but no extra blocks rendered.
				 */ 
};

#endif
