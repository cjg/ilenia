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

class ProgressiveDownload:
    """
    Download a file from an url, with a progressbar and some usefull
    info: downloaded percentage, downloaded kbytes, filesize in
    kbytes, download rate and elapsed time.
    """
    def __init__(self, url, filename=None, start=True, custom_report=None):
        """
        Keyword arguments:
        url           -- the full url of the file to download
        filename      -- the filename of the downloaded file, if it is not
                         defined it is taken from the url (default None)
        start         -- start download immediately (default True)
        custom_report -- a function that manages the information supplied by
                         ProgressiveDownload, it should accept five
                         arguments: downloaded percentage (int), downloaded
                         kbytes (int), filesize in  kbytes (int), download
                         rate (str) and elapsed time. (default None: use
                         internal progressbar)
        """
        self.url = url
        if filename:
            self.filename=filename
        else:
            self.filename=url[url.rindex("/")+1:]
        if custom_report:
            self.report=custom_report
        else:
            self.report=self._progressbar
        if start:
            self.start_download()

    def start_download(self):
        """
        Start download.
        """
        if self.report == self._progressbar:
            print "Downloading %s" % self.url

        filename = "%s.part" % self.filename

        self.start_time = time.time()

        urllib.urlretrieve(self.url, self.filename, self._reporthook)

        os.rename(filename, self.filename)
        
        if self.report == self._progressbar:
            sys.stdout.write("\n")

    def _reporthook(self, numblocks, blocksize, filesize):
        try:
            downloaded_percentage = min((numblocks*blocksize*100)/filesize, 100)
        except:
            downloaded_percentage = 100
        if numblocks != 0:
            rate = ((numblocks * blocksize)/
                    (time.time()-self.start_time)/1024.0)
            downloaded_kbyte = int(numblocks * blocksize/1024.0)
            filesize_kbyte = int(filesize/1024.0)
            rate = str(rate)
            rate = rate[:rate.index(".")+2]
            elapsed_time = int(time.time()-self.start_time)
            self.report(downloaded_percentage, downloaded_kbyte, filesize_kbyte,
                        rate, elapsed_time)

    def _progressbar(self, downloaded_percentage, downloaded_kbyte,
                    filesize_kbyte, rate, elapsed_time):
        if not sys.stdout.isatty():
            return
        sys.stdout.write("\b"*79)
        bar = "[%s%s] %s%s%% %s%s kb/s   %s/%skb" % (
            "#"*int(downloaded_percentage*(30.0/100)),     
            " "*(30-int(downloaded_percentage*(30.0/100))),
            " "*(3-len(str(downloaded_percentage))),
            str(downloaded_percentage),
            " "*(6-len(rate)),
            str(rate),
            str(downloaded_kbyte),
            str(filesize_kbyte)
            )
            
        seconds = elapsed_time
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
            
if __name__ == "__main__":
    if len(sys.argv)==1:
        print "Usage: progdownload.py url [filename]"
    elif len(sys.argv)==2:
        ProgressiveDownload(sys.argv[1])
    else:
        ProgressiveDownload(sys.argv[1], sys.argv[2])
