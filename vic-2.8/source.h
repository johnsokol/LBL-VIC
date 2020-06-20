/*
 * Copyright (c) 1994 Regents of the University of California.
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
 * @(#) $Header: source.h,v 1.2 96/07/01 20:11:47 mccanne Exp $ (LBL)
 */

#ifndef vic_source_h
#define vic_source_h

#include <string.h>
#include "sys-time.h"
#include "config.h"
#include "timer.h"
#include "inet.h"
#include "Tcl.h"
#include "rtp.h"

class SourceManager;

#define SOURCE_HASH 1024

class PacketHandler : public TclObject {
    public:
	virtual ~PacketHandler();
	inline PacketHandler(int hdrlen) : hdrlen_(hdrlen), delvar_(0) { }
	inline int hdrlen() const { return (hdrlen_); }
	virtual void recv(const struct rtphdr*,
			  const u_char* data, int len) = 0;
	inline u_int32_t delvar() const { return (delvar_); }
	inline void delvar(u_int32_t v) { delvar_ = v; }
    protected:
	int hdrlen_;
	int delvar_;
};

class Source : public TclObject {
    public:
	Source(u_int32_t srcid, u_int32_t ssrc, u_int32_t addr);
	virtual ~Source();

	PacketHandler* activate(int format);
	PacketHandler* change_format(int format);
	void deactivate();

	virtual int command(int argc, const char*const* argv);
	inline PacketHandler* handler() const { return (handler_); }

	void lts_done(const timeval& now) { lts_done_ = now; }
	void lts_data(const timeval& now) { lts_data_ = now; }
	void action() { if (!busy_) set_busy(); }
	void lts_ctrl(const timeval& now) { lts_ctrl_ = now; }
	void sts_data(u_int32_t now) { sts_data_ = now; }
	void sts_ctrl(u_int32_t now) { sts_ctrl_ = now; }

	inline const timeval& lts_ctrl() const { return (lts_ctrl_); }
	inline const timeval& lts_data() const { return (lts_data_); }
	inline const timeval& lts_done() const { return (lts_done_); }
	inline int sts_ctrl() const { return (sts_ctrl_); }
	inline int sts_data() const { return (sts_data_); }

	inline const char* sdes(int t) const { return (sdes_[t]); }
	void sdes(int t, const char* value);
	inline u_int32_t addr() const { return (addr_); }
	inline void addr(u_int32_t a) { addr_ = a; }
	inline u_int32_t srcid() const { return (srcid_); }
	inline void srcid(u_int32_t s) { srcid_ = s; }
	inline u_int32_t ssrc() const { return (ssrc_); }
	inline void ssrc(u_int32_t s) { ssrc_ = s; }
	inline void format(int v) { format_ = v; }
	inline int format() const { return (format_); }
	inline void mute(int v) { mute_ = v; }
	inline int mute() const { return (mute_); }
	inline void ismixer(int v) { ismixer_ = v; }
	inline int ismixer() const { return (ismixer_); }
	void clear_counters();

	/*XXX should start at random values*/
	inline u_int32_t nb() const { return (nb_); }
	inline u_int32_t nf() const { return (nf_); }
	inline u_int32_t np() const { return (np_); }
	inline u_int32_t nm() const { return (nm_); }
	inline u_int32_t ns() const { return (cs_ - fs_); } /* no. expected */
	inline u_int32_t ehs() const { return (-(fs_ & 0xffff0000) | cs_); }
	inline u_int32_t cs() const { return (cs_); }
	inline u_int32_t snp() const { return (snp_); }
	inline u_int32_t sns() const { return (sns_); }
	inline u_int32_t runt() const { return (nrunt_); }
	inline u_int32_t dups() const { return (ndup_); }
	inline u_int32_t badsesslen() const { return (badsesslen_); }
	inline u_int32_t badsessver() const { return (badsessver_); }
	inline u_int32_t badsessopt() const { return (badsessopt_); }
	inline u_int32_t badsdes() const { return (badsdes_); }
	inline u_int32_t badbye() const { return (badbye_); }
	inline void nb(int v) { nb_ += v; }
	inline void nf(int v) { nf_ += v; }
	inline void np(int v) { np_ += v; }
	inline void nm(int v) { nm_ += v; }
	inline void snp(int v) { snp_ = v; }
	inline void sns(int v) { sns_ = v; }
	inline void fs(int v) { fs_ = v; }
	inline void runt(int v) { nrunt_ += v; }
	inline void badsesslen(int v) { badsesslen_ += v; }
	inline void badsessver(int v) { badsessver_ += v; }
	inline void badsessopt(int v) { badsessopt_ += v; }
	inline void badsdes(int v) { badsdes_ += v; }
	inline void badbye(int v) { badbye_ += v; }
	int cs(u_int16_t v);
	int checkseq(u_int16_t v);
	void lost(int);

	Source* next_;		/* link for SourceManager source list */
	Source* hlink_;		/* link for SourceManager hash table */
    protected:
	char* stats(char* cp) const;
	void set_busy();

	PacketHandler* handler_;

	u_int32_t srcid_;	/* rtp global src id (CSRC), net order */
	u_int32_t ssrc_;	/* rtp global sync src id (SSRC), net order) */
	u_int32_t addr_;	/* address of sender (net order) */

	u_int32_t sts_data_;	/* sndr ts from last data packet (net order) */
	u_int32_t sts_ctrl_;	/* sndr ts from last control packet */
	timeval lts_data_;	/* local unix time for last data packet */
	timeval lts_ctrl_;	/* local unix time for last control packet */
	timeval lts_done_;	/* local unix time for bye packet */

	u_int32_t fs_;		/* first seq. no received */
	u_int32_t cs_;		/* current (most recent) seq. no received */
	u_int32_t np_;		/* no. packets received  */
	u_int32_t nf_;		/* no. video frames received */
	u_int32_t nb_;		/* no. bytes received */
	u_int32_t nm_;		/* no. misordered packets detected */
	u_int32_t snp_;		/* last advertised no. pkts received */
	u_int32_t sns_;		/* last advertised no. pkts exptected */
	u_int32_t ndup_;	/* no. of duplicate packets (via RTP seqno) */
	u_int32_t nrunt_;	/* count of packets too small */
	
	/* following errors are from session (rtcp) processing */
	u_int32_t badsesslen_;	/* bad header length field */
	u_int32_t badsessver_;	/* bad header version number */
	u_int32_t badsessopt_;	/* unrecognized option */
	u_int32_t badsdes_;	/* sdes cnt > available data */
	u_int32_t badbye_;	/* bye cnt > available data */

	int format_;		/* RTP packet type */
	int mute_;		/* true if source muted */
	int lost_;		/* true when signal lost */
	int busy_;		/* nonzero. during talk spurt */
	int ismixer_;		/* true if source has acted as a 'mixer' */

#define SOURCE_NSEQ 64
	u_int16_t seqno_[SOURCE_NSEQ];
	char* sdes_[RTCP_SDES_MAX + 1];
};

class SourceManager : public TclObject {
    public:
	SourceManager();
	static inline SourceManager& instance() { return (instance_); }
	void init(u_int32_t localid, u_int32_t localaddr);
	virtual int command(int argc, const char*const* argv);
	Source* lookup(u_int32_t srcid, u_int32_t ssrc, u_int32_t addr);
	Source* demux(u_int32_t srcid, u_int32_t addr, u_int16_t seq);
	Source* consult(u_int32_t srcid);
	inline int nsources() const { return (nsources_); }
	inline Source* sources() const { return (sources_); }

	void CheckActiveSources(double msgint);
	void ListSources();

	u_int32_t clock() const { return (clock_); }
	inline Source* localsrc() const { return (localsrc_); }

	void sortactive(char*) const;
	void remove(Source*);
    protected:
	static int compare(const void*, const void*);
	Source* enter(Source* s);
	void remove_from_hashtable(Source* s);

	Source* lookup_duplicate(u_int32_t srcid, u_int32_t addr);

	int nsources_;
	Source* sources_;
	u_int32_t clock_;
	int keep_sites_;
	u_int site_drop_time_;
	Source* localsrc_;
	Source* generator_;
	Source* hashtab_[SOURCE_HASH];

	static SourceManager instance_;
};

inline int Source::checkseq(u_int16_t v)
{
	int k = v & (SOURCE_NSEQ-1);
	if (seqno_[k] != v) {
		seqno_[k] = v;
		return (0);
	} else {
		++ndup_;
		return (-1);
	}
}

inline int Source::cs(u_int16_t v)
{
	/*
	 * This routine updates a 32 bit sequence number based on
	 * the rtp packet's 16 bit seq. no.
	 */
	register int c = cs_;
	register int d = v - c;
	if (d < -1024 || d > 1024) {
		cs_ = v;
		if (v < 512 && c > 0x10000-512) {
			/*
			 * seq no. wrapped - subtract 64k from fs to
			 * account for it.
			 */
			fs_ -= 0x10000;
		} else {
			/*
			 * the seq. no made a very large jump.  assume
			 * that the other side restarted without telling
			 * us about it so just re-sync (i.e., pretend
			 * this was the first packet).
			 */
			fs_ = v - 1;
			np_ = 0;
			nf_ = 0;
			nb_ = 0;
			snp_ = 0;
			nm_ = 0;
		}
	} else if (d > 0) {
		/* d <= 0 means duplicate or reordered packet - ignore */
		cs_ = v;
		if (d < 0)
			/* out of order */
			++nm_;
	}
	return (checkseq(v));
}

#endif
