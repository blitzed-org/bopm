#/usr/bin/perl
#Copyright (C) 2002  Erik Fears
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#
#      Foundation, Inc.
#      59 Temple Place - Suite 330
#      Boston, MA  02111-1307, USA.


 
use strict;
use Socket;
use IO::Select;
use IO::Socket::INET;



#Options
my %BOPM    = (
                 NICK  => 'bopm',            #Our bopm hostname
              );


my %IRC =     (
                  NAME     => 'bopm.blitzed.org', #Our server name
                  HOST     => 'localhost',        #Remote server we're linking to
                  PORT     => '6667',             #Port of remote server we're linking to
                  PASS     => 'link',             #Link password from C/N
              );


#Bahamut
my %PROTOCOL = (
                  NICK         => '{nick} 1 {ts} +o {username} {hostname} {server} 0 {ip} :{realname}',
                  CAPAB        => 'TS3 NOQUIT SSJOIN BURST UNCONNECT NICKIP TSMODE',
               );

my %NICKFORMAT = (
                    NICK     => 1,
                    USERNAME => 5,
                    HOSTNAME => 6,
                    SERVER   => 7,
                    IP       => 9,
                    REALNAME => 10,
                 );

###### END CONFIGURATION ######

my %IRC_FUNCTIONS = (
                     'PING'    => \&m_ping,
                     'NICK'    => \&m_nick,
                    );


#Global Variables
my $IRC_SOCKET;          #IRC Connection
my $IRC_DATA;            #Data read from IRC

my $SELECT = new IO::Select; 

main();

# main
#
# Main initializes the main listening socket
# and handles the main daemon loop.

sub main #()
{
   my $read;

   irc_init();
   irc_connect();

   while(true)
   {
      irc_cycle();
   }

}


# do_log
#
# Log!
 
sub do_log #($data)
{
   my $data = $_[0];
   print STDOUT "[" . scalar localtime() . "] " . $data . "\n";
}


# init
#
# Initialize IRC socket
#

sub irc_init #()
{
   if(!socket($IRC_SOCKET, PF_INET, SOCK_STREAM, getprotobyname('tcp')))
   {
      do_log(sprintf('IRC -> Error initializing IRC socket: %s', $!));
      die;
   }

   $SELECT->add($$IRC_SOCKET);
}


# irc_cycle
#
# Run select() on the IRC client and bopm connections to
# check for new data. Reconnect if needed.

sub irc_cycle #()
{
   my $handle;
   my $dcc;
   my @ready;
   my @errored;

   #do error events
   @errored = $SELECT->has_error(0);
   
   foreach $handle (@errored)
   {
      if($handle == $$IRC_SOCKET)
      {
         do_log('IRC -> IRC socket has_error');
         irc_reconnect();
         next;
      }
   }


   #do read events
   @ready = $SELECT->can_read(.1);
  
   foreach $handle (@ready)
   {
      #Data from IRC server
      if($handle == $$IRC_SOCKET)
      {
         irc_read();
         next;
      }
   }
}

# irc_connect
#
# Connect to IRC and send registration data
#

sub irc_connect #()
{
   if(!connect($$IRC_SOCKET, sockaddr_in($IRC{PORT}, inet_aton($IRC{HOST}))))
   {
      do_log(sprintf('IRC -> Error connecting to IRC host: %s', $!));
   }

   irc_send(sprintf('PASS %s', $IRC{PASS}));
   irc_send(sprintf('CAPAB %s',$PROTOCOL{CAPAB}));
   irc_send(sprintf('SERVER %s', $IRC{NAME}));
}



# irc_reconnect
#
# Reconnct to IRC server
#

sub irc_reconnect #()
{

   do_log('IRC -> Reconnecting to server');

   close($$IRC_SOCKET);
   $SELECT->remove($$IRC_SOCKET);

   sleep(30);

   if(!socket($IRC_SOCKET, PF_INET, SOCK_STREAM, getprotobyname('tcp')))
   {
      do_log(sprintf('IRC -> Error initializing IRC socket: %s', $!));
      die;
   }

   irc_connect();
}



# irc_send
#
# Send data to IRC server
#
# $_[0] IRC Data to send

sub irc_send #($data)
{
   my $data = $_[0];


   do_log(sprintf('IRC SEND -> %s', $data));

   $data .= "\n\n";

   if(!send($$IRC_SOCKET, $data, 0))
   {
      do_log(sprintf('IRC -> send() error: %s', $!));
      irc_reconnect();
   }
}


# irc_read
#
# Read data from IRC server
#

sub irc_read #()
{
   my $data;
   my $pos;
   my $line;

   if(sysread($$IRC_SOCKET, $data, 512) == 0)
   {
      do_log('IRC -> Read error from server');
      irc_reconnect();
      return;
   }

   $data = $IRC_DATA . $data;

   while(($pos = index($data, "\n")) != -1)
   {
      $line = substr($data, 0, $pos + 1, "");
      chomp $line;
      irc_parse($line);
   }
   $IRC_DATA = $data;
}



sub irc_parse #($line)
{
   my $line = $_[0];
  
   my @parv;
   my $command;
   my $message;
   my %source;

   chomp $line;

   do_log(sprintf('IRC READ -> %s', $line));

   if(index($line, ':', 1) != -1)
   {
      @parv = split(/\s+/, substr($line, 0, index($line, ':', 1)));
      $message = substr($line, index($line, ':', 1) + 1, length($line)); 
   }
   else
   {
      @parv = split(/\s+/, $line);
   }

   push @parv, $message;

   if($parv[0] =~ /:/)
   {
      $parv[0] = substr($parv[0], 1, length($parv[0]));
   }
   else
   {
      unshift @parv, $IRC{HOST};
   }

   #parse the nick!user@host if it exists
   if($parv[0] =~ /([^!]+)!([^@]+)@(.*)/)
   {
      $source{nickname} = $1;
      $source{username} = $2;
      $source{hostname} = $3;
      $source{is_user}     = 1;
   }
   else { $source{is_user}   = 0; }
  
   
   if(exists($IRC_FUNCTIONS{$parv[1]}))
   {
      $IRC_FUNCTIONS{$parv[1]}(\@parv, \%source);
   }
}


# m_ping
#
# PING from server. 
#
# parv[0] = SOURCE
# parv[1] = PING
# parv[2] = PACKAGE
#

sub m_ping # \@parv, \%source
{
   my $parv = $_[0];
   irc_send(sprintf('PONG :%s', $$parv[2]));  
} 

# m_nick
#

sub m_nick
{
   my $parv = $_[0];
   my $conn;

   if(@$parv <= 3)
   {
      return;
   }
   shift @$parv;

   $conn = sprintf('*** Notice -- Client connecting: %s (%s@%s) [%s] {class}',
                   $$parv[$NICKFORMAT{NICK}],
                   $$parv[$NICKFORMAT{USERNAME}],
                   $$parv[$NICKFORMAT{HOSTNAME}],
                   inet_ntoa(pack("N", $$parv[$NICKFORMAT{IP}])),
                  );

   #send hybrid connection notice
   irc_send(sprintf(':%s NOTICE %s :%s', $IRC{NAME}, $BOPM{NICK}, $conn)); 
}