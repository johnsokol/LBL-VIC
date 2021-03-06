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
 *
 * @(#) $Header: ntp-time.h,v 1.5 96/03/22 08:31:27 van Exp $ (LBL)
 */
#ifndef vic_ntp_time_h
#define vic_ntp_time_h


#include "sys-time.h"
#include "config.h"
#include "rtp.h"

/*
 * convert microseconds to fraction of second * 2^32 (i.e., the lsw of
 * a 64-bit ntp timestamp).  This routine uses the factorization
 * 2^32/10^6 = 4096 + 256 - 1825/32 which results in a max conversion
 * error of 3 * 10^-7 and an average error of half that.
 */
inline u_int usec2ntp(u_int usec)
{
	u_int t = (usec * 1825) >> 5;
	return ((usec << 12) + (usec << 8) - t);
}

/*
 * Number of seconds between 1-Jan-1900 and 1-Jan-1970
 */
#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800

/*
 * Return a 64-bit ntp timestamp (UTC time relative to Jan 1, 1970).
 * gettimeofday conveniently gives us the correct reference -- we just
 * need to convert sec+usec to a 64-bit fixed point (with binary point
 * at bit 32).
 */
inline ntp64 ntp64time(timeval tv)
{
	ntp64 n;
	n.upper = (u_int)tv.tv_sec + GETTIMEOFDAY_TO_NTP_OFFSET;
	n.lower = usec2ntp((u_int)tv.tv_usec);
	return (n);
}

inline u_int32_t ntptime(timeval t)
{
	u_int s = (u_int)t.tv_sec + GETTIMEOFDAY_TO_NTP_OFFSET;
	return (s << 16 | usec2ntp((u_int)t.tv_usec) >> 16);
}

inline u_int32_t ntptime()
{
	timeval tv;
	::gettimeofday(&tv, 0);
	return (ntptime(tv));
}

inline timeval unixtime()
{
	timeval tv;
	::gettimeofday(&tv, 0);
	return (tv);
}
#endif
