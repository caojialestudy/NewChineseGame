/**
* XiangQi Wizard Light - A Very Simple Chinese Chess Program
* Designed by Morning Yellow, Version: 0.6, Last Modified: Mar. 2008
* Copyright (C) 2004-2008 www.xqbase.com
*
* ����С��ʦ 0.6 ��Ŀ�꣺
* һ��ʵ�ֿ��ֿ⣻
* ����ʵ��PVS(��Ҫ��������)��
* �����Ѹ��ڵ������������������������������ԣ�
* �ġ��˷��ɳ���������û����Ĳ��ȶ��ԡ�
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "Define.h"

using namespace std;

// ���̷�Χ
const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_RIGHT = 11;

// ���ӱ��
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;

// ��������
const int MAX_GEN_MOVES = 128; // ���������߷���
const int MAX_MOVES = 256;     // ������ʷ�߷���
const int LIMIT_DEPTH = 64;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int BAN_VALUE = MATE_VALUE - 100; // �����и��ķ�ֵ�����ڸ�ֵ����д���û���
const int WIN_VALUE = MATE_VALUE - 200; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int DRAW_VALUE = 20;     // ����ʱ���صķ���(ȡ��ֵ)
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ
const int RANDOM_MASK = 7;     // ����Է�ֵ
const int NULL_MARGIN = 400;   // �ղ��ü��������߽�
const int NULL_DEPTH = 2;      // �ղ��ü��Ĳü����
const int HASH_SIZE = 1 << 20; // �û�����С
const int HASH_ALPHA = 1;      // ALPHA�ڵ���û�����
const int HASH_BETA = 2;       // BETA�ڵ���û�����
const int HASH_PV = 3;         // PV�ڵ���û�����
const int BOOK_SIZE = 16384;   // ���ֿ��С

// �ж������Ƿ��������е�����
static const char ccInBoard[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// �ж������Ƿ��ھŹ�������
static const char ccInFort[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// �жϲ����Ƿ�����ض��߷������飬1=˧(��)��2=��(ʿ)��3=��(��)
static const char ccLegalSpan[512] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};

// ���ݲ����ж����Ƿ����ȵ�����
static const char ccKnightPin[512] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, -16, 0, -16, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 16, 0, 16, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};

// ˧(��)�Ĳ���
static const char ccKingDelta[4] = { -16, -1, 1, 16 };
// ��(ʿ)�Ĳ���
static const char ccAdvisorDelta[4] = { -17, -15, 15, 17 };
// ���Ĳ�������˧(��)�Ĳ�����Ϊ����
static const char ccKnightDelta[4][2] = { { -33, -31 }, { -18, 14 }, { -14, 18 }, { 31, 33 } };
// ���������Ĳ���������(ʿ)�Ĳ�����Ϊ����
static const char ccKnightCheckDelta[4][2] = { { -33, -18 }, { -31, -14 }, { 14, 31 }, { 18, 33 } };

// ���̳�ʼ����
static const unsigned char cucpcStartup[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 20, 19, 18, 17, 16, 17, 18, 19, 20, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0,
	0, 0, 0, 22, 0, 22, 0, 22, 0, 22, 0, 22, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 0, 0, 0,
	0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 12, 11, 10, 9, 8, 9, 10, 11, 12, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ����λ�ü�ֵ��
static const BYTE cucvlPiecePos[7][256] = {
	{ // ˧(��)
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 11, 15, 11, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��(ʿ)
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 20, 0, 20, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 20, 0, 20, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��(��)
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 20, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 18, 0, 0, 0, 23, 0, 0, 0, 18, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 20, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 90, 90, 90, 96, 90, 96, 90, 90, 90, 0, 0, 0, 0,
		0, 0, 0, 90, 96, 103, 97, 94, 97, 103, 96, 90, 0, 0, 0, 0,
		0, 0, 0, 92, 98, 99, 103, 99, 103, 99, 98, 92, 0, 0, 0, 0,
		0, 0, 0, 93, 108, 100, 107, 100, 107, 100, 108, 93, 0, 0, 0, 0,
		0, 0, 0, 90, 100, 99, 103, 104, 103, 99, 100, 90, 0, 0, 0, 0,
		0, 0, 0, 90, 98, 101, 102, 103, 102, 101, 98, 90, 0, 0, 0, 0,
		0, 0, 0, 92, 94, 98, 95, 98, 95, 98, 94, 92, 0, 0, 0, 0,
		0, 0, 0, 93, 92, 94, 95, 92, 95, 94, 92, 93, 0, 0, 0, 0,
		0, 0, 0, 85, 90, 92, 93, 78, 93, 92, 90, 85, 0, 0, 0, 0,
		0, 0, 0, 88, 85, 90, 88, 90, 88, 90, 85, 88, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 206, 208, 207, 213, 214, 213, 207, 208, 206, 0, 0, 0, 0,
		0, 0, 0, 206, 212, 209, 216, 233, 216, 209, 212, 206, 0, 0, 0, 0,
		0, 0, 0, 206, 208, 207, 214, 216, 214, 207, 208, 206, 0, 0, 0, 0,
		0, 0, 0, 206, 213, 213, 216, 216, 216, 213, 213, 206, 0, 0, 0, 0,
		0, 0, 0, 208, 211, 211, 214, 215, 214, 211, 211, 208, 0, 0, 0, 0,
		0, 0, 0, 208, 212, 212, 214, 215, 214, 212, 212, 208, 0, 0, 0, 0,
		0, 0, 0, 204, 209, 204, 212, 214, 212, 204, 209, 204, 0, 0, 0, 0,
		0, 0, 0, 198, 208, 204, 212, 212, 212, 204, 208, 198, 0, 0, 0, 0,
		0, 0, 0, 200, 208, 206, 212, 200, 212, 206, 208, 200, 0, 0, 0, 0,
		0, 0, 0, 194, 206, 204, 212, 200, 212, 204, 206, 194, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 100, 100, 96, 91, 90, 91, 96, 100, 100, 0, 0, 0, 0,
		0, 0, 0, 98, 98, 96, 92, 89, 92, 96, 98, 98, 0, 0, 0, 0,
		0, 0, 0, 97, 97, 96, 91, 92, 91, 96, 97, 97, 0, 0, 0, 0,
		0, 0, 0, 96, 99, 99, 98, 100, 98, 99, 99, 96, 0, 0, 0, 0,
		0, 0, 0, 96, 96, 96, 96, 100, 96, 96, 96, 96, 0, 0, 0, 0,
		0, 0, 0, 95, 96, 99, 96, 100, 96, 99, 96, 95, 0, 0, 0, 0,
		0, 0, 0, 96, 96, 96, 96, 96, 96, 96, 96, 96, 0, 0, 0, 0,
		0, 0, 0, 97, 96, 100, 99, 101, 99, 100, 96, 97, 0, 0, 0, 0,
		0, 0, 0, 96, 97, 98, 98, 98, 98, 98, 97, 96, 0, 0, 0, 0,
		0, 0, 0, 96, 96, 97, 99, 99, 99, 97, 96, 96, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}, { // ��(��)
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 9, 9, 9, 11, 13, 11, 9, 9, 9, 0, 0, 0, 0,
		0, 0, 0, 19, 24, 34, 42, 44, 42, 34, 24, 19, 0, 0, 0, 0,
		0, 0, 0, 19, 24, 32, 37, 37, 37, 32, 24, 19, 0, 0, 0, 0,
		0, 0, 0, 19, 23, 27, 29, 30, 29, 27, 23, 19, 0, 0, 0, 0,
		0, 0, 0, 14, 18, 20, 27, 29, 27, 20, 18, 14, 0, 0, 0, 0,
		0, 0, 0, 7, 0, 13, 0, 16, 0, 13, 0, 7, 0, 0, 0, 0,
		0, 0, 0, 7, 0, 7, 0, 15, 0, 7, 0, 7, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
};

// �ж������Ƿ���������
inline BOOL IN_BOARD(int sq) {
	return ccInBoard[sq] != 0;
}

// �ж������Ƿ��ھŹ���
inline BOOL IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

// ��ø��ӵĺ�����
inline int RANK_Y(int sq) {
	return sq >> 4;
}

// ��ø��ӵ�������
inline int FILE_X(int sq) {
	return sq & 15;
}

// ����������ͺ������ø���
inline int COORD_XY(int x, int y) {
	return x + (y << 4);
}

// ��ת����
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// ������ˮƽ����
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// �����괹ֱ����
inline int RANK_FLIP(int y) {
	return 15 - y;
}

// ����ˮƽ����
inline int MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// ����ˮƽ����
inline int SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

// �߷��Ƿ����˧(��)�Ĳ���
inline BOOL KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// �߷��Ƿ������(ʿ)�Ĳ���
inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// �߷��Ƿ������(��)�Ĳ���
inline BOOL BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// ��(��)�۵�λ��
inline int BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

// ���ȵ�λ��
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// �Ƿ�δ����
inline BOOL HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

// �Ƿ��ѹ���
inline BOOL AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

// �Ƿ��ںӵ�ͬһ��
inline BOOL SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// ��ú�ڱ��(������8��������16)
inline int SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

// ��öԷ���ڱ��
inline int OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

// ����߷������
inline int SRC(int mv) {
	return mv & 255;
}

// ����߷����յ�
inline int DST(int mv) {
	return mv >> 8;
}

// ���������յ����߷�
inline int MOVE(int sqSrc, int sqDst) {
	return sqSrc + sqDst * 256;
}

// �߷�ˮƽ����
inline int MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

// RC4������������
struct RC4Struct {
	BYTE s[256];
	int x, y;

	void InitZero(void);   // �ÿ���Կ��ʼ��������������
	BYTE NextByte(void) {  // ��������������һ���ֽ�
		BYTE uc;
		x = (x + 1) & 255;
		y = (y + s[x]) & 255;
		uc = s[x];
		s[x] = s[y];
		s[y] = uc;
		return s[(s[x] + s[y]) & 255];
	}
	DWORD NextLong(void) { // ���������������ĸ��ֽ�
		BYTE uc0, uc1, uc2, uc3;
		uc0 = NextByte();
		uc1 = NextByte();
		uc2 = NextByte();
		uc3 = NextByte();
		return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
	}
};

// �ÿ���Կ��ʼ��������������
void RC4Struct::InitZero(void) {
	int i, j;
	BYTE uc;

	x = y = j = 0;
	for (i = 0; i < 256; i++) {
		s[i] = i;
	}
	for (i = 0; i < 256; i++) {
		j = (j + s[i]) & 255;
		uc = s[i];
		s[i] = s[j];
		s[j] = uc;
	}
}

// Zobrist�ṹ
struct ZobristStruct {
	DWORD dwKey, dwLock0, dwLock1;

	void InitZero(void) {                 // �������Zobrist
		dwKey = dwLock0 = dwLock1 = 0;
	}
	void InitRC4(RC4Struct &rc4) {        // �����������Zobrist
		dwKey = rc4.NextLong();
		dwLock0 = rc4.NextLong();
		dwLock1 = rc4.NextLong();
	}
	void Xor(const ZobristStruct &zobr) { // ִ��XOR����
		dwKey ^= zobr.dwKey;
		dwLock0 ^= zobr.dwLock0;
		dwLock1 ^= zobr.dwLock1;
	}
	void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
		dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
		dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
		dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
	}
};

// Zobrist��
static struct {
	ZobristStruct Player;
	ZobristStruct Table[14][256];
} Zobrist;

// ��ʼ��Zobrist��
static void InitZobrist(void) {
	int i, j;
	RC4Struct rc4;

	rc4.InitZero();
	Zobrist.Player.InitRC4(rc4);
	for (i = 0; i < 14; i++) {
		for (j = 0; j < 256; j++) {
			Zobrist.Table[i][j].InitRC4(rc4);
		}
	}
}

// ��ʷ�߷���Ϣ(ռ4�ֽ�)
struct MoveStruct {
	WORD wmv;
	BYTE ucpcCaptured, ucbCheck;
	DWORD dwKey;

	void Set(int mv, int pcCaptured, BOOL bCheck, DWORD dwKey_) {
		wmv = mv;
		ucpcCaptured = pcCaptured;
		ucbCheck = bCheck;
		dwKey = dwKey_;
	}
}; // mvs

// ����ṹ
struct PositionStruct {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance, nMoveNum;        // ������ڵ�Ĳ�������ʷ�߷���
	MoveStruct mvsList[MAX_MOVES];  // ��ʷ�߷���Ϣ�б�(AI�������ܻ��õ�����Ҫ�޸�)
	ZobristStruct zobr;             // Zobrist

	void ClearBoard(void) {         // �������
		sdPlayer = vlWhite = vlBlack = nDistance = 0;
		memset(ucpcSquares, 0, 256);
		zobr.InitZero();
	}
	void SetIrrev(void) {           // ���(��ʼ��)��ʷ�߷���Ϣ
		mvsList[0].Set(0, 0, Checked(), zobr.dwKey);
		nMoveNum = 1;
	}

	void Startup(void);             // ��ʼ������
	void ChangeSide(void) {         // �������ӷ�
		sdPlayer = 1 - sdPlayer;
		zobr.Xor(Zobrist.Player);
	}
	void AddPiece(int sq, int pc) { // �������Ϸ�һö����
		ucpcSquares[sq] = pc;
		// �췽�ӷ֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)����
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
			zobr.Xor(Zobrist.Table[pc - 8][sq]);
		}
		else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
			zobr.Xor(Zobrist.Table[pc - 9][sq]);
		}
	}
	void DelPiece(int sq, int pc) { // ������������һö����
		ucpcSquares[sq] = 0;
		// �췽���֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)�ӷ�
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
			zobr.Xor(Zobrist.Table[pc - 8][sq]);
		}
		else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
			zobr.Xor(Zobrist.Table[pc - 9][sq]);
		}
	}
	int Evaluate(void) const {      // �������ۺ���
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	BOOL InCheck(void) const {      // �Ƿ񱻽���
		return mvsList[nMoveNum - 1].ucbCheck;
	}
	BOOL Captured(void) const {     // ��һ���Ƿ����
		return mvsList[nMoveNum - 1].ucpcCaptured != 0;
	}
	int MovePiece(int mv);                      // ��һ���������
	void UndoMovePiece(int mv, int pcCaptured); // ������һ���������
	BOOL MakeMove(int mv);                      // ��һ����
	void UndoMakeMove(void) {                   // ������һ����
		nDistance--;
		nMoveNum--;
		ChangeSide();
		UndoMovePiece(mvsList[nMoveNum].wmv, mvsList[nMoveNum].ucpcCaptured);
	} 
	
	void NullMove(void) {                       // ��һ���ղ�
		DWORD dwKey;
		dwKey = zobr.dwKey;
		ChangeSide();
		mvsList[nMoveNum].Set(0, 0, FALSE, dwKey);
		nMoveNum++;
		nDistance++;
	}
	void UndoNullMove(void) {                   // ������һ���ղ�
		nDistance--;
		nMoveNum--;
		ChangeSide();
	}
	// ���������߷������"bCapture"Ϊ"TRUE"��ֻ���ɳ����߷�
	int GenerateMoves(int *mvs, BOOL bCapture = FALSE) const;
	BOOL LegalMove(int mv) const;               // �ж��߷��Ƿ����
	BOOL Checked(void) const;                   // �ж��Ƿ񱻽���
	BOOL IsMate(void);                          // �ж��Ƿ�ɱ
	int DrawValue(void) const {                 // �����ֵ
		return (nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
	}
	int RepStatus(int nRecur = 1) const;        // ����ظ�����
	int RepValue(int nRepStatus) const {        // �ظ������ֵ
		int vlReturn;
		vlReturn = ((nRepStatus & 2) == 0 ? 0 : nDistance - BAN_VALUE) +
			((nRepStatus & 4) == 0 ? 0 : BAN_VALUE - nDistance);
		return vlReturn == 0 ? DrawValue() : vlReturn;
	}
	BOOL NullOkay(void) const {                 // �ж��Ƿ������ղ��ü�
		return (sdPlayer == 0 ? vlWhite : vlBlack) > NULL_MARGIN;
	}
	void Mirror(PositionStruct &posMirror) const; // �Ծ��澵��
};

// ��ʼ������
void PositionStruct::Startup(void) {
	int sq, pc;
	ClearBoard();
	for (sq = 0; sq < 256; sq++) {
		pc = cucpcStartup[sq];
		if (pc != 0) {
			AddPiece(sq, pc);
		}
	}
	SetIrrev();
}

// ��һ���������
int PositionStruct::MovePiece(int mv) {
	int sqSrc, sqDst, pc, pcCaptured;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pcCaptured = ucpcSquares[sqDst];
	if (pcCaptured != 0) {
		DelPiece(sqDst, pcCaptured);
	}
	pc = ucpcSquares[sqSrc];
	DelPiece(sqSrc, pc);
	AddPiece(sqDst, pc);
	return pcCaptured;
}

// ������һ���������,pcΪ��mvʱ�յ�ԭ��������
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
	int sqSrc, sqDst, pc;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pc = ucpcSquares[sqDst];
	DelPiece(sqDst, pc);
	AddPiece(sqSrc, pc);
	if (pcCaptured != 0) {
		AddPiece(sqDst, pcCaptured);
	}
}

// ��һ����
BOOL PositionStruct::MakeMove(int mv) {
	int pcCaptured;
	DWORD dwKey;
	dwKey = zobr.dwKey;
	pcCaptured = MovePiece(mv);
	if (Checked()) {
		UndoMovePiece(mv, pcCaptured);
		return FALSE;
	}
	ChangeSide();
	mvsList[nMoveNum].Set(mv, pcCaptured, Checked(), dwKey);
	nMoveNum++;
	nDistance++;
	return TRUE;
}



// "GenerateMoves"����
const BOOL GEN_CAPTURE = TRUE;

// ���������߷������"bCapture"Ϊ"TRUE"��ֻ���ɳ����߷�
int PositionStruct::GenerateMoves(int *mvs, BOOL bCapture) const {
	int i, j, nGenMoves, nDelta, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcSrc, pcDst;
	// ���������߷�����Ҫ�������¼������裺

	nGenMoves = 0;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	for (sqSrc = 0; sqSrc < 256; sqSrc++) {

		// 1. �ҵ�һ���������ӣ����������жϣ�
		pcSrc = ucpcSquares[sqSrc];
		if ((pcSrc & pcSelfSide) == 0) {
			continue;
		}

		// 2. ��������ȷ���߷�
		switch (pcSrc - pcSelfSide) {
		case PIECE_KING:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_ADVISOR:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_BISHOP:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0)) {
					continue;
				}
				sqDst += ccAdvisorDelta[i];
				pcDst = ucpcSquares[sqDst];
				if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_KNIGHT:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (ucpcSquares[sqDst] != 0) {
					continue;
				}
				for (j = 0; j < 2; j++) {
					sqDst = sqSrc + ccKnightDelta[i][j];
					if (!IN_BOARD(sqDst)) {
						continue;
					}
					pcDst = ucpcSquares[sqDst];
					if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						if (!bCapture) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
					}
					else {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_CANNON:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						if (!bCapture) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
					}
					else {
						break;
					}
					sqDst += nDelta;
				}
				sqDst += nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst != 0) {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_PAWN:
			sqDst = SQUARE_FORWARD(sqSrc, sdPlayer);
			if (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			if (AWAY_HALF(sqSrc, sdPlayer)) {
				for (nDelta = -1; nDelta <= 1; nDelta += 2) {
					sqDst = sqSrc + nDelta;
					if (IN_BOARD(sqDst)) {
						pcDst = ucpcSquares[sqDst];
						if (bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
					}
				}
			}
			break;
		}
	}
	return nGenMoves;
}

// �ж��߷��Ƿ����
BOOL PositionStruct::LegalMove(int mv) const {
	int sqSrc, sqDst, sqPin;
	int pcSelfSide, pcSrc, pcDst, nDelta;
	// �ж��߷��Ƿ�Ϸ�����Ҫ�������µ��жϹ��̣�

	// 1. �ж���ʼ���Ƿ����Լ�������
	sqSrc = SRC(mv);
	pcSrc = ucpcSquares[sqSrc];
	pcSelfSide = SIDE_TAG(sdPlayer);
	if ((pcSrc & pcSelfSide) == 0) {
		return FALSE;
	}

	// 2. �ж�Ŀ����Ƿ����Լ�������
	sqDst = DST(mv);
	pcDst = ucpcSquares[sqDst];
	if ((pcDst & pcSelfSide) != 0) {
		return FALSE;
	}

	// 3. �������ӵ����ͼ���߷��Ƿ����
	switch (pcSrc - pcSelfSide) {
	case PIECE_KING:
		return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
	case PIECE_ADVISOR:
		return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
	case PIECE_BISHOP:
		return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
			ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
	case PIECE_KNIGHT:
		sqPin = KNIGHT_PIN(sqSrc, sqDst);
		return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if (SAME_RANK(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -1 : 1);
		}
		else if (SAME_FILE(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -16 : 16);
		}
		else {
			return FALSE;
		}
		sqPin = sqSrc + nDelta;
		while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
			sqPin += nDelta;
		}
		if (sqPin == sqDst) {
			return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
		}
		else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
			sqPin += nDelta;
			while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
				sqPin += nDelta;
			}
			return sqPin == sqDst;
		}
		else {
			return FALSE;
		}
	case PIECE_PAWN:
		if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
			return TRUE;
		}
		return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
	default:
		return FALSE;
	}
}

// �ж��Ƿ񱻽���
BOOL PositionStruct::Checked() const {
	int i, j, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcDst, nDelta;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	// �ҵ������ϵ�˧(��)�����������жϣ�

	for (sqSrc = 0; sqSrc < 256; sqSrc++) {
		if (ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING) {
			continue;
		}

		// 1. �ж��Ƿ񱻶Է��ı�(��)����
		if (ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
			return TRUE;
		}
		for (nDelta = -1; nDelta <= 1; nDelta += 2) {
			if (ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
				return TRUE;
			}
		}

		// 2. �ж��Ƿ񱻶Է���������(����(ʿ)�Ĳ�����������)
		for (i = 0; i < 4; i++) {
			if (ucpcSquares[sqSrc + ccAdvisorDelta[i]] != 0) {
				continue;
			}
			for (j = 0; j < 2; j++) {
				pcDst = ucpcSquares[sqSrc + ccKnightCheckDelta[i][j]];
				if (pcDst == pcOppSide + PIECE_KNIGHT) {
					return TRUE;
				}
			}
		}

		// 3. �ж��Ƿ񱻶Է��ĳ����ڽ���(������˧����)
		for (i = 0; i < 4; i++) {
			nDelta = ccKingDelta[i];
			sqDst = sqSrc + nDelta;
			while (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
			sqDst += nDelta;
			while (IN_BOARD(sqDst)) {
				int pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_CANNON) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
		}
		return FALSE;
	}
	return FALSE;
}

// �ж��Ƿ�ɱ
BOOL PositionStruct::IsMate(void) {
	int i, nGenMoveNum, pcCaptured;
	int mvs[MAX_GEN_MOVES];

	nGenMoveNum = GenerateMoves(mvs);
	for (i = 0; i < nGenMoveNum; i++) {
		pcCaptured = MovePiece(mvs[i]);
		if (!Checked()) {
			UndoMovePiece(mvs[i], pcCaptured);
			return FALSE;
		}
		else {
			UndoMovePiece(mvs[i], pcCaptured);
		}
	}
	return TRUE;
}

// ����ظ�����
int PositionStruct::RepStatus(int nRecur) const {
	BOOL bSelfSide, bPerpCheck, bOppPerpCheck;
	const MoveStruct *lpmvs;

	bSelfSide = FALSE;
	bPerpCheck = bOppPerpCheck = TRUE;
	lpmvs = mvsList + nMoveNum - 1;
	while (lpmvs->wmv != 0 && lpmvs->ucpcCaptured == 0) {
		if (bSelfSide) {
			bPerpCheck = bPerpCheck && lpmvs->ucbCheck;
			if (lpmvs->dwKey == zobr.dwKey) {
				nRecur--;
				if (nRecur == 0) {
					return 1 + (bPerpCheck ? 2 : 0) + (bOppPerpCheck ? 4 : 0);
				}
			}
		}
		else {
			bOppPerpCheck = bOppPerpCheck && lpmvs->ucbCheck;
		}
		bSelfSide = !bSelfSide;
		lpmvs--;
	}
	return 0;
}

// �Ծ��澵��
void PositionStruct::Mirror(PositionStruct &posMirror) const {
	int sq, pc;
	posMirror.ClearBoard();
	for (sq = 0; sq < 256; sq++) {
		pc = ucpcSquares[sq];
		if (pc != 0) {
			posMirror.AddPiece(MIRROR_SQUARE(sq), pc);
		}
	}
	if (sdPlayer == 1) {
		posMirror.ChangeSide();
	}
	posMirror.SetIrrev();
}

static PositionStruct pos; // ����ʵ��

// ��ͼ�ν����йص�ȫ�ֱ���
static struct {
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped, bGameOver;                     // �Ƿ�ת���̣��Ƿ���Ϸ����(���ü�������ȥ)
} Xqwl;

// �û�����ṹ
struct HashItem {
	BYTE ucDepth, ucFlag;
	short svl;
	WORD wmv, wReserved;
	DWORD dwLock0, dwLock1;
};

// ���ֿ���ṹ
struct BookItem {
	unsigned long dwLock;
	unsigned short wmv, wvl;
};

// �������йص�ȫ�ֱ���
static struct {
	int mvResult;                  // �����ߵ���
	int nHistoryTable[65536];      // ��ʷ��
	int mvKillers[LIMIT_DEPTH][2]; // ɱ���߷���
	HashItem HashTable[HASH_SIZE]; // �û���
	int nBookSize;                 // ���ֿ��С
	BookItem BookTable[BOOK_SIZE]; // ���ֿ�
} Search;

/*
// װ�뿪�ֿ�
static void LoadBook(void) {
	HRSRC hrsrc;
	hrsrc = FindResource(Xqwl.hInst, "BOOK_DATA", RT_RCDATA);
	Search.nBookSize = SizeofResource(Xqwl.hInst, hrsrc) / sizeof(BookItem);
	if (Search.nBookSize > BOOK_SIZE) {
		Search.nBookSize = BOOK_SIZE;
	}
	memcpy(Search.BookTable, LockResource(LoadResource(Xqwl.hInst, hrsrc)),
		Search.nBookSize * sizeof(BookItem));
}
*/

//װ�뿪�ֿ�
static void LoadBook()
{
	FILE* fp;
	fp = fopen("BOOK.dat", "rb");
	if (fp == nullptr)
		cout << "can not open the file" << endl;
	int i = 0;
	while (!feof(fp))
	{
		fread(&Search.BookTable[i], sizeof(struct BookItem), 1, fp);
		i++;
	}
	Search.nBookSize = i - 1;
	fclose(fp);
}

static int CompareBook(const void *lpbk1, const void *lpbk2) {
	DWORD dw1, dw2;
	dw1 = ((BookItem *)lpbk1)->dwLock;
	dw2 = ((BookItem *)lpbk2)->dwLock;
	return dw1 > dw2 ? 1 : dw1 < dw2 ? -1 : 0;
}

// �������ֿ�
static int SearchBook(void) {
	int i, vl, nBookMoves, mv;
	int mvs[MAX_GEN_MOVES], vls[MAX_GEN_MOVES];
	BOOL bMirror;
	BookItem bkToSearch, *lpbk;
	PositionStruct posMirror;
	// �������ֿ�Ĺ��������¼�������

	// 1. ���û�п��ֿ⣬����������
	if (Search.nBookSize == 0) {
		return 0;
	}
	// 2. ������ǰ����
	bMirror = FALSE;
	bkToSearch.dwLock = pos.zobr.dwLock1;
	lpbk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
	// 3. ���û���ҵ�����ô������ǰ����ľ������
	if (lpbk == NULL) {
		bMirror = TRUE;
		pos.Mirror(posMirror);
		bkToSearch.dwLock = posMirror.zobr.dwLock1;
		lpbk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
	}
	// 4. ����������Ҳû�ҵ�������������
	if (lpbk == NULL) {
		return 0;
	}
	// 5. ����ҵ�������ǰ���һ�����ֿ���
	while (lpbk >= Search.BookTable && lpbk->dwLock == bkToSearch.dwLock) {
		lpbk--;
	}
	lpbk++;
	// 6. ���߷��ͷ�ֵд�뵽"mvs"��"vls"������
	vl = nBookMoves = 0;
	while (lpbk < Search.BookTable + Search.nBookSize && lpbk->dwLock == bkToSearch.dwLock) {
		mv = (bMirror ? MIRROR_MOVE(lpbk->wmv) : lpbk->wmv);
		if (pos.LegalMove(mv)) {
			mvs[nBookMoves] = mv;
			vls[nBookMoves] = lpbk->wvl;
			vl += vls[nBookMoves];
			nBookMoves++;
			if (nBookMoves == MAX_GEN_MOVES) {
				break; // ��ֹ"BOOK.DAT"�к����쳣����
			}
		}
		lpbk++;
	}
	if (vl == 0) {
		return 0; // ��ֹ"BOOK.DAT"�к����쳣����
	}
	// 7. ����Ȩ�����ѡ��һ���߷�
	vl = rand() % vl;
	for (i = 0; i < nBookMoves; i++) {
		vl -= vls[i];
		if (vl < 0) {
			break;
		}
	}
	return mvs[i];
}

// ��ȡ�û�����
static int ProbeHash(int vlAlpha, int vlBeta, int nDepth, int &mv) {
	BOOL bMate; // ɱ���־�������ɱ�壬��ô����Ҫ�����������
	HashItem hsh;

	hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.dwLock0 != pos.zobr.dwLock0 || hsh.dwLock1 != pos.zobr.dwLock1) {
		mv = 0;
		return -MATE_VALUE;
	}
	mv = hsh.wmv;
	bMate = FALSE;
	if (hsh.svl > WIN_VALUE) {
		if (hsh.svl < BAN_VALUE) {
			return -MATE_VALUE; // ���ܵ��������Ĳ��ȶ��ԣ������˳���������ŷ������õ�
		}
		hsh.svl -= pos.nDistance;
		bMate = TRUE;
	}
	else if (hsh.svl < -WIN_VALUE) {
		if (hsh.svl > -BAN_VALUE) {
			return -MATE_VALUE; // ͬ��
		}
		hsh.svl += pos.nDistance;
		bMate = TRUE;
	}
	if (hsh.ucDepth >= nDepth || bMate) {
		if (hsh.ucFlag == HASH_BETA) {
			return (hsh.svl >= vlBeta ? hsh.svl : -MATE_VALUE);
		}
		else if (hsh.ucFlag == HASH_ALPHA) {
			return (hsh.svl <= vlAlpha ? hsh.svl : -MATE_VALUE);
		}
		return hsh.svl;
	}
	return -MATE_VALUE;
};

// �����û�����
static void RecordHash(int nFlag, int vl, int nDepth, int mv) {
	HashItem hsh;
	hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.ucDepth > nDepth) {
		return;
	}
	hsh.ucFlag = nFlag;
	hsh.ucDepth = nDepth;
	if (vl > WIN_VALUE) {
		if (mv == 0 && vl <= BAN_VALUE) {
			return; // ���ܵ��������Ĳ��ȶ��ԣ�����û������ŷ��������˳�
		}
		hsh.svl = vl + pos.nDistance;
	}
	else if (vl < -WIN_VALUE) {
		if (mv == 0 && vl >= -BAN_VALUE) {
			return; // ͬ��
		}
		hsh.svl = vl - pos.nDistance;
	}
	else {
		hsh.svl = vl;
	}
	hsh.wmv = mv;
	hsh.dwLock0 = pos.zobr.dwLock0;
	hsh.dwLock1 = pos.zobr.dwLock1;
	Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)] = hsh;
};

// MVV/LVAÿ�������ļ�ֵ
static BYTE cucMvvLva[24] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 3, 4, 3, 2, 0,
	5, 1, 1, 3, 4, 3, 2, 0
};

// ��MVV/LVAֵ
inline int MvvLva(int mv) {
	return (cucMvvLva[pos.ucpcSquares[DST(mv)]] << 3) - cucMvvLva[pos.ucpcSquares[SRC(mv)]];
}

// "qsort"��MVV/LVAֵ����ıȽϺ���
static int CompareMvvLva(const void *lpmv1, const void *lpmv2) {
	return MvvLva(*(int *)lpmv2) - MvvLva(*(int *)lpmv1);
}

// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}


// �߷�����׶�
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

// �߷�����ṹ
struct SortStruct {
	int mvHash, mvKiller1, mvKiller2; // �û����߷�������ɱ���߷�
	int nPhase, nIndex, nGenMoves;    // ��ǰ�׶Σ���ǰ���õڼ����߷����ܹ��м����߷�
	int mvs[MAX_GEN_MOVES];           // ���е��߷�

	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
		mvHash = mvHash_;
		mvKiller1 = Search.mvKillers[pos.nDistance][0];
		mvKiller2 = Search.mvKillers[pos.nDistance][1];
		nPhase = PHASE_HASH;
	}
	int Next(void); // �õ���һ���߷�
};

// �õ���һ���߷�
int SortStruct::Next(void) {
	int mv;
	switch (nPhase) {
		// "nPhase"��ʾ�ŷ����������ɽ׶Σ�����Ϊ��

		// 0. �û����ŷ���������ɺ�����������һ�׶Σ�
	case PHASE_HASH:
		nPhase = PHASE_KILLER_1;
		if (mvHash != 0) {
			return mvHash;
		}
		// ���ɣ�����û��"break"����ʾ"switch"����һ��"case"ִ��������������һ��"case"����ͬ

		// 1. ɱ���ŷ�����(��һ��ɱ���ŷ�)����ɺ�����������һ�׶Σ�
	case PHASE_KILLER_1:
		nPhase = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
			return mvKiller1;
		}

		// 2. ɱ���ŷ�����(�ڶ���ɱ���ŷ�)����ɺ�����������һ�׶Σ�
	case PHASE_KILLER_2:
		nPhase = PHASE_GEN_MOVES;
		if (mvKiller2 != mvHash && mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
			return mvKiller2;
		}

		// 3. ���������ŷ�����ɺ�����������һ�׶Σ�
	case PHASE_GEN_MOVES:
		nPhase = PHASE_REST;
		nGenMoves = pos.GenerateMoves(mvs);
		qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
		nIndex = 0;

		// 4. ��ʣ���ŷ�����ʷ��������
	case PHASE_REST:
		while (nIndex < nGenMoves) {
			mv = mvs[nIndex];
			nIndex++;
			if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2) {
				return mv;
			}
		}

		// 5. û���ŷ��ˣ������㡣
	default:
		return 0;
	}
}

// ������߷��Ĵ���
inline void SetBestMove(int mv, int nDepth) {
	int *lpmvKillers;
	Search.nHistoryTable[mv] += nDepth * nDepth;
	lpmvKillers = Search.mvKillers[pos.nDistance];
	if (lpmvKillers[0] != mv) {
		lpmvKillers[1] = lpmvKillers[0];
		lpmvKillers[0] = mv;
	}
}

// ��̬(Quiescence)��������
static int SearchQuiesc(int vlAlpha, int vlBeta) {
	int i, nGenMoves;
	int vl, vlBest;
	int mvs[MAX_GEN_MOVES];
	// һ����̬������Ϊ���¼����׶�

	// 1. ����ظ�����
	vl = pos.RepStatus();
	if (vl != 0) {
		return pos.RepValue(vl);
	}

	// 2. ���Ｋ����Ⱦͷ��ؾ�������
	if (pos.nDistance == LIMIT_DEPTH) {
		return pos.Evaluate();
	}

	// 3. ��ʼ�����ֵ
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)

	if (pos.InCheck()) {
		// 4. �����������������ȫ���߷�
		nGenMoves = pos.GenerateMoves(mvs);
		qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
	}
	else {

		// 5. �������������������������
		vl = pos.Evaluate();
		if (vl > vlBest) {
			vlBest = vl;
			if (vl >= vlBeta) {
				return vl;
			}
			if (vl > vlAlpha) {
				vlAlpha = vl;
			}
		}

		// 6. �����������û�нضϣ������ɳ����߷�
		nGenMoves = pos.GenerateMoves(mvs, GEN_CAPTURE);
		qsort(mvs, nGenMoves, sizeof(int), CompareMvvLva);
	}

	// 7. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i])) {
			vl = -SearchQuiesc(-vlBeta, -vlAlpha);
			pos.UndoMakeMove();

			// 8. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					return vl;        // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 9. �����߷����������ˣ��������ֵ
	return vlBest == -MATE_VALUE ? pos.nDistance - MATE_VALUE : vlBest;
}

// "SearchFull"�Ĳ���
const BOOL NO_NULL = TRUE;

// �����߽�(Fail-Soft)��Alpha-Beta��������
static int SearchFull(int vlAlpha, int vlBeta, int nDepth, BOOL bNoNull = FALSE) {
	int nHashFlag, vl, vlBest;
	int mv, mvBest, mvHash, nNewDepth;
	SortStruct Sort;
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	// 1. ����ˮƽ�ߣ�����þ�̬����(ע�⣺���ڿղ��ü�����ȿ���С����)
	if (nDepth <= 0) {
		return SearchQuiesc(vlAlpha, vlBeta);
	}

	// 1-1. ����ظ�����(ע�⣺��Ҫ�ڸ��ڵ��飬�����û���߷���)
	vl = pos.RepStatus();
	if (vl != 0) {
		return pos.RepValue(vl);
	}

	// 1-2. ���Ｋ����Ⱦͷ��ؾ�������
	if (pos.nDistance == LIMIT_DEPTH) {
		return pos.Evaluate();
	}

	// 1-3. �����û����ü������õ��û����߷�
	vl = ProbeHash(vlAlpha, vlBeta, nDepth, mvHash);
	if (vl > -MATE_VALUE) {
		return vl;
	}

	// 1-4. ���Կղ��ü�(���ڵ��Betaֵ��"MATE_VALUE"�����Բ����ܷ����ղ��ü�)
	if (!bNoNull && !pos.InCheck() && pos.NullOkay()) {
		pos.NullMove();
		vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
		pos.UndoNullMove();
		if (vl >= vlBeta) {
			return vl;
		}
	}

	// 2. ��ʼ�����ֵ������߷�
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

	// 3. ��ʼ���߷�����ṹ
	Sort.Init(mvHash);

	// 4. ��һ����Щ�߷��������еݹ�
	while ((mv = Sort.Next()) != 0) {
		if (pos.MakeMove(mv)) {
			// ��������
			nNewDepth = pos.InCheck() ? nDepth : nDepth - 1;
			// PVS
			if (vlBest == -MATE_VALUE) {
				vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
			}
			else {
				vl = -SearchFull(-vlAlpha - 1, -vlAlpha, nNewDepth);
				if (vl > vlAlpha && vl < vlBeta) {
					vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
				}
			}
			pos.UndoMakeMove();

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					nHashFlag = HASH_BETA;
					mvBest = mv;      // Beta�߷�Ҫ���浽��ʷ��
					break;            // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					nHashFlag = HASH_PV;
					mvBest = mv;      // PV�߷�Ҫ���浽��ʷ��
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ�����������ֵ
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.nDistance - MATE_VALUE;
	}
	// ��¼���û���
	RecordHash(nHashFlag, vlBest, nDepth, mvBest);
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		SetBestMove(mvBest, nDepth);
	}
	return vlBest;
}

// ���ڵ��Alpha-Beta��������
static int SearchRoot(int nDepth) {
	int vl, vlBest, mv, nNewDepth;
	SortStruct Sort;

	vlBest = -MATE_VALUE;
	Sort.Init(Search.mvResult);
	while ((mv = Sort.Next()) != 0) {
		if (pos.MakeMove(mv)) {
			nNewDepth = pos.InCheck() ? nDepth : nDepth - 1;
			if (vlBest == -MATE_VALUE) {
				vl = -SearchFull(-MATE_VALUE, MATE_VALUE, nNewDepth, NO_NULL);
			}
			else {
				vl = -SearchFull(-vlBest - 1, -vlBest, nNewDepth);
				if (vl > vlBest) {
					vl = -SearchFull(-MATE_VALUE, -vlBest, nNewDepth, NO_NULL);
				}
			}
			pos.UndoMakeMove();
			if (vl > vlBest) {
				vlBest = vl;
				Search.mvResult = mv;
				if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
					vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
				}
			}
		}
	}
	RecordHash(HASH_PV, vlBest, nDepth, Search.mvResult);
	SetBestMove(Search.mvResult, nDepth);
	return vlBest;
}

// ����������������
static void SearchMain(void) {
	int i, t, vl, nGenMoves;
	int mvs[MAX_GEN_MOVES];

	// ��ʼ��
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int));       // �����ʷ��
	memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // ���ɱ���߷���
	memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));  // ����û���
	t = clock();       // ��ʼ����ʱ��
	pos.nDistance = 0; // ��ʼ����

	// �������ֿ�
	Search.mvResult = SearchBook();
	if (Search.mvResult != 0) {
		pos.MakeMove(Search.mvResult);
		if (pos.RepStatus(3) == 0) {
			pos.UndoMakeMove();
			return;
		}
		pos.UndoMakeMove();
	}

	// ����Ƿ�ֻ��Ψһ�߷�
	vl = 0;
	nGenMoves = pos.GenerateMoves(mvs);
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i])) {
			pos.UndoMakeMove();
			Search.mvResult = mvs[i];
			vl++;
		}
	}
	if (vl == 1) {
		return;
	}

	// �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchRoot(i);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// ����һ�룬����ֹ����
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}

// ��ʼ�����
static void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
	Xqwl.bGameOver = FALSE;
}

//����AI�ظ�һ���壬�ú����ṩ���������һ������AI��һ����Ľӿ�
int botResponse()
{
	SearchMain();
	pos.MakeMove(Search.mvResult);
	return Search.mvResult;
}

//���壬�˻ص���һ��
void retract()
{

}


void initAI()
{
	InitZobrist();
	Xqwl.bFlipped = false;
	LoadBook();
	Startup();
}