#include <iostream>
using namespace std;

void gasFinding(int x, int y, int i, int j, int groupIndex, int board[9][9], char group[9][9], char groupGasState[81], char groupGasPosition[81][2], char positionState[9][9], char unicGasGroup[9][9])
{
    if (board[i][j] == 0)
    {
        switch (groupGasState[groupIndex])
        {
        case 0:
            groupGasPosition[groupIndex][0] = i;
            groupGasPosition[groupIndex][1] = j;
            groupGasState[groupIndex] = 1;
            if (board[x][y] == 1)
            {
                positionState[i][j] |= 2;
            }
            else
            {
                positionState[i][j] |= 1;
                unicGasGroup[i][j] = groupIndex;
            }
            break;
        case 1: //如果之前有唯一气的话现在就有两个气了
            groupGasState[groupIndex] = 2;
            if (board[x][y] == 1)
            {
                //己方
                positionState[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] |= 4;
            }
            else if (unicGasGroup[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] == groupIndex) //如果是自己留下来的唯一气的话，现在就不是唯一气了，否则还是
            {
                //对方
                positionState[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] &= ~1;
            }
            break;
        case 2:
            if (board[x][y] == 1)
            {
                positionState[i][j] |= 4;
            }
            //如果是对方的话可以不管，对方的非唯一气没有影响
            break;
        }
    }
}

void grouping(int x, int y, int groupIndex, int board[9][9], char group[9][9], char groupGasState[81], char groupGasPosition[81][2], char positionState[9][9], char unicGasGroup[9][9])
{
    group[x][y] = groupIndex;
    //找气
    if (x != 0)
    {
        gasFinding(x, y, x - 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 0)
    {
        gasFinding(x, y, x, y - 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (x != 8)
    {
        gasFinding(x, y, x + 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 8)
    {
        gasFinding(x, y, x, y + 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }

    //递归分组
    if (x != 0 && group[x - 1][y] == -1 && board[x][y] == board[x - 1][y])
    {
        grouping(x - 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 0 && group[x][y - 1] == -1 && board[x][y] == board[x][y - 1])
    {
        grouping(x, y - 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (x != 8 && group[x + 1][y] == -1 && board[x][y] == board[x + 1][y])
    {
        grouping(x + 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 8 && group[x][y + 1] == -1 && board[x][y] == board[x][y + 1])
    {
        grouping(x, y + 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
}

//result里面1是可能的落子位置，0是不可能的位置，返回值为可落子位置总数
int getValidPositions(int board[9][9], int result[9][9])
{
    int r = 0;
    int groupCount = 0;
    char group[9][9];        //保存各位置棋子的分组，-1表示未分组，数字表示分组的索引
    char unicGasGroup[9][9]; //如果是对方唯一气的话，存下是哪一组的唯一气
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            unicGasGroup[i][j] = -1;
            group[i][j] = -1;
        }
    }
    char groupGasState[81] = {0};       //保存各分组的气的状态，分为0无气、1有单一气、2有多气
    char groupGasPosition[81][2] = {0}; //唯一气的位置
    char positionState[9][9] = {0};     //由低位开始，+1表示为对方唯一气，+2表示为己方唯一气，+4表示为己方非唯一气
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (group[i][j] != -1 || board[i][j] == 0) //跳过已分组的元素以及空格
            {
                continue;
            }
            grouping(i, j, groupCount, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
            groupCount++;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (board[i][j] != 0) //不是空格，不能下
            {
                result[i][j] = 0;
                continue;
            }
            if (positionState[i][j] % 2 == 1) //对方唯一气，不能下
            {
                result[i][j] = 0;
                continue;
            }
            if (positionState[i][j] >> 2 == 1) //己方非唯一气，能下
            {
                result[i][j] = 1;
                r++;
                continue;
            }
            if (positionState[i][j] >> 1 % 2 == 1) //己方唯一气，边上还没气的话就不能下
            {
                if((i!=0&&board[i-1][j]==0)||(j!=0&&board[i][j-1]==0)||(i!=8&&board[i+1][j]==0)||(j!=8&&board[i][j+1]==0)){
                    result[i][j] = 1;
                }else{
                    result[i][j] = 0;
                }
                continue;
            }
            //啥都不是，能下
            result[i][j] = 1;
            r++;
        }
    }
    return r;
}

int main()
{
    int board[9][9] = {
        {1, 1, 1, 1, 0, 0, 0, 0, 0},
        {-1, -1, -1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    int result[9][9] = {0};
    getValidPositions(board, result);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }
}