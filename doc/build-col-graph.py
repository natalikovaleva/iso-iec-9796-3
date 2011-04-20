#!/usr/bin/python

import sys
import os.path
from math import log as log
from math import ceil as ceil
from subprocess import Popen
from subprocess import PIPE

def usage():
    print("Usage: " + sys.argv[0] + " data.col L_rec" )
    exit(1)

def check_params():
    if len(sys.argv) < 3:
        usage()
    if not os.path.isfile(sys.argv[1]):
        print(sys.argv[1] + " is not a file")
        usage()
    if (int(sys.argv[2]) > 128) or (int(sys.argv[2]) < 1):
        print(sys.argv[2] + " is not proper byte size of L_rec")
        usage()


def __main__():
    check_params()

    datain = file(sys.argv[1], 'r')
    origrd = int(sys.argv[2]) * 8
    logval = origrd
    maxval = 2**logval

    for line in datain:
        columns = line.split(' ')
        current = int(columns[0])
        currlog = int(ceil(log(current, 2)))
        logval  = max(currlog, logval)

    if logval != origrd:
        print("Expected bit length: " + str(origrd))
        print("Real bit length: " + str(logval))

    gnuplot = Popen(["gnuplot"], stdin=PIPE)

    if len(sys.argv) >= 3:
        title = sys.argv[1]
    else:
        title = sys.argv[3]

    title = title + " L_red: " + str(origrd) + " bits"

    gin = gnuplot.stdin
    gin.write("set title \"" + title + "\"\n")
    gin.write("set grid\n")
    gin.write("set terminal svg size 1024,768 dynamic\n")
    gin.write("set output \"" + sys.argv[1] + ".svg\"\n")
    gin.write("plot " + str(2**origrd) + " lt 1, " + str(2**logval) +
              " lt 1, " + "\"" + sys.argv[1] + "\" using 1 lt 3\n")
    gin.write("exit\n")

    Popen.wait(gnuplot)

__main__()
