#!/usr/bin/perl
#
#	catply.pl -- cat for .PLY files
#
#	A simple script to concatenate a number of .PLY files that are stored
#	as ASCII text. Input files are read sequentially and the result is
#	written to STDOUT.
#
#	Author: Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>

use strict;
use warnings;

my @files		= @ARGV;

# These are the _global_ totals that will be updated by the parse_header()
# function.
my $num_vertices	= 0;
my $num_faces		= 0;

# Stores faces and vertices for individual files in order to be able to
# calculate the correct offset when concatenating them.
my %faces	= ();
my %vertices	= ();

foreach my $file (@files)
{
	parse_header($file);
}

print STDOUT <<EOF
ply
format ascii 1.0
element vertex $num_vertices
property float x
property float y
property float z
element face $num_faces
property list uchar int vertex_indices
end_header
EOF
;

foreach my $file (@files)
{
	parse_vertices($file);
}

my $offset = 0;
foreach my $file (@files)
{
	parse_connectivity($file, $offset);
	$offset += $vertices{$file};
}

# Read header of .PLY file in order to determine the number of vertices of the
# resulting file.
sub parse_header
{
	my ($file) = @_;
	open(IN, "<", $file) or die "Unable to open input file \"$file\"";

	$faces{$file}		= 0;
	$vertices{$file}	= 0;

	while(my $line = <IN>)
	{
		if($line =~ m/element(\s+)vertex(\s+)(\d+)/i)
		{
			$vertices{$file} = $3;
			$num_vertices += $vertices{$file};
		}
		elsif($line =~ m/element(\s+)face(\s+)(\d+)/i)
		{
			$faces{$file} += $3;
			$num_faces += $faces{$file};
		}

		last if($vertices{$file} > 0 and $faces{$file} > 0);
	}

	close(IN);
}

# Parses the vertices of a .PLY file and prints their coordinates to STDOUT.
# Header information and connectivity information is skipped.
sub parse_vertices
{
	my ($file) = @_;
	open(IN, "<", $file) or die "Unable to open input file \"$file\"";

	while(my $line = <IN>)
	{
		last if($line =~ m/end_header/i);
	}

	# We already know the number of vertices -- do not read any further.
	for(my $i = 0; $i < $vertices{$file}; $i++)
	{
		my $line = <IN>;
		$line =~ m/(\S+)\s+(\S+)\s+(\S+)\s+.*/;
		print $1 . " " . $2 . " " . $3 . "\n";
	}

	close(IN);
}

# Parses the connectivity information of a .PLY file while adjusting the
# offsets accordingly.
sub parse_connectivity
{
	my ($file, $offset) = @_;
	open(IN, "<", $file) or die "Unable to open input file \"$file\"";

	# Skip header
	while(my $line = <IN>)
	{
		last if($line =~ m/end_header/i);
	}

	# Skip vertex information
	for(my $i = 0; $i < $vertices{$file}; $i++)
	{
		my $line = <IN>;
	}

	# Print connectivity information with modified offsets
	for(my $i = 0; $i < $faces{$file}; $i++)
	{ 
		my $line = <IN>;
		my @indices = ($line =~ m/(\d+)/g); # match _all_ indices on the line

		# Modify the offset from the second index on; the first index
		# indicates the number of indices following on the current line
		print $indices[0] . " ";
		for(my $j = 1; $j < $#indices+1; $j++)
		{
			print STDOUT ($indices[$j]+$offset) . " ";
		}
		print STDOUT "\n";
	}

	close(IN);
}


