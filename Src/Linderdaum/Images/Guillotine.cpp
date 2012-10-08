/** @file GuillotineBinPack.cpp
   @author Jukka Jylänki

   @brief Implements different bin packer algorithms that use the GUILLOTINE data structure.

   This work is released to Public Domain, do whatever you want with it.

	// Taken over by Sergey Kosarevsky, 2012
*/

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

#include <utility>
#include <iostream>
#include <cassert>
#include <limits>
#include <cstdlib>
#include <cstring>

#include "Guillotine.h"

using namespace std;

namespace Guillotine
{


bool IsContainedIn( const Rect& a, const Rect& b )
{
	return a.x >= b.x && a.y >= b.y
	       && a.x + a.width <= b.x + b.width
	       && a.y + a.height <= b.y + b.height;
}

#if !defined(_DEBUG)
#if defined( assert )
#	undef assert
#endif
#	define assert( expr )
#endif

GuillotineBinPack::GuillotineBinPack()
	: binWidth( 0 ),
	  binHeight( 0 )
{
}

GuillotineBinPack::GuillotineBinPack( int width, int height )
{
	Init( width, height );
}

void GuillotineBinPack::Init( int width, int height )
{
	binWidth = width;
	binHeight = height;

#ifdef _DEBUG
	disjointRects.Clear();
#endif

	// Clear any memory of previously packed rectangles.
	usedRectangles.clear();

	// We start with a single big free rectangle that spans the whole bin.
	Rect n;
	n.x = 0;
	n.y = 0;
	n.width = width;
	n.height = height;

	freeRectangles.clear();
	freeRectangles.push_back( n );
}

void GuillotineBinPack::Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, bool merge,
                                FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod )
{
	dst.clear();

	// Remember variables about the best packing choice we have made so far during the iteration process.
	int bestFreeRect = 0;
	int bestRect = 0;
	bool bestFlipped = false;

	// Pack rectangles one at a time until we have cleared the rects array of all rectangles.
	// rects will get destroyed in the process.
	while ( rects.size() > 0 )
	{
		// Stores the penalty score of the best rectangle placement - bigger=worse, smaller=better.
		int bestScore = std::numeric_limits<int>::max();

		for ( size_t i = 0; i < freeRectangles.size(); ++i )
		{
			for ( size_t j = 0; j < rects.size(); ++j )
			{
				// If this rectangle is a perfect match, we pick it instantly.
				if ( rects[j].width == freeRectangles[i].width && rects[j].height == freeRectangles[i].height )
				{
					bestFreeRect = i;
					bestRect = j;
					bestFlipped = false;
					bestScore = std::numeric_limits<int>::min();
					i = freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
					break;
				}
				// If flipping this rectangle is a perfect match, pick that then.
				else if ( rects[j].height == freeRectangles[i].width && rects[j].width == freeRectangles[i].height )
				{
					bestFreeRect = i;
					bestRect = j;
					bestFlipped = true;
					bestScore = std::numeric_limits<int>::min();
					i = freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
					break;
				}
				// Try if we can fit the rectangle upright.
				else if ( rects[j].width <= freeRectangles[i].width && rects[j].height <= freeRectangles[i].height )
				{
					int score = ScoreByHeuristic( rects[j].width, rects[j].height, freeRectangles[i], rectChoice );

					if ( score < bestScore )
					{
						bestFreeRect = i;
						bestRect = j;
						bestFlipped = false;
						bestScore = score;
					}
				}
				// If not, then perhaps flipping sideways will make it fit?
				else if ( rects[j].height <= freeRectangles[i].width && rects[j].width <= freeRectangles[i].height )
				{
					int score = ScoreByHeuristic( rects[j].height, rects[j].width, freeRectangles[i], rectChoice );

					if ( score < bestScore )
					{
						bestFreeRect = i;
						bestRect = j;
						bestFlipped = true;
						bestScore = score;
					}
				}
			}
		}

		// If we didn't manage to find any rectangle to pack, abort.
		if ( bestScore == std::numeric_limits<int>::max() )
		{
			return;
		}

		// Otherwise, we're good to go and do the actual packing.
		Rect newNode;
		newNode.x = freeRectangles[bestFreeRect].x;
		newNode.y = freeRectangles[bestFreeRect].y;
		newNode.width = rects[bestRect].width;
		newNode.height = rects[bestRect].height;

		if ( bestFlipped )
		{
			std::swap( newNode.width, newNode.height );
		}

		// Remove the free space we lost in the bin.
		SplitFreeRectByHeuristic( freeRectangles[bestFreeRect], newNode, splitMethod );
		freeRectangles.erase( freeRectangles.begin() + bestFreeRect );

		// Remove the rectangle we just packed from the input list.
		rects.erase( rects.begin() + bestRect );

		// Perform a Rectangle Merge step if desired.
		if ( merge )
		{
			MergeFreeList();
		}

		// Remember the new used rectangle.
		usedRectangles.push_back( newNode );

		// Check that we're really producing correct packings here.
		assert( disjointRects.Add( newNode ) == true );
	}
}

/// @return True if r fits inside freeRect (possibly rotated).
bool Fits( const RectSize& r, const Rect& freeRect )
{
	return ( r.width <= freeRect.width && r.height <= freeRect.height ) ||
	       ( r.height <= freeRect.width && r.width <= freeRect.height );
}

/// @return True if r fits perfectly inside freeRect, i.e. the leftover area is 0.
bool FitsPerfectly( const RectSize& r, const Rect& freeRect )
{
	return ( r.width == freeRect.width && r.height == freeRect.height ) ||
	       ( r.height == freeRect.width && r.width == freeRect.height );
}

Rect GuillotineBinPack::Insert( int width, int height, bool merge, FreeRectChoiceHeuristic rectChoice,
                                GuillotineSplitHeuristic splitMethod )
{
	// Find where to put the new rectangle.
	int freeNodeIndex = 0;
	Rect newRect = FindPositionForNewNode( width, height, rectChoice, &freeNodeIndex );

	// Abort if we didn't have enough space in the bin.
	if ( newRect.height == 0 )
	{
		return newRect;
	}

	// Remove the space that was just consumed by the new rectangle.
	SplitFreeRectByHeuristic( freeRectangles[freeNodeIndex], newRect, splitMethod );
	freeRectangles.erase( freeRectangles.begin() + freeNodeIndex );

	// Perform a Rectangle Merge step if desired.
	if ( merge )
	{
		MergeFreeList();
	}

	// Remember the new used rectangle.
	usedRectangles.push_back( newRect );

	// Check that we're really producing correct packings here.
	assert( disjointRects.Add( newRect ) == true );

	return newRect;
}

/// Computes the ratio of used surface area to the total bin area.
float GuillotineBinPack::Occupancy() const
{
	///\todo The occupancy rate could be cached/tracked incrementally instead
	///      of looping through the list of packed rectangles here.
	unsigned long usedSurfaceArea = 0;

	for ( size_t i = 0; i < usedRectangles.size(); ++i )
	{
		usedSurfaceArea += usedRectangles[i].width * usedRectangles[i].height;
	}

	return ( float )usedSurfaceArea / ( binWidth * binHeight );
}

/// Returns the heuristic score value for placing a rectangle of size width*height into freeRect. Does not try to rotate.
int GuillotineBinPack::ScoreByHeuristic( int width, int height, const Rect& freeRect, FreeRectChoiceHeuristic rectChoice )
{
	switch ( rectChoice )
	{
		case RectBestAreaFit:
			return ScoreBestAreaFit( width, height, freeRect );
		case RectBestShortSideFit:
			return ScoreBestShortSideFit( width, height, freeRect );
		case RectBestLongSideFit:
			return ScoreBestLongSideFit( width, height, freeRect );
		case RectWorstAreaFit:
			return ScoreWorstAreaFit( width, height, freeRect );
		case RectWorstShortSideFit:
			return ScoreWorstShortSideFit( width, height, freeRect );
		case RectWorstLongSideFit:
			return ScoreWorstLongSideFit( width, height, freeRect );
		default:
			assert( false );
			return std::numeric_limits<int>::max();
	}
}

int GuillotineBinPack::ScoreBestAreaFit( int width, int height, const Rect& freeRect )
{
	return freeRect.width * freeRect.height - width * height;
}

int GuillotineBinPack::ScoreBestShortSideFit( int width, int height, const Rect& freeRect )
{
	int leftoverHoriz = abs( freeRect.width - width );
	int leftoverVert = abs( freeRect.height - height );
	int leftover = min( leftoverHoriz, leftoverVert );
	return leftover;
}

int GuillotineBinPack::ScoreBestLongSideFit( int width, int height, const Rect& freeRect )
{
	int leftoverHoriz = abs( freeRect.width - width );
	int leftoverVert = abs( freeRect.height - height );
	int leftover = max( leftoverHoriz, leftoverVert );
	return leftover;
}

int GuillotineBinPack::ScoreWorstAreaFit( int width, int height, const Rect& freeRect )
{
	return -ScoreBestAreaFit( width, height, freeRect );
}

int GuillotineBinPack::ScoreWorstShortSideFit( int width, int height, const Rect& freeRect )
{
	return -ScoreBestShortSideFit( width, height, freeRect );
}

int GuillotineBinPack::ScoreWorstLongSideFit( int width, int height, const Rect& freeRect )
{
	return -ScoreBestLongSideFit( width, height, freeRect );
}

Rect GuillotineBinPack::FindPositionForNewNode( int width, int height, FreeRectChoiceHeuristic rectChoice, int* nodeIndex )
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	int bestScore = std::numeric_limits<int>::max();

	/// Try each free rectangle to find the best one for placement.
	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		// If this is a perfect fit upright, choose it immediately.
		if ( width == freeRectangles[i].width && height == freeRectangles[i].height )
		{
			bestNode.x = freeRectangles[i].x;
			bestNode.y = freeRectangles[i].y;
			bestNode.width = width;
			bestNode.height = height;
			bestScore = std::numeric_limits<int>::min();
			*nodeIndex = i;
			assert( disjointRects.Disjoint( bestNode ) );
			break;
		}
		// If this is a perfect fit sideways, choose it.
		else if ( height == freeRectangles[i].width && width == freeRectangles[i].height )
		{
			bestNode.x = freeRectangles[i].x;
			bestNode.y = freeRectangles[i].y;
			bestNode.width = height;
			bestNode.height = width;
			bestScore = std::numeric_limits<int>::min();
			*nodeIndex = i;
			assert( disjointRects.Disjoint( bestNode ) );
			break;
		}
		// Does the rectangle fit upright?
		else if ( width <= freeRectangles[i].width && height <= freeRectangles[i].height )
		{
			int score = ScoreByHeuristic( width, height, freeRectangles[i], rectChoice );

			if ( score < bestScore )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestScore = score;
				*nodeIndex = i;
				assert( disjointRects.Disjoint( bestNode ) );
			}
		}
		// Does the rectangle fit sideways?
		else if ( height <= freeRectangles[i].width && width <= freeRectangles[i].height )
		{
			int score = ScoreByHeuristic( height, width, freeRectangles[i], rectChoice );

			if ( score < bestScore )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestScore = score;
				*nodeIndex = i;
				assert( disjointRects.Disjoint( bestNode ) );
			}
		}
	}

	return bestNode;
}

void GuillotineBinPack::SplitFreeRectByHeuristic( const Rect& freeRect, const Rect& placedRect, GuillotineSplitHeuristic method )
{
	// Compute the lengths of the leftover area.
	const int w = freeRect.width - placedRect.width;
	const int h = freeRect.height - placedRect.height;

	// Placing placedRect into freeRect results in an L-shaped free area, which must be split into
	// two disjoint rectangles. This can be achieved with by splitting the L-shape using a single line.
	// We have two choices: horizontal or vertical.

	// Use the given heuristic to decide which choice to make.

	bool splitHorizontal;

	switch ( method )
	{
		case SplitShorterLeftoverAxis:
			// Split along the shorter leftover axis.
			splitHorizontal = ( w <= h );
			break;
		case SplitLongerLeftoverAxis:
			// Split along the longer leftover axis.
			splitHorizontal = ( w > h );
			break;
		case SplitMinimizeArea:
			// Maximize the larger area == minimize the smaller area.
			// Tries to make the single bigger rectangle.
			splitHorizontal = ( placedRect.width * h > w * placedRect.height );
			break;
		case SplitMaximizeArea:
			// Maximize the smaller area == minimize the larger area.
			// Tries to make the rectangles more even-sized.
			splitHorizontal = ( placedRect.width* h <= w * placedRect.height );
			break;
		case SplitShorterAxis:
			// Split along the shorter total axis.
			splitHorizontal = ( freeRect.width <= freeRect.height );
			break;
		case SplitLongerAxis:
			// Split along the longer total axis.
			splitHorizontal = ( freeRect.width > freeRect.height );
			break;
		default:
			splitHorizontal = true;
			assert( false );
	}

	// Perform the actual split.
	SplitFreeRectAlongAxis( freeRect, placedRect, splitHorizontal );
}

/// This function will add the two generated rectangles into the freeRectangles array. The caller is expected to
/// remove the original rectangle from the freeRectangles array after that.
void GuillotineBinPack::SplitFreeRectAlongAxis( const Rect& freeRect, const Rect& placedRect, bool splitHorizontal )
{
	// Form the two new rectangles.
	Rect bottom;
	bottom.x = freeRect.x;
	bottom.y = freeRect.y + placedRect.height;
	bottom.height = freeRect.height - placedRect.height;

	Rect right;
	right.x = freeRect.x + placedRect.width;
	right.y = freeRect.y;
	right.width = freeRect.width - placedRect.width;

	if ( splitHorizontal )
	{
		bottom.width = freeRect.width;
		right.height = placedRect.height;
	}
	else // Split vertically
	{
		bottom.width = placedRect.width;
		right.height = freeRect.height;
	}

	// Add the new rectangles into the free rectangle pool if they weren't degenerate.
	if ( bottom.width > 0 && bottom.height > 0 )
	{
		freeRectangles.push_back( bottom );
	}

	if ( right.width > 0 && right.height > 0 )
	{
		freeRectangles.push_back( right );
	}

	assert( disjointRects.Disjoint( bottom ) );
	assert( disjointRects.Disjoint( right ) );
}

void GuillotineBinPack::MergeFreeList()
{
#ifdef _DEBUG
	DisjointRectCollection test;

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		assert( test.Add( freeRectangles[i] ) == true );
	}

#endif

	// Do a Theta(n^2) loop to see if any pair of free rectangles could me merged into one.
	// Note that we miss any opportunities to merge three rectangles into one. (should call this function again to detect that)
	for ( size_t i = 0; i < freeRectangles.size(); ++i )
		for ( size_t j = i + 1; j < freeRectangles.size(); ++j )
		{
			if ( freeRectangles[i].width == freeRectangles[j].width && freeRectangles[i].x == freeRectangles[j].x )
			{
				if ( freeRectangles[i].y == freeRectangles[j].y + freeRectangles[j].height )
				{
					freeRectangles[i].y -= freeRectangles[j].height;
					freeRectangles[i].height += freeRectangles[j].height;
					freeRectangles.erase( freeRectangles.begin() + j );
					--j;
				}
				else if ( freeRectangles[i].y + freeRectangles[i].height == freeRectangles[j].y )
				{
					freeRectangles[i].height += freeRectangles[j].height;
					freeRectangles.erase( freeRectangles.begin() + j );
					--j;
				}
			}
			else if ( freeRectangles[i].height == freeRectangles[j].height && freeRectangles[i].y == freeRectangles[j].y )
			{
				if ( freeRectangles[i].x == freeRectangles[j].x + freeRectangles[j].width )
				{
					freeRectangles[i].x -= freeRectangles[j].width;
					freeRectangles[i].width += freeRectangles[j].width;
					freeRectangles.erase( freeRectangles.begin() + j );
					--j;
				}
				else if ( freeRectangles[i].x + freeRectangles[i].width == freeRectangles[j].x )
				{
					freeRectangles[i].width += freeRectangles[j].width;
					freeRectangles.erase( freeRectangles.begin() + j );
					--j;
				}
			}
		}

#ifdef _DEBUG
	test.Clear();

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		assert( test.Add( freeRectangles[i] ) == true );
	}

#endif
}



MaxRectsBinPack::MaxRectsBinPack()
	: binWidth( 0 ),
	  binHeight( 0 )
{
}

MaxRectsBinPack::MaxRectsBinPack( int width, int height )
{
	Init( width, height );
}

void MaxRectsBinPack::Init( int width, int height )
{
	binWidth = width;
	binHeight = height;

	Rect n;
	n.x = 0;
	n.y = 0;
	n.width = width;
	n.height = height;

	usedRectangles.clear();

	freeRectangles.clear();
	freeRectangles.push_back( n );
}

Rect MaxRectsBinPack::Insert( int width, int height, FreeRectChoiceHeuristic method )
{
	Rect newNode;
	int score1; // Unused in this function. We don't need to know the score after finding the position.
	int score2;

	switch ( method )
	{
		case RectBestShortSideFit:
			newNode = FindPositionForNewNodeBestShortSideFit( width, height, score1, score2 );
			break;
		case RectBottomLeftRule:
			newNode = FindPositionForNewNodeBottomLeft( width, height, score1, score2 );
			break;
		case RectContactPointRule:
			newNode = FindPositionForNewNodeContactPoint( width, height, score1 );
			break;
		case RectBestLongSideFit:
			newNode = FindPositionForNewNodeBestLongSideFit( width, height, score2, score1 );
			break;
		case RectBestAreaFit:
			newNode = FindPositionForNewNodeBestAreaFit( width, height, score1, score2 );
			break;
		default:
			newNode = Rect();
	}

	if ( newNode.height == 0 )
	{
		return newNode;
	}

	size_t numRectanglesToProcess = freeRectangles.size();

	for ( size_t i = 0; i < numRectanglesToProcess; ++i )
	{
		if ( SplitFreeNode( freeRectangles[i], newNode ) )
		{
			freeRectangles.erase( freeRectangles.begin() + i );
			--i;
			--numRectanglesToProcess;
		}
	}

	PruneFreeList();

	usedRectangles.push_back( newNode );
	return newNode;
}

void MaxRectsBinPack::Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, FreeRectChoiceHeuristic method )
{
	dst.clear();

	while ( rects.size() > 0 )
	{
		int bestScore1 = std::numeric_limits<int>::max();
		int bestScore2 = std::numeric_limits<int>::max();
		int bestRectIndex = -1;
		Rect bestNode;

		for ( size_t i = 0; i < rects.size(); ++i )
		{
			int score1;
			int score2;
			Rect newNode = ScoreRect( rects[i].width, rects[i].height, method, score1, score2 );

			if ( score1 < bestScore1 || ( score1 == bestScore1 && score2 < bestScore2 ) )
			{
				bestScore1 = score1;
				bestScore2 = score2;
				bestNode = newNode;
				bestRectIndex = i;
			}
		}

		if ( bestRectIndex == -1 )
		{
			return;
		}

		PlaceRect( bestNode );
		rects.erase( rects.begin() + bestRectIndex );
	}
}

void MaxRectsBinPack::PlaceRect( const Rect& node )
{
	size_t numRectanglesToProcess = freeRectangles.size();

	for ( size_t i = 0; i < numRectanglesToProcess; ++i )
	{
		if ( SplitFreeNode( freeRectangles[i], node ) )
		{
			freeRectangles.erase( freeRectangles.begin() + i );
			--i;
			--numRectanglesToProcess;
		}
	}

	PruneFreeList();

	usedRectangles.push_back( node );
	//    dst.push_back(bestNode); ///\todo Refactor so that this compiles.
}

Rect MaxRectsBinPack::ScoreRect( int width, int height, FreeRectChoiceHeuristic method, int& score1, int& score2 ) const
{
	Rect newNode;
	score1 = std::numeric_limits<int>::max();
	score2 = std::numeric_limits<int>::max();

	switch ( method )
	{
		case RectBestShortSideFit:
			newNode = FindPositionForNewNodeBestShortSideFit( width, height, score1, score2 );
			break;
		case RectBottomLeftRule:
			newNode = FindPositionForNewNodeBottomLeft( width, height, score1, score2 );
			break;
		case RectContactPointRule:
			newNode = FindPositionForNewNodeContactPoint( width, height, score1 );
			score1 = -score1; // Reverse since we are minimizing, but for contact point score bigger is better.
			break;
		case RectBestLongSideFit:
			newNode = FindPositionForNewNodeBestLongSideFit( width, height, score2, score1 );
			break;
		case RectBestAreaFit:
			newNode = FindPositionForNewNodeBestAreaFit( width, height, score1, score2 );
			break;
		default:
			newNode = Rect();
	}

	// Cannot fit the current rectangle.
	if ( newNode.height == 0 )
	{
		score1 = std::numeric_limits<int>::max();
		score2 = std::numeric_limits<int>::max();
	}

	return newNode;
}

/// Computes the ratio of used surface area.
float MaxRectsBinPack::Occupancy() const
{
	unsigned long usedSurfaceArea = 0;

	for ( size_t i = 0; i < usedRectangles.size(); ++i )
	{
		usedSurfaceArea += usedRectangles[i].width * usedRectangles[i].height;
	}

	return ( float )usedSurfaceArea / ( binWidth * binHeight );
}

Rect MaxRectsBinPack::FindPositionForNewNodeBottomLeft( int width, int height, int& bestY, int& bestX ) const
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	bestY = std::numeric_limits<int>::max();

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if ( freeRectangles[i].width >= width && freeRectangles[i].height >= height )
		{
			int topSideY = freeRectangles[i].y + height;

			if ( topSideY < bestY || ( topSideY == bestY && freeRectangles[i].x < bestX ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestY = topSideY;
				bestX = freeRectangles[i].x;
			}
		}

		if ( freeRectangles[i].width >= height && freeRectangles[i].height >= width )
		{
			int topSideY = freeRectangles[i].y + width;

			if ( topSideY < bestY || ( topSideY == bestY && freeRectangles[i].x < bestX ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestY = topSideY;
				bestX = freeRectangles[i].x;
			}
		}
	}

	return bestNode;
}

Rect MaxRectsBinPack::FindPositionForNewNodeBestShortSideFit( int width, int height,
                                                              int& bestShortSideFit, int& bestLongSideFit ) const
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	bestShortSideFit = std::numeric_limits<int>::max();

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if ( freeRectangles[i].width >= width && freeRectangles[i].height >= height )
		{
			int leftoverHoriz = abs( freeRectangles[i].width - width );
			int leftoverVert = abs( freeRectangles[i].height - height );
			int shortSideFit = min( leftoverHoriz, leftoverVert );
			int longSideFit = max( leftoverHoriz, leftoverVert );

			if ( shortSideFit < bestShortSideFit || ( shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}

		if ( freeRectangles[i].width >= height && freeRectangles[i].height >= width )
		{
			int flippedLeftoverHoriz = abs( freeRectangles[i].width - height );
			int flippedLeftoverVert = abs( freeRectangles[i].height - width );
			int flippedShortSideFit = min( flippedLeftoverHoriz, flippedLeftoverVert );
			int flippedLongSideFit = max( flippedLeftoverHoriz, flippedLeftoverVert );

			if ( flippedShortSideFit < bestShortSideFit || ( flippedShortSideFit == bestShortSideFit && flippedLongSideFit < bestLongSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestShortSideFit = flippedShortSideFit;
				bestLongSideFit = flippedLongSideFit;
			}
		}
	}

	return bestNode;
}

Rect MaxRectsBinPack::FindPositionForNewNodeBestLongSideFit( int width, int height,
                                                             int& bestShortSideFit, int& bestLongSideFit ) const
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	bestLongSideFit = std::numeric_limits<int>::max();

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if ( freeRectangles[i].width >= width && freeRectangles[i].height >= height )
		{
			int leftoverHoriz = abs( freeRectangles[i].width - width );
			int leftoverVert = abs( freeRectangles[i].height - height );
			int shortSideFit = min( leftoverHoriz, leftoverVert );
			int longSideFit = max( leftoverHoriz, leftoverVert );

			if ( longSideFit < bestLongSideFit || ( longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}

		if ( freeRectangles[i].width >= height && freeRectangles[i].height >= width )
		{
			int leftoverHoriz = abs( freeRectangles[i].width - height );
			int leftoverVert = abs( freeRectangles[i].height - width );
			int shortSideFit = min( leftoverHoriz, leftoverVert );
			int longSideFit = max( leftoverHoriz, leftoverVert );

			if ( longSideFit < bestLongSideFit || ( longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}
	}

	return bestNode;
}

Rect MaxRectsBinPack::FindPositionForNewNodeBestAreaFit( int width, int height,
                                                         int& bestAreaFit, int& bestShortSideFit ) const
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	bestAreaFit = std::numeric_limits<int>::max();

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		int areaFit = freeRectangles[i].width * freeRectangles[i].height - width * height;

		// Try to place the rectangle in upright (non-flipped) orientation.
		if ( freeRectangles[i].width >= width && freeRectangles[i].height >= height )
		{
			int leftoverHoriz = abs( freeRectangles[i].width - width );
			int leftoverVert = abs( freeRectangles[i].height - height );
			int shortSideFit = min( leftoverHoriz, leftoverVert );

			if ( areaFit < bestAreaFit || ( areaFit == bestAreaFit && shortSideFit < bestShortSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestShortSideFit = shortSideFit;
				bestAreaFit = areaFit;
			}
		}

		if ( freeRectangles[i].width >= height && freeRectangles[i].height >= width )
		{
			int leftoverHoriz = abs( freeRectangles[i].width - height );
			int leftoverVert = abs( freeRectangles[i].height - width );
			int shortSideFit = min( leftoverHoriz, leftoverVert );

			if ( areaFit < bestAreaFit || ( areaFit == bestAreaFit && shortSideFit < bestShortSideFit ) )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestShortSideFit = shortSideFit;
				bestAreaFit = areaFit;
			}
		}
	}

	return bestNode;
}

/// Returns 0 if the two intervals i1 and i2 are disjoint, or the length of their overlap otherwise.
int CommonIntervalLength( int i1start, int i1end, int i2start, int i2end )
{
	if ( i1end < i2start || i2end < i1start )
	{
		return 0;
	}

	return min( i1end, i2end ) - max( i1start, i2start );
}

int MaxRectsBinPack::ContactPointScoreNode( int x, int y, int width, int height ) const
{
	int score = 0;

	if ( x == 0 || x + width == binWidth )
	{
		score += height;
	}

	if ( y == 0 || y + height == binHeight )
	{
		score += width;
	}

	for ( size_t i = 0; i < usedRectangles.size(); ++i )
	{
		if ( usedRectangles[i].x == x + width || usedRectangles[i].x + usedRectangles[i].width == x )
		{
			score += CommonIntervalLength( usedRectangles[i].y, usedRectangles[i].y + usedRectangles[i].height, y, y + height );
		}

		if ( usedRectangles[i].y == y + height || usedRectangles[i].y + usedRectangles[i].height == y )
		{
			score += CommonIntervalLength( usedRectangles[i].x, usedRectangles[i].x + usedRectangles[i].width, x, x + width );
		}
	}

	return score;
}

Rect MaxRectsBinPack::FindPositionForNewNodeContactPoint( int width, int height, int& bestContactScore ) const
{
	Rect bestNode;
	memset( &bestNode, 0, sizeof( Rect ) );

	bestContactScore = -1;

	for ( size_t i = 0; i < freeRectangles.size(); ++i )
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if ( freeRectangles[i].width >= width && freeRectangles[i].height >= height )
		{
			int score = ContactPointScoreNode( freeRectangles[i].x, freeRectangles[i].y, width, height );

			if ( score > bestContactScore )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = width;
				bestNode.height = height;
				bestContactScore = score;
			}
		}

		if ( freeRectangles[i].width >= height && freeRectangles[i].height >= width )
		{
			int score = ContactPointScoreNode( freeRectangles[i].x, freeRectangles[i].y, width, height );

			if ( score > bestContactScore )
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = height;
				bestNode.height = width;
				bestContactScore = score;
			}
		}
	}

	return bestNode;
}

bool MaxRectsBinPack::SplitFreeNode( Rect freeNode, const Rect& usedNode )
{
	// Test with SAT if the rectangles even intersect.
	if ( usedNode.x >= freeNode.x + freeNode.width || usedNode.x + usedNode.width <= freeNode.x ||
	     usedNode.y >= freeNode.y + freeNode.height || usedNode.y + usedNode.height <= freeNode.y )
	{
		return false;
	}

	if ( usedNode.x < freeNode.x + freeNode.width && usedNode.x + usedNode.width > freeNode.x )
	{
		// New node at the top side of the used node.
		if ( usedNode.y > freeNode.y && usedNode.y < freeNode.y + freeNode.height )
		{
			Rect newNode = freeNode;
			newNode.height = usedNode.y - newNode.y;
			freeRectangles.push_back( newNode );
		}

		// New node at the bottom side of the used node.
		if ( usedNode.y + usedNode.height < freeNode.y + freeNode.height )
		{
			Rect newNode = freeNode;
			newNode.y = usedNode.y + usedNode.height;
			newNode.height = freeNode.y + freeNode.height - ( usedNode.y + usedNode.height );
			freeRectangles.push_back( newNode );
		}
	}

	if ( usedNode.y < freeNode.y + freeNode.height && usedNode.y + usedNode.height > freeNode.y )
	{
		// New node at the left side of the used node.
		if ( usedNode.x > freeNode.x && usedNode.x < freeNode.x + freeNode.width )
		{
			Rect newNode = freeNode;
			newNode.width = usedNode.x - newNode.x;
			freeRectangles.push_back( newNode );
		}

		// New node at the right side of the used node.
		if ( usedNode.x + usedNode.width < freeNode.x + freeNode.width )
		{
			Rect newNode = freeNode;
			newNode.x = usedNode.x + usedNode.width;
			newNode.width = freeNode.x + freeNode.width - ( usedNode.x + usedNode.width );
			freeRectangles.push_back( newNode );
		}
	}

	return true;
}

void MaxRectsBinPack::PruneFreeList()
{
	/*
	///  Would be nice to do something like this, to avoid a Theta(n^2) loop through each pair.
	///  But unfortunately it doesn't quite cut it, since we also want to detect containment.
	///  Perhaps there's another way to do this faster than Theta(n^2).

	if (freeRectangles.size() > 0)
	   clb::sort::QuickSort(&freeRectangles[0], freeRectangles.size(), NodeSortCmp);

	for(size_t i = 0; i < freeRectangles.size()-1; ++i)
	   if (freeRectangles[i].x == freeRectangles[i+1].x &&
	       freeRectangles[i].y == freeRectangles[i+1].y &&
	       freeRectangles[i].width == freeRectangles[i+1].width &&
	       freeRectangles[i].height == freeRectangles[i+1].height)
	   {
	      freeRectangles.erase(freeRectangles.begin() + i);
	      --i;
	   }
	*/

	/// Go through each pair and remove any rectangle that is redundant.
	for ( size_t i = 0; i < freeRectangles.size(); ++i )
		for ( size_t j = i + 1; j < freeRectangles.size(); ++j )
		{
			if ( IsContainedIn( freeRectangles[i], freeRectangles[j] ) )
			{
				freeRectangles.erase( freeRectangles.begin() + i );
				--i;
				break;
			}

			if ( IsContainedIn( freeRectangles[j], freeRectangles[i] ) )
			{
				freeRectangles.erase( freeRectangles.begin() + j );
				--j;
			}
		}
}


ShelfBinPack::ShelfBinPack()
	: binWidth( 0 ),
	  binHeight( 0 ),
	  useWasteMap( false ),
	  currentY( 0 ),
	  usedSurfaceArea( 0 )
{
}

ShelfBinPack::ShelfBinPack( int width, int height, bool useWasteMap )
{
	Init( width, height, useWasteMap );
}

void ShelfBinPack::Init( int width, int height, bool useWasteMap_ )
{
	useWasteMap = useWasteMap_;
	binWidth = width;
	binHeight = height;

	currentY = 0;
	usedSurfaceArea = 0;

	shelves.clear();
	StartNewShelf( 0 );

	if ( useWasteMap )
	{
		wasteMap.Init( width, height );
		wasteMap.GetFreeRectangles().clear();
	}
}

bool ShelfBinPack::CanStartNewShelf( int height ) const
{
	return shelves.back().startY + shelves.back().height + height <= binHeight;
}

void ShelfBinPack::StartNewShelf( int startingHeight )
{
	if ( shelves.size() > 0 )
	{
		assert( shelves.back().height != 0 );
		currentY += shelves.back().height;

		assert( currentY < binHeight );
	}

	Shelf shelf;
	shelf.currentX = 0;
	shelf.height = startingHeight;
	shelf.startY = currentY;

	assert( shelf.startY + shelf.height <= binHeight );
	shelves.push_back( shelf );
}

bool ShelfBinPack::FitsOnShelf( const Shelf& shelf, int width, int height, bool canResize ) const
{
	const int shelfHeight = canResize ? ( binHeight - shelf.startY ) : shelf.height;

	if ( ( shelf.currentX + width <= binWidth && height <= shelfHeight ) ||
	     ( shelf.currentX + height <= binWidth && width <= shelfHeight ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ShelfBinPack::RotateToShelf( const Shelf& shelf, int& width, int& height ) const
{
	// If the width > height and the long edge of the new rectangle fits vertically onto the current shelf,
	// flip it. If the short edge is larger than the current shelf height, store
	// the short edge vertically.
	if ( ( width > height && width > binWidth - shelf.currentX ) ||
	     ( width > height && width < shelf.height ) ||
	     ( width < height && height > shelf.height && height <= binWidth - shelf.currentX ) )
	{
		swap( width, height );
	}
}

void ShelfBinPack::AddToShelf( Shelf& shelf, int width, int height, Rect& newNode )
{
	assert( FitsOnShelf( shelf, width, height, true ) );

	// Swap width and height if the rect fits better that way.
	RotateToShelf( shelf, width, height );

	// Add the rectangle to the shelf.
	newNode.x = shelf.currentX;
	newNode.y = shelf.startY;
	newNode.width = width;
	newNode.height = height;
	shelf.usedRectangles.push_back( newNode );

	// Advance the shelf end position horizontally.
	shelf.currentX += width;
	assert( shelf.currentX <= binWidth );

	// Grow the shelf height.
	shelf.height = max( shelf.height, height );
	assert( shelf.height <= binHeight );

	usedSurfaceArea += width * height;
}

Rect ShelfBinPack::Insert( int width, int height, ShelfChoiceHeuristic method )
{
	Rect newNode;

	// First try to pack this rectangle into the waste map, if it fits.
	if ( useWasteMap )
	{
		newNode = wasteMap.Insert( width, height, true, GuillotineBinPack::RectBestShortSideFit,
		                           GuillotineBinPack::SplitMaximizeArea );

		if ( newNode.height != 0 )
		{
			// Track the space we just used.
			usedSurfaceArea += width * height;

			return newNode;
		}
	}

	switch ( method )
	{
		case ShelfNextFit:

			if ( FitsOnShelf( shelves.back(), width, height, true ) )
			{
				AddToShelf( shelves.back(), width, height, newNode );
				return newNode;
			}

			break;
		case ShelfFirstFit:

			for ( size_t i = 0; i < shelves.size(); ++i )
				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					AddToShelf( shelves[i], width, height, newNode );
					return newNode;
				}

			break;

		case ShelfBestAreaFit:
		{
			// Best Area Fit rule: Choose the shelf with smallest remaining shelf area.
			Shelf* bestShelf = 0;
			unsigned long bestShelfSurfaceArea = ( unsigned long ) - 1;

			for ( size_t i = 0; i < shelves.size(); ++i )
			{
				// Pre-rotate the rect onto the shelf here already so that the area fit computation
				// is done correctly.
				RotateToShelf( shelves[i], width, height );

				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					unsigned long surfaceArea = ( binWidth - shelves[i].currentX ) * shelves[i].height;

					if ( surfaceArea < bestShelfSurfaceArea )
					{
						bestShelf = &shelves[i];
						bestShelfSurfaceArea = surfaceArea;
					}
				}
			}

			if ( bestShelf )
			{
				AddToShelf( *bestShelf, width, height, newNode );
				return newNode;
			}
		}
		break;

		case ShelfWorstAreaFit:
		{
			// Worst Area Fit rule: Choose the shelf with smallest remaining shelf area.
			Shelf* bestShelf = 0;
			int bestShelfSurfaceArea = -1;

			for ( size_t i = 0; i < shelves.size(); ++i )
			{
				// Pre-rotate the rect onto the shelf here already so that the area fit computation
				// is done correctly.
				RotateToShelf( shelves[i], width, height );

				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					int surfaceArea = ( binWidth - shelves[i].currentX ) * shelves[i].height;

					if ( surfaceArea > bestShelfSurfaceArea )
					{
						bestShelf = &shelves[i];
						bestShelfSurfaceArea = surfaceArea;
					}
				}
			}

			if ( bestShelf )
			{
				AddToShelf( *bestShelf, width, height, newNode );
				return newNode;
			}
		}
		break;

		case ShelfBestHeightFit:
		{
			// Best Height Fit rule: Choose the shelf with best-matching height.
			Shelf* bestShelf = 0;
			int bestShelfHeightDifference = 0x7FFFFFFF;

			for ( size_t i = 0; i < shelves.size(); ++i )
			{
				// Pre-rotate the rect onto the shelf here already so that the height fit computation
				// is done correctly.
				RotateToShelf( shelves[i], width, height );

				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					int heightDifference = max( shelves[i].height - height, 0 );
					assert( heightDifference >= 0 );

					if ( heightDifference < bestShelfHeightDifference )
					{
						bestShelf = &shelves[i];
						bestShelfHeightDifference = heightDifference;
					}
				}
			}

			if ( bestShelf )
			{
				AddToShelf( *bestShelf, width, height, newNode );
				return newNode;
			}
		}
		break;

		case ShelfBestWidthFit:
		{
			// Best Width Fit rule: Choose the shelf with smallest remaining shelf width.
			Shelf* bestShelf = 0;
			int bestShelfWidthDifference = 0x7FFFFFFF;

			for ( size_t i = 0; i < shelves.size(); ++i )
			{
				// Pre-rotate the rect onto the shelf here already so that the height fit computation
				// is done correctly.
				RotateToShelf( shelves[i], width, height );

				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					int widthDifference = binWidth - shelves[i].currentX - width;
					assert( widthDifference >= 0 );

					if ( widthDifference < bestShelfWidthDifference )
					{
						bestShelf = &shelves[i];
						bestShelfWidthDifference = widthDifference;
					}
				}
			}

			if ( bestShelf )
			{
				AddToShelf( *bestShelf, width, height, newNode );
				return newNode;
			}
		}
		break;

		case ShelfWorstWidthFit:
		{
			// Worst Width Fit rule: Choose the shelf with smallest remaining shelf width.
			Shelf* bestShelf = 0;
			int bestShelfWidthDifference = -1;

			for ( size_t i = 0; i < shelves.size(); ++i )
			{
				// Pre-rotate the rect onto the shelf here already so that the height fit computation
				// is done correctly.
				RotateToShelf( shelves[i], width, height );

				if ( FitsOnShelf( shelves[i], width, height, i == shelves.size() - 1 ) )
				{
					int widthDifference = binWidth - shelves[i].currentX - width;
					assert( widthDifference >= 0 );

					if ( widthDifference > bestShelfWidthDifference )
					{
						bestShelf = &shelves[i];
						bestShelfWidthDifference = widthDifference;
					}
				}
			}

			if ( bestShelf )
			{
				AddToShelf( *bestShelf, width, height, newNode );
				return newNode;
			}
		}
		break;

	}

	// The rectangle did not fit on any of the shelves. Open a new shelf.

	// Flip the rectangle so that the long side is horizontal.
	if ( width < height && height <= binWidth )
	{
		swap( width, height );
	}

	if ( CanStartNewShelf( height ) )
	{
		if ( useWasteMap )
		{
			MoveShelfToWasteMap( shelves.back() );
		}

		StartNewShelf( height );
		assert( FitsOnShelf( shelves.back(), width, height, true ) );
		AddToShelf( shelves.back(), width, height, newNode );
		return newNode;
	}

	/*
	   ///\todo This is problematic: If we couldn't start a new shelf - should we give up
	   ///      and move all the remaining space of the bin for the waste map to track,
	   ///      or should we just wait if the next rectangle would fit better? For now,
	   ///      don't add the leftover space to the waste map.
	   else if (useWasteMap)
	   {
	      assert(binHeight - shelves.back().startY >= shelves.back().height);
	      shelves.back().height = binHeight - shelves.back().startY;
	      if (shelves.back().height > 0)
	         MoveShelfToWasteMap(shelves.back());

	      // Try to pack the rectangle again to the waste map.
	      GuillotineBinPack::Node node = wasteMap.Insert(width, height, true, 1, 3);
	      if (node.height != 0)
	      {
	         newNode.x = node.x;
	         newNode.y = node.y;
	         newNode.width = node.width;
	         newNode.height = node.height;
	         return newNode;
	      }
	   }
	*/

	// The rectangle didn't fit.
	memset( &newNode, 0, sizeof( Rect ) );
	return newNode;
}

void ShelfBinPack::MoveShelfToWasteMap( Shelf& shelf )
{
	std::vector<Rect> &freeRects = wasteMap.GetFreeRectangles();

	// Add the gaps between each rect top and shelf ceiling to the waste map.
	for ( size_t i = 0; i < shelf.usedRectangles.size(); ++i )
	{
		const Rect& r = shelf.usedRectangles[i];
		Rect newNode;
		newNode.x = r.x;
		newNode.y = r.y + r.height;
		newNode.width = r.width;
		newNode.height = shelf.height - r.height;

		if ( newNode.height > 0 )
		{
			freeRects.push_back( newNode );
		}
	}

	shelf.usedRectangles.clear();

	// Add the space after the shelf end (right side of the last rect) and the shelf right side.
	Rect newNode;
	newNode.x = shelf.currentX;
	newNode.y = shelf.startY;
	newNode.width = binWidth - shelf.currentX;
	newNode.height = shelf.height;

	if ( newNode.width > 0 )
	{
		freeRects.push_back( newNode );
	}

	// This shelf is DONE.
	shelf.currentX = binWidth;

	// Perform a rectangle merge step.
	wasteMap.MergeFreeList();
}

/// Computes the ratio of used surface area to the bin area.
float ShelfBinPack::Occupancy() const
{
	return ( float )usedSurfaceArea / ( binWidth * binHeight );
}


SkylineBinPack::SkylineBinPack()
	: binWidth( 0 ),
	  binHeight( 0 )
{
}

SkylineBinPack::SkylineBinPack( int width, int height, bool useWasteMap )
{
	Init( width, height, useWasteMap );
}

void SkylineBinPack::Init( int width, int height, bool useWasteMap_ )
{
	binWidth = width;
	binHeight = height;

	useWasteMap = useWasteMap_;

#ifdef _DEBUG
	disjointRects.Clear();
#endif

	usedSurfaceArea = 0;
	skyLine.clear();
	SkylineNode node;
	node.x = 0;
	node.y = 0;
	node.width = binWidth;
	skyLine.push_back( node );

	if ( useWasteMap )
	{
		wasteMap.Init( width, height );
		wasteMap.GetFreeRectangles().clear();
	}
}

void SkylineBinPack::Insert( std::vector<RectSize> &rects, std::vector<Rect> &dst, LevelChoiceHeuristic method )
{
	dst.clear();

	while ( rects.size() > 0 )
	{
		Rect bestNode;
		int bestScore1 = std::numeric_limits<int>::max();
		int bestScore2 = std::numeric_limits<int>::max();
		int bestSkylineIndex = -1;
		int bestRectIndex = -1;

		for ( size_t i = 0; i < rects.size(); ++i )
		{
			Rect newNode;
			int score1 = 0;
			int score2 = 0;
			int index  = 0;

			switch ( method )
			{
				case LevelBottomLeft:
					newNode = FindPositionForNewNodeBottomLeft( rects[i].width, rects[i].height, score1, score2, index );
					assert( disjointRects.Disjoint( newNode ) );
					break;
				case LevelMinWasteFit:
					newNode = FindPositionForNewNodeMinWaste( rects[i].width, rects[i].height, score2, score1, index );
					assert( disjointRects.Disjoint( newNode ) );
					break;
				default:
					newNode = Rect();
					assert( false );
					break;
			}

			if ( newNode.height != 0 )
			{
				if ( score1 < bestScore1 || ( score1 == bestScore1 && score2 < bestScore2 ) )
				{
					bestNode = newNode;
					bestScore1 = score1;
					bestScore2 = score2;
					bestSkylineIndex = index;
					bestRectIndex = i;
				}
			}
		}

		if ( bestRectIndex == -1 )
		{
			return;
		}

		// Perform the actual packing.
		assert( disjointRects.Disjoint( bestNode ) );
#ifdef _DEBUG
		disjointRects.Add( bestNode );
#endif
		AddSkylineLevel( bestSkylineIndex, bestNode );
		usedSurfaceArea += rects[bestRectIndex].width * rects[bestRectIndex].height;
		rects.erase( rects.begin() + bestRectIndex );
		dst.push_back( bestNode );
	}
}

Rect SkylineBinPack::Insert( int width, int height, LevelChoiceHeuristic method )
{
	// First try to pack this rectangle into the waste map, if it fits.
	Rect node = wasteMap.Insert( width, height, true, GuillotineBinPack::RectBestShortSideFit,
	                             GuillotineBinPack::SplitMaximizeArea );
	assert( disjointRects.Disjoint( node ) );

	if ( node.height != 0 )
	{
		Rect newNode;
		newNode.x = node.x;
		newNode.y = node.y;
		newNode.width = node.width;
		newNode.height = node.height;
		usedSurfaceArea += width * height;
		assert( disjointRects.Disjoint( newNode ) );
#ifdef _DEBUG
		disjointRects.Add( newNode );
#endif
		return newNode;
	}

	switch ( method )
	{
		case LevelBottomLeft:
			return InsertBottomLeft( width, height );
		case LevelMinWasteFit:
			return InsertMinWaste( width, height );
		default:
			assert( false );
			return node;
	}
}

bool SkylineBinPack::RectangleFits( int skylineNodeIndex, int width, int height, int& y ) const
{
	int x = skyLine[skylineNodeIndex].x;

	if ( x + width > binWidth )
	{
		return false;
	}

	int widthLeft = width;
	int i = skylineNodeIndex;
	y = skyLine[skylineNodeIndex].y;

	while ( widthLeft > 0 )
	{
		y = max( y, skyLine[i].y );

		if ( y + height > binHeight )
		{
			return false;
		}

		widthLeft -= skyLine[i].width;
		++i;
		assert( i < ( int )skyLine.size() || widthLeft <= 0 );
	}

	return true;
}

int SkylineBinPack::ComputeWastedArea( int skylineNodeIndex, int width, int height, int y ) const
{
	(height);

	int wastedArea = 0;
	const int rectLeft = skyLine[skylineNodeIndex].x;
	const int rectRight = rectLeft + width;

	for ( ; skylineNodeIndex < ( int )skyLine.size() && skyLine[skylineNodeIndex].x < rectRight; ++skylineNodeIndex )
	{
		if ( skyLine[skylineNodeIndex].x >= rectRight || skyLine[skylineNodeIndex].x + skyLine[skylineNodeIndex].width <= rectLeft )
		{
			break;
		}

		int leftSide = skyLine[skylineNodeIndex].x;
		int rightSide = min( rectRight, leftSide + skyLine[skylineNodeIndex].width );
		assert( y >= skyLine[skylineNodeIndex].y );
		wastedArea += ( rightSide - leftSide ) * ( y - skyLine[skylineNodeIndex].y );
	}

	return wastedArea;
}

bool SkylineBinPack::RectangleFits( int skylineNodeIndex, int width, int height, int& y, int& wastedArea ) const
{
	bool fits = RectangleFits( skylineNodeIndex, width, height, y );

	if ( fits )
	{
		wastedArea = ComputeWastedArea( skylineNodeIndex, width, height, y );
	}

	return fits;
}

void SkylineBinPack::AddWasteMapArea( int skylineNodeIndex, int width, int height, int y )
{
	(height);

	const int rectLeft = skyLine[skylineNodeIndex].x;
	const int rectRight = rectLeft + width;

	for ( ; skylineNodeIndex < ( int )skyLine.size() && skyLine[skylineNodeIndex].x < rectRight; ++skylineNodeIndex )
	{
		if ( skyLine[skylineNodeIndex].x >= rectRight || skyLine[skylineNodeIndex].x + skyLine[skylineNodeIndex].width <= rectLeft )
		{
			break;
		}

		int leftSide = skyLine[skylineNodeIndex].x;
		int rightSide = min( rectRight, leftSide + skyLine[skylineNodeIndex].width );
		assert( y >= skyLine[skylineNodeIndex].y );

		Rect waste;
		waste.x = leftSide;
		waste.y = skyLine[skylineNodeIndex].y;
		waste.width = rightSide - leftSide;
		waste.height = y - skyLine[skylineNodeIndex].y;

		assert( disjointRects.Disjoint( waste ) );
		wasteMap.GetFreeRectangles().push_back( waste );
	}
}

void SkylineBinPack::AddSkylineLevel( int skylineNodeIndex, const Rect& rect )
{
	// First track all wasted areas and mark them into the waste map if we're using one.
	if ( useWasteMap )
	{
		AddWasteMapArea( skylineNodeIndex, rect.width, rect.height, rect.y );
	}

	SkylineNode newNode;
	newNode.x = rect.x;
	newNode.y = rect.y + rect.height;
	newNode.width = rect.width;
	skyLine.insert( skyLine.begin() + skylineNodeIndex, newNode );

	assert( newNode.x + newNode.width <= binWidth );
	assert( newNode.y <= binHeight );

	for ( size_t i = skylineNodeIndex + 1; i < skyLine.size(); ++i )
	{
		assert( skyLine[i-1].x <= skyLine[i].x );

		if ( skyLine[i].x < skyLine[i-1].x + skyLine[i-1].width )
		{
			int shrink = skyLine[i-1].x + skyLine[i-1].width - skyLine[i].x;

			skyLine[i].x += shrink;
			skyLine[i].width -= shrink;

			if ( skyLine[i].width <= 0 )
			{
				skyLine.erase( skyLine.begin() + i );
				--i;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	MergeSkylines();
}

void SkylineBinPack::MergeSkylines()
{
	for ( size_t i = 0; i < skyLine.size() - 1; ++i )
		if ( skyLine[i].y == skyLine[i+1].y )
		{
			skyLine[i].width += skyLine[i+1].width;
			skyLine.erase( skyLine.begin() + ( i + 1 ) );
			--i;
		}
}

Rect SkylineBinPack::InsertBottomLeft( int width, int height )
{
	int bestHeight;
	int bestWidth;
	int bestIndex;
	Rect newNode = FindPositionForNewNodeBottomLeft( width, height, bestHeight, bestWidth, bestIndex );

	if ( bestIndex != -1 )
	{
		assert( disjointRects.Disjoint( newNode ) );
		// Perform the actual packing.
		AddSkylineLevel( bestIndex, newNode );

		usedSurfaceArea += width * height;
#ifdef _DEBUG
		disjointRects.Add( newNode );
#endif
	}
	else
	{
		memset( &newNode, 0, sizeof( Rect ) );
	}

	return newNode;
}

Rect SkylineBinPack::FindPositionForNewNodeBottomLeft( int width, int height, int& bestHeight, int& bestWidth, int& bestIndex ) const
{
	bestHeight = std::numeric_limits<int>::max();
	bestIndex = -1;
	// Used to break ties if there are nodes at the same level. Then pick the narrowest one.
	bestWidth = std::numeric_limits<int>::max();
	Rect newNode;
	memset( &newNode, 0, sizeof( newNode ) );

	for ( size_t i = 0; i < skyLine.size(); ++i )
	{
		int y;

		if ( RectangleFits( i, width, height, y ) )
		{
			if ( y + height < bestHeight || ( y + height == bestHeight && skyLine[i].width < bestWidth ) )
			{
				bestHeight = y + height;
				bestIndex = i;
				bestWidth = skyLine[i].width;
				newNode.x = skyLine[i].x;
				newNode.y = y;
				newNode.width = width;
				newNode.height = height;
				assert( disjointRects.Disjoint( newNode ) );
			}
		}

		if ( RectangleFits( i, height, width, y ) )
		{
			if ( y + width < bestHeight || ( y + width == bestHeight && skyLine[i].width < bestWidth ) )
			{
				bestHeight = y + width;
				bestIndex = i;
				bestWidth = skyLine[i].width;
				newNode.x = skyLine[i].x;
				newNode.y = y;
				newNode.width = height;
				newNode.height = width;
				assert( disjointRects.Disjoint( newNode ) );
			}
		}
	}

	return newNode;
}

Rect SkylineBinPack::InsertMinWaste( int width, int height )
{
	int bestHeight;
	int bestWastedArea;
	int bestIndex;
	Rect newNode = FindPositionForNewNodeMinWaste( width, height, bestHeight, bestWastedArea, bestIndex );

	if ( bestIndex != -1 )
	{
		assert( disjointRects.Disjoint( newNode ) );
		// Perform the actual packing.
		AddSkylineLevel( bestIndex, newNode );

		usedSurfaceArea += width * height;
#ifdef _DEBUG
		disjointRects.Add( newNode );
#endif
	}
	else
	{
		memset( &newNode, 0, sizeof( newNode ) );
	}

	return newNode;
}

Rect SkylineBinPack::FindPositionForNewNodeMinWaste( int width, int height, int& bestHeight, int& bestWastedArea, int& bestIndex ) const
{
	bestHeight = std::numeric_limits<int>::max();
	bestWastedArea = std::numeric_limits<int>::max();
	bestIndex = -1;
	Rect newNode;
	memset( &newNode, 0, sizeof( newNode ) );

	for ( size_t i = 0; i < skyLine.size(); ++i )
	{
		int y;
		int wastedArea;

		if ( RectangleFits( i, width, height, y, wastedArea ) )
		{
			if ( wastedArea < bestWastedArea || ( wastedArea == bestWastedArea && y + height < bestHeight ) )
			{
				bestHeight = y + height;
				bestWastedArea = wastedArea;
				bestIndex = i;
				newNode.x = skyLine[i].x;
				newNode.y = y;
				newNode.width = width;
				newNode.height = height;
				assert( disjointRects.Disjoint( newNode ) );
			}
		}

		if ( RectangleFits( i, height, width, y, wastedArea ) )
		{
			if ( wastedArea < bestWastedArea || ( wastedArea == bestWastedArea && y + width < bestHeight ) )
			{
				bestHeight = y + width;
				bestWastedArea = wastedArea;
				bestIndex = i;
				newNode.x = skyLine[i].x;
				newNode.y = y;
				newNode.width = height;
				newNode.height = width;
				assert( disjointRects.Disjoint( newNode ) );
			}
		}
	}

	return newNode;
}

/// Computes the ratio of used surface area.
float SkylineBinPack::Occupancy() const
{
	return ( float )usedSurfaceArea / ( binWidth * binHeight );
}

#if !defined(_DEBUG)
#	undef assert
#endif

} // namespace Guillotine

/*
 * 30/01/2012
     It's here
*/
