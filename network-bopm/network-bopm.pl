#!/usr/bin/perl
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
                 HOSTNAME  => 'bopm.scanner',     #Our bopm hostname
                 IP        => '127.0.0.1',        #Our bopm's introduced IP

                 PORT      => 5555,
                 PASS      => 'bopm',             #Our bopm's password
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


my %IRC_FUNCTIONS = (
                     '001'     => \&m_perform,
                     '302'     => \&m_userhostreply, 
                     'PING'    => \&m_ping,
                     'PRIVMSG' => \&m_privmsg,
                     'NICK'    => \&m_nick,
                    );

my %BOPM_FUNCTIONS = (
                     'NICK'    => \&bopm_nick,
                     'USER'    => \&bopm_user,
                     );

#Global Variables
my $IRC_SOCKET;          #IRC Connection
my $IRC_DATA;            #Data read from IRC

my $BOPM_SOCKET;         #Bopm connection
my $BOPM_DATA;           #Data read from BOPM
my $BOPM_WAITING = 0;    #bool, waiting for data?
my $BOPM_AUTH    = 0;    #Has bopm authenticated yet?

my $SELECT = new IO::Select; 

main();

# main
#
# Main initializes the main listening socket
# and handles the main daemon loop.

sub main #()
{
   my $read;

   bopm_listen();

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

      if($handle == $BOPM_SOCKET)
      {
         do_log('BOPM -> BOPM socket has_error');
         bopm_listen();
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

      #Connect to $BOPM_LISTEN
      if(($handle == $BOPM_SOCKET) && $BOPM_WAITING)
      {
         do_log('BOPM -> Got connection');

         $SELECT->remove($BOPM_SOCKET);
         $BOPM_SOCKET =  $BOPM_SOCKET->accept();
         $SELECT->add($BOPM_SOCKET);
         $BOPM_WAITING = 0;
         next
      }

      if($handle == $BOPM_SOCKET)
      {
         bopm_read();
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



# m_perform
#
# Successfull connection (perform)
#

sub m_perform # \@parv, \%source
{
   my $parv = $_[0];
}


# m_privmsg
#
# privmsg to channel OR user
#
# parv[0] source
# parv[1] PRIVMSG
# parv[2] target
# parv[3] message

sub m_privmsg #\@parv, \%source
{
   my $parv = $_[0];
   my $source = $_[1];

   bopm_send(sprintf(':%s!user@host PRIVMSG %s :%s', $$parv[0], $$parv[2], $$parv[3]));
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

   $conn = sprintf(':%s NOTICE %s :*** Notice -- Client connecting: %s (%s@%s) [%s] {class}',
                   $IRC{HOST}, 
                   $BOPM{NICK},
                   $$parv[$NICKFORMAT{NICK}],
                   $$parv[$NICKFORMAT{USERNAME}],
                   $$parv[$NICKFORMAT{HOSTNAME}],
                   inet_ntoa(pack("N", $$parv[$NICKFORMAT{IP}])),
                  );
   #send hybrid connection notice
   bopm_send($conn);
}

# m_privmsg
#
# privmsg to channel OR user
#
# parv[0] source
# parv[1] 302
# parv[2] nick
# parv[3] reply

sub m_userhostreply
{
   my $parv = $_[0];

   bopm_send(sprintf(':%s 302 %s %s', $$parv[0], $$parv[2], $$parv[3]));
}

########################################## BOPM #####################################################

sub bopm_listen
{

   if($BOPM_SOCKET)
   {
      bopm_close();
   }

   $BOPM_SOCKET = new IO::Socket::INET( Proto     => "tcp",
                                        Listen    => 1,
                                        Reuse     => 1,
                                        LocalPort => $BOPM{PORT}
                                      );  
   $SELECT->add($BOPM_SOCKET);

   if(!$BOPM_SOCKET)
   {
      do_log(sprintf('BOPM -> Could not bind to port %d', $BOPM{PORT}));
      exit;
   }

   $BOPM_WAITING = 1;
   $BOPM_AUTH    = 0;
}

sub bopm_close
{
   $SELECT->remove($BOPM_SOCKET);
   close($BOPM_SOCKET);

   irc_send(sprintf(':%s QUIT :Dead', $BOPM{NICK}))
}


# bopm_read
#
# Read data from bopm
#

sub bopm_read #()
{
   my $data;
   my $pos;
   my $line;

   if(sysread($BOPM_SOCKET, $data, 512) == 0)
   {
      do_log('BOPM -> Read error from bopm');
      bopm_listen();
      return;
   }

   $data = $BOPM_DATA . $data;

   while(($pos = index($data, "\n")) != -1)
   {
      $line = substr($data, 0, $pos + 1, "");
      chomp $line;
      bopm_parse($line);
   }
   $BOPM_DATA = $data;
}


sub bopm_parse
{
   my $line = $_[0];

   my @parv;
   my $command;
   my $message;
   my %source;

   $line =~ s/[\r\n]+//g;

   do_log(sprintf('BOPM READ -> %s', $line));


   if(index($line, ':') != -1)
   {
      @parv = split(/\s+/, substr($line, 0, index($line, ':')));
      $message = substr($line, index($line, ':') + 1, length($line));
      push @parv, $message;
   }
   else
   {
      @parv = split(/\s+/, $line);
   }
   #check if this is an auth
   if($parv[0] eq 'PASS')
   {
      if($parv[1] eq $BOPM{PASS})
      {
         $BOPM_AUTH = 1;
      }
      else
      {
         bopm_listen();
      }
   }

   if(exists($BOPM_FUNCTIONS{$parv[0]}))
   {
      $BOPM_FUNCTIONS{$parv[0]}(\@parv);
   }
   else
   {
      return if(!$BOPM_AUTH);
      irc_send(sprintf(':%s %s', $BOPM{NICK}, $line));
   } 
}

# bopm_send
#
# Send data to bopm

sub bopm_send #($data)
{
   my $data = $_[0];

   #don't send any data to listening sockets or bopms that havn't PASSed yet
   return if($BOPM_WAITING || !$BOPM_AUTH);

   do_log(sprintf('BOPM SEND -> %s', $data));

   $data .= "\n\n";

   if(!send($BOPM_SOCKET, $data, 0))
   {
      do_log(sprintf('BOPM -> send() error: %s', $!));
      bopm_listen();
   }
}

sub bopm_introduce
{
   my $nick;

   #Form NICK line
   $nick = $PROTOCOL{NICK};
  
   $nick =~ s/\{nick\}/$BOPM{NICK}/g;
   $nick =~ s/\{username\}/$BOPM{USERNAME}/g;
   $nick =~ s/\{hostname\}/$BOPM{HOSTNAME}/g;
   $nick =~ s/\{server\}/$IRC{NAME}/g;
   $nick =~ s/\{ip\}/69/g;
   $nick =~ s/\{realname\}/$BOPM{REALNAME}/g;
   $nick =~ s/\{ts\}/1/g;

   irc_send(sprintf('NICK %s', $nick));
}


sub bopm_nick
{
   my $parv = $_[0];
   $BOPM{NICK} = $$parv[1];
}

sub bopm_user
{
   my $parv = $_[0];

   $BOPM{USERNAME} = $$parv[1];
   $BOPM{REALNAME} = $$parv[4];
   bopm_introduce();   
   bopm_send("001 Welcome to BOPM!");
}
