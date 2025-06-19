#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#define LOG(fmt, ...) printf("[%s : %d]" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

//void func(const char *fmt, ...)
//{
//    va_list args;
//    va_start(args, fmt);
//    while(*fmt)
//    {
//        if(*fmt == 'd')
//        {
//            int i = va_arg(args, int);
//            printf("%d ", i);
//        }
// //       fmt++;
//    }

   // for(int i = 0; i < n + n; i++)
   // {
   //     printf("%d\n", va_arg(args, int));
   // }
//    va_end(args);
//}
//

template <typename ...args>
void print(args... arg)
{
    int arr[] = {(std::cout << arg << std::endl, 0)...};

}

int main()
{
    // func("ddd", 1, 2, 3);
    // LOG("loading...sum = %d, a = %f, b = %lf", 10, 1.5, 1.000345);
    print("abs", 1, 3, "t");
}









