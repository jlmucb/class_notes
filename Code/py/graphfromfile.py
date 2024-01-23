#!/usr/bin/python
import sys, getopt
from matplotlib.pyplot import plot, show
import matplotlib.pyplot as plt

class Coords:
  def __init__(self, x, y):
    self.x = x
    self.y = y

def getCoords( line ):
  a = ''
  b = ''
  l = line.strip()
  for i in range(0, len(l)):
    if l[i] == ' ':
      break
    a = a + l[i]
  if i == 0:
    return ""
  for j in range(i + 1, len(l)):
    if l[j] != ' ':
      break
  for i in range(j,len(l)):
    if l[i] == ' ':
      break
    b = b + l[i]
  c = Coords(float(a), float(b))
  return c

def printCoordList( lst ):
  c = Coords(0.0, 0.0)
  return c

def stripComment( l ):
  for i in range(0, len(l)):
    if l[i] == '#':
      return l[0:i].strip()
  return l.strip()


def main(argv=sys.argv):
  inputfile = sys.argv[1]
  outputfile = sys.argv[2]

  f = open(inputfile)
  l = []
  for line in f:
    l.append(line)
  f.close()

  title= ''
  xlabel=''
  ylabel=''
  for i in range(0, len(l)):
    # print 'line ', i, ' ', l[i].rstrip()
    nl = stripComment(l[i])
    if len(nl) <= 0:
      continue
    if title == '':
      title = nl
    elif xlabel == '':
      xlabel = nl
    elif ylabel == '':
      ylabel = nl
      break
  print 'title: ', title, ', xlabel: ', xlabel, ', ylabel: ', ylabel

  coords = []
  for j in range(i + 1, len(l)):
    nl = stripComment(l[j])
    t = getCoords(nl)
    if t == '':
      continue
    coords.append(t)

  xlist = []
  ylist = []
  for i in range(0, len(coords)):
    xlist.append(coords[i].x)
    ylist.append(coords[i].y)

  top = ylist[0]
  bottom = ylist[0]
  left = xlist[0]
  right = xlist[0]
  for i in range(0, len(coords)):
    if ylist[i] > top:
      top = ylist[i]
    if ylist[i] < bottom:
      bottom = ylist[i]
    if xlist[i] > right:
      right = xlist[i]
    if xlist[i] < left:
      left = xlist[i]

  left = left - .05 * (right - left)
  right = right + .05 * (right - left)
  bottom = bottom - .05 * (top - bottom)
  top = top + .05 * (top - bottom)

  print 'left: ', left, ', right: ', right
  print 'bottom: ', bottom, ', top: ', top
  print len(coords), ' points'
  plt.title(title)
  plt.xlabel(xlabel)
  plt.ylabel(ylabel)
  plt.axes().set_aspect('equal')
  plt.axis([left, right, bottom, top])
  plt.plot(xlist, ylist)
  plt.savefig(outputfile)
  # plt.show()

main(sys.argv)


  
