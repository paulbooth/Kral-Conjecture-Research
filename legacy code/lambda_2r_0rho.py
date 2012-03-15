from graphmath import shortestpathmatrix, labels, adjacencymatrix, auto_label, try_label
from graph import *
from msvcrt import kbhit, getch
import os.path
import os
import psyco
import subprocess
import pickle
import sys
from copy import deepcopy

def recordholegraphhelper(num, adj, lnum, numholes):
    try:
        print lnum,":",numholes
        with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+str(num)+'vert_atleastonehole.txt','a') as res:            
            res.write(adj)
        with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+str(num)+'vert_atleastonehole_aux.txt','a') as res:            
            str = "" +adj + " upperboundlambda:" + str(lnum) + " holes:"+str(numholes)
            res.write(str)
        return 0
    except:
        print "problem writing to file!"
        print "trying again!"
        return 1

def recordholegraph(num, adj, lnum, numholes):
    while (recordholegraphhelper(num, adj, lnum, numholes)):
        pass
    
def recordlargelambdagraph(num, adj, lnum, numholes):
    while (recordlargelambdagraphhelper(num, adj, lnum, numholes)):
        pass
    
def recordlargelambdagraphhelper(num, adj, lnum, numholes):
    try:
        print lnum,":",numholes
        with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+str(num)+'vert_lambdagreaterthan8.txt','a') as res:            
            res.write(adj)
        
        with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+str(num)+'vert_lambdagreaterthan8_aux.txt','a') as res:
            string = "" +adj + " upperboundlambda:" + str(lnum) + " holes:"+str(numholes)
            res.write(string)
        return 0
    except:
        print "" +adj + " upperboundlambda:" + str(lnum) + " holes:"+str(numholes)
        print "problem writing to file!"
        print "trying again!"
        return 1
def readshortcode(numverts, f):
    yield numverts
    yield 4
    s=f.read(1)
    while s:
        yield (ord(s))
        s=f.read(1)

def main(num=4, superawesomecounter=0):
    stop=False
    force_quit=False
    print num, superawesomecounter
    g=Graph(labelingConstraints=[2,1])
    with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+'%0*d' % (2,num)+'_4_3.scd','rb') as f:
        nums=readshortcode(num, f)
        mistercounter=0
        print '0000000',
        for cgraph in g.loadshortcode(nums):
            mistercounter+=1
            superawesomecounter+=1
            labels=auto_label(cgraph.get_vertices(), cgraph.edges, [2,1], 8, holes_mode="minimize")
            print "done getting labels"
            #find labels
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

                #find holes                
                numholes=0
                for hole in xrange(minlabel+1,maxlabel):
                    if hole not in labels:
                        numholes+=1

                if numholes > 0:
                    adj=str(adjacencymatrix(cgraph.get_vertices(),cgraph.edges))
                    recordholegraph(num, adj, lnum, numholes)
                if lnum > 8:
                    adj=str(adjacencymatrix(cgraph.get_vertices(),cgraph.edges))
                    recordlargelambdagraph(num, adj, lnum, numholes)
                # test criteria for a 'success'                        
                if lnum > 8 and numholes > 0:
                    adj=str(adjacencymatrix(cgraph.get_vertices(),cgraph.edges))
                    with open('C:\\My Dropbox\\GPGlabelings\\graph program 26\\0rho\\'+str(num)+'found.txt','a') as res:            
                        res.write(adj)
                    print "WOOOOOOOOOOOOOO:"
                    print adj
                    print num,mistercounter
                print "lambda:"+str(lnum)+" holes:"+str(numholes)
                sys.stdout.write("\b\b\b\b\b\b\b%07d" % mistercounter)
            if kbhit():
                print '.',
                if getch()=='q':
                    force_quit=True
                    print "yo imma be the one to end this. to be continued."
            print "completed "+str(mistercounter)+" graphs for "+str(num)
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

    print "starting next"
    return superawesomecounter
    

psyco.full()
nums=[11,12,14,15]
supercounter = 0
for num in nums:
    supercounter=main(num, supercounter)

