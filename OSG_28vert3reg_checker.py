# OSG_28vert3reg_checker.py
# python functions related to reading in shortcode output from the genreg program
# and checking the labelings for 28 vertex 3-regular graphs
# There are 40497138011 non-isomorphic 3-regular graphs on 28 vertices.
# Author: Paul Booth (thepaulbooth@gmail.com)

# the filename of the scd file.
# if partitioned, the file will be of this form
filename = '28_3_3#1.scd'

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
    n = 28
    k = 3
    i = 2
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

        yield adj

# The main function to read a shortcode file, generate the adjacency matrices, label, and test for the qualities in the Kral conjecture
def analyze_graphs():
	with open(filename,'rb') as f:
		nums = readshortcode(f)
		for adj in loadshortcode(nums):
			# TODO (thepaulbooth): send the adjacency matrix to the C++, get output and whatever
			print adj

# turns an adjacency matrix into a shortest path matrix
# only calculates the paths of up-to distance 2 for speed (only doing L(2,1) labeling)
# mutates the input argument
# not used as long as adjacency matrix is passed to the C++
def adj_to_spm(adj):
        #multiply adj mat by self to find paths of length two, -more, don't care about
        for dim1 in xrange(n):
            for dim3 in xrange(n):
                #twopaths=False
                if dim1==dim3:
                    adj[dim1][dim3]=0
                    continue
                if adj[dim1][dim3]==1:
                    continue
                for dim2 in xrange(n):
                    if adj[dim1][dim2]==1 and adj[dim2][dim3]==1:
                        #if twopaths:
                        adj[dim1][dim3]=2
                        break
##                        else:
##                            twopaths=True;
        previouscode=currentcode[:]
        yield adj


