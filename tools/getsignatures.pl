#!/usr/bin/perl

my $content =`curl -s https://en.wikipedia.org/wiki/List_of_file_signatures`;


if ($content =~ /<table class="wikitable sortable">(.*?)<\/table>/gis) {
   $content = $1;
}
else {
  print "Problem getting signatures from wikipedia!\n";
}


my @rows = split "<tr>",$content;

foreach  my $row (@rows) {


  my ($extension,$description,$offset,$iso,$sig) = split "</td>",$row;

  $extension =~ s/\R//g;
  $sig =~ s/\R//g;
  $description =~ s/\R//g;

#  <td>ilbm

  if ( $extension =~ /^<td>([a-zA-Z0-9]+)/g ) {
   $extension = $1;
  }

  next unless ($extension=~/^\w+$/ );

  if ( $sig =~ /<td><pre>(.*?)<\/pre>/) {
    $sig = $1;
    $sig =~ s/\s//g;
  }

  my @sigbytes = ($sig=~ m/[0-9A-F]{2}/g);

  if ($description=~ /<td><a.*?>(.*?)<\/a>/) {
    $description=$1;
  }

  $description =~ s/(<.*?>|\[.*?\])//g;

  print "$extension|$description|@sigbytes\n";

}


