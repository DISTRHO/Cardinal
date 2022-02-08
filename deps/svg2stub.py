#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# DISTRHO Plugin Framework (DPF)
# Copyright (C) 2012-2022 Filipe Coelho <falktx@falktx.com>
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with
# or without fee is hereby granted, provided that the above copyright notice and this
# permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
# TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
# NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import os
import sys
import xml.etree.ElementTree as ET

# -----------------------------------------------------

def svg2stub(filename_in, filename_out):
    node = ET.parse(filename_in).getroot()

    with open(filename_out, 'w') as fh:
        fh.write('<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n')
        fh.write('<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"')
        for key, value in node.items():
            if '{' in key:
                continue
            fh.write(' %s="%s"' % (key, value))
        fh.write('></svg>')

# -----------------------------------------------------

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: %s <in-filename> <out-filename>" % sys.argv[0])
        quit()

    filename_in = sys.argv[1]
    filename_out = sys.argv[2]

    if not os.path.exists(filename_in):
        print("File '%s' does not exist" % filename_in)
        quit()

    # dump code now
    svg2stub(filename_in, filename_out)
