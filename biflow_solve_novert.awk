awk '{ 
if ($1 == "n") {
	print "n", 2*$2, $3; print "n", 2*$2 - 1, $3;
} else if ($1 == "p") {
	print "p min", $3 * 2, $4 * 2;
} else if ($1 == "a") {
	if (($2 > 0) && ( $3 > 0)) {
		print "a", 2*$2 - 1, 2*$3 - 1, $4, $5, $6;
		print "a", 2*$3, 2*$2, $4, $5, $6;
	} else if (($2 < 0) && ( $3 < 0)) {
		print "a", -2*$3 - 1, -2*$2 - 1, $4, $5, $6;
		print "a", -2*$2, -2*$3, $4, $5, $6;
	} else if (($2 > 0) && ( $3 < 0)) {
		print "a", 2*$2 - 1, -2*$3 , $4, $5, $6;
		print "a", -2*$3 - 1, 2*$2 , $4, $5, $6;
	} else if (($2 < 0) && ( $3 > 0)) {
		print "a", -2*$2, 2*$3 - 1 , $4, $5, $6;
		print "a", 2*$3, -2*$2 - 1 , $4, $5, $6;
	}
} else {
	print $0;
}
}' $1 | ~/asm/cs2-4.3/cs2.exe | perl -e '{
	$flowDefFile = $ARGV[0];

	# read in flow results into hash
	while ($line = <STDIN>) {
		if ($line =~ /f\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)/) {
			$from = $1;
			$to = $2;
			$flow = $3;

			#unmonotonize node indicises
			$fromb = int(($from + 1)/ 2);
			$tob = int(($to + 1)/ 2);
			if (($from % 2) == 0) {
				$fromb *= -1;
			}
			if (($to % 2) == 0) {
				$tob *= -1;
			}

			$key = $fromb . "," . $tob;
			if (defined($hash{$key})) { #this is for the special case of self-loops (direction switching)
				$hash{$key} += $flow / 2;
			} else {
				$hash{$key} = $flow / 2;
			}
		}
	}

	#print "Hash Contents: \n"; print "$_\t$hash{$_}\n" for (sort keys %hash); print "done\n\n";

	# read in flow definition file again and append it with the hash results
	open(FLOWDEF, $flowDefFile) || die("Could not open file!");
	while ($line = <FLOWDEF>) {
		if ($line =~ /^a\s+(-*[0-9]+)\s+(-*[0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)/) {
			if ($1 != -$2) { #if not a self-loop
				$flow = $hash{"$1,$2"} + $hash{-1*$2 . "," . -1*$1};
			} else { #direction switching self-loop 
				$flow = $hash{"$1,$2"};
			}
			print "a $1 $2 $3 $4 $5 $flow\n";
		} else {
			print $line;
		}
	}
}' $1

