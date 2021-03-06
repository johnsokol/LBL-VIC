#
# Copyright (c) 1993-1995 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#	This product includes software developed by the Computer Systems
#	Engineering Group at Lawrence Berkeley Laboratory.
# 4. Neither the name of the University nor of the Laboratory may be used
#    to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# @(#) $Header: ui-util.tcl,v 1.12 96/02/05 16:02:19 mccanne Exp $ (LBL)

proc smallfont { } {
	return [resource smallfont]
}

proc mediumfont { } {
	return [resource medfont]
}

proc disfont { } {
	return [resource disablefont]
}

set nids 0
proc uniqueID { } {
	global nids
	incr nids
	return $nids
}

proc isCIF fmt {
	# only supported CIF format is h.261
	if { $fmt == "h261" } {
		return 1
	}
	return 0
}

proc toggle_window w {
	if { ![winfo exists $w] } { build$w }
	global created$w
	if { ! [info exists created$w] } {
		set created$w 1
		wm transient $w .
		update idletasks
		set x [winfo rootx .]
		set y [winfo rooty .]
		incr y [winfo height .]
		incr y -[winfo reqheight $w]
		incr y -20
 		# adjust for virtual desktops
		incr x [winfo vrootx .]
		incr y [winfo vrooty .]
		if { $y < 0 } { set y 0 }
		if { $x < 0 } {
			set x 0
		} else {
			set right [expr [winfo screenwidth .] - \
					[winfo reqwidth $w]]
			if { $x > $right } {
				set x $right
			}
		}
		wm geometry $w +$x+$y
		wm deiconify $w
	} elseif { [winfo ismapped $w] } {
		wm withdraw $w
	} else {
		wm deiconify $w
	}
}

proc create_toplevel { w title } {
	global V
	if [info exists V(visual)] {
		toplevel $w -class $V(class) \
			-visual $V(visual) -colormap $V(colormap)
	} else {
		toplevel $w -class $V(class)
	}
	set title [resource iconPrefix]$title
	wm withdraw $w
	wm transient $w .
	wm title $w $title
	wm iconname $w $title
	bind $w <Enter> "focus $w"
}

#
# Return true if the two strings provide more or less the
# same informational content
#
proc cname_redundant { name cname } {
	set ni [string first @ $name]
	if { $ni < 0 } {
		return 0
	}
	set ci [string first @ $cname]
	if { $ci < 0 } {
		return 0
	}
	if { [string compare \
		[string range $name 0 $ni] \
		[string range $cname 0 $ci]] == 0 } {
		return 1
	}
	return 0
}

proc open_dialog text {
	set w .dialog    
	catch "destroy $w"
	global V
	create_toplevel $w "$V(app) error"

	label $w.label -text "$V(app): $text" -font [mediumfont] \
		-borderwidth 2 -relief groove
	button $w.button -text Dismiss -command "destroy $w" -font [mediumfont]
	pack $w.label -expand 1 -fill x -ipadx 4 -ipady 4
	pack $w.button -pady 4

	wm withdraw $w
	update idletasks
	set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
		- [winfo vrootx [winfo parent $w]]]
	set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
		- [winfo vrooty [winfo parent $w]]]
	wm geom $w +$x+$y
	wm deiconify $w

	bind $w <Enter> "focus $w"
}

proc helpitem { w text } {
	global helpno
	set f [resource helpFont]
	canvas $w.bullet -width 12 -height 12 
	$w.bullet create oval 6 3 12 9 -fill black
	message $w.msg -justify left -anchor w -font $f -width 450 -text $text
	pack $w.bullet -side left -anchor ne -pady 5
	pack $w.msg -side left -expand 1 -fill x -anchor nw
}

proc create_help_window { w items } {
	global V
	create_toplevel $w "$V(class) Help"

	frame $w.frame -borderwidth 0 -relief flat
	set p $w.frame
	set n 0
	foreach m $items {
		set h $w.h$n
		incr n
		frame $h
		helpitem $h $m
		pack $h -expand 1 -fill both
	}
	button $w.frame.ok -text " Dismiss " -borderwidth 2 -relief raised \
		-command "wm withdraw $w" -font [mediumfont] 
	pack $w.frame.ok -pady 6 -padx 6 -anchor e
	pack $w.frame -expand 1 -fill both
}


set current_icon_mark "XXX"

proc mark_icon mark {
	global current_icon_mark
	if {$mark != $current_icon_mark} {
		set current_icon_mark $mark
		global V
		append mark [resource iconPrefix]$V(confname)
		wm iconname . $mark
	}
}

proc mk.entry { w action text } {
	entry $w.entry -relief raised -borderwidth 1 -exportselection 1 \
		-font [resource entryFont]
	global entryTab
	set entryTab($w.entry:action) $action
	set entryTab($w.entry:value) $text
	$w.entry insert 0 $text
}

proc toggleKey w {
        global V entryTab
	if $V(encrypt) {
		crypt_set $entryTab($w.entry:value)
	} else {
		crypt_clear
	}
}

proc disableKeyEditor w {
	$w.button configure -state disabled
	set entryTab($w.entry:value) ""
}

proc enableKeyEditor w {
	$w.button configure -state normal
}

proc updateKey { w key } {
	set w [winfo parent $w]
	set key [string trim $key]
	if { $key == "" } {
		crypt_clear
		disableKeyEditor $w
	} elseif { [crypt_set $key] < 0 } {
		# illegal key - clear it the entry
		disableKeyEditor $w
	} else {
		enableKeyEditor $w
	}
}

proc mk.key w {
	global V
	frame $w.key
	checkbutton $w.key.button -text Key: -relief flat -font [smallfont] \
		-command "toggleKey $w.key" -variable V(encrypt) \
		-disabledforeground gray40
	mk.entry $w.key updateKey [resource sessionKey]
	if !$V(encrypt) {
		$w.key.button configure -state disabled
	}
	pack $w.key.button -side left
	pack $w.key.entry -side left -fill x -expand 1
}

proc updateName { w name } {
	set name [string trim $name]
	if { $name != ""} {
		[srctab local] sdes name $name
		return 0
	}
	return -1
}
