import copy

def adjacency_to_dist2_matrix(adj):
    """ finds paths-of-length-2 matrix using adjacency matrix
        adj[i][j] = 2 iff dist(vert i, vert j) = 2
                  = 1 iff dist(vert i, vert j) = 1
                  = 0 otherwise """
    if (len(adj) == 0):
        return list(adj)
    l2m = [ list(r) for r in adj]
    adj2 = matrix_mult(adj, adj)
    for i in range(len(adj2)): # rows
        for j in range(len(adj2[0])): # columns
            adj2[i][j] = 2*int(adj2[i][j]>0)
            if adj[i][j]:
                adj2[i][j] = 1
            if (i == j):
                adj2[i][j] = 0
    return adj2

def matrix_mult(matrix1,matrix2):
    """ multiplies matrices """
    if len(matrix1[0]) != len(matrix2):
        # Check matrix dimensions
        print 'Matrices must be m*n and n*p to multiply!'
    else:
        # Multiply if correct dimensionsa
        new_matrix = [[0 for row in range(len(matrix1))] for col in range(len(matrix2[0]))]
        for i in range(len(matrix1)):
            for j in range(len(matrix2[0])):
                for k in range(len(matrix2)):
                    new_matrix[i][j] += matrix1[i][k]*matrix2[k][j]
        return new_matrix

def check_labelling(adj, labs):
    """ returns whether an L(2,1) labelling is correct """
    adj2 = adjacency_to_dist2_matrix(adj)
    for i in range(len(adj)): # rows
        for j in range(len(adj[0])): # columns
            diff = abs(labs[i] - labs[j])
            if adj[i][j] == 1 and diff < 2:
                return False
            if adj2[i][j] == 2 and diff < 1:
                return False
    return True

def lambda_labelling(adj):
    """ returns a list of all possible lambda labelings of a graph """
    global _minL
    global _minlabs
    global _all_minlabs

    n = len(adj)
    # L = 2*n # the max possible label
    L = 6 # we know that the graphs we are searching for
          # will only use the labels 0, 2, 4, and 6
          # so setting a maximum label of 6 is safe
    labs = [0 for i in range(n)]
    I = [1 for i in range(n)]

    _minL = L
    _minlabs = []
    _all_minlabs = []
    
    make_labellings(labs, I, n, L, adj)

    return _all_minlabs


def make_labellings(labs, I, n, L, adj):
    """ helper function for lambda_labelling """
    global _minL
    global _minlabs
    global _all_minlabs
    
    print labs
    isCorrect = check_labelling(adj, labs)
    if isCorrect:
        maxlab = max(labs)
        if maxlab < _minL:
            _minL = maxlab
            _minlabs = labs
            _all_minlabs.append(_minlabs)
    for i in range(n):
        if I[i] == 1:
            for j in range(2,L+1,2):
                new_labs = labs[:]
                new_labs[i] = j
                new_I = I[:]
                for k in range(0, i+1): new_I[k] = 0
                make_labellings(new_labs, new_I, n, L, adj)