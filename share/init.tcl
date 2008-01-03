# $Id$

# internal proc to make help function available to TCL procs
proc __help { args } {
	set command [lindex $args 0]
	if [info exists ::__help_proc($command)] {
		return [eval $::__help_proc($command) $args]
	} elseif [info exists ::__help_text($command)] {
		return $::__help_text($command)
	} elseif {[info commands $command] ne ""} {
		error "No help for command: $command"
	} else {
		error "Unknown command: $command"
	}
}
proc set_help_text { command help } {
	set ::__help_text($command) $help
}

# internal proc to make tabcompletion available to TCL procs
proc __tabcompletion { args } {
	set command [lindex $args 0]
	set result ""
	if [info exists ::__tabcompletion_proc_sensitive($command)] {
		set result [eval $::__tabcompletion_proc_sensitive($command) $args]
		lappend result true
	} elseif [info exists ::__tabcompletion_proc_insensitive($command)] {
		set result [eval $::__tabcompletion_proc_insensitive($command) $args]
		lappend result false
	}
	return $result
}
proc set_tabcompletion_proc { command proc {case_sensitive true} } {
	if $case_sensitive {
		set ::__tabcompletion_proc_sensitive($command) $proc
	} else {
		set ::__tabcompletion_proc_insensitive($command) $proc
	}
}

set_help_text data_file \
"Resolve data file. First try user directory, if the file doesn't exist
there try the system directory."
proc data_file { file } {
	global env
	set user_file $env(OPENMSX_USER_DATA)/$file
	if [file exists $user_file] { return $user_file }
	return $env(OPENMSX_SYSTEM_DATA)/$file
}

# Source all .tcl files in user and system scripts directory. Prefer
# the version in the user directory in case a script exists in both
set user_scripts [glob -dir $env(OPENMSX_USER_DATA)/scripts -tails -nocomplain *.tcl]
set system_scripts [glob -dir $env(OPENMSX_SYSTEM_DATA)/scripts -tails -nocomplain *.tcl]
foreach script [lsort -unique [concat $user_scripts $system_scripts]] {
	set script [data_file scripts/$script]
	if {[catch {source $script}]} {
		puts stderr "Error while executing $script\n$errorInfo"
	}
}
