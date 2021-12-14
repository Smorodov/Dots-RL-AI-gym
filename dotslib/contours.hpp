#pragma once
#include <vector>

#define HOLE_BORDER 1
#define OUTER_BORDER 2
// ------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------
class Border
{
public:
	int seq_num;
	int border_type;
	// -----------------------------
	// 
	// -----------------------------
	inline Border()
	{
		seq_num=0;
		border_type=0;
	}
};
// ------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------
class GraphNode
{
public:
	int row;
	int col;
	// -----------------------------
	// 
	// -----------------------------
	inline GraphNode()
	{
		row = 0;
		col = 0;
	}
	// -----------------------------
	// 
	// -----------------------------
	inline GraphNode(int r, int c)
	{
		row = r;
		col = c;
	}
	// -----------------------------
	// 
	// -----------------------------
	inline void setGraphNode(int r, int c)
	{
		row = r;
		col = c;
	}
	// -----------------------------
	// 
	// -----------------------------
	inline bool sameGraphNode(GraphNode p)
	{
		return row == p.row && col == p.col;
	}
};

inline bool operator==(GraphNode& lhs, GraphNode& rhs)
{	
	return (lhs.row == rhs.row && lhs.col == rhs.col);
}
inline bool operator!=(GraphNode& lhs, GraphNode& rhs) { return !(lhs == rhs); }

inline bool operator==(const GraphNode& lhs, const  GraphNode& rhs)
{
	return (lhs.row == rhs.row && lhs.col == rhs.col);
}
inline bool operator!=(const GraphNode& lhs, const  GraphNode& rhs)
{
	return !(lhs == rhs);
}
// ------------------------------------------------------------------------
// class for storing information on the current border, the first child, next sibling, and the parent. 
// ------------------------------------------------------------------------
class HierarchyNode
{
public:
	int parent;
	int first_child;
	int next_sibling;
	Border border;
	// -----------------------------
	// 
	// -----------------------------
	inline HierarchyNode()
	{
		parent=-1;
		first_child=-1;
		next_sibling=-1;
	}
	// -----------------------------
	// 
	// -----------------------------
	inline HierarchyNode(int p, int fc, int ns)
	{
		parent = p;
		first_child = fc;
		next_sibling = ns;
	}
	// -----------------------------
	// 
	// -----------------------------
	inline void reset()
	{
		parent = -1;
		first_child = -1;
		next_sibling = -1;
	}
};
// ------------------------------------------------------------------------
// 4-connected version
// step around a pixel CCW 
// ------------------------------------------------------------------------
inline void stepCCW4(GraphNode &current, GraphNode pivot)
{
	if (current.col > pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col);
	}
	else if (current.col < pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col);
	}
	else if (current.row > pivot.row)
	{
		current.setGraphNode(pivot.row, pivot.col + 1);
	}
	else if (current.row < pivot.row)
	{
		current.setGraphNode(pivot.row, pivot.col - 1);
	}
}
// ------------------------------------------------------------------------
// 4-connected version
// step around a pixel CW 
// ------------------------------------------------------------------------
inline void stepCW4(GraphNode &current, GraphNode pivot)
{
	if (current.col > pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col);
	}
	else if (current.col < pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col);
	}
	else if (current.row > pivot.row)
	{
		current.setGraphNode(pivot.row, pivot.col - 1);
	}
	else if (current.row < pivot.row)
	{
		current.setGraphNode(pivot.row, pivot.col + 1);
	}
}
// ------------------------------------------------------------------------
// 8-connected version
// step around a pixel CCW in the 8-connect neighborhood.
// ------------------------------------------------------------------------
inline void stepCCW8(GraphNode &current, GraphNode pivot)
{
	if (current.row == pivot.row && current.col > pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col + 1);
	}
	else if (current.col > pivot.col && current.row < pivot.row)
	{
		current.setGraphNode(pivot.row - 1, pivot.col);
	}
	else if (current.row < pivot.row && current.col == pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col - 1);
	}
	else if (current.row < pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row, pivot.col - 1);
	}
	else if (current.row == pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col - 1);
	}
	else if (current.row > pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col);
	}
	else if (current.row > pivot.row && current.col == pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col + 1);
	}
	else if (current.row > pivot.row && current.col > pivot.col)
	{
		current.setGraphNode(pivot.row, pivot.col + 1);
	}
}
// ------------------------------------------------------------------------
// 8-connected version
// step around a pixel CW in the 8-connect neighborhood. 
// ------------------------------------------------------------------------
inline void stepCW8(GraphNode &current, GraphNode pivot)
{
	if (current.row == pivot.row && current.col > pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col + 1);
	}
	else if (current.col > pivot.col && current.row < pivot.row)
	{
		current.setGraphNode(pivot.row, pivot.col + 1);
	}
	else if (current.row < pivot.row && current.col == pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col + 1);
	}
	else if (current.row < pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col);
	}
	else if (current.row == pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row - 1, pivot.col - 1);
	}
	else if (current.row > pivot.row && current.col < pivot.col)
	{
		current.setGraphNode(pivot.row, pivot.col - 1);
	}
	else if (current.row > pivot.row && current.col == pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col - 1);
	}
	else if (current.row > pivot.row && current.col > pivot.col)
	{
		current.setGraphNode(pivot.row + 1, pivot.col);
	}
}
// ------------------------------------------------------------------------
// checks if a given pixel is out of bounds of the graph 
// ------------------------------------------------------------------------
inline bool nodeOutOfBounds(GraphNode p, int numrows, int numcols)
{
	return (p.col >= numcols || p.row >= numrows || p.col < 0 || p.row < 0);
}
// ------------------------------------------------------------------------
// 4-connected version
// marks a node as examined after passing through
// ------------------------------------------------------------------------
inline void markExamined4(GraphNode mark, GraphNode center, bool checked[4])
{
	//p3.row, p3.col + 1
	int loc = -1;
	//    3
	//  2 x 0
	//    1
	if (mark.col > center.col)
	{
		loc = 0;
	}
	else if (mark.col < center.col)
	{
		loc = 2;
	}
	else if (mark.row > center.row)
	{
		loc = 1;
	}
	else if (mark.row < center.row)
	{
		loc = 3;
	}
	if (loc == -1)
	{
		throw std::exception("Error: markExamined Failed");
	}
	checked[loc] = true;
	return;
}

// ------------------------------------------------------------------------
// 8-connected version
// marks a pixel as examined after passing through in the 8-connected case
// ------------------------------------------------------------------------
inline void markExamined8(GraphNode mark, GraphNode center, bool checked[8])
{
	//p3.row, p3.col + 1
	int loc = -1;
	//  5 6 7
	//  4 x 0
	//  3 2 1
	if (mark.row == center.row && mark.col > center.col)
	{
		loc = 0;
	}
	else if (mark.col > center.col && mark.row < center.row)
	{
		loc = 7;
	}
	else if (mark.row < center.row && mark.col == center.col)
	{
		loc = 6;
	}
	else if (mark.row < center.row && mark.col < center.col)
	{
		loc = 5;
	}
	else if (mark.row == center.row && mark.col < center.col)
	{
		loc = 4;
	}
	else if (mark.row > center.row && mark.col < center.col)
	{
		loc = 3;
	}
	else if (mark.row > center.row && mark.col == center.col)
	{
		loc = 2;
	}
	else if (mark.row > center.row && mark.col > center.col)
	{
		loc = 1;
	}
	if (loc == -1)
	{
		throw std::exception("Error: markExamined Failed");
	}
	checked[loc] = true;
	return;
}

// ------------------------------------------------------------------------
// 4-connected version
//checks if given pixel has already been examined
// ------------------------------------------------------------------------
inline bool isExamined4(bool checked[4])
{
	//p3.row, p3.col + 1
	return checked[0];
}

// ------------------------------------------------------------------------
// 8-connected version 
// ------------------------------------------------------------------------
inline bool isExamined8(bool checked[8])
{
	//p3.row, p3.col + 1
	return checked[0];
}

// ------------------------------------------------------------------------
// follows a border from start to finish given a starting point 
// ------------------------------------------------------------------------
inline void followBorder4(std::vector<std::vector<int>> &image, int row, int col, GraphNode p2, Border NBD, std::vector<std::vector<GraphNode>> &contours)
{
	int numrows = image.size();
	int numcols = image[0].size();
	GraphNode current(p2.row, p2.col);
	GraphNode start(row, col);
	std::vector<GraphNode> point_storage;
	//(3.1)
	//Starting from (i2, j2), look around clockwise the pixels in the neighborhood of (i, j) and find a nonzero pixel.
	//Let (i1, j1) be the first found nonzero pixel. If no nonzero pixel is found, assign -NBD to fij and go to (4).
	do
	{
		stepCW4(current, start);
		if (current.sameGraphNode(p2))
		{
			image[start.row][start.col] = -NBD.seq_num;
			point_storage.push_back(start);
			contours.push_back(point_storage);
			return;
		}
	}
	while (nodeOutOfBounds(current, numrows, numcols) || image[current.row][current.col] == 0);
	GraphNode p1 = current;
	//(3.2)
	//(i2, j2) <- (i1, j1) and (i3, j3) <- (i, j).
	GraphNode p3 = start;
	GraphNode p4;
	p2 = p1;
	bool checked[4];	
	while (true)
	{
		//(3.3)
		//Starting from the next element of the pixel(i2, j2) in the counterclockwise order, examine counterclockwise the pixels in the
		//neighborhood of the current pixel(i3, j3) to find a nonzero pixel and let the first one be(i4, j4).
		current = p2;
		for (int i = 0; i <4; i++)
		{
			checked[i] = false;
		}
		do
		{
			markExamined4(current, p3, checked);
			stepCCW4(current, p3);
		}
		while (nodeOutOfBounds(current, numrows, numcols) || image[current.row][current.col] == 0);
		p4 = current;
		//Change the value fi3, j3 of the pixel(i3, j3) as follows :
		//	If the pixel(i3, j3 + 1) is a 0 - pixel examined in the substep(3.3) then fi3, j3 <- - NBD.
		//	If the pixel(i3, j3 + 1) is not a 0 - pixel examined in the substep(3.3) and fi3, j3 = 1, then fi3, j3 ←NBD.
		//	Otherwise, do not change fi3, j3.
		if ( (p3.col+1 >= numcols || image[p3.row][p3.col + 1] == 0) && isExamined4(checked))
		{
			image[p3.row][p3.col] = -NBD.seq_num;
		}
		else if (p3.col + 1 < numcols && image[p3.row][p3.col] == 1)
		{
			image[p3.row][p3.col] = NBD.seq_num;
		}
		point_storage.push_back(p3);
		//(3.5)
		//If(i4, j4) = (i, j) and (i3, j3) = (i1, j1) (coming back to the starting point), then go to(4);
		//otherwise, (i2, j2) <- (i3, j3), (i3, j3) <- (i4, j4), and go back to(3.3).
		if (p4.sameGraphNode(start) && p3.sameGraphNode(p1))
		{
			contours.push_back(point_storage);
			return;
		}
		p2 = p3;
		p3 = p4;
	}
}

// ------------------------------------------------------------------------
// follows a border from start to finish given a starting point 
// ------------------------------------------------------------------------
inline void followBorder8(std::vector<std::vector<int>>& graph, int row, int col, GraphNode p2, Border NBD, std::vector<std::vector<GraphNode>>& contours)
{
	int numrows = graph.size();
	int numcols = graph[0].size();
	GraphNode current(p2.row, p2.col);
	GraphNode start(row, col);
	std::vector<GraphNode> point_storage;
	//(3.1)
	//Starting from (i2, j2), look around clockwise the pixels in the neighborhood of (i, j) and find a nonzero pixel.
	//Let (i1, j1) be the first found nonzero pixel. If no nonzero pixel is found, assign -NBD to fij and go to (4).
	do
	{
		stepCW8(current, start);
		if (current.sameGraphNode(p2))
		{
			graph[start.row][start.col] = -NBD.seq_num;
			point_storage.push_back(start);
			contours.push_back(point_storage);
			return;
		}
	} while (nodeOutOfBounds(current, numrows, numcols) || graph[current.row][current.col] == 0);
	GraphNode p1 = current;
	//(3.2)
	//(i2, j2) <- (i1, j1) and (i3, j3) <- (i, j).
	GraphNode p3 = start;
	GraphNode p4;
	p2 = p1;
	bool checked[8];
	while (true)
	{
		//(3.3)
		//Starting from the next element of the pixel(i2, j2) in the counterclockwise order, examine counterclockwise the pixels in the
		//neighborhood of the current pixel(i3, j3) to find a nonzero pixel and let the first one be(i4, j4).
		current = p2;
		for (int i = 0; i < 8; i++)
		{
			checked[i] = false;
		}
		do
		{
			markExamined8(current, p3, checked);
			stepCCW8(current, p3);
		} while (nodeOutOfBounds(current, numrows, numcols) || graph[current.row][current.col] == 0);
		p4 = current;
		//Change the value fi3, j3 of the pixel(i3, j3) as follows :
		//	If the pixel(i3, j3 + 1) is a 0 - pixel examined in the substep(3.3) then fi3, j3 <- - NBD.
		//	If the pixel(i3, j3 + 1) is not a 0 - pixel examined in the substep(3.3) and fi3, j3 = 1, then fi3, j3 ←NBD.
		//	Otherwise, do not change fi3, j3.
		if ((p3.col + 1 >= numcols || graph[p3.row][p3.col + 1] == 0) && isExamined8(checked))
		{
			graph[p3.row][p3.col] = -NBD.seq_num;
		}
		else if (p3.col + 1 < numcols && graph[p3.row][p3.col] == 1)
		{
			graph[p3.row][p3.col] = NBD.seq_num;
		}
		point_storage.push_back(p3);
		//(3.5)
		//If(i4, j4) = (i, j) and (i3, j3) = (i1, j1) (coming back to the starting point), then go to(4);
		//otherwise, (i2, j2) <- (i3, j3), (i3, j3) <- (i4, j4), and go back to(3.3).
		if (p4.sameGraphNode(start) && p3.sameGraphNode(p1))
		{
			contours.push_back(point_storage);
			return;
		}
		p2 = p3;
		p3 = p4;
	}
}
// ------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------
inline int detectContours(std::vector<std::vector<int>> graph, std::vector<std::vector<GraphNode>>& contours, std::vector<HierarchyNode>& hierarchy, bool is8connected=true)
{
	Border NBD, LNBD;
	if (graph.empty())
	{
		throw std::exception("Graph is empty.");
	}
	if (graph[0].empty())
	{
		throw std::exception("Graph first row is empty.");
	}
	else
	{
		for (auto r : graph)
		{
			if (graph[0].size()!= r.size())
			{
				throw std::exception("Graph must have the same number of nodes for each row.");
			}
		}
	}

	int numrows = graph.size();
	int numcols = graph[0].size();

	LNBD.border_type = HOLE_BORDER;
	NBD.border_type = HOLE_BORDER;
	NBD.seq_num = 1;
	//hierarchy tree will be stored as an vector of nodes instead of using an actual tree since we need to access a node based on its index
	//see definition for HierarchyNode
	//-1 denotes NULL
	//vector<HierarchyNode> hierarchy;
	HierarchyNode temp_node(-1, -1, -1);
	temp_node.border = NBD;
	hierarchy.push_back(temp_node);
	GraphNode p2;
	bool border_start_found;
	for (int r = 0; r < numrows; ++r)
	{
		LNBD.seq_num = 1;
		LNBD.border_type = HOLE_BORDER;
		for (int c = 0; c < numcols; c++)
		{
			border_start_found = false;
			//Phase 1: Find border
			//If fij = 1 and fi, j-1 = 0, then decide that the pixel (i, j) is the border following starting point
			//of an outer border, increment NBD, and (i2, j2) <- (i, j - 1).
			if ((graph[r][c] == 1 && c - 1 < 0) || (graph[r][c] == 1 && graph[r][c - 1] == 0))
			{
				NBD.border_type = OUTER_BORDER;
				NBD.seq_num += 1;
				p2.setGraphNode(r,c-1);
				border_start_found = true;
			}
			//Else if fij >= 1 and fi,j+1 = 0, then decide that the pixel (i, j) is the border following
			//starting point of a hole border, increment NBD, (i2, j2) ←(i, j + 1), and LNBD ← fij in case fij > 1.
			else if ( c+1 < numcols && (graph[r][c] >= 1 && graph[r][c + 1] == 0))
			{
				NBD.border_type = HOLE_BORDER;
				NBD.seq_num += 1;
				if (graph[r][c] > 1)
				{
					LNBD.seq_num = graph[r][c];
					LNBD.border_type = hierarchy[LNBD.seq_num-1].border.border_type;
				}
				p2.setGraphNode(r, c + 1);
				border_start_found = true;
			}
			if (border_start_found)
			{
				//Phase 2: Store Parent
				//				current = new TreeNode(NBD);
				temp_node.reset();
				if (NBD.border_type == LNBD.border_type)
				{
					temp_node.parent = hierarchy[LNBD.seq_num - 1].parent;
					temp_node.next_sibling = hierarchy[temp_node.parent - 1].first_child;
					hierarchy[temp_node.parent - 1].first_child = NBD.seq_num;
					temp_node.border = NBD;
					hierarchy.push_back(temp_node);
					//					cout << "indirect: " << NBD.seq_num << "  parent: " << LNBD.seq_num <<endl;
				}
				else
				{
					if (hierarchy[LNBD.seq_num-1].first_child != -1)
					{
						temp_node.next_sibling = hierarchy[LNBD.seq_num-1].first_child;
					}
					temp_node.parent = LNBD.seq_num;
					hierarchy[LNBD.seq_num-1].first_child = NBD.seq_num;
					temp_node.border = NBD;
					hierarchy.push_back(temp_node);
					//					cout << "direct: " << NBD.seq_num << "  parent: " << LNBD.seq_num << endl;
				}
				//Phase 3: Follow border
				if (is8connected)
				{
					followBorder8(graph, r, c, p2, NBD, contours);
				}
				else
				{
					followBorder4(graph, r, c, p2, NBD, contours);
				}
			}
			//Phase 4: Continue to next border
			//If fij != 1, then LNBD <- abs( fij ) and resume the raster scan from the pixel(i, j + 1).
			//The algorithm terminates when the scan reaches the lower right corner of the picture.
			if (abs(graph[r][c]) > 1)
			{
				LNBD.seq_num = abs(graph[r][c]);
				LNBD.border_type = hierarchy[LNBD.seq_num - 1].border.border_type;
			}
		}
	}
	return contours.size();
}


// ---------------------------------------
// tools
// ---------------------------------------
bool isAllInOneRow(std::vector<GraphNode>& contour)
{
	bool result = true;
	int  start = contour[0].row;
	for (auto p : contour)
	{
		if (start != p.row)
		{
			result = false;
			break;
		}
	}
	return result;
}
// ---------------------------------------
// if all points in contour have the same row
// it will hang up. So, need to check it.
// See isAllInOneRow method. Checked in contains method.
// ---------------------------------------
inline GraphNode getPrevPoint(std::vector<GraphNode>& contour, std::vector<GraphNode>::iterator current)
{
	int currentY = current->row;
	for (auto prev = current;;)
	{
		if (prev == contour.begin())
			prev = --contour.end();
		else
			--prev;

		if (prev->row != currentY)
			return *prev;
	}
}

// ---------------------------------------
// if all points in contour have the same row
// it will hang up. So, need to check it.
// See isAllInOneRow method. Checked in contains method.
// ---------------------------------------
inline GraphNode getNextPoint(std::vector<GraphNode>& contour, int& shift, std::vector<GraphNode>::iterator current)
{
	int currentY = current->row;
	shift = 0;
	for (auto next = current;;)
	{
		++shift;
		if (next == --contour.end())
			next = contour.begin();
		else
			++next;

		if (next->row != currentY)
			return *next;
	}
}
// -------------------------------
// Scan row 
// -------------------------------
inline bool contains(GraphNode& point, std::vector<GraphNode>& contour)
{
	if (isAllInOneRow(contour))
	{
		return false;
	}
	// k - a count of points in the same line with "point" object
	// i - crosses count
	int i = 0, shift;

	auto itr = contour.begin();
	auto itrEnd = contour.end();
	while (itr != itrEnd)
	{
		if (itr->row != point.row)
		{
			++itr;
			continue;
		}

		if (itr->col == point.col)
			return true;

		GraphNode& prevPoint = getPrevPoint(contour, itr);
		GraphNode& nextPoint = getNextPoint(contour, shift, itr);

		if (itr->col < point.col && prevPoint.row != nextPoint.row && shift == 1)
			++i;

		++itr;
	}
	return i % 2;
}


inline int doubleArea(std::vector<GraphNode>& polygon)
{
	int res = 0;
	GraphNode prevPoint = polygon.back();
	for (auto itr = polygon.begin(), itrEnd = polygon.end(); itr != itrEnd; ++itr)
	{
		res += (itr->col - prevPoint.col) * (itr->row + prevPoint.row);
		prevPoint = *itr;
	}
	return std::abs(res);
}


