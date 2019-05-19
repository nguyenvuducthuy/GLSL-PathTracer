#include "TopLevelBVH.h"

#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <glm/glm.hpp>

namespace GLSLPathTracer
{

	Bounds Union(const Bounds& a, const Bounds& b)
	{
		return Bounds(glm::min(a.lower, b.lower), glm::max(a.upper, b.upper));
	};

	TopLevelBVH* TopLevelBVHBuilder::Build(const Bounds* items, int n)
	{
		nodes.resize(2 * n);
		usedNodes = 0;

		bounds.assign(items, items + n);

		// create indices array
		indices.resize(n);
		for (int i = 0; i < n; ++i)
			indices[i] = i;

		BuildRecursive(0, n);

		// copy nodes to a new array and return
		TopLevelBVHNode* nodesCopy = new TopLevelBVHNode[usedNodes];
		memcpy(nodesCopy, &nodes[0], usedNodes * sizeof(TopLevelBVHNode));

		TopLevelBVH *bvh = new TopLevelBVH();
		bvh->nodes = nodesCopy;
		bvh->numNodes = usedNodes;

		return bvh;
	}

	Bounds TopLevelBVHBuilder::CalcBounds(const int* indices, int n)
	{
		Bounds u;

		for (int i = 0; i < n; ++i)
			u = Union(u, bounds[indices[i]]);

		return u;
	}

	int TopLevelBVHBuilder::LongestAxis(const glm::vec3& v)
	{
		if (v.x > v.y && v.x > v.z)
			return 0;
		if (v.y > v.z)
			return 1;
		else
			return 2;
	}


	float TopLevelBVHBuilder::Area(const Bounds& b)
	{
		glm::vec3 edges = b.GetEdges();

		return 2.0f*(edges.x*edges.y + edges.x*edges.z + edges.y*edges.z);

	}

	int TopLevelBVHBuilder::PartitionObjectsSAH(int start, int end, Bounds rangeBounds)
	{
		assert(end - start >= 2);

		int n = end - start;
		glm::vec3 edges = rangeBounds.GetEdges();

		int longestAxis = LongestAxis(edges);

		// sort along longest axis
		std::sort(&indices[0] + start, &indices[0] + end, PartitionMedianPredicate(&bounds[0], longestAxis));

		// total area for range from [0, split]
		std::vector<float> leftAreas(n);
		// total area for range from (split, end]
		std::vector<float> rightAreas(n);

		Bounds left;
		Bounds right;

		// build cumulative bounds and area from left and right
		for (int i = 0; i < n; ++i)
		{
			left = Union(left, bounds[indices[start + i]]);
			right = Union(right, bounds[indices[end - i - 1]]);

			leftAreas[i] = Area(left);
			rightAreas[n - i - 1] = Area(right);
		}

		float invTotalArea = 1.0f / Area(rangeBounds);

		// find split point i that minimizes area(left[i]) + area(right[i])
		int minSplit = 0;
		float minCost = FLT_MAX;

		for (int i = 0; i < n; ++i)
		{
			float pBelow = leftAreas[i] * invTotalArea;
			float pAbove = rightAreas[i] * invTotalArea;

			float cost = pBelow * i + pAbove * (n - i);

			if (cost < minCost)
			{
				minCost = cost;
				minSplit = i;
			}
		}

		return start + minSplit + 1;
	}

	int TopLevelBVHBuilder::AddNode()
	{
		assert(usedNodes < nodes.size());

		int index = usedNodes;
		++usedNodes;

		return index;
	}

	// returns the index of the node created for this range [start, end)
	int TopLevelBVHBuilder::BuildRecursive(int start, int end)
	{
		assert(start < end);

		const int n = end - start;
		const int nodeIndex = AddNode();

		TopLevelBVHNode node;
		node.bounds = CalcBounds(&indices[start], end - start);

		if (n <= maxItemsPerLeaf)
		{
			node.leaf = -1.0f;
			node.leftIndex = indices[start];
			node.rightIndex = indices[start];
		}
		else
		{
			int split = PartitionObjectsSAH(start, end, node.bounds);

			if (split == start || split == end)
			{
				// partitioning failed, split down the middle
				split = (start + end) / 2;
			}

			node.leaf = 0.0f;
			node.leftIndex = BuildRecursive(start, split);
			node.rightIndex = BuildRecursive(split, end);
		}

		// output node
		nodes[nodeIndex] = node;

		return nodeIndex;
	}
}
