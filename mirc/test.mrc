; Hey, here's some test code
echo TEST Code line match [ 2 = $line ]
; Test comment
echo TEST Code line match [ 4 = $line ]
%first = Correl
set %last Roush


%middle = Joseph
%name = %first $&
	%middle %last
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
	echo TEST Code line match [ 25 = $line ]
}
on *:TEXT:something:echo hey I got a message

on *:QUIT:{
	echo Quitting!
}
dostuff
