/*
 * Copyright (c) 1991-1995 Regents of the University of California.
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
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
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
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: crypt.cc,v 1.7 95/10/12 23:57:02 mccanne Exp $ (LBL)";
#endif

#include "crypt.h"

/*XXX*/
#define PROTOTYPES 1 
#include "global.h"
#include "md5.h"

Crypt::Crypt() : badpktlen_(0), badpbit_(0)
{
}

Crypt::~Crypt()
{
}

int Crypt::command(int argc, const char*const* argv)
{
	if (argc == 3) {
		Tcl& tcl = Tcl::instance();
		if (strcmp(argv[1], "key") == 0) {
			if (set_key(argv[2]) != 0)
				tcl.result("0");
			else
				tcl.result("1");
			return (TCL_OK);
		}
	}
	return (TclObject::command(argc, argv));
}

int Crypt::set_key(const char* key)
{
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (u_char*)key, strlen(key));
	u_char hash[16];
	MD5Final((u_char *)hash, &context);
	return (install_key(hash));
}
