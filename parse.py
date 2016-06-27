#!/opt/local/bin/pypy
#!/usr/bin/python

import sys
from collections import deque, defaultdict
import multiprocessing

class defaultlist(list):
    def __init__(self, fx):
        self._fx = fx
    def _fill(self, index):
        while len(self) <= index:
            self.append(self._fx())
    def __setitem__(self, index, value):
        self._fill(index)
        list.__setitem__(self, index, value)
    def __getitem__(self, index):
        self._fill(index)
        return list.__getitem__(self, index)

class Color:
	count = 0
	def __init__( self, color ):
		self.index = Color.count
		self.color = color
		Color.count += 1

	@staticmethod
	def ByColor( color ):
		return next( c for c in Color.Options if c.color == color )
	def __repr__( self ):
		return self.color

Color.RED    = Color("RED")
Color.GREEN  = Color("GREEN")
Color.BLUE   = Color("BLUE")
Color.YELLOW = Color("YELLOW")
Color.Options = [ Color.RED, Color.GREEN, Color.BLUE, Color.YELLOW ]

class Cell:

	def __init__( self, col,x,y ):
		self.x = x
		self.y = y
		self.color = Color.ByColor( col )
		self.adj = set()
		self.adj_col = defaultdict(set)
		self.rdist = 100000
		self.visited = False

	def orthadj( self, cells ):
		if self.x > 0:
			self.adj.add( cells[self.x-1][self.y] )
		if self.y > 0:
			self.adj.add( cells[self.x][self.y-1] )
		if self.x < len(cells)-1:
			self.adj.add( cells[self.x+1][self.y] )
		if self.y < len(cells)-1:
			self.adj.add( cells[self.x][self.y+1] )
	def __repr__( self ):
		return "("+str(self.x)+","+str(self.y)+"="+self.color.color[0]+")"

rows=[]
cells=[]

print( "Parsing...")
filename= len(sys.argv) > 1 and sys.argv[1] or "medium.cgp"
with open( filename ) as f:
	N = int(f.readline().strip() )

	for x in range(N):
		col = []
		for y in range(N):
			cell =  Cell(f.readline().strip(),x,y) 
			cells.append( cell )
			col.append(cell)

		rows.append( col )



for c in cells:
	c.orthadj( rows )

for c in cells:
	added = True
	
	while added:
		added = False
		for o in c.adj.copy():
			if c.color == o.color:
				added = True

				#merge
				o.adj.remove(c)
				c.adj.remove(o)
				#update neighbors
				for n in o.adj:
					n.adj.remove( o )
					n.adj.add( c )
					c.adj.add( n )
				o.adj.clear()




cells = [ c for c in cells if c.adj ]

for c in cells:
	for n in c.adj:
		c.adj_col[ n.color ].add( n ) 

#BEGIN SOLVE

def BFS( root, cells ):

	for c in cells:
		c.parent = None
		c.dist = 0
	root.parent = None
	root.dist = 0

	q = deque([root])

	while len(q) > 0:
		n = q.popleft()
		#visit adjacent in order of the number of unvisited adjacents
		adj = sorted( n.adj, key = lambda l:sum( 1 for x in l.adj if x.parent is None ), reverse=True )
		for a in adj:
			if a == n.parent:
				#don't go to parent
				continue
			if a.parent != None:
				#already connected
				continue
			a.parent = n
			q.append( a )

			p = a.parent
			while p != None:
				p.dist = max( p.dist, a.dist + 1 )
				a = p
				p = a.parent

	root.rdist = root.dist

#This finds the best candidate starting point

#First, find the cells with the most immediate neighbors
cells.sort( key=lambda l:len(l.adj) , reverse=True )
for c in cells[:100]:
	#then, find the worst-case distance to every cell staring from this one
	BFS( c, cells )

#find the lowest total worst-case cost first
cells.sort( key=lambda l:l.rdist )

#print( str(cells[0])+": "+str(cells[0].rdist)+" "+str(cells[0].adj))
#exit(1)

#good = [rows[24][37], 
#		rows[28][34], 
#		rows[33][28],
#		rows[37][34],
#		rows[27][31],
#		rows[18][37],
#		rows[43][36] ]

def get_h( cell ):
	h = 0
	for col,adj in cell.adj_col.iteritems():
		for n in adj:
			h = max( h, n.dist+1)
	return h

depths = defaultlist( lambda:[0,0,0,0] )
def tallydepth( g, color ):
	try:
		depths[g][color.index] += 1
	except:
		print( g, color.index ) 


class WhiteCell:
	def __init__( self, node, color ):
		newwhite = node.adj_col[color]
		self.white = node.white | newwhite
		self.adj_col = {}
		for col in Color.Options:
			if col == color:
				continue
			adj = col in node.adj_col and node.adj_col[ col ] or set()
			self.adj_col[col] = adj.union( *[a.adj_col[col] for a in newwhite] ) - self.white

def idastar_step( node, g, bound, sol, my_col=-1, my_dep=-1 ):
	h = get_h( node )
	if h == 0:
		return ( True, 0 )
	f = g + h
	if f > bound:
		return (False, f )
	
	min_t = 10000
	for color,adj in sorted( node.adj_col.iteritems(), key=lambda (k,v):len(v), reverse=True):
		#tallydepth(g,color)
		if g == my_dep and color.index != my_col:
			#print( my_dep, my_col, color.index )
			continue
		found, f_n = idastar_step( WhiteCell( node, color), g + 1, bound, sol, my_col, my_dep ) 
		min_t = min( min_t, f_n )
		if found:
			sol.append( color )
			return ( found, min_t )

	return ( False, min_t )

def idastar( root, cells ):
	BFS( root, cells )
	bound = -1
	newbound = 15 #adjust this to be close to the bound to minimize subtree traversal
	root.white = set( [root] )

	found = False
	sol = []
	while not found and bound != newbound:
		bound = newbound
		found, newbound = idastar_step( root, 0, bound, sol )
	sol.append( (root.x, root.y ) )
	sol.reverse()
	return sol

def idastar_mp_run( root, cells, my_dep, my_col, q ):
	#print( "Starting with "+str(my_dep)+" "+str(my_col) )
	found = False
	sol = []
	bound = -1
	newbound = 15 #adjust this to be close to the bound to minimize subtree traversal
	while not found and bound != newbound:
		bound = newbound
		found, newbound = idastar_step( root, 0, bound, sol, my_col, my_dep )
	sol.append( (root.x, root.y ) )
	sol.append( len( sol ) )
	sol.reverse()
	q.put( sol )


def idastar_mp( root, cells ):
	BFS( root, cells )
	root.white = set( [root] )
	procs = []
	q = multiprocessing.Queue()
	for i in range(4):
		proc = multiprocessing.Process( target = idastar_mp_run, args = ( root, cells, 8, i, q ) )
		proc.start()
		procs.append( proc )

	sols = []

	for proc in procs:
		proc.join()
		while not q.empty():
			sols.append( q.get() )
		#print(sols)

	sols.sort( key=lambda x:x[0] )
	return sols[0]
	


print("Solving...")
def serial_solve( cells ):
	sols = []
	for c in cells[:10]: #Look at 10 best starting locations
		try:
			sol =  idastar( c, cells ) 
			print( len(sol), sol )
			sols.append(sol)
		except KeyboardInterrupt:
			break

	if sols:
		sols.sort( key=lambda l:len(l) )
		print("Best:")
		print( len(sols[0]), sols[0] )

def parallel_solve( cells ):
	sols = []
	for c in cells[:10]: #Look at 10 best starting locations
		sol =  idastar_mp( c, cells ) 
		print( sol )
	if sols:
		sols.sort( key=lambda l:len(l) )
		print("Best:")
		print( len(sols[0]), sols[0] )

parallel_solve( cells )
#print( len(sols[0]), sols[0] )

#idastar( cells[0], cells )



