/*
Copyright (c) 2000-2009 Lee Thomason (www.grinninglizard.com)
Grinning Lizard Utilities.

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
*/

#ifndef GRINNINGLIZARD_MICROPATHER_INCLUDED
#define GRINNINGLIZARD_MICROPATHER_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4786 )   // Debugger truncating names.
#pragma warning( disable : 4530 )   // Exception handler isn't used
#endif

/** @mainpage MicroPather

   MicroPather is a path finder and A* solver (astar or a-star) written in platform independent
   C++ that can be easily integrated into existing code. MicroPather focuses on being a path
   finding engine for video games but is a generic A* solver. MicroPather is open source, with
   a license suitable for open source or commercial use.

   An overview of using MicroPather is in the <A HREF="../readme.htm">readme</A> or
   on the Grinning Lizard website: http://grinninglizard.com/micropather/
*/

#include <vector>
#include <memory.h>
#include <float.h>

using namespace std;

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#if defined( _DEBUG )
#  if defined( _MSC_VER )
#     define MPASSERT( x )    if ( !(x)) { _asm { int 3 } }
#  else
#     include <assert.h>
#     define MPASSERT assert
#  endif
#else
#  define MPASSERT( x ) {}
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
#include <stdlib.h>
typedef uintptr_t    MP_UPTR;
#elif defined (__GNUC__) && (__GNUC__ >= 3 )
#include <stdlib.h>
typedef uintptr_t    MP_UPTR;
#else
// Assume not 64 bit pointers. Get a new compiler.
typedef unsigned MP_UPTR;
#endif

//#define MICROPATHER_STRESS

namespace micropather
{
	/**
	   Used to pass the cost of states from the cliet application to MicroPather. This
	   structure is copied in a vector.

	   @sa AdjacentCost
	*/
	struct StateCost
	{
		void* state;   ///< The state as a void*
		float cost; ///< The cost to the state. Use FLT_MAX for infinite cost.
	};

	/**
	   A pure abstract class used to define a set of callbacks.
	   The client application inherits from
	   this class, and the methods will be called when MicroPather::Solve() is invoked.

	   The notion of a "state" is very important. It must have the following properties:
	   - Unique
	   - Unchanging (unless MicroPather::Reset() is called)

	   If the client application represents states as objects, then the state is usually
	   just the object cast to a void*. If the client application sees states as numerical
	   values, (x,y) for example, then state is an encoding of these values. MicroPather
	   never interprets or modifies the value of state.
	*/
	class Graph
	{
	public:
		virtual ~Graph() {}

		/**
		   Return the least possible cost between 2 states. For example, if your pathfinding
		   is based on distance, this is simply the straight distance between 2 points on the
		   map. If you pathfinding is based on minimum time, it is the minimal travel time
		   between 2 points given the best possible terrain.
		*/
		virtual float LeastCostEstimate( void* stateStart, void* stateEnd ) = 0;

		/**
		   Return the exact cost from the given state to all its neighboring states. This
		   may be called multiple times, or cached by the solver. It *must* return the same
		   exact values for every call to MicroPather::Solve(). It should generally be a simple,
		   fast function with no callbacks into the pather.
		*/
		virtual void AdjacentCost( void* state, std::vector< micropather::StateCost > *adjacent ) = 0;

		/**
		   This function is only used in DEBUG mode - it dumps output to stdout. Since void*
		   aren't really human readable, normally you print out some concise info (like "(1,2)")
		   without an ending newline.
		*/
		virtual void  PrintStateInfo( void* state ) = 0;
	};

	class PathNode;

	struct NodeCost
	{
		PathNode* node;
		float cost;
	};

	/*
	   Every state (void*) is represented by a PathNode in MicroPather. There
	   can only be one PathNode for a given state.
	*/
	class PathNode
	{
	public:
		void Init( unsigned _frame, void* _state, float _costFromStart, float _estToGoal, PathNode* _parent )
		{
			state = _state;
			costFromStart = _costFromStart;
			estToGoal = _estToGoal;
			CalcTotalCost();
			parent = _parent;
			frame = _frame;
			inOpen = 0;
			inClosed = 0;
		}

		void Clear()
		{
			memset( this, 0, sizeof( PathNode ) );
			numAdjacent = -1;
			cacheIndex  = -1;
		}
		void InitSentinel()
		{
			Clear();
			Init( 0, 0, FLT_MAX, FLT_MAX, 0 );
			prev = next = this;
		}

		void* state;      ///< the client state
		float costFromStart; ///< exact
		float estToGoal;  ///< estimated
		float totalCost;  ///< could be a function, but save some math.
		PathNode* parent; ///< the parent is used to reconstruct the path
		unsigned frame;      ///< unique id for this path, so the solver can distinguish correct from stale values

		int numAdjacent;  ///< -1  is unknown & needs to be queried
		int cacheIndex;      ///< position in cache

		PathNode* child[2];  // Binary search in the hash table. [left, right]
		PathNode* next, *prev;  // used by open queue

		bool inOpen;
		bool inClosed;

		void Unlink() { next->prev = prev; prev->next = next; next = prev = 0; }

		void AddBefore( PathNode* addThis ) { addThis->next = this; addThis->prev = prev; prev->next = addThis; prev = addThis; }

#ifdef DEBUG
		void CheckList()
		{
			MPASSERT( totalCost == FLT_MAX );

			for ( PathNode* it = next; it != this; it = it->next )
			{
				MPASSERT( it->prev == this || it->totalCost >= it->prev->totalCost );
				MPASSERT( it->totalCost <= it->next->totalCost );
			}
		}
#endif

		void CalcTotalCost() { totalCost = ( costFromStart < FLT_MAX && estToGoal < FLT_MAX ) ? costFromStart + estToGoal : FLT_MAX; }

	private:
		void operator=( const PathNode& );
	};

	/** Memory manager for the PathNodes. */
	class PathNodePool
	{
	public:
		PathNodePool( unsigned _allocate, unsigned _typicalAdjacent ): firstBlock( 0 ),
			blocks( 0 ),
#if defined( MICROPATHER_STRESS )
			allocate( 32 ),
#else
			allocate( _allocate ),
#endif
			nAllocated( 0 ),
			nAvailable( 0 )
		{
			freeMemSentinel.InitSentinel();

			cacheCap = allocate * _typicalAdjacent;
			cacheSize = 0;
			cache = ( NodeCost* )malloc( cacheCap * sizeof( NodeCost ) );

			// Want the behavior that if the actual number of states is specified, the cache
			// will be at least that big.
			hashShift = 3; // 8 (only useful for stress testing)

#if !defined( MICROPATHER_STRESS )

			while ( HashSize() < allocate ) { ++hashShift; }

#endif

			hashTable = ( PathNode** )calloc( HashSize(), sizeof( PathNode* ) );

			blocks = firstBlock = NewBlock();
			// printf( "HashSize=%d allocate=%d\n", HashSize(), allocate );
			totalCollide = 0;
		}

		~PathNodePool()
		{
			Clear();
			free( firstBlock );
			free( cache );
			free( hashTable );
#ifdef TRACK_COLLISION
			printf( "Total collide=%d HashSize=%d HashShift=%d\n", totalCollide, HashSize(), hashShift );
#endif
		}

		// Free all the memory except the first block. Resets all memory.
		void Clear()
		{
#ifdef TRACK_COLLISION
			// Collision tracking code.
			int collide = 0;

			for ( unsigned i = 0; i < HashSize(); ++i )
			{
				if ( hashTable[i] && ( hashTable[i]->child[0] || hashTable[i]->child[1] ) ) { ++collide; }
			}

			//printf( "PathNodePool %d/%d collision=%d %.1f%%\n", nAllocated, HashSize(), collide, 100.0f*(float)collide/(float)HashSize() );
			totalCollide += collide;
#endif

			Block* b = blocks;

			while ( b )
			{
				Block* temp = b->nextBlock;

				if ( b != firstBlock ) { free( b ); }

				b = temp;
			}

			blocks = firstBlock; // Don't delete the first block (we always need at least that much memory.)

			// Set up for new allocations (but don't do work we don't need to. Reset/Clear can be called frequently.)
			if ( nAllocated > 0 )
			{
				freeMemSentinel.next = &freeMemSentinel;
				freeMemSentinel.prev = &freeMemSentinel;

				memset( hashTable, 0, sizeof( PathNode* )*HashSize() );

				for ( unsigned i = 0; i < allocate; ++i ) { freeMemSentinel.AddBefore( &firstBlock->pathNode[i] ); }
			}

			nAvailable = allocate;
			nAllocated = 0;
			cacheSize = 0;
		}

		// Essentially:
		// pNode = Find();
		// if ( !pNode )
		//    pNode = New();
		//
		// Get the PathNode associated with this state. If the PathNode already
		// exists (allocated and is on the current frame), it will be returned.
		// Else a new PathNode is allocated and returned. The returned object
		// is always fully initialized.
		//
		// NOTE: if the pathNode exists (and is current) all the initialization
		//       parameters are ignored.
		PathNode* GetPathNode( unsigned frame, void* _state, float _costFromStart, float _estToGoal, PathNode* _parent )
		{
			unsigned key = Hash( _state );

			PathNode* root = hashTable[key];

			while ( root )
			{
				if ( root->state == _state )
				{
					// Correct state, wrong frame.
					if ( root->frame != frame ) { root->Init( frame, _state, _costFromStart, _estToGoal, _parent ); }

					// This is the correct state and correct frame.
					break;
				}

				root = ( _state < root->state ) ? root->child[0] : root->child[1];
			}

			if ( !root )
			{
				// allocate new one
				root = Alloc();
				root->Clear();
				root->Init( frame, _state, _costFromStart, _estToGoal, _parent );
				AddPathNode( key, root );
			}

			return root;
		}

		// Store stuff in cache
		bool PushCache( const NodeCost* nodes, int nNodes, int* start )
		{
			*start = -1;

			if ( nNodes + cacheSize <= cacheCap )
			{
				for ( int i = 0; i < nNodes; ++i ) { cache[i+cacheSize] = nodes[i]; }

				*start = cacheSize;
				cacheSize += nNodes;
				return true;
			}

			return false;
		}

		// Get neighbors from the cache
		// Note - always access this with an offset. Can get re-allocated.
		void GetCache( int start, int nNodes, NodeCost* nodes )
		{
			MPASSERT( start >= 0 && start < cacheCap );
			MPASSERT( nNodes > 0 );
			MPASSERT( start + nNodes <= cacheCap );
			memcpy( nodes, &cache[start], sizeof( NodeCost )*nNodes );
		}

		// Return all the allocated states. Useful for visuallizing what
		// the pather is doing.
		void AllStates( unsigned frame, std::vector< void* >* stateVec )
		{
			for ( Block* b = blocks; b; b = b->nextBlock )
				for ( unsigned i = 0; i < allocate; ++i )
					if ( b->pathNode[i].frame == frame ) { stateVec->push_back( b->pathNode[i].state ); }
		}

	private:
		struct Block
		{
			Block* nextBlock;
			PathNode pathNode[1];
		};

		unsigned Hash( void* voidval )
		{
			/*
			   Spent quite some time on this, and the result isn't quite satifactory. The
			   input set is the size of a void*, and is generally (x,y) pairs or memory pointers.

			   FNV resulting in about 45k collisions in a (large) test and some other approaches about the same.

			   Simple folding reduces collisions to about 38k - big improvement. However, that may
			   be an artifact of the (x,y) pairs being well distributed. And for either the x,y case
			   or the pointer case, there are probably very poor hash table sizes that cause "overlaps"
			   and grouping. (An x,y encoding with a hashShift of 8 is begging for trouble.)

			   The best tested results are simple folding, but that seems to beg for a pathelogical case.
			   FNV-1a was the next best choice, without obvious pathelogical holes.

			   Finally settled on h%HashMask(). Simple, but doesn't have the obvious collision cases of folding.
			*/
			/*
			   // Time: 567
			   // FNV-1a
			   // http://isthe.com/chongo/tech/comp/fnv/
			   // public domain.
			   MP_UPTR val = (MP_UPTR)(voidval);
			   const unsigned char *p = (unsigned char *)(&val);
			   unsigned int h = 2166136261;

			   for( size_t i=0; i<sizeof(MP_UPTR); ++i, ++p ) {
			      h ^= *p;
			      h *= 16777619;
			   }
			   // Fold the high bits to the low bits. Doesn't (generally) use all
			   // the bits since the shift is usually < 16, but better than not
			   // using the high bits at all.
			   return ( h ^ (h>>hashShift) ^ (h>>(hashShift*2)) ^ (h>>(hashShift*3)) ) & HashMask();
			   */
			/*
			// Time: 526
			MP_UPTR h = (MP_UPTR)(voidval);
			return ( h ^ (h>>hashShift) ^ (h>>(hashShift*2)) ^ (h>>(hashShift*3)) ) & HashMask();
			*/

			// Time: 512
			// The HashMask() is used as the divisor. h%1024 has lots of common
			// repetitions, but h%1023 will move things out more.
			MP_UPTR h = ( MP_UPTR )( voidval );
			return h % HashMask();
		}

		unsigned HashSize() const { return 1 << hashShift; }
		unsigned HashMask() const { return ( ( 1 << hashShift ) - 1 ); }

		void AddPathNode( unsigned key, PathNode* root )
		{
			if ( hashTable[key] )
			{
				PathNode* p = hashTable[key];

				while ( true )
				{
					int dir = ( root->state < p->state ) ? 0 : 1;

					if ( p->child[dir] )
					{
						p = p->child[dir];
					}
					else
					{
						p->child[dir] = root;
						break;
					}
				}
			}
			else
			{
				hashTable[key] = root;
			}
		}

		Block* NewBlock()
		{
			Block* block = ( Block* ) calloc( 1, sizeof( Block ) + sizeof( PathNode ) * ( allocate - 1 ) );
			block->nextBlock = 0;

			nAvailable += allocate;

			for ( unsigned i = 0; i < allocate; ++i ) { freeMemSentinel.AddBefore( &block->pathNode[i] ); }

			return block;
		}

		PathNode* Alloc()
		{
			if ( freeMemSentinel.next == &freeMemSentinel )
			{
				MPASSERT( nAvailable == 0 );

				Block* b = NewBlock();
				b->nextBlock = blocks;
				blocks = b;
				MPASSERT( freeMemSentinel.next != &freeMemSentinel );
			}

			PathNode* pathNode = freeMemSentinel.next;
			pathNode->Unlink();

			++nAllocated;
			MPASSERT( nAvailable > 0 );
			--nAvailable;
			return pathNode;
		}

		PathNode**  hashTable;
		Block*      firstBlock;
		Block*      blocks;

		NodeCost*   cache;
		int      cacheCap;
		int      cacheSize;

		PathNode freeMemSentinel;
		unsigned allocate;            // how big a block of pathnodes to allocate at once
		unsigned nAllocated;          // number of pathnodes allocated (from Alloc())
		unsigned nAvailable;          // number available for allocation

		unsigned hashShift;
		unsigned totalCollide;
	};

	/// Create a MicroPather object to solve for a best path. Detailed usage notes are on the main page.
	class MicroPather
	{
		friend class micropather::PathNode;
	private:
		class OpenQueue
		{
		public:
			OpenQueue( Graph* _graph ): graph( _graph )
			{
				sentinel = ( PathNode* ) sentinelMem;
				sentinel->InitSentinel();
#ifdef DEBUG
				sentinel->CheckList();
#endif
			}
			~OpenQueue() {}

			void Push( PathNode* pNode )
			{
				MPASSERT( pNode->inOpen == 0 );
				MPASSERT( pNode->inClosed == 0 );

#ifdef DEBUG_PATH_DEEP
				printf( "Open Push: " );
				graph->PrintStateInfo( pNode->state );
				printf( " total=%.1f\n", pNode->totalCost );
#endif

				// Add sorted. Lowest to highest cost path. Note that the sentinel has
				// a value of FLT_MAX, so it should always be sorted in.
				MPASSERT( pNode->totalCost < FLT_MAX );
				PathNode* iter = sentinel->next;

				while ( true )
				{
					if ( pNode->totalCost < iter->totalCost )
					{
						iter->AddBefore( pNode );
						pNode->inOpen = 1;
						break;
					}

					iter = iter->next;
				}

				MPASSERT( pNode->inOpen ); // make sure this was actually added.
#ifdef DEBUG
				sentinel->CheckList();
#endif
			}

			PathNode* Pop()
			{
				MPASSERT( sentinel->next != sentinel );
				PathNode* pNode = sentinel->next;
				pNode->Unlink();
#ifdef DEBUG
				sentinel->CheckList();
#endif

				MPASSERT( pNode->inClosed == 0 );
				MPASSERT( pNode->inOpen == 1 );
				pNode->inOpen = 0;

#ifdef DEBUG_PATH_DEEP
				printf( "Open Pop: " );
				graph->PrintStateInfo( pNode->state );
				printf( " total=%.1f\n", pNode->totalCost );
#endif

				return pNode;
			}

			void Update( PathNode* pNode )
			{
#ifdef DEBUG_PATH_DEEP
				printf( "Open Update: " );
				graph->PrintStateInfo( pNode->state );
				printf( " total=%.1f\n", pNode->totalCost );
#endif

				MPASSERT( pNode->inOpen );

				// If the node now cost less than the one before it,
				// move it to the front of the list.
				if ( pNode->prev != sentinel && pNode->totalCost < pNode->prev->totalCost )
				{
					pNode->Unlink();
					sentinel->next->AddBefore( pNode );
				}

				// If the node is too high, move to the right.
				if ( pNode->totalCost > pNode->next->totalCost )
				{
					PathNode* it = pNode->next;
					pNode->Unlink();

					while ( pNode->totalCost > it->totalCost )
					{
						it = it->next;
					}

					it->AddBefore( pNode );
#ifdef DEBUG
					sentinel->CheckList();
#endif
				}
			}

			inline bool Empty() const { return sentinel->next == sentinel; }

		private:
			OpenQueue( const OpenQueue& );   // undefined and unsupported
			void operator=( const OpenQueue& );

			PathNode* sentinel;
			int sentinelMem[ ( sizeof( PathNode ) + sizeof( int ) ) / sizeof( int ) ];
			Graph* graph;  // for debugging
		};

		class ClosedSet
		{
		public:
			ClosedSet( Graph* _graph )    { this->graph = _graph; }
			~ClosedSet()   {}

			void Add( PathNode* pNode )
			{
#ifdef DEBUG_PATH_DEEP
				printf( "Closed add: " );
				graph->PrintStateInfo( pNode->state );
				printf( " total=%.1f\n", pNode->totalCost );
#endif
#ifdef DEBUG
				MPASSERT( pNode->inClosed == 0 );
				MPASSERT( pNode->inOpen == 0 );
#endif
				pNode->inClosed = 1;
			}

			void Remove( PathNode* pNode )
			{
#ifdef DEBUG_PATH_DEEP
				printf( "Closed remove: " );
				graph->PrintStateInfo( pNode->state );
				printf( " total=%.1f\n", pNode->totalCost );
#endif
				MPASSERT( pNode->inClosed == 1 );
				MPASSERT( pNode->inOpen == 0 );

				pNode->inClosed = 0;
			}

		private:
			ClosedSet( const ClosedSet& );
			void operator=( const ClosedSet& );
			Graph* graph;
		};

	public:
		enum
		{
			SOLVED,
			NO_SOLUTION,
			START_END_SAME,
		};

		/**
		   Construct the pather, passing a pointer to the object that implements
		   the Graph callbacks.

		   @param graph      The "map" that implements the Graph callbacks.
		   @param allocate      How many states should be internally allocated at a time. This
		                  can be hard to get correct. The higher the value, the more memory
		                  MicroPather will use.
		                  - If you have a small map (a few thousand states?) it may make sense
		                    to pass in the maximum value. This will cache everything, and MicroPather
		                    will only need one main memory allocation. For a chess board, allocate
		                    would be set to 8x8 (64)
		                  - If your map is large, something like 1/4 the number of possible
		                    states is good. For example, Lilith3D normally has about 16,000
		                    states, so 'allocate' should be about 4000.
		                   - If your state space is huge, use a multiple (5-10x) of the normal
		                    path. "Occasionally" call Reset() to free unused memory.
		   @param typicalAdjacent  Used to determine cache size. The typical number of adjacent states
		                     to a given state. (On a chessboard, 8.) Higher values use a little
		                     more memory.
		*/
		MicroPather( micropather::Graph* _graph, unsigned allocate = 250, unsigned typicalAdjacent = 6 )
			: pathNodePool( allocate, typicalAdjacent ), graph( _graph ), frame( 0 ), checksum( 0 )
		{}

		~MicroPather() {}


		/**
		   Solve for the path from start to end.

		   @param startState Input, the starting state for the path.
		   @param endState      Input, the ending state for the path.
		   @param path       Output, a vector of states that define the path. Empty if not found.
		   @param totalCost  Output, the cost of the path, if found.
		   @return           Success or failure, expressed as SOLVED, NO_SOLUTION, or START_END_SAME.
		*/
		int Solve( void* startNode, void* endNode, vector< void* >* path, float* cost )
		{

#ifdef DEBUG_PATH
			printf( "Path: " );
			graph->PrintStateInfo( startNode );
			printf( " --> " );
			graph->PrintStateInfo( endNode );
			printf( " min cost=%f\n", graph->LeastCostEstimate( startNode, endNode ) );
#endif

			*cost = 0.0f;

			if ( startNode == endNode ) { return START_END_SAME; }

			++frame;

			OpenQueue open( graph );
			ClosedSet closed( graph );

			PathNode* newPathNode = pathNodePool.GetPathNode( frame, startNode, 0, graph->LeastCostEstimate( startNode, endNode ), 0 );

			open.Push( newPathNode );
			stateCostVec.resize( 0 );
			nodeCostVec.resize( 0 );

			while ( !open.Empty() )
			{
				PathNode* node = open.Pop();

				if ( node->state == endNode )
				{
					GoalReached( node, startNode, endNode, path );
					*cost = node->costFromStart;
#ifdef DEBUG_PATH
					DumpStats();
#endif
					return SOLVED;
				}
				else
				{
					closed.Add( node );

					// We have not reached the goal - add the neighbors.
					GetNodeNeighbors( node, &nodeCostVec );

					for ( int i = 0; i < node->numAdjacent; ++i )
					{
						// Not actually a neighbor, but useful. Filter out infinite cost.
						if ( nodeCostVec[i].cost == FLT_MAX ) { continue; }

						PathNode* child = nodeCostVec[i].node;
						float newCost = node->costFromStart + nodeCostVec[i].cost;

						PathNode* inOpen   = child->inOpen ? child : 0;
						PathNode* inClosed = child->inClosed ? child : 0;
						PathNode* inEither = ( PathNode* )( ( ( MP_UPTR )inOpen ) | ( ( MP_UPTR )inClosed ) );

						MPASSERT( inEither != node );
						MPASSERT( !( inOpen && inClosed ) );

						if ( inEither )
						{
							if ( newCost < child->costFromStart )
							{
								child->parent = node;
								child->costFromStart = newCost;
								child->estToGoal = graph->LeastCostEstimate( child->state, endNode );
								child->CalcTotalCost();

								if ( inOpen ) { open.Update( child ); }
							}
						}
						else
						{
							child->parent = node;
							child->costFromStart = newCost;
							child->estToGoal = graph->LeastCostEstimate( child->state, endNode ),
							       child->CalcTotalCost();

							MPASSERT( !child->inOpen && !child->inClosed );
							open.Push( child );
						}
					}
				}
			}

#ifdef DEBUG_PATH
			DumpStats();
#endif
			return NO_SOLUTION;
		}

		/**
		   Find all the states within a given cost from startState.

		   @param startState Input, the starting state for the path.
		   @param near       All the states within 'maxCost' of 'startState', and cost to that state.
		   @param maxCost    Input, the maximum cost that will be returned. (Higher values return
		                  larger 'near' sets and take more time to compute.)
		   @return           Success or failure, expressed as SOLVED or NO_SOLUTION.
		*/
		int SolveForNearStates( void* startState, std::vector< StateCost >* near, float maxCost )
		{
			/* http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm

			 1  function Dijkstra(Graph, source):
			 2      for each vertex v in Graph:           // Initializations
			 3          dist[v] := infinity               // Unknown distance function from source to v
			 4          previous[v] := undefined          // Previous node in optimal path from source
			 5      dist[source] := 0                     // Distance from source to source
			 6      Q := the set of all nodes in Graph
			      // All nodes in the graph are unoptimized - thus are in Q
			 7      while Q is not empty:                 // The main loop
			 8          u := vertex in Q with smallest dist[]
			 9          if dist[u] = infinity:
			10              break                         // all remaining vertices are inaccessible from source
			11          remove u from Q
			12          for each neighbor v of u:         // where v has not yet been removed from Q.
			13              alt := dist[u] + dist_between(u, v)
			14              if alt < dist[v]:             // Relax (u,v,a)
			15                  dist[v] := alt
			16                  previous[v] := u
			17      return dist[]
			*/

			++frame;

			OpenQueue open( graph );         // nodes to look at
			ClosedSet closed( graph );

			nodeCostVec.resize( 0 );
			stateCostVec.resize( 0 );

			PathNode closedSentinel;
			closedSentinel.Clear();
			closedSentinel.Init( frame, 0, FLT_MAX, FLT_MAX, 0 );
			closedSentinel.next = closedSentinel.prev = &closedSentinel;

			PathNode* newPathNode = pathNodePool.GetPathNode( frame, startState, 0, 0, 0 );
			open.Push( newPathNode );

			while ( !open.Empty() )
			{
				PathNode* node = open.Pop();  // smallest dist
				closed.Add( node );           // add to the things we've looked at
				closedSentinel.AddBefore( node );

				// Too far away to ever get here.
				if ( node->totalCost > maxCost ) { continue; }

				GetNodeNeighbors( node, &nodeCostVec );

				for ( int i = 0; i < node->numAdjacent; ++i )
				{
					MPASSERT( node->costFromStart < FLT_MAX );
					float newCost = node->costFromStart + nodeCostVec[i].cost;

					PathNode* inOpen   = nodeCostVec[i].node->inOpen ? nodeCostVec[i].node : 0;
					PathNode* inClosed = nodeCostVec[i].node->inClosed ? nodeCostVec[i].node : 0;
					MPASSERT( !( inOpen && inClosed ) );
					PathNode* inEither = inOpen ? inOpen : inClosed;
					MPASSERT( inEither != node );

					// Do nothing. This path is not better than existing.
					if ( inEither && inEither->costFromStart <= newCost ) { continue; }

					// Groovy. We have new information or improved information.
					PathNode* child = nodeCostVec[i].node;
					MPASSERT( child->state != newPathNode->state ); // should never re-process the parent.

					child->parent = node;
					child->costFromStart = newCost;
					child->estToGoal = 0;
					child->totalCost = child->costFromStart;

					if ( inOpen )
					{
						open.Update( inOpen );
					}
					else if ( !inClosed )
					{
						open.Push( child );
					}
				}
			}

			near->clear();

			for ( PathNode* pNode = closedSentinel.next; pNode != &closedSentinel; pNode = pNode->next )
			{
				if ( pNode->totalCost <= maxCost )
				{
					StateCost sc;
					sc.cost = pNode->totalCost;
					sc.state = pNode->state;

					near->push_back( sc );
				}
			}

#ifdef DEBUG

			for ( unsigned i = 0; i < near->size(); ++i )
			{
				for ( unsigned k = i + 1; k < near->size(); ++k )
				{
					MPASSERT( near->at( i ).state != near->at( k ).state );
				}
			}

#endif

			return SOLVED;
		}

		/** Should be called whenever the cost between states or the connection between states changes.
		   Also frees overhead memory used by MicroPather, and calling will free excess memory.
		*/
		void Reset()
		{
			pathNodePool.Clear();
			frame = 0;
			checksum = 0;
		}

		/**
		   Return the "checksum" of the last path returned by Solve(). Useful for debugging,
		   and a quick way to see if 2 paths are the same.
		*/
		MP_UPTR Checksum()   { return checksum; }

		// Debugging function to return all states that were used by the last "solve"
		void StatesInPool( std::vector< void* >* stateVec )
		{
			stateVec->clear();
			pathNodePool.AllStates( frame, stateVec );
		}

	private:
		MicroPather( const MicroPather& );  // undefined and unsupported
		void operator=( const MicroPather ); // undefined and unsupported

		PathNodePool         pathNodePool;
		std::vector< StateCost >   stateCostVec;  // local to Solve, but put here to reduce memory allocation
		std::vector< NodeCost >    nodeCostVec;   // local to Solve, but put here to reduce memory allocation

		Graph* graph;
		unsigned frame;                  // incremented with every solve, used to determine if cached data needs to be refreshed
		MP_UPTR checksum;                // the checksum of the last successful "Solve".

		void GoalReached( PathNode* node, void* start, void* end, vector< void* > *_path )
		{
			std::vector< void* >& path = *_path;
			path.clear();

			// We have reached the goal.
			// How long is the path? Used to allocate the vector which is returned.
			int count = 1;
			PathNode* it = node;

			while ( it->parent ) { ++count; it = it->parent; }

			// Now that the path has a known length, allocate
			// and fill the vector that will be returned.
			if ( count < 3 )
			{
				// Handle the short, special case.
				path.resize( 2 );
				path[0] = start;
				path[1] = end;
			}
			else
			{
				path.resize( count );

				path[0] = start;
				path[count-1] = end;
				count -= 2;
				it = node->parent;

				while ( it->parent )
				{
					path[count] = it->state;
					it = it->parent;
					--count;
				}
			}

			checksum = 0;
#ifdef DEBUG_PATH
			printf( "Path: " );
			int counter = 0;
#endif

			for ( unsigned k = 0; k < path.size(); ++k )
			{
				checksum += ( ( MP_UPTR )( path[k] ) ) << ( k % 8 );

#ifdef DEBUG_PATH
				graph->PrintStateInfo( path[k] );
				printf( " " );
				++counter;

				if ( counter == 8 ) { printf( "\n" ); counter = 0; }

#endif
			}

#ifdef DEBUG_PATH
			printf( "Cost=%.1f Checksum %d\n", node->costFromStart, checksum );
#endif
		}

		void GetNodeNeighbors( PathNode* node, std::vector< NodeCost >* pNodeCost )
		{
			if ( node->numAdjacent == 0 )
			{
				// it has no neighbors.
				pNodeCost->resize( 0 );
			}
			else if ( node->cacheIndex < 0 )
			{
				// Not in the cache. Either the first time or just didn't fit. We don't know
				// the number of neighbors and need to call back to the client.
				stateCostVec.resize( 0 );
				graph->AdjacentCost( node->state, &stateCostVec );

#ifdef DEBUG
				{
					// If this assert fires, you have passed a state
					// as its own neighbor state. This is impossible -- bad things will happen.
					for ( unsigned i = 0; i < stateCostVec.size(); ++i ) { MPASSERT( stateCostVec[i].state != node->state ); }
				}
#endif

				pNodeCost->resize( stateCostVec.size() );
				node->numAdjacent = stateCostVec.size();

				if ( node->numAdjacent > 0 )
				{
					// Now convert to pathNodes.
					// Note that the microsoft std library is actually pretty slow.
					// Move things to temp vars to help.
					const unsigned stateCostVecSize = stateCostVec.size();
					const StateCost* stateCostVecPtr = &stateCostVec.at( 0 );
					NodeCost* pNodeCostPtr = &pNodeCost->at( 0 );

					for ( unsigned i = 0; i < stateCostVecSize; ++i )
					{
						void* state = stateCostVecPtr[i].state;
						pNodeCostPtr[i].cost = stateCostVecPtr[i].cost;
						pNodeCostPtr[i].node = pathNodePool.GetPathNode( frame, state, FLT_MAX, FLT_MAX, 0 );
					}

					// Can this be cached?
					int start = 0;

					if ( pNodeCost->size() > 0 && pathNodePool.PushCache( pNodeCostPtr, pNodeCost->size(), &start ) ) { node->cacheIndex = start; }
				}
			}
			else
			{
				// In the cache!
				pNodeCost->resize( node->numAdjacent );
				NodeCost* pNodeCostPtr = &pNodeCost->at( 0 );
				pathNodePool.GetCache( node->cacheIndex, node->numAdjacent, pNodeCostPtr );

				// A node is uninitialized (even if memory is allocated) if it is from a previous frame.
				// Check for that, and Init() as necessary.
				for ( int i = 0; i < node->numAdjacent; ++i )
				{
					PathNode* pNode = pNodeCostPtr[i].node;

					if ( pNode->frame != frame ) { pNode->Init( frame, pNode->state, FLT_MAX, FLT_MAX, 0 ); }
				}
			}
		}

#ifdef DEBUG
		/*
		void DumpStats()
		{
		   int hashTableEntries = 0;
		   for( int i=0; i<HASH_SIZE; ++i ) { if ( hashTable[i] ) { ++hashTableEntries; } }

		   int pathNodeBlocks = 0;
		   for( PathNode* node = pathNodeMem; node; node = node[ALLOCATE-1].left ) { ++pathNodeBlocks; }
		   printf( "HashTableEntries=%d/%d PathNodeBlocks=%d [%dk] PathNodes=%d SolverCalled=%d\n",
		           hashTableEntries, HASH_SIZE, pathNodeBlocks, pathNodeBlocks*ALLOCATE*sizeof(PathNode)/1024,
		           pathNodeCount, frame );
		}
		*/
#endif
	};
}; // namespace micropather

#endif


//////// Micropather test

#ifdef MICROPATHER_TEST

/*
Copyright (c) 2000-2005 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include <ctype.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <iostream>

using namespace micropather;

const int MAPX = 30;
const int MAPY = 10;
const char gMap[MAPX* MAPY+1] =
   //"012345678901234567890123456789"
   "     |      |                |"
   "     |      |----+    |      +"
   "---+ +---DD-+      +--+--+    "
   "   |                     +-- +"
   "        +----+  +---+         "
   "---+ +  D    D            |   "
   "   | |  +----+    +----+  +--+"
   "   D |            |    |      "
   "   | +-------+  +-+    |--+   "
   "---+                   |     +";

class Dungeon: public Graph
{
private:
	int playerX, playerY;
	std::vector<void*> path;
	bool doorsOpen, showConsidered;

	MicroPather* pather;
public:
	// Use a very small memory block to stress the pather
	Dungeon() : playerX( 0 ), playerY( 0 ), doorsOpen( false ), showConsidered( false ), pather( 0 )
	{ pather = new MicroPather( this, 20 ); }

	virtual ~Dungeon() { delete pather; }

	int X()  { return playerX; }
	int Y() { return playerY; }

	unsigned Checksum() { return pather->Checksum(); }

	void ClearPath() { path.resize( 0 ); }

	void ToggleTouched() { showConsidered = !showConsidered; pather->Reset(); }

	void ToggleDoor() { doorsOpen = !doorsOpen; pather->Reset(); }

	int Passable( int nx, int ny )
	{
		if ( nx >= 0 && nx < MAPX && ny >= 0 && ny < MAPY )
		{
			int index = ny * MAPX + nx;
			char c = gMap[ index ];

			if ( c == ' ' ) { return 1; }
			else if ( c == 'D' ) { return 2; }
		}

		return 0;
	}

	int SetPos( int nx, int ny )
	{
		int result = 0;

		if ( Passable( nx, ny ) == 1 )
		{
			float totalCost;

			if ( showConsidered ) { pather->Reset(); }

			result = pather->Solve( XYToNode( playerX, playerY ), XYToNode( nx, ny ), &path, &totalCost );

			if ( result == MicroPather::SOLVED ) { playerX = nx; playerY = ny; }

			printf( "Pather returned %d\n", result );
		}

		return result;
	}

	void Print()
	{
		char buf[ MAPX+1 ];

		std::vector< void* > stateVec;

		if ( showConsidered ) { pather->StatesInPool( &stateVec ); }

		printf( " doors %s\n", doorsOpen ? "open" : "closed" );
		printf( " 0         10        20\n" );
		printf( " 012345678901234567890123456789\n" );

		for ( int j = 0; j < MAPY; ++j )
		{
			// Copy in the line.
			memcpy( buf, &gMap[MAPX*j], MAPX + 1 );
			buf[MAPX] = 0;

			// Wildly inefficient demo code.
			unsigned size = path.size();

			for ( unsigned k = 0; k < size; ++k )
			{
				int x, y;
				NodeToXY( path[k], &x, &y );

				if ( y == j ) { buf[x] = '0' + k % 10; }
			}

			if ( showConsidered )
			{
				for ( unsigned k = 0; k < stateVec.size(); ++k )
				{
					int x, y;
					NodeToXY( stateVec[k], &x, &y );

					if ( y == j ) { buf[x] = 'x'; }
				}
			}

			// Insert the player
			if ( j == playerY ) { buf[playerX] = 'i'; }

			printf( "%d%s\n", j % 10, buf );
		}
	}

	void NodeToXY( void* node, int* x, int* y )
	{
		int index = ( int )node;
		*y = index / MAPX;
		*x = index - *y * MAPX;
	}

	void* XYToNode( int x, int y ) { return ( void* ) ( y * MAPX + x );   }

	virtual float LeastCostEstimate( void* nodeStart, void* nodeEnd )
	{
		int xStart, yStart, xEnd, yEnd;
		NodeToXY( nodeStart, &xStart, &yStart );
		NodeToXY( nodeEnd, &xEnd, &yEnd );

		/* Compute the minimum path cost using distance measurement. It is possible
		   to compute the exact minimum path using the fact that you can move only
		   on a straight line or on a diagonal, and this will yield a better result.
		*/
		int dx = xStart - xEnd;
		int dy = yStart - yEnd;
		return ( float ) sqrt( ( double )( dx * dx ) + ( double )( dy * dy ) );
	}

	virtual void AdjacentCost( void* node, std::vector< StateCost > *neighbors )
	{
		int x, y;
		const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
		const float cost[8] = { 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f };

		NodeToXY( node, &x, &y );

		for ( int i = 0; i < 8; ++i )
		{
			int nx = x + dx[i], ny = y + dy[i];
			int pass = Passable( nx, ny );

			if ( pass > 0 )
			{
				StateCost nodeCost = { XYToNode( nx, ny ), ( pass == 1 || doorsOpen )  ? cost[i] : FLT_MAX };
				neighbors->push_back( nodeCost );
			}
		}
	}

	virtual void PrintStateInfo( void* node )
	{
		int x, y;
		NodeToXY( node, &x, &y );
		printf( "(%d,%d)", x, y );
	}
};

int main()
{
	{
		// Run the test code.
		Dungeon test;

		const int NUM_TEST = 5;
		int tx[NUM_TEST]  = {   24,   25,   10,   6, 0  }; // x of test
		int ty[NUM_TEST]  = {   9, 9, 5, 5, 0  }; // y of test
		int door[NUM_TEST]   = {   0, 0, 0, 1, 0  }; // toggle door? (before move)
		int result[NUM_TEST] = { MicroPather::SOLVED, MicroPather::SOLVED, MicroPather::NO_SOLUTION, MicroPather::SOLVED, MicroPather::SOLVED };
		unsigned check[NUM_TEST]   = { 139640, 884, 0, 129313, 2914 };

		for ( int i = 0; i < NUM_TEST; ++i )
		{
			if ( door[i] ) { test.ToggleDoor(); }

			int _result = test.SetPos( tx[i], ty[i] );

			if ( _result == result[i] )
			{
				// result good.
				if ( _result == MicroPather::SOLVED )
				{
					if ( test.Checksum() == check[i] )
					{
						printf( ">> Test %d to (%d,%d) ok\n", i, tx[i], ty[i] );
					}
					else
					{
						printf( ">> Test %d to (%d,%d) BAD CHECKSUM\n", i, tx[i], ty[i] );
					}
				}
				else
				{
					printf( ">> Test %d to (%d,%d) ok\n", i, tx[i], ty[i] );
				}
			}
			else
			{
				printf( ">> Test %d to (%d,%d) BAD RESULT\n", i, tx[i], ty[i] );
			}
		}
	}

	Dungeon dungeon;
	bool done = false;
	char buf[ 256 ];

	while ( !done )
	{
		dungeon.Print();
		printf( "\n# # to move, q to quit, r to redraw, d to toggle doors, t for touched\n" );

		std::cin.getline( buf, 256 );

		if ( *buf )
		{
			if ( buf[0] == 'q' )
			{
				done = true;
			}
			else if ( buf[0] == 'd' )
			{
				dungeon.ToggleDoor();
				dungeon.ClearPath();
			}
			else if ( buf[0] == 't' )
			{
				dungeon.ToggleTouched();
			}
			else if ( buf[0] == 'r' )
			{
				dungeon.ClearPath();
			}
			else if ( isdigit( buf[0] ) )
			{
				int x, y;
				sscanf( buf, "%d %d", &x, &y );  // sleazy, I know
				dungeon.SetPos( x, y );
			}
		}
		else
		{
			dungeon.ClearPath();
		}
	}

	return 0;
}

#endif // PATHER_TEST


#ifdef MICROPATHER_SPEED_TEST

#include <ctype.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include <vector>

using namespace micropather;

//#define PROFILING_RUN

#ifdef _MSC_VER
typedef unsigned __int64 U64;
#else
typedef unsigned long long U64;
#endif
typedef unsigned int U32;

#ifdef _MSC_VER
inline U64 FastTime()
{
	union
	{
		U64 result;
		struct
		{
			U32 lo;
			U32 hi;
		} split;
	} u;
	u.result = 0;

	_asm
	{
		//pushad;   // don't need - aren't using "emit"
		cpuid;      // force all previous instructions to complete - else out of order execution can confuse things
		rdtsc;
		mov u.split.hi, edx;
		mov u.split.lo, eax;
		//popad;
	}
	return u.result;
}
#else
#define rdtscll(val)  __asm__ __volatile__ ("rdtsc" : "=A" (val))
inline U64 FastTime()
{
	U64 val;
	rdtscll( val );
	return val;
}
#endif

inline int TimeDiv( U64 time, int count )
{
	if ( count == 0 ) { count = 1; }

	U64 timePer = time / ( U64 )count;
	MPASSERT( timePer < INT_MAX );
	return ( int ) timePer / 1000;
}

const int MAPX = 90;
const int MAPY = 20;
const char gMap[MAPX* MAPY+1] =
   //"012345678901234567890123456789"
   "     |      |                |     |      ||               |     |      |                |"
   "     |      |----+    |      +     |      ||---+    |      +     |      |----+    |      +"
   "---+ +---  -+      +--+--+    ---+ +---  -+|     +--+--+    ---+ +---  -+      +--+--+    "
   "   |                     +-- +   |        ||           +-- +   |                     +-- +"
   "        +----+  +---+                     ||  +---+                 +----+  +---+         "
   "---+ +  +    +            |   ---+ +                    |  2---+ +  +    +            |   "
   "   | |  +----+    +----+  +--+   | |      ||    +----+  +--+322| |  +----+    +----+  +--+"
   "     |            |    |           |      ||    |    |   222232  |            |    |      "
   "   | +-------+  +-+    |------------------+|  +-+    |--+  2223| +-------+  +-+    |--+   "
   "---+                   |                  ||         |  222+---+                   |     +"
   "     |      |          |                  ||          22233|     |      |                |"
   "     |      |----+    ++                  ||---+3333|    22+     |      |----+    |      +"
   "---+ +---  -+      +--+-------------------||22223+--+--+    ---+ +---  -+      +--+--+    "
   "   |                     +-- +   |          22223333   +-- +   |                     +-- +"
   "        +----+  +---+                 +---+|  +---+     222         +----+  +---+         "
   "---+ +  +    +            |   ---+ +  +   +|            |222---+ +  +    +            |   "
   "   | |  +----+    +----+  +--+   | |  +---+|  22+----+  +--+233|2|  +----+    +----+  +--+"
   "     |            |    |           |      ||2222|    |    2223333|            |    |      "
   "   | +-------+  +-+    |--+      | +------++  +-+    |--+      |2+-------+  +-+    |--+   "
   "---+                   |     +---+        ||         |     +---+                   |      ";

class Dungeon : public Graph
{
	public:
	std::vector<void*> path;
	MicroPather* aStar;
	int maxDir;

	Dungeon()
	{
		aStar = new MicroPather( this, MAPX * MAPY / 4, 6 );
		maxDir = 4;
	}

	virtual ~Dungeon()
	{
		delete aStar;
	}

	int Passable( int nx, int ny )
	{
		if (    nx >= 0 && nx < MAPX
		        && ny >= 0 && ny < MAPY )
		{
			int index = ny * MAPX + nx;
			char c = gMap[ index ];

			if ( c == ' ' )
			{
				return 1;
			}
			else if ( c >= '1' && c <= '9' )
			{
				int val = c - '0';
				MPASSERT( val > 0 );
				return val;
			}
		}

		return 0;
	}

	void NodeToXY( void* node, int* x, int* y )
	{
		int index = ( int )node;
		*y = index / MAPX;
		*x = index - *y * MAPX;
	}

	void* XYToNode( int x, int y )
	{
		return ( void* ) ( y * MAPX + x );
	}


	virtual float LeastCostEstimate( void* nodeStart, void* nodeEnd )
	{
		int xStart, yStart, xEnd, yEnd;
		NodeToXY( nodeStart, &xStart, &yStart );
		NodeToXY( nodeEnd, &xEnd, &yEnd );

		int dx = xStart - xEnd;
		int dy = yStart - yEnd;
		return ( float ) sqrt( ( double )( dx * dx ) + ( double )( dy * dy ) );
	}

	virtual void  AdjacentCost( void* node, std::vector< StateCost > *neighbors )
	{
		int x, y;
		//             E  N  W   S     NE  NW  SW SE
		const int dx[8] = { 1, 0, -1, 0, 1, -1, -1, 1 };
		const int dy[8] = { 0, -1, 0, 1, -1, -1, 1, 1 };
		const float cost[8] = { 1.0f, 1.0f, 1.0f, 1.0f,
		                        1.41f, 1.41f, 1.41f, 1.41f
		                      };

		NodeToXY( node, &x, &y );

		for ( int i = 0; i < maxDir; ++i )
		{
			int nx = x + dx[i];
			int ny = y + dy[i];

			int pass = Passable( nx, ny );

			if ( pass > 0 )
			{
				// Normal floor
				StateCost nodeCost = { XYToNode( nx, ny ), cost[i] * ( float )( pass ) };
				neighbors->push_back( nodeCost );
			}
		}
	}

	virtual void  PrintStateInfo( void* node )
	{
		int x, y;
		NodeToXY( node, &x, &y );
		printf( "(%2d,%2d)", x, y );
	}

};


int main( int argc, const char* argv[] )
{
	Dungeon dungeon;

#ifdef PROFILING_RUN
	const int NUM_TEST = 117;
#else
	const int NUM_TEST = 389;
#endif

	int   indexArray[ NUM_TEST ];
	float costArray[ NUM_TEST ];
	unsigned   checksumArray[ NUM_TEST ];
	U64   timeArray[ NUM_TEST ];
	int   resultArray[ NUM_TEST ];

	int i;

	bool useBinaryHash = false;
	bool useList = false;
	bool debug = false;

#ifdef DEBUG
	debug = true;
#endif
#ifdef USE_BINARY_HASH
	useBinaryHash = true;
#endif
#ifdef USE_LIST
	useList = true;
#endif

	printf( "SpeedTest binaryHash=%s list=%s debug=%s\n",
	        useBinaryHash ? "true" : "false",
	        useList ? "true" : "false",
	        debug ? "true" : "false" );

	// Set up the test indices. Distribute, then randomize.
	for ( i = 0; i < NUM_TEST; ++i )
	{
		indexArray[i] = ( MAPX * MAPY ) * i / NUM_TEST;
		costArray[i] = 0.0f;

		int y = indexArray[i] / MAPX;
		int x = indexArray[i] - MAPX * y;

		while ( !dungeon.Passable( x, y ) )
		{
			indexArray[i] += 1;
			y = indexArray[i] / MAPX;
			x = indexArray[i] - MAPX * y;
		}
	}

	for ( i = 0; i < NUM_TEST; ++i )
	{
		int swapWith = rand() % NUM_TEST;
		int temp = indexArray[i];
		indexArray[i] = indexArray[swapWith];
		indexArray[swapWith] = temp;
	}

	int compositeScore = 0;

	for ( int numDir = 4; numDir <= 8; numDir += 4 )
	{
		dungeon.maxDir = numDir;
		dungeon.aStar->Reset();

		for ( int reset = 0; reset <= 1; ++reset )
		{
//			if ( reset && numDir > PathNode::MAX_CACHE )
//				continue;   // setting makes no sense - shouldn't cache when cache too small

			clock_t clockStart = clock();

			for ( i = 0; i < NUM_TEST; ++i )
			{
				if ( reset )
				{
					dungeon.aStar->Reset();
				}

				int startState = indexArray[i];
				int endState = indexArray[ ( i==( NUM_TEST-1 ) ) ? 0 : i+1];

				U64 start = FastTime();
				resultArray[i] = dungeon.aStar->Solve( ( void* )startState, ( void* )endState, &dungeon.path, &costArray[i] );
				U64 end = FastTime();

				if ( !reset )
				{
					checksumArray[i] = 0;
				}

				timeArray[i] = end - start;

				if ( resultArray[i] == MicroPather::SOLVED )
				{
					if ( !reset )
					{
						checksumArray[i] = dungeon.aStar->Checksum();
					}
					else
					{
						MPASSERT( checksumArray[i] == dungeon.aStar->Checksum() );
					}
				}
			}

			clock_t clockEnd = clock();

#ifndef PROFILING_RUN
			// -------- Results ------------ //
			const float shortPath = ( float )( MAPX / 4 );
			const float medPath = ( float )( MAPX / 2 );

			int count[5] = { 0, 0, 0, 0 };   // short, med, long, fail short, fail long
			U64 time[5] = { 0, 0, 0, 0 };

			for ( i = 0; i < NUM_TEST; ++i )
			{
				if ( resultArray[i] == MicroPather::SOLVED )
				{
					if ( costArray[i] < shortPath )
					{
						++count[0];
						time[0] += timeArray[i];
					}
					else if ( costArray[i] < medPath )
					{
						++count[1];
						time[1] += timeArray[i];
					}
					else
					{
						++count[2];
						time[2] += timeArray[i];
					}
				}
				else if ( resultArray[i] == MicroPather::NO_SOLUTION )
				{

					int startState = indexArray[i];
					int endState = indexArray[ ( i==( NUM_TEST-1 ) ) ? 0 : i+1];
					int startX, startY, endX, endY;
					dungeon.NodeToXY( ( void* )startState, &startX, &startY );
					dungeon.NodeToXY( ( void* )endState, &endX, &endY );

					int distance = abs( startX - endX ) + abs( startY - endY );

					if ( distance < shortPath )
					{
						++count[3];
						time[3] += timeArray[i];
					}
					else
					{
						++count[4];
						time[4] += timeArray[i];
					}
				}
			}

			printf( "Average of %d runs. Reset=%s. Dir=%d.\n",
			        NUM_TEST, reset ? "true" : "false", numDir );
			printf( "short(%4d, cutoff=%.1f)= %d\n", count[0], shortPath, TimeDiv( time[0], count[0] ) );
			printf( "med  (%4d, cutoff=%.1f)= %d\n", count[1], medPath, TimeDiv( time[1], count[1] ) );
			printf( "long (%4d)             = %d\n", count[2], TimeDiv( time[2], count[2] ) );
			printf( "fail (%4d) short       = %d\n", count[3], TimeDiv( time[3], count[3] ) );
			printf( "fail (%4d) long        = %d\n", count[4], TimeDiv( time[4], count[4] ) );

			int total = 0;

			for ( int k = 0; k < 5; ++k )
			{
				total += TimeDiv( time[k], count[k] );
			}

			printf( "Average                = %d\n", total / 5 );
			compositeScore += total;

			//printf( "Average time / test     = %.3f ms\n\n", 1000.0f * float(clockEnd-clockStart)/float( NUM_TEST*CLOCKS_PER_SEC ) );
#endif
			//dungeon.aStar->DumpHashTable();
		}
	}

	printf( "Composite average = %d\n", compositeScore / ( 5 * 4 ) );

	return 0;
}

#endif // SPEED_TEST
