"""
    prettyprint.py
    Tue Sep 20 16:10:43 2005
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

def prettyprint(row, rows_width=[]):
    if len(rows_width)<1:
        row_width = (79-(len(row)-1))/len(row)
        for cell in row:
            rows_width.append(row_width)

    formatted_row = ""

    for cell in row:
        cell_width = rows_width[row.index(cell)]
        if len(cell)>cell_width:
            cell = cell[0:cell_width-1]
        formatted_row = "%s%s%s " % (formatted_row, cell,
                                     " "*(cell_width-len(cell)))
    print formatted_row
