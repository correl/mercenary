; Hey, here's some test code
echo TEST Code line match [ 2 = $line ]
; Test comment
echo TEST Code line match [ 4 = $line ]
set name Correl
%first = Correl
%middle = Joseph
%name = %first $&
	%middle Roush
echo Hello %name $+ ! $&
	How $lower(ArE YoU) $+ ?
showversion
alias dostuff {
	; Not very useful, but good for testing the parser!
	var %b = 42
	%b = $calc(%b * 3)
	return %b;
}
alias -l getversion {
	var %version = version $version
	return %version
}
alias -l showversion {
	echo You're testing Mercenary $getversion
	echo TEST Code line match [ 23 = $line ]
}
dostuff
