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

import sys, urllib, string

def reporthook(numblocks, blocksize, filesize):
    try:
        completed = min((numblocks*blocksize*100)/filesize, 100)
    except:
        completed = 100
    if numblocks != 0:
        sys.stdout.write("\b"*79)
        bar = "[%s%s] %s%s%%" % ("#"*int(completed*(72.0/100)),
                                 " "*(72-int(completed*(72.0/100))),
                                 " "*(3-len(str(completed))),
                                 str(completed))
        sys.stdout.write(bar)
        sys.stdout.flush()

def progdownload(url, filename):
    print "Downloading %s" % url
    urllib.urlretrieve(url, filename,
                       lambda numblocks, blocksize, filesize:
                       reporthook(numblocks, blocksize, filesize))
    sys.stdout.write('\n')

if __name__ == "__main__":
    progdownload("http://commvulitlei.altervista.org/discografia/just_a_pulp_song.zip",
             "just_a_pulp_song.zip")
