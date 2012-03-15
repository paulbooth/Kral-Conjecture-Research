# OSG_28vert3reg_checker.py
# python functions related to reading in shortcode output from the genreg program
# and checking the labellings for 28 vertex 3-regular graphs
# There are 40497138011 non-isomorphic 3-regular graphs on 28 vertices.
# Author: Paul Booth (thepaulbooth@gmail.com)

from copy import deepcopy

# the filename of the scd file.
# if partitioned, the file will be of this form
filename = '16_3_3.scd' #'28_3_3#1.scd'

# the number of vertices
number_of_vertices = 16

# filename of interesting graphs
output_filename = 'rho3_graphs_' + str(number_of_vertices) + '.txt'

# generator that yields the shortcode of a given .scd (shortcode) file f
# @param f: the file that has the shortcode, opened with (rb) to allow reading bytes
def readshortcode(f):
    s=f.read(1) # read the first value
    while s:
        yield (ord(s)) 
        s=f.read(1)


# generator to yield adjacency matrices loaded from a shortcode sequence
# @param shortcode: generator that yields the shortcode sequence, like the result of readshortcode(f).
# 	shortcode should be a sequence that is in the following format:
#   First: the number of same elements from the last code (as they are generated, the beginnings of each code 
#  		looks like the beginning of the last code, so this saves space)
# 	Each current code has these numsame elements from the last code, and a total length of k*n/2, so the next k*n/2 - numsame are the rest of the current code
#	Each current code generated as above is basically a minimum adjacency list 
#		The first k are the indices (indices start at 1) of the vertices that the first vertex is connected to.
# 		After a vertex has been connected to by previous vertex, it has that many less vertices to connect to
#			so the number of things to be read in from the shortcode related to that vertex is decreased by that many
#			This is stored as the list called degrees in the code.
#			so if when we reach vertex 4, and 4 has already been connected by vertices 1,2, and 3,
#				we won't actually read in any numbers from currentcode (for 3-regular graphs) since vertex 4 _already_ has degree 3
#			so if currentcode (with n=4) is [2,3,4,3,4,4] it means that 
#				vertex 1 is connected to vertices 2,3, and 4. [read in 3 numbers from currentcode]
#				vertex 2 is connected to vertices 3, and 4 (but also already connected to by vertex 1) [read in 2 more numbers from currentcode]
#				vertex 3 is connected to vertex 4 (and already connected to by vertices 1, and 2)
#				vertex 4 has been connected to 1,2, and 3 [read in zero numbers from currentcode]
def loadshortcode(shortcode):
    n = number_of_vertices
    k = 3
    previouscode=[]
    # we step through the huge list of numbers by grabbing the number of same 
    for numsame in shortcode:
    	# we first get the current code, since 
        currentcode=[]
        if numsame>0:
            currentcode=previouscode[:numsame]
        for j in xrange(k*n/2-numsame): 
            currentcode.append(shortcode.next())
        # now we keep track of the number of times that each vertex has already been used
        # by the end, this will be filled with k's since the graph is k-regular
        degrees=[0]*n
        adj=[] # use this to store the adjacency matrix
        for j in xrange(n):
            adj.append(([0]*n)) # initialize the adjacency matrix
        j=0
        for currentvertnum in xrange(n):#go through every vertex number from 0 to n-1
            for l in xrange(k-degrees[currentvertnum]): # the shortcode has k numbers for each vertex, unless that vertex was used before
                adj[currentvertnum][currentcode[j]-1]=1 # there is an edge between these two!
                adj[currentcode[j]-1][currentvertnum]=1 # and by symmetry, an edge here, too!
                degrees[currentcode[j]-1]+=1 # we used this vertex one more time, so when we go to read for that vertex, there will be one less number
                j+=1
        previouscode=currentcode[:]
        # We have an adjacency matrix here. To turn it into an spm, the rest follows
        yield adj

        #muor dim1 in xrange(n):
#             for dim3 in xrange(n):
#                 #twopaths=False
#                 if dim1==dim3:
#                     adj[dim1][dim3]=0
#                     continue
#                 if adj[dim1][dim3]==1:
#                     continue
#                 for dim2 in xrange(n):
#                     if adj[dim1][dim2]==1 and adj[dim2][dim3]==1:
#                         #if twopaths:
#                         adj[dim1][dim3]=2
#                         break
# ##                        else:
# ##                            twopaths=True;
#         yield adjltiply adj mat by self to find paths of length two, -more, don't care about
#         for dim1 in xrange(n):
#             for dim3 in xrange(n):
#                 #twopaths=False
#                 if dim1==dim3:
#                     adj[dim1][dim3]=0
#                     continue
#                 if adj[dim1][dim3]==1:
#                     continue
#                 for dim2 in xrange(n):
#                     if adj[dim1][dim2]==1 and adj[dim2][dim3]==1:
#                         #if twopaths:
#                         adj[dim1][dim3]=2
#                         break
# ##                        else:
# ##                            twopaths=True;
#         yield adj

# returns the number of unused labels in a minimum labelling
def get_number_of_holes(labelling):
    max_label = max(labelling)
    labels = [0] * (max_label + 1)
    for label in labelling:
        labels[label] = 1
    return max_label - sum(labels)

# gets the rho for a graph given the list of minimum labellings
# returns -1 if there are no labellings
def get_rho(labellings):
    if (len(labellings) == 0):
        return -1
    min_num_holes = get_number_of_holes(labellings[0])
    for labelling in labellings:
        num_holes = get_number_of_holes(labelling)
        if (num_holes < min_num_holes):
            min_num_holes = num_holes
    return min_num_holes

# turns an adjacency matrix into a shortest path matrix
# only calculates the paths of up-to distance 2 for speed (only doing L(2,1) labelling)
# not used as long as adjacency matrix is passed to the C++
def adj_to_spm(adj):
    spm = [r[:] for r in adj]
    n = number_of_vertices
    #multiply adj mat by self to find paths of length two, -more, don't care about
    for dim1 in xrange(n):
        for dim3 in xrange(n):
            #twopaths=False
            if dim1==dim3:
                spm[dim1][dim3]=0
                continue
            if spm[dim1][dim3]==1:
                continue
            for dim2 in xrange(n):
                if spm[dim1][dim2]==1 and spm[dim2][dim3]==1:
                    #if twopaths:
                    spm[dim1][dim3]=2
                    break
##                        else:
##                            twopaths=True;
    return spm


# The main function to read a shortcode file, generate the adjacency matrices, label, and test for the qualities in the Kral conjecture
def analyze_graphs():
    num_graphs_analyzed = 0;
    with open(filename,'rb') as f:
        nums = readshortcode(f)
        for adj in loadshortcode(nums):
            # TODO (thepaulbooth): send the adjacency matrix to the C++, get output and whatever
            #print adj
            # labellings = autolabel.lambda_labelling(adj)
            # rho = get_rho(labellings)
            # if (rho >= 3):
            #     with open(output_filename, 'a') as outfile:
            #         outfile.write(str(adj) + '\n')
            num_graphs_analyzed += 1
            print "Checking graph #" + str(num_graphs_analyzed)
            labels = auto_label(adj_to_spm(adj))
            #print adj

            if labels != False:
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
                    with open(output_filename, 'a') as outfile:
                        outfile.write('works:' + str(num_graphs_analyzed) + '\n')
                        for row in adj:
                            outfile.write(str(row) + '\n')
                        outfile.write('labels:' + str(labels) + '\n')



##################################

def auto_label(spm):
    
    initvertex = 0
    # TODO: is there an optimum place to start?
    
    memory = [None] # memory is a list so that changes made to it's contents will carry out of the methods

    # There was a sketchy error happening if you don't explicitly pass "set=[]".  It just builds the dsets together from each auto-labeling...
    labs=["NULL"]*len(spm)
    for label in [0,2]:
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
            memory[0] = [number_of_labels,labels[:]] # send a copy of the labels. This labels[:] used to be deepcopy(labels)
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
        for l in [0,2,4,6]: 
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


###################################










analyze_graphs()