// Huffuman.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>

using namespace std;

// ÿһ�����Ŷ���Ϊһ���ṹ�壬�����ַ��ͳ��ֵ�Ƶ��
typedef struct // ����ṹ��
{
	unsigned char	character; // �ַ�
	unsigned int	frequency; // ���ֵ�Ƶ��
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

struct MinHeapNode // ��ʾ�������ڵ�
{
	char data;
	unsigned int freq; // Ƶ��/Ȩֵ
	MinHeapNode *left, *right; // �����ӽڵ�
	MinHeapNode(char data, unsigned freq) // ���캯��
	{
		left = right = NULL;
		this->data = data;
		this->freq = freq;
	}
};
typedef struct MinHeapNode MinHeapNode;

struct compare // �Ƚϴ�С
{
	bool operator() (MinHeapNode *l, MinHeapNode *r)
	{
		return (l->freq > r->freq);
	}
};

static void get_huffman_code(MinHeapNode *root, string code) // ��������������
{
	if (!root)
	{
		return;
	}

	if (root->data != -1) // �����Ϊ-1��˵�����˹����������ն˽ڵ�
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
	CharNode nodeArr[256] = { { 0, 0 } }; // �½�һ������(256��)��������ʼ��Ϊ0
	while (inputFile.good()) // û����Ҳû���ļ�ĩβ
	{
		cout << buf;
		nodeArr[buf].character = buf;
		nodeArr[buf].frequency++; // ͳ��Ƶ��
		buf = inputFile.get();
	}

	cout << endl;

	priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap; // �������ȼ�����
	for (int idx = 0; idx < 256; idx++)
	{
		if (nodeArr[idx].frequency > 0)
		{
			cout << "Node " << idx << ":[" << nodeArr[idx].character << ", " << nodeArr[idx].frequency << "]" << endl;
			minHeap.push(new MinHeapNode(nodeArr[idx].character, nodeArr[idx].frequency)); // ���д�С��������
		}
	}

	MinHeapNode *leftNode = NULL, *rightNode = NULL, *topNode = NULL;
	while (minHeap.size() != 1) // �����������������ȼ�������ֻʣ��1���ڵ㣨���ڵ㣩����ֹͣ����
	{
		leftNode = minHeap.top(); // ȡ����С��1���ڵ�
		minHeap.pop();

		rightNode = minHeap.top(); // ȡ����С��1���ڵ�
		minHeap.pop();

		topNode = new MinHeapNode(-1, leftNode->freq + rightNode->freq); // ���1���½ڵ㣨-1Ϊԭ�������ڵĽڵ㣩
		topNode -> left = leftNode;
		topNode->right = rightNode;
		minHeap.push(topNode);
	}

	get_huffman_code(topNode, "");

	inputFile.close();
	return 0;
}

