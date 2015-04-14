use warnings;
use strict;

sub dumpArr
{
    my ($prefix, @arrayParam) = @_;

    print "$prefix";
    foreach my $value (@arrayParam) {
        print $value." ";
    }
    print "\n";
}

sub dumpDict 
{
    my ($prefix, %dictParam) = @_;
    
    while (my ($key, $value) = each %dictParam) {
        if (ref($value) eq "HASH") {
            print $prefix."$key => {\n";
            dumpDict("  ".$prefix, %{$value});
            print $prefix."}\n";
        } elsif (ref($value) eq "ARRAY") { 
            print $prefix."$key => {\n";
            dumpArr("  ".$prefix, @{$value});
            print $prefix."}\n";
        } else {
            print $prefix."$key => $value\n";
        }
    }
}

my @casArr = ("1", "2", "3");
my %casDict1 = ("g" => 1, "b" => \@casArr);
my %casDict = ("g" => 1, "b" => \%casDict1);
my %tempDict = ( "good" => 1, "bad" => 0, "cascade" => \%casDict);
dumpDict("  ", %tempDict);
