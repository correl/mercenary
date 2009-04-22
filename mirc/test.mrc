; Hey, here's some test code
set name Correl
%first = Correl
%middle = Joseph
%name = %first $&
	%middle Roush
echo Hello %name $+ ! $&
	How $lower(ArE YoU) $+ ?
echo You're testing Mercenary v $+ $version
alias dostuff {
	; Not very useful, but good for testing the parser!
	var %b = 42
	%b = $calc(%b * 3)
	return %b;
}
alias -l dosomethingelse {
	; Useless local alias!
	echo -s Busy doing nothing
}
dostuff that nobody will see
dosomethingelse entirely
donothing because this alias doesn't exist