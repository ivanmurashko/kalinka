#!/usr/bin/perl -w
#--------------------------------------------------------------------------
#  FILE.........: testhttp.pl
#
#  AUTHOR.......: ipp <ivan.murashko@gmail.com>
#
#  DESCRIPTION..: The test script for Kalinka http server
#
#  COPYRIGHT....: Copyright (c) 2007-2009 Kalinka team
#
#  REVISION.....: $Header: 
#
#  HISTORY......: DATE         COMMENT
#                 -----------  --------------------------------------------
#                 17-Apr-2009  Created - ipp
#--------------------------------------------------------------------------

use strict;
use Thread; 

# Global variables
my $addr="http://localhost:8080/path1";

#--------------------------------------------------------------------------
# Thread body
#--------------------------------------------------------------------------
sub threadBody(){ 
    my ($number) = @_;

    my $cmd = "wget -q -O test$number.flv $addr";
    print "Run '$cmd' at thread num. $number\n";
    
    system($cmd);
}


#--------------------------------------------------------------------------
# Main programm
#--------------------------------------------------------------------------

my $helpPrompt = "
-h or --help - Show this page and exit
-n - Number of threads to be run (default 10)
-a - requested HTTP address (default http://localhost:8080/path1)
";


#---------------------------------------------------------------
# Parses command line args
#
my $i=0;
my $numThread=10;

my @threads;

foreach (@ARGV) {
    if ($_ eq '-h' || $_ eq '--help') {
	print $helpPrompt; exit;
    } elsif ($_ eq '-n') {
	$numThread = $ARGV[$i + 1];
    } elsif ($_ eq '-a') {
	$addr = $ARGV[$i + 1];
    } 
    $i++;
}

for ($i = 0; $i < $numThread; $i++) {
    $threads[$i] = new Thread \&threadBody, $i; 
    sleep 1;
}

print "\nSLEEPING\n";

sleep 100;

print "\nFinished\n";
