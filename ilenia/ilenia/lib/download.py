"""
    download.py
    Thu Sep 08 11:26:50 2005
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

import urllib, os.path, string, md5
from progdownload import ProgressiveDownload

def download(pkg, repos):
    path, filename = get_url(pkg, repos)
    md5sum = get_md5sum(pkg)
    if filename and md5sum:
        ProgressiveDownload("%s/%s" % (path, filename),
                            os.path.join("/tmp", filename))
        if md5sum == do_md5sum(os.path.join("/tmp", filename)):
            return os.path.join("/tmp", filename)
    return None

def get_url(pkg, repos):
    f_io = file(os.path.join(os.path.sep, "var", "lib", "ilenia", pkg["repo"],
                             "FILELIST.TXT"))
    lines = f_io.readlines()
    for line in lines:
        try:
            version = pkg["version"]
        except:
            version = pkg["r_version"]
        if pkg["name"]+"-"+version in line:
            url = line[line.index("./")+1:]
            url = "%s/%s" % (repos.get_url(pkg["repo"]), url)
            url = url.replace("//", "/")
            url = url.replace(":/", "://")
            url = url.replace("\n", "")
            path, filename = url.rsplit("/", 1)
            return path, filename
    return None

def get_md5sum(pkg):
    f_io = file(os.path.join(pkg["repo"], "CHECKSUMS.md5"))
    lines = f_io.readlines()
    for line in lines:
        try:
            version = pkg["version"]
        except:
            version = pkg["r_version"]
        if pkg["name"]+"-"+version in line:
            md5sum = line[:line.index(" ")].strip()
            return md5sum
    return None

def do_md5sum(file_name):
    f_io = file(file_name)
    m = md5.new()
    while True:
        d = f_io.read(8096)
        if not d:
            break
        m.update(d)
    checksum = m.hexdigest()
    return checksum
