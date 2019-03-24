// Huffuman.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>

using namespace std;

// 每一个符号定义为一个结构体，包括字符和出现的频次
typedef struct // 定义结构体
{
	unsigned char	character; // 字符
	unsigned int	frequency; // 出现的频次
} CharNode;

static bool open_input_file(ifstream & input, const char *inputFileName)
{
	input.open(inputFileName);
	if (!input.is_open())
	{
		return false;
	}
	return true;
}

struct MinHeapNode // 表示二叉树节点
{
	char data;
	unsigned int freq; // 频次/权值
	MinHeapNode *left, *right; // 左右子节点
	MinHeapNode(char data, unsigned freq) // 构造函数
	{
		left = right = NULL;
		this->data = data;
		this->freq = freq;
	}
};
typedef struct MinHeapNode MinHeapNode;

struct compare // 比较大小
{
	bool operator() (MinHeapNode *l, MinHeapNode *r)
	{
		return (l->freq > r->freq);
	}
};

static void get_huffman_code(MinHeapNode *root, string code) // 哈夫曼编码码字
{
	if (!root)
	{
		return;
	}

	if (root->data != -1) // 如果不为-1，说明到了哈夫曼树的终端节点
	{
		cout << root -> data << ": " << code << endl;
	}

	get_huffman_code(root->left, code + "0");
	get_huffman_code(root->right, code + "1");
}

int _tmain(int argc, _TCHAR* argv[])
{
	ifstream inputFile;
	if (!open_input_file(inputFile, "input.txt"))
	{
		cout << "Error: opening input file failed!" << endl;
		return -1;
	}

	char buf = inputFile.get();
	CharNode nodeArr[256] = { { 0, 0 } }; // 新建一个数组(256项)，并都初始化为0
	while (inputFile.good()) // 没出错，也没到文件末尾
	{
		cout << buf;
		nodeArr[buf].character = buf;
		nodeArr[buf].frequency++; // 统计频次
		buf = inputFile.get();
	}

	cout << endl;

	priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap; // 定义优先级队列
	for (int idx = 0; idx < 256; idx++)
	{
		if (nodeArr[idx].frequency > 0)
		{
			cout << "Node " << idx << ":[" << nodeArr[idx].character << ", " << nodeArr[idx].frequency << "]" << endl;
			minHeap.push(new MinHeapNode(nodeArr[idx].character, nodeArr[idx].frequency)); // 进行从小到大排序
		}
	}

	MinHeapNode *leftNode = NULL, *rightNode = NULL, *topNode = NULL;
	while (minHeap.size() != 1) // 建立哈夫曼树：优先级队列中只剩下1个节点（根节点），就停止运行
	{
		leftNode = minHeap.top(); // 取出最小的1个节点
		minHeap.pop();

		rightNode = minHeap.top(); // 取出最小的1个节点
		minHeap.pop();

		topNode = new MinHeapNode(-1, leftNode->freq + rightNode->freq); // 添加1个新节点（-1为原来不存在的节点）
		topNode -> left = leftNode;
		topNode->right = rightNode;
		minHeap.push(topNode);
	}

	get_huffman_code(topNode, "");

	inputFile.close();
	return 0;
}

