#include<cstdio> 
using namespace std;
long long a[1][6] = {1, 2, 3, 4, 5, 6}, b[6][1] = {1, 2, 3, 4, 5, 6};
int main()
{
    long long result = 0;
    for (int i = 0; i < 1; i++)
        for (int j = 0; j < 1; j++)
            for (int k = 0; k < 6; k++)
            {
                result += a[i][k] * b[k][j];
            }
    return 0;
}
