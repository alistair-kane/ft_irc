
#!/bin/sh
# Copyright 2014 Vivien Didelot <vivien@didelot.org>
# Licensed under the terms of the GNU GPL v3, or any later version.

PASS=test
NICK=irccat42
SERVER=127.0.0.1
PORT=4242
CHAN="#test"

#  USER guest 0 * :Ronnie Reagan   ; User registering themselves with a
#                                    username of "guest" and real name
#                                    "Ronnie Reagan".

{
  # join channel and say hi
  cat << IRC
PASS $PASS
NICK $NICK
USER irccat 8 x : irccat
JOIN $CHAN
PRIVMSG $CHAN :Greetings!
IRC

  # forward messages from STDIN to the chan, indefinitely
  while read line ; do
    echo "$line" | sed "s/^/PRIVMSG $CHAN :/"
  done

  # close connection
  echo QUIT
} | nc $SERVER $PORT | while read line ; do
  case "$line" in
    *PRIVMSG\ $CHAN\ :*) echo "$line" | cut -d: -f3- ;;
    #*) echo "[IGNORE] $line" >&2 ;;
  esac
done