#!/usr/bin/perl -w

use strict;
use Getopt::Std;


# text2c.pl [-n name] [-o outfile] [infile]

# escaping from package Encode::Escape::ASCII
# [YOU Hyun Jo]
# AUTHOR 
# 
# you, <you at cpan dot org> 
# 
# COPYRIGHT AND LICENSE 
# 
# Copyright (C) 2007 by you
# 
# This library is free software; you can redistribute it and/or 
# modify it under the same terms as Perl itself, either Perl 
# version 5.8.8 or, at your option, any later version of Perl 5 
# you may have available.
#

my %ESCAPED = ( 
    "\\" => '\\', 
    "\"" => '"',

    "\a" => 'a',
    "\b" => 'b',
    "\f" => 'f',
    "\n" => 'n', 
    "\r" => 'r', 
    "\t" => 't',
);

sub chr2hex($)
{
    my($c) = @_;
    return sprintf("\\x%02x", ord($c));
}

sub escape($)
{
  $_ = shift;

  s/([\a\b\f\r\n\t\"\\])/\\$ESCAPED{$1}/sg;
  s/\0x0b/\\v/sg; # \v -- not a perl escape.
  s/([\x00-\x1f\x7f-\xff])/chr2hex($1)/gse;
  return $_;

}

my %opts;
my $name;
getopt('o:n:',\%opts);

if ($opts{'o'})
{
  open(STDOUT, ">", $opts{'o'});
  $name = $1 if $opts{'o'} =~ m/^([A-Za-z][A-Za-z0-9_]*)/;
}

if ($opts{'n'})
{
  $name = $opts{'n'};
}

print "const char *$name = \n" if ($name);

while ($_ = <>)
{
  printf("  \"%s\"\n", escape($_));
}
print ";\n" if ($name);

close(STDOUT) if ($opts{'o'});
