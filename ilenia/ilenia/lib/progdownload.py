"""
    progdownload.py
    Thu Sep 08 13:02:23 2005
    Copyright 2005 - 2006 Coviello Giuseppe
    immigrant@email.it
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
"""

import sys, urllib, string, time

start_time = time.time()

def reporthook(numblocks, blocksize, filesize):
    global start_time
    try:
        completed = min((numblocks*blocksize*100)/filesize, 100)
    except:
        completed = 100
    if numblocks != 0:
        rate = ((numblocks * blocksize)/(time.time()-start_time)/1024.0)
        rate = str(rate)
        rate = rate[:rate.index(".")+2]
        sys.stdout.write("\b"*79)
        bar = "[%s%s] %s%s%% %s%s kb/s   %s/%skb" % (
            "#"*int(completed*(30.0/100)),     
            " "*(30-int(completed*(30.0/100))),
            " "*(3-len(str(completed))),
            str(completed),
            " "*(6-len(rate)),
            rate,
            str(int(numblocks * blocksize/1024.0)),
            str(int(filesize/1024.0)),
            )
        seconds = int(time.time()-start_time)
        minutes = int(seconds/60)
        hours = int(minutes/60)

        seconds = seconds - minutes * 60
        minutes = minutes - hours * 60
        
        hours = str(hours)
        minutes = str(minutes)
        seconds = str(seconds)
        
        timer = "%s:%s%s:%s%s" % (hours, "0"*(2-len(minutes)), minutes,
                                  "0"*(2-len(seconds)), seconds)
        
        bar = "%s%s%s" % (bar, " "*(72-len(bar)), timer)
        sys.stdout.write(bar)
        sys.stdout.flush()

def progdownload(url, filename):
    print "Downloading %s" % url
    urllib.urlretrieve(url, filename, reporthook)
    sys.stdout.write('\n')

if __name__ == "__main__":
    progdownload("http://commvulitlei.altervista.org/discografia/just_a_pulp_song.zip",
             "just_a_pulp_song.zip")
