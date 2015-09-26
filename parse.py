#!/usr/bin/python

from collections import deque, defaultdict

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
with open("hard.cgp") as f:
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
	#print(c)
	added = True
	
	while added:
		added = False
		for o in c.adj.copy():
			if c.color == o.color:
				#print("\t"+str(o))
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

				#print("\t\t->"+str(c.adj))



cells = [ c for c in cells if c.adj ]

for c in cells:
	for n in c.adj:
		c.adj_col[ n.color ].add( n ) 

good = [rows[24][37], 
		rows[28][34], 
		rows[33][28],
		rows[37][34],
		rows[27][31],
		rows[18][37],
		rows[43][36] ]

#cells.sort( key=lambda l:len(l.adj) , reverse=True )
#print( str(cells[0])+" "+str(cells[0].adj))

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

import copy

def get_h( cell ):
	h = 0
	for col,adj in cell.adj_col.iteritems():
		for n in adj:
			h = max( h, n.dist)
	return h

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

found = True
sol = []

def idastar_step( node, g, bound ):
	global found, sol
	h = get_h( node )
	if h == 0:
		print("Found! " + str(len(node.white) ))
		found = True
		return
	f = g + h
	if f > bound:
		return f
	
	min_t = 10000
	for color,adj in sorted( node.adj_col.iteritems(), key=lambda (k,v):len(v), reverse=True):
		min_t = min( min_t, idastar_step( WhiteCell( node, color), g + 1, bound ) )
		if found:
			sol.append( color )
			return

	return min_t

def idastar( root, cells ):
	global found, sol
	found = False
	sol = []
	#ncells = copy.deepcopy( cells )
	BFS( root, cells )
	bound = -1
	newbound = get_h( root )
	root.white = set( [root] )
	while not found and bound != newbound:
		bound = newbound
		newbound = idastar_step( root, 0, bound )
		print("newbound="+str(newbound))
	sol.reverse()
	print( root.x, root.y)
	print( len(sol), sol )


idastar( good[0], cells )

#for c in cells[:100]:
#	BFS( c, cells )
#cells.sort( key=lambda l:l.rdist )


