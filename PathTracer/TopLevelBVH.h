#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <glm/glm.hpp>

namespace GLSLPathTracer
{
	struct Bounds
	{
		Bounds()
			: lower(FLT_MAX)
			, upper(-FLT_MAX) {}

		Bounds(const glm::vec3& lower, const glm::vec3& upper) : lower(lower), upper(upper) {}

		glm::vec3 GetCenter() const { return 0.5f*(lower + upper); }
		glm::vec3 GetEdges() const { return upper - lower; }

		glm::vec3 lower;
		glm::vec3 upper;
	};

	struct TopLevelBVHNode
	{
		Bounds bounds;

		// for leaf nodes these store the range into the items array
		float leftIndex;
		float rightIndex;
		float leaf;
	};

	struct TopLevelBVH
	{
		TopLevelBVH() : nodes(NULL), numNodes(0) {}
		TopLevelBVHNode* nodes;
		int numNodes;
	};

	struct PartitionMedianPredicate
	{
		PartitionMedianPredicate(const Bounds* bounds, int a) : bounds(bounds), axis(a) {}

		bool operator()(int a, int b) const
		{
			return bounds[a].GetCenter()[axis] < bounds[b].GetCenter()[axis];
		}

		const Bounds* bounds;
		int axis;
	};

	class TopLevelBVHBuilder
	{
	public:
		TopLevelBVHBuilder(int maxItemsPerLeaf = 1) : maxItemsPerLeaf(maxItemsPerLeaf) {}
		TopLevelBVH* Build(const Bounds* items, int n);
		std::vector<TopLevelBVHNode> nodes;
		int usedNodes;
		std::vector<Bounds> bounds;
		std::vector<int> indices;
		int maxItemsPerLeaf;

	private:
		Bounds CalcBounds(const int* indices, int n);
		int LongestAxis(const glm::vec3& v);	
		float Area(const Bounds& b);
		int PartitionObjectsSAH(int start, int end, Bounds rangeBounds);
		int AddNode();
		int BuildRecursive(int start, int end);
	};
}
