from graphmath import shortestpathmatrix, labels, adjacencymatrix
from graph import *
from msvcrt import kbhit, getch
import os.path
import os
import psyco
import subprocess
import pickle
import sys
from copy import deepcopy
partition=2000
def findnumber():
    num=1
    with open('C:\\My Dropbox\\GPGlabelings\\boo.txt','r+') as boo:
        num=int(boo.read())
        boo.seek(0)
        boo.write(str(num+1))
    return num
def readshortcode(f):
        yield 24
        yield 3
        s=f.read(1)
        while s:
            yield (ord(s))
            s=f.read(1)
##    def readshortcode(f):
##        k=3
##        n=24
##        s=f.read(1)
##        nums=[n, k]
##        while s:
##            nums.append(ord(s))
##            s=f.read(1)
##        return nums
def setupfile(num):
    if int(num)>partition:
        return False
    p=subprocess.Popen(['C:\\My Dropbox\\GPGlabelings\\graph program 26\\genreg.exe','24','3', '-s','-m',str(num),str(partition)])
    p.wait()
    while os.path.isfile('C:\\My Dropbox\\GPGlabelings\\graph program 26\\24_3_3-'+str(num)+'.scd'):
        pass
    return os.path.isfile('C:\\My Dropbox\\GPGlabelings\\graph program 26\\24_3_3#'+str(num)+'.scd')

def main(originalnum,num, superawesomecounter=0):
    stop=False
    force_quit=False
    print originalnum, num, superawesomecounter
    stop=not setupfile(num)
    if not stop:
        g=Graph(labelingConstraints=[2,1])
        with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\24_3_3#'+str(num)+'.scd','rb') as f:
            nums=readshortcode(f)
            mistercounter=0
            print '0000000',
            for cgraph in g.loadshortcode(nums):
                mistercounter+=1
                superawesomecounter+=1
##                cgraph.quickdraw_to_file()
##                f=file('boo.graph', 'wb')
##                pickle.dump(cgraph,f)
##                f.close()
##                print "autolabeling start:",
                labels=auto_label(cgraph.get_vertices(), cgraph.edges)
##                print '\b\b\b\b\b\bend:',
##                for lcv in xrange(len("autolabeling start:")):
##                    sys.stdout.write('\b')
                if labels!=False:
                    minlabel=5000;
                    maxlabel=-1;
                    for label in labels:
                        if label!="NULL":
                            if label>maxlabel:
                                maxlabel=label
                            if label<minlabel:
                                minlabel=label
                    lnum=maxlabel-minlabel
                    numholes=0
                    for hole in xrange(minlabel+1,maxlabel):
                        if hole not in labels:
                            numholes+=1
                    if lnum>=6 and numholes>=3:
                        adj=str(adjacencymatrix(cgraph.get_vertices(),cgraph.edges))
                        with open('C:\\My Dropbox\\GPGlabelings\\'+str(originalnum)+'found.txt','a') as res:            
                            res.write(adj)
                        print "WOOOOOOOOOOOOOO:"
                        print adj
                        print num,mistercounter
                        os.system('pause')
                    #print "lambda:"+str(lnum)+" holes:"+str(numholes)
                    sys.stdout.write("\b\b\b\b\b\b\b%07d" % mistercounter)
                if kbhit():
                    print '.',
                    if getch()=='q':
                        force_quit=True
                        print "yo imma be the one to end this. to be continued."
            print "completed "+str(mistercounter)+" graphs for "+str(num)
        if os.path.isfile('C:\\My Dropbox\\GPGlabelings\\graph program 26\\24_3_3#'+str(num)+'.scd'):
            os.remove('C:\\My Dropbox\\GPGlabelings\\graph program 26\\24_3_3#'+str(num)+'.scd')
    if not force_quit and kbhit():
        print '.',
        if getch()=='q':
            force_quit=True
            print "yo imma be the one to end this. to be continued."
    if force_quit:
        print "COME BACK NEXT TIME!"
        print superawesomecounter
        os.system('pause')
        sys.exit()
    if num<partition:
        if stop:
            print "no graphs for "+str(num)
##        num=findnumber()
        print "starting next"
##        main(originalnum,num, superawesomecounter)
        return superawesomecounter
    else:
        print "DONE"
        print originalnum, num, partition, superawesomecounter
        os.system('pause')
        sys.exit()

def auto_label(verts, edges):
    size = 24
    spm = shortestpathmatrix(verts,edges)
    labs = labels(verts)

    # TODO: calculate lower bound of labeling, and test to see if it is better than this person's guess--this may eliminate really long wait
    # TODO: for reals, change from index to actual number
    
    initvertex = 0
    # TODO: is there an optimum place to start?
    
    memory = [None] # memory is a list so that changes made to it's contents will carry out of the methods

    # There was a sketchy error happening if you don't explicitly pass "set=[]".  It just builds the dsets together from each auto-labeling...
    
    for label in xrange(3):
       res = try_label(spm, initvertex, label, labs, memory)
       if res != False:
           return res
    if memory[0] != None:
        return memory[0][1]  # when minimizing
    return False
    
def minimize_holes(labels, memory):
    labels_used = [0] * 7
    for entry in labels:
        labels_used[entry] = 1
    number_of_labels = sum(labels_used)
    if number_of_labels == 7:
        return labels
    else:
        if memory[0] == None or number_of_labels > memory[0][0]:
            memory[0] = [number_of_labels,deepcopy(labels)]
        return False

def try_label (spm, vertex, label, labels, memory = [None]):
    labels[vertex] = label
    if not check_vertex_labeling(spm[vertex], vertex, labels):
        return False

    if (len(labels)-1 == vertex):
        return minimize_holes(labels, memory)

    if labels[vertex+1] != 'NULL':
        
        if try_label (spm, vertex+1, labels[vertex+1], labels, memory) != False:
            return labels

    else:
        for l in xrange(7): 
#        for l in range(maxlabel+1):   # TODO: improve efficiency here by selecting which colors we can use?  or only useful for going through list in right order?  won't speed up that much...
            if try_label(spm, vertex+1, l, labels, memory) != False:
                return labels
        labels[vertex+1] = 'NULL' # reset vertex labeling so that it is not triggered as a pre-configured one next time through the algorithm

    return False

def check_vertex_labeling (sprow, vertex, labels):
    for i in range(vertex):
        dist = sprow[i]
        if 0 < dist <= 2: # if these two vertices are close enough to matter...
            labdist = abs(labels[i]-labels[vertex]) # the "labeling distance"; separation of the labels
            if labdist < [2,1][dist-1]: #checks the coloring constraints to make sure it works
                return False
    return True

psyco.full()
originalnum=findnumber()
num=originalnum
main(originalnum,num)
supercounter=0
while 1:
    num=findnumber()
    supercounter=main(originalnum,num, supercounter)

