/** @file GuillotineBinPack.h
   @author Jukka Jylï¿_nki

   @brief Implements different bin packer algorithms that use the GUILLOTINE data structure.

   This work is released to Public Domain, do whatever you want with it.

	// Taken over by Sergey Kosarevsky, 2012
*/
#pragma once

#include <vector>

namespace Guillotine
{

struct RectSize
{
	int width;
	int height;
};

struct Rect
{
	int x;
	int y;
	int width;
	int height;
};

/// Performs a lexicographic compare on (rect short side, rect long side).
/// @return -1 if the smaller side of a is shorter than the smaller side of b, 1 if the other way around.
///   If they are equal, the larger side length is used as a tie-breaker.
///   If the rectangles are of same size, returns 0.
int CompareRectShortSide( const Rect& a, const Rect& b );

/// Performs a lexicographic compare on (x, y, width, height).
int NodeSortCmp( const Rect& a, const Rect& b );

/// Returns true if a is contained in b.
bool IsContainedIn( const Rect& a, const Rect& b );

class DisjointRectCollection
{
public:
	std::vector<Rect> rects;

	bool Add( const Rect& r )
	{
		// Degenerate rectangles are ignored.
		if ( r.width == 0 || r.height == 0 )
		{
			return true;
		}

		if ( !Disjoint( r ) )
		{
			return false;
		}

		rects.push_back( r );
		return true;
	}

	void Clear()
	{
		rects.clear();
	}

	bool Disjoint( const Rect& r ) const
	{
		// Degenerate rectangles are ignored.
		if ( r.width == 0 || r.height == 0 )
		{
			return true;
		}

		for ( size_t i = 0; i < rects.size(); ++i )
			if ( !Disjoint( rects[i], r ) )
			{
				return false;
			}

		return true;
	}

	static bool Disjoint( const Rect& a, const Rect& b )
	{
		if ( a.x + a.width <= b.x ||
		     b.x + b.width <= a.x ||
		     a.y + a.height <= b.y ||
		     b.y + b.height <= a.y )
		{
			return true;
		}

		return false;
	}
};

/** GuillotineBinPack implements different variants of bin packer algorithms that use the GUILLOTINE data structure
   to keep track of the free space of the bin where rectangles may be placed. */
class GuillotineBinPack
{
public:
	/// The initial bin size will be (0,0). Call Init to set the bin size.
	GuillotineBinPack();

	/// Initializes a new bin of the given size.
	GuillotineBinPack( int width, int height );

	/// (Re)initializes the packer to an empty bin of width x height units. Call whenever
	/// you need to restart with a new bin.
	void Init( int width, int height );

	/// Specifies the different choice heuristics that can be used when deciding which of the free subrectangles
	/// to place the to-be-packed rectangle into.
	enum FreeRectChoiceHeuristic
	{
		RectBestAreaFit, ///< -BAF
		RectBestShortSideFit, ///< -BSSF
		RectBestLongSideFit, ///< -BLSF
		RectWorstAreaFit, ///< -WAF
		RectWorstShortSideFit, ///< -WSSF
		RectWorstLongSideFit ///< -WLSF
	};

	/// Specifies the different choice heuristics that can be used when the packer needs to decide whether to
	/// subdivide the remaining free space in horizontal or vertical direction.
	enum GuillotineSplitHeuristic
	{
		SplitShorterLeftoverAxis, ///< -SLAS
		SplitLongerLeftoverAxis, ///< -LLAS
		SplitMinimizeArea, ///< -MINAS, Try to make a single big rectangle at the expense of making the other small.
		SplitMaximizeArea, ///< -MAXAS, Try to make both remaining rectangles as even-sized as possible.
		SplitShorterAxis, ///< -SAS
		SplitLongerAxis ///< -LAS
	};

	/// Inserts a single rectangle into the bin. The packer might rotate the rectangle, in which case the returned
	/// struct will have the width and height values swapped.
	/// @param merge If true, performs free Rectangle Merge procedure after packing the new rectangle. This procedure
	///      tries to defragment the list of disjoint free rectangles to improve packing performance, but also takes up
	///      some extra time.
	/// @param rectChoice The free rectangle choice heuristic rule to use.
	/// @param splitMethod The free rectangle split heuristic rule to use.
	Rect Insert( int width, int height, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod );

	/// Inserts a list of rectangles into the bin.
	/// @param rects The list of rectangles to add. This list will be destroyed in the packing process.
	/// @param dst The outputted list of rectangles. Note that the indices will not correspond to the input indices.
	/// @param merge If true, performs Rectangle Merge operations during the packing process.
	/// @param rectChoice The free rectangle choice heuristic rule to use.
	/// @param splitMethod The free rectangle split heuristic rule to use.
	void Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, bool merge,
	             FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod );

// Implements GUILLOTINE-MAXFITTING, an experimental heuristic that's really cool but didn't quite work in practice.
//	void InsertMaxFitting(std::vector<RectSize> &rects, std::vector<Rect> &dst, bool merge,
//		FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);

	/// Computes the ratio of used/total surface area. 0.00 means no space is yet used, 1.00 means the whole bin is used.
	float Occupancy() const;

	/// Returns the internal list of disjoint rectangles that track the free area of the bin. You may alter this vector
	/// any way desired, as long as the end result still is a list of disjoint rectangles.
	std::vector<Rect> &GetFreeRectangles() { return freeRectangles; }

	/// Returns the list of packed rectangles. You may alter this vector at will, for example, you can move a Rect from
	/// this list to the Free Rectangles list to free up space on-the-fly, but notice that this causes fragmentation.
	std::vector<Rect> &GetUsedRectangles() { return usedRectangles; }

	/// Performs a Rectangle Merge operation. This procedure looks for adjacent free rectangles and merges them if they
	/// can be represented with a single rectangle. Takes up Theta(|freeRectangles|^2) time.
	void MergeFreeList();

private:
	int binWidth;
	int binHeight;

	/// Stores a list of all the rectangles that we have packed so far. This is used only to compute the Occupancy ratio,
	/// so if you want to have the packer consume less memory, this can be removed.
	std::vector<Rect> usedRectangles;

	/// Stores a list of rectangles that represents the free area of the bin. This rectangles in this list are disjoint.
	std::vector<Rect> freeRectangles;

#ifdef _DEBUG
	/// Used to track that the packer produces proper packings.
	DisjointRectCollection disjointRects;
#endif

	/// Goes through the list of free rectangles and finds the best one to place a rectangle of given size into.
	/// Running time is Theta(|freeRectangles|).
	/// @param nodeIndex [out] The index of the free rectangle in the freeRectangles array into which the new
	///      rect was placed.
	/// @return A Rect structure that represents the placement of the new rect into the best free rectangle.
	Rect FindPositionForNewNode( int width, int height, FreeRectChoiceHeuristic rectChoice, int* nodeIndex );

	static int ScoreByHeuristic( int width, int height, const Rect& freeRect, FreeRectChoiceHeuristic rectChoice );
	// The following functions compute (penalty) score values if a rect of the given size was placed into the
	// given free rectangle. In these score values, smaller is better.

	static int ScoreBestAreaFit( int width, int height, const Rect& freeRect );
	static int ScoreBestShortSideFit( int width, int height, const Rect& freeRect );
	static int ScoreBestLongSideFit( int width, int height, const Rect& freeRect );

	static int ScoreWorstAreaFit( int width, int height, const Rect& freeRect );
	static int ScoreWorstShortSideFit( int width, int height, const Rect& freeRect );
	static int ScoreWorstLongSideFit( int width, int height, const Rect& freeRect );

	/// Splits the given L-shaped free rectangle into two new free rectangles after placedRect has been placed into it.
	/// Determines the split axis by using the given heuristic.
	void SplitFreeRectByHeuristic( const Rect& freeRect, const Rect& placedRect, GuillotineSplitHeuristic method );

	/// Splits the given L-shaped free rectangle into two new free rectangles along the given fixed split axis.
	void SplitFreeRectAlongAxis( const Rect& freeRect, const Rect& placedRect, bool splitHorizontal );
};

/** MaxRectsBinPack implements the MAXRECTS data structure and different bin packing algorithms that
   use this structure. */
class MaxRectsBinPack
{
public:
	/// Instantiates a bin of size (0,0). Call Init to create a new bin.
	MaxRectsBinPack();

	/// Instantiates a bin of the given size.
	MaxRectsBinPack( int width, int height );

	/// (Re)initializes the packer to an empty bin of width x height units. Call whenever
	/// you need to restart with a new bin.
	void Init( int width, int height );

	/// Specifies the different heuristic rules that can be used when deciding where to place a new rectangle.
	enum FreeRectChoiceHeuristic
	{
		RectBestShortSideFit, ///< -BSSF: Positions the rectangle against the short side of a free rectangle into which it fits the best.
		RectBestLongSideFit, ///< -BLSF: Positions the rectangle against the long side of a free rectangle into which it fits the best.
		RectBestAreaFit, ///< -BAF: Positions the rectangle into the smallest free rect into which it fits.
		RectBottomLeftRule, ///< -BL: Does the Tetris placement.
		RectContactPointRule ///< -CP: Choosest the placement where the rectangle touches other rects as much as possible.
	};

	/// Inserts the given list of rectangles in an offline/batch mode, possibly rotated.
	/// @param rects The list of rectangles to insert. This vector will be destroyed in the process.
	/// @param dst [out] This list will contain the packed rectangles. The indices will not correspond to that of rects.
	/// @param method The rectangle placement rule to use when packing.
	void Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, FreeRectChoiceHeuristic method );

	/// Inserts a single rectangle into the bin, possibly rotated.
	Rect Insert( int width, int height, FreeRectChoiceHeuristic method );

	/// Computes the ratio of used surface area to the total bin area.
	float Occupancy() const;

private:
	int binWidth;
	int binHeight;

	std::vector<Rect> usedRectangles;
	std::vector<Rect> freeRectangles;

	/// Computes the placement score for placing the given rectangle with the given method.
	/// @param score1 [out] The primary placement score will be outputted here.
	/// @param score2 [out] The secondary placement score will be outputted here. This isu sed to break ties.
	/// @return This struct identifies where the rectangle would be placed if it were placed.
	Rect ScoreRect( int width, int height, FreeRectChoiceHeuristic method, int& score1, int& score2 ) const;

	/// Places the given rectangle into the bin.
	void PlaceRect( const Rect& node );

	/// Computes the placement score for the -CP variant.
	int ContactPointScoreNode( int x, int y, int width, int height ) const;

	Rect FindPositionForNewNodeBottomLeft( int width, int height, int& bestY, int& bestX ) const;
	Rect FindPositionForNewNodeBestShortSideFit( int width, int height, int& bestShortSideFit, int& bestLongSideFit ) const;
	Rect FindPositionForNewNodeBestLongSideFit( int width, int height, int& bestShortSideFit, int& bestLongSideFit ) const;
	Rect FindPositionForNewNodeBestAreaFit( int width, int height, int& bestAreaFit, int& bestShortSideFit ) const;
	Rect FindPositionForNewNodeContactPoint( int width, int height, int& contactScore ) const;

	/// @return True if the free node was split.
	bool SplitFreeNode( Rect freeNode, const Rect& usedNode );

	/// Goes through the free rectangle list and removes any redundant entries.
	void PruneFreeList();
};

/** ShelfBinPack implements different bin packing algorithms that use the SHELF data structure. ShelfBinPack
also uses GuillotineBinPack for the waste map if it is enabled. */
class ShelfBinPack
{
public:
	/// Default ctor initializes a bin of size (0,0). Call Init() to init an instance.
	ShelfBinPack();

	ShelfBinPack( int width, int height, bool useWasteMap );

	/// Clears all previously packed rectangles and starts packing from scratch into a bin of the given size.
	void Init( int width, int height, bool useWasteMap );

	/// Defines different heuristic rules that can be used in the packing process.
	enum ShelfChoiceHeuristic
	{
		ShelfNextFit, ///< -NF: We always put the new rectangle to the last open shelf.
		ShelfFirstFit, ///< -FF: We test each rectangle against each shelf in turn and pack it to the first where it fits.
		ShelfBestAreaFit, ///< -BAF: Choose the shelf with smallest remaining shelf area.
		ShelfWorstAreaFit, ///< -WAF: Choose the shelf with the largest remaining shelf area.
		ShelfBestHeightFit, ///< -BHF: Choose the smallest shelf (height-wise) where the rectangle fits.
		ShelfBestWidthFit, ///< -BWF: Choose the shelf that has the least remaining horizontal shelf space available after packing.
		ShelfWorstWidthFit, ///< -WWF: Choose the shelf that will have most remainining horizontal shelf space available after packing.
	};

	/// Inserts a single rectangle into the bin. The packer might rotate the rectangle, in which case the returned
	/// struct will have the width and height values swapped.
	/// @param method The heuristic rule to use for choosing a shelf if multiple ones are possible.
	Rect Insert( int width, int height, ShelfChoiceHeuristic method );

	/// Computes the ratio of used surface area to the total bin area.
	float Occupancy() const;

private:
	int binWidth;
	int binHeight;

	/// Stores the starting y-coordinate of the latest (topmost) shelf.
	int currentY;

	/// Tracks the total consumed surface area.
	unsigned long usedSurfaceArea;

	/// If true, the following GuillotineBinPack structure is used to recover the SHELF data structure from losing space.
	bool useWasteMap;
	GuillotineBinPack wasteMap;

	/// Describes a horizontal slab of space where rectangles may be placed.
	struct Shelf
	{
		/// The x-coordinate that specifies where the used shelf space ends.
		/// Space between [0, currentX[ has been filled with rectangles, [currentX, binWidth[ is still available for filling.
		int currentX;

		/// The y-coordinate of where this shelf starts, inclusive.
		int startY;

		/// Specifices the height of this shelf. The topmost shelf is "open" and its height may grow.
		int height;

		/// Lists all the rectangles in this shelf.
		std::vector<Rect> usedRectangles;
	};

	std::vector<Shelf> shelves;

	/// Parses through all rectangles added to the given shelf and adds the gaps between the rectangle tops and the shelf
	/// ceiling into the waste map. This is called only once when the shelf is being closed and a new one is opened.
	void MoveShelfToWasteMap( Shelf& shelf );

	/// Returns true if the rectangle of size width*height fits on the given shelf, possibly rotated.
	/// @param canResize If true, denotes that the shelf height may be increased to fit the object.
	bool FitsOnShelf( const Shelf& shelf, int width, int height, bool canResize ) const;

	/// Measures and if desirable, flips width and height so that the rectangle fits the given shelf the best.
	/// @param width [in,out] The width of the rectangle.
	/// @param height [in,out] The height of the rectangle.
	void RotateToShelf( const Shelf& shelf, int& width, int& height ) const;

	/// Adds the rectangle of size width*height into the given shelf, possibly rotated.
	/// @param newNode [out] The added rectangle will be returned here.
	void AddToShelf( Shelf& shelf, int width, int height, Rect& newNode );

	/// Returns true if there is still room in the bin to start a new shelf of the given height.
	bool CanStartNewShelf( int height ) const;

	/// Creates a new shelf of the given starting height, which will become the topmost 'open' shelf.
	void StartNewShelf( int startingHeight );
};

/** Implements bin packing algorithms that use the SKYLINE data structure to store the bin contents. Uses
   GuillotineBinPack as the waste map. */
class SkylineBinPack
{
public:
	/// Instantiates a bin of size (0,0). Call Init to create a new bin.
	SkylineBinPack();

	/// Instantiates a bin of the given size.
	SkylineBinPack( int binWidth, int binHeight, bool useWasteMap );

	/// (Re)initializes the packer to an empty bin of width x height units. Call whenever
	/// you need to restart with a new bin.
	void Init( int binWidth, int binHeight, bool useWasteMap );

	/// Defines the different heuristic rules that can be used to decide how to make the rectangle placements.
	enum LevelChoiceHeuristic
	{
		LevelBottomLeft,
		LevelMinWasteFit
	};

	/// Inserts the given list of rectangles in an offline/batch mode, possibly rotated.
	/// @param rects The list of rectangles to insert. This vector will be destroyed in the process.
	/// @param dst [out] This list will contain the packed rectangles. The indices will not correspond to that of rects.
	/// @param method The rectangle placement rule to use when packing.
	void Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, LevelChoiceHeuristic method );

	/// Inserts a single rectangle into the bin, possibly rotated.
	Rect Insert( int width, int height, LevelChoiceHeuristic method );

	/// Computes the ratio of used surface area to the total bin area.
	float Occupancy() const;

private:
	int binWidth;
	int binHeight;

#ifdef _DEBUG
	DisjointRectCollection disjointRects;
#endif

	/// Represents a single level (a horizontal line) of the skyline/horizon/envelope.
	struct SkylineNode
	{
		/// The starting x-coordinate (leftmost).
		int x;

		/// The y-coordinate of the skyline level line.
		int y;

		/// The line width. The ending coordinate (inclusive) will be x+width-1.
		int width;
	};

	std::vector<SkylineNode> skyLine;

	unsigned long usedSurfaceArea;

	/// If true, we use the GuillotineBinPack structure to recover wasted areas into a waste map.
	bool useWasteMap;
	GuillotineBinPack wasteMap;

	Rect InsertBottomLeft( int width, int height );
	Rect InsertMinWaste( int width, int height );

	Rect FindPositionForNewNodeMinWaste( int width, int height, int& bestHeight, int& bestWastedArea, int& bestIndex ) const;
	Rect FindPositionForNewNodeBottomLeft( int width, int height, int& bestHeight, int& bestWidth, int& bestIndex ) const;

	bool RectangleFits( int skylineNodeIndex, int width, int height, int& y ) const;
	bool RectangleFits( int skylineNodeIndex, int width, int height, int& y, int& wastedArea ) const;
	int ComputeWastedArea( int skylineNodeIndex, int width, int height, int y ) const;

	void AddWasteMapArea( int skylineNodeIndex, int width, int height, int y );

	void AddSkylineLevel( int skylineNodeIndex, const Rect& rect );

	/// Merges all skyline nodes that are at the same level.
	void MergeSkylines();
};

} // namespace Guillotine

/*
 * 30/01/2012
     It's here
*/
