/*
 * Copyright (c) 1995 The Regents of the University of California.
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
 * 	This product includes software developed by the Network Research
 * 	Group at Lawrence Berkeley National Laboratory.
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
static char rcsid[] =
    "@(#) $Header: module.cc,v 1.3 95/10/23 21:00:38 kfall Exp $ (LBL)";
#endif

#include "module.h"

char* Module::fttoa(int ft)
{
	switch (ft) {
	case FT_YUV_411:  return ("411");
	case FT_YUV_422:  return ("422");
	case FT_YUV_CIF:  return ("cif");
	case FT_JPEG:	  return ("jpeg");
	case FT_DCT:	  return ("dct");
	}
	return ("");
}

int Module::atoft(const char* s)
{
	if (strcasecmp(s, "411") == 0)
		return FT_YUV_411;
	if (strcasecmp(s, "422") == 0)
		return FT_YUV_422;
	if (strcasecmp(s, "cif") == 0)
		return FT_YUV_CIF;
	if (strcasecmp(s, "jpeg") == 0)
		return FT_JPEG;
	if (strcasecmp(s, "dct") == 0)
		return FT_DCT;
	return (-1);
}

Module::Module(int ft)
	: ft_(ft), target_(0),
	  width_(0), height_(0), framesize_(0)
{
}

int Module::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	if (argc == 2) {
		if (strcmp(argv[1], "frame-format") == 0) {
			tcl.result(fttoa(ft_));
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "target") == 0) {
			target_ = (Module*)TclObject::lookup(argv[2]);
			return (TCL_OK);
		}
	}
	return (TclObject::command(argc, argv));
}

TransmitterModule::TransmitterModule(int ft) : Module(ft)
{
}

int TransmitterModule::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	if (argc == 3) {
		if (strcmp(argv[1], "transmitter") == 0) {
			tx_ = (Transmitter*)TclObject::lookup(argv[2]);
			return (TCL_OK);
		}
	}
	return (Module::command(argc, argv));
}
