#!/usr/bin/env perl

$retcode = 0;

$clang_format_bin = $ARGV[0];

foreach my $file(@ARGV[1.. $#ARGV]) {
    $output = `$clang_format_bin -style=file -output-replacements-xml $file`;
    if(file_formated_ok($output) == 0) {
        print "$file Is badly formated\n";
        $retcode = -1
    }
}

exit $retcode;

sub file_formated_ok
{
    $clang_format_output = $_[0];
    if(index($clang_format_output, "replacement offset") != -1) {
        return 0;
    }
    return 1;
}