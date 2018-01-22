#include<cstdio> 
using namespace std;
long ack(int m, int n)
{
    if (m == 0)
        return (n + 1);
    if (m > 0)
    {
        if (n == 0)
            return ack(m - 1, 1);
        if (n > 0)
            return ack(m - 1, ack(m, n - 1));
    }
}
int main()
{
    int res = ack(3, 3);
    int a = res;
    return 0;
}
