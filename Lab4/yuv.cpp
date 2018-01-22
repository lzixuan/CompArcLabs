#include <iostream>
#include<fstream>
#include<immintrin.h>
#include<emmintrin.h>
#include<xmmintrin.h>
#include<cstring>
#include<cstdio>
#include<cstdlib>
#include<time.h>
#include<string>
using namespace std;

#define MM(x) (* ( (__m64*)x) )
//相应结构体
struct yuv_pic
{
	short y[1080][1920];
	short u[1080][1920];
	short v[1080][1920];
};
struct yuv32
{
	int y[1080][1920];
	int u[1080][1920];
	int v[1080][1920];
};
struct rgb_pic
{
	short r[1080][1920];
	short g[1080][1920];
	short b[1080][1920];
};
struct rgb32
{
	int r[1080][1920];
	int g[1080][1920];
	int b[1080][1920];
};
void store_pic(string filename, yuv_pic &temp)
{
	FILE *fout = fopen(filename.c_str(), "a+");
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j++)
		{
			unsigned char out = temp.y[i][j];
			fwrite(&out, 1, 1, fout);
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			unsigned char out = temp.u[i][j];
			fwrite(&out, 1, 1, fout);
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			unsigned char out = temp.v[i][j];
			fwrite(&out, 1, 1, fout);
		}
	fclose(fout);
}
void store_pic(string filename, yuv32 &temp)
{
	FILE *fout = fopen(filename.c_str(), "a+");
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j++)
		{
			unsigned char out = temp.y[i][j];
			fwrite(&out, 1, 1, fout);
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			unsigned char out = temp.u[i][j];
			fwrite(&out, 1, 1, fout);
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			unsigned char out = temp.v[i][j];
			fwrite(&out, 1, 1, fout);
		}
	fclose(fout);
}
double process_without_simd(int type, yuv_pic &yuv1, rgb_pic &rgb1, yuv_pic &yuv2, rgb_pic &rgb2);
double process_with_mmx(int type, yuv_pic &yuv1, rgb_pic &rgb1, yuv_pic &yuv2, rgb_pic &rgb2);
double process_with_sse(int type, yuv32 &yuv1, rgb32 &rgb1, yuv32 &yuv2, rgb32 &rgb2);
double process_with_avx(int type, yuv32 &yuv1, rgb32 &rgb1, yuv32 &yuv2, rgb32 &rgb2);
yuv_pic yuv1, yuv2;
rgb_pic rgb1, rgb2;
yuv32 yuv3, yuv4;
rgb32 rgb3, rgb4;
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "too few arguments!" << endl;
		exit(0);
	}
	if (argc > 3)
	{
		cout << "too many arguments!" << endl;
		exit(0);
	}
	if (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0)
	{
		cout << "usage: ./yuv <type(1 or 2)>" << endl;
		exit(0);
	}
	FILE *fin, *fin2;
	fin = fopen("dem1.yuv", "rb");
	fin2 = fopen("dem2.yuv", "rb");
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j++)
		{
			fread(&yuv1.y[i][j], 1, 1, fin);
			yuv3.y[i][j] = (int)yuv1.y[i][j];
			fread(&yuv2.y[i][j], 1, 1, fin2);
			yuv4.y[i][j] = (int)yuv2.y[i][j];
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			fread(&yuv1.u[i][j], 1, 1, fin);
			yuv1.u[i][j + 1] = yuv1.u[i][j];
			yuv1.u[i + 1][j] = yuv1.u[i][j];
			yuv1.u[i + 1][j + 1] = yuv1.u[i][j];

			yuv3.u[i][j] = (int)yuv1.u[i][j];
			yuv3.u[i][j + 1] = (int)yuv1.u[i][j];
			yuv3.u[i + 1][j] = (int)yuv1.u[i][j];
			yuv3.u[i + 1][j + 1] = (int)yuv1.u[i][j];

			fread(&yuv2.u[i][j], 1, 1, fin2);
			yuv2.u[i][j + 1] = yuv2.u[i][j];
			yuv2.u[i + 1][j] = yuv2.u[i][j];
			yuv2.u[i + 1][j + 1] = yuv2.u[i][j];

			yuv4.u[i][j] = (int)yuv2.u[i][j];
			yuv4.u[i][j + 1] = (int)yuv2.u[i][j];
			yuv4.u[i + 1][j] = (int)yuv2.u[i][j];
			yuv4.u[i + 1][j + 1] = (int)yuv2.u[i][j];
		}
	for (int i = 0; i < 1080; i += 2)
		for (int j = 0; j < 1920; j += 2)
		{
			fread(&yuv1.v[i][j], 1, 1, fin);
			yuv1.v[i][j + 1] = yuv1.v[i][j];
			yuv1.v[i + 1][j] = yuv1.v[i][j];
			yuv1.v[i + 1][j + 1] = yuv1.v[i][j];

			yuv3.v[i][j] = (int)yuv1.v[i][j];
			yuv3.v[i][j + 1] = (int)yuv1.v[i][j];
			yuv3.v[i + 1][j] = (int)yuv1.v[i][j];
			yuv3.v[i + 1][j + 1] = (int)yuv1.v[i][j];

			fread(&yuv2.v[i][j], 1, 1, fin2);
			yuv2.v[i][j + 1] = yuv2.v[i][j];
			yuv2.v[i + 1][j] = yuv2.v[i][j];
			yuv2.v[i + 1][j + 1] = yuv2.v[i][j];

			yuv4.v[i][j] = (int)yuv2.v[i][j];
			yuv4.v[i][j + 1] = (int)yuv2.v[i][j];
			yuv4.v[i + 1][j] = (int)yuv2.v[i][j];
			yuv4.v[i + 1][j + 1] = (int)yuv2.v[i][j];
		}
	fclose(fin);
	fclose(fin2);
	int type = atoi(argv[1]);
	double time1, time2, time3, time4;
	if (type == 1)
	{
		time1 = process_without_simd(1, yuv1, rgb1, yuv2, rgb2);
		cout << "fade time without simd: " << time1 << "s" << endl;
		time2 = process_with_mmx(1, yuv1, rgb1, yuv2, rgb2);
		cout << "fade time with MMX: " << time2 << "s" << endl;
		time3 = process_with_sse(1, yuv3, rgb3, yuv4, rgb4);
		cout << "fade time with SSE: " << time3 << "s" << endl;
		time4 = process_with_avx(1, yuv3, rgb3, yuv4, rgb4);
		cout << "fade time with AVX: " << time4 << "s" << endl;
	}
	else
	{
		time1 = process_without_simd(2, yuv1, rgb1, yuv2, rgb2);
		cout << "overlay time without simd: " << time1 << "s" << endl;
		time2 = process_with_mmx(2, yuv1, rgb1, yuv2, rgb2);
		cout << "overlay time with MMX: " << time2 << "s" << endl;
		time3 = process_with_sse(2, yuv3, rgb3, yuv4, rgb4);
		cout << "overlay time with SSE: " << time3 << "s" << endl;
		time4 = process_with_avx(2, yuv3, rgb3, yuv4, rgb4);
		cout << "overlay time with AVX: " << time4 << "s" << endl;
	}
	system("pause");
	return 0;
}



//下面的4个函数应该统计出图像处理的时间;
//函数参数和返回值可以需要自己定.
double process_without_simd(int type, yuv_pic &yuv1, rgb_pic &rgb1, yuv_pic &yuv2, rgb_pic &rgb2)
{
	double time = 0;
	int time_s, time_e;
	string filename;
	yuv_pic temp;
	time_s = clock();
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j++)
		{
			rgb1.r[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 1.596027 * (yuv1.v[i][j] - 128);
			rgb1.b[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 2.017232 * (yuv1.u[i][j] - 128);
			rgb1.g[i][j] = 1.164383 * (yuv1.y[i][j] - 16) - 0.391762 * (yuv1.u[i][j] - 128) - 0.812968 * (yuv1.v[i][j] - 128);

			if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
			if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
			if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
			if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
			if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
			if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;
		}
	if (type == 1)
	{
		for (int a = 1; a < 255; a += 3)
		{
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j++)
				{
					int r, g, b;
					r = rgb1.r[i][j] * a / 256;
					g = rgb1.g[i][j] * a / 256;
					b = rgb1.b[i][j] * a / 256;

					temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;
				}
			time_e = clock();
			time += (time_e - time_s) / 1000.0;
#ifdef DEBUG
			filename = "naive_fade.yuv";
			store_pic(filename, temp);
#endif

			time_s = clock();
		}
		
	}
	else
	{
		for (int i = 0; i < 1080; i++)
			for (int j = 0; j < 1920; j++)
			{
				rgb2.r[i][j] = 1.164383 * (yuv2.y[i][j] - 16) + 1.596027 * (yuv2.v[i][j] - 128);
				rgb2.b[i][j] = 1.164383 * (yuv2.y[i][j] - 16) + 2.017232 * (yuv2.u[i][j] - 128);
				rgb2.g[i][j] = 1.164383 * (yuv2.y[i][j] - 16) - 0.391762 * (yuv2.u[i][j] - 128) - 0.812968 * (yuv2.v[i][j] - 128);

				if (rgb2.r[i][j] < 0)	rgb2.r[i][j] = 0;
				if (rgb2.r[i][j] > 255)	rgb2.r[i][j] = 255;
				if (rgb2.b[i][j] < 0)	rgb2.b[i][j] = 0;
				if (rgb2.b[i][j] > 255)	rgb2.b[i][j] = 255;
				if (rgb2.g[i][j] < 0)	rgb2.g[i][j] = 0;
				if (rgb2.g[i][j] > 255)	rgb2.g[i][j] = 255;
			}
		for (int a = 1; a < 255; a += 3)
		{
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j++)
				{
					int r, g, b;
					r = (rgb1.r[i][j] * a + rgb2.r[i][j] * (256 - a)) / 256;
					g = (rgb1.g[i][j] * a + rgb2.g[i][j] * (256 - a)) / 256;
					b = (rgb1.b[i][j] * a + rgb2.b[i][j] * (256 - a)) / 256;

					temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;
				}
			time_e = clock();
			time += (time_e - time_s) / 1000.0;

#ifdef DEBUG
			filename = "naive_overlay.yuv";
			store_pic(filename, temp);
#endif

			time_s = clock();
		}
		
	}
	return time;
}
//乘法+右移的近似计算，注释里有浮点运算+并行加减的代码
double process_with_mmx(int type, yuv_pic &yuv1, rgb_pic &rgb1, yuv_pic &yuv2, rgb_pic &rgb2)
{
	double time = 0;
	int time_s, time_e;
	string filename;
	yuv_pic temp;
	time_s = clock();
	//转成RGB时最后加的常数
	__m64 const1 = _mm_set1_pi16(-223);
	__m64 const2 = _mm_set1_pi16(-277);
	__m64 const3 = _mm_set1_pi16(136);
	//各属性前面的系数（乘上2的幂后接近整数）
	__m64 y = _mm_set1_pi16(75);//128
	__m64 v1 = _mm_set1_pi16(102);//64
	__m64 u2 = _mm_set1_pi16(65);//32
	__m64 u3 = _mm_set1_pi16(100);//256
	__m64 v3 = _mm_set1_pi16(104);//128
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j+=4)
		{
			/*rgb1.r[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 1.596027 * (yuv1.v[i][j] - 128);
			rgb1.b[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 2.017232 * (yuv1.u[i][j] - 128);
			rgb1.g[i][j] = 1.164383 * (yuv1.y[i][j] - 16) - 0.391762 * (yuv1.u[i][j] - 128) - 0.812968 * (yuv1.v[i][j] - 128);*/
			//只用MMX并行最后的加法，浮点乘除加减保留
			/*rgb1.r[i][j] = 1.164383 * yuv1.y[i][j] + 1.596027 * yuv1.v[i][j];
			rgb1.r[i][j + 1] = 1.164383 * yuv1.y[i][j + 1] + 1.596027 * yuv1.v[i][j + 1];
			rgb1.r[i][j + 2] = 1.164383 * yuv1.y[i][j + 2] + 1.596027 * yuv1.v[i][j + 2];
			rgb1.r[i][j + 3] = 1.164383 * yuv1.y[i][j + 3] + 1.596027 * yuv1.v[i][j + 3];

			rgb1.b[i][j] = 1.164383 * yuv1.y[i][j] + 2.017232 * yuv1.u[i][j];
			rgb1.b[i][j + 1] = 1.164383 * yuv1.y[i][j + 1] + 2.017232 * yuv1.u[i][j + 1];
			rgb1.b[i][j + 2] = 1.164383 * yuv1.y[i][j + 2] + 2.017232 * yuv1.u[i][j + 2];
			rgb1.b[i][j + 3] = 1.164383 * yuv1.y[i][j + 3] + 2.017232 * yuv1.u[i][j + 3];

			rgb1.g[i][j] = 1.164383 * yuv1.y[i][j] - 0.391762 * yuv1.u[i][j] - 0.812968 * yuv1.v[i][j];
			rgb1.g[i][j + 1] = 1.164383 * yuv1.y[i][j + 1] - 0.391762 * yuv1.u[i][j + 1] - 0.812968 * yuv1.v[i][j + 1];
			rgb1.g[i][j + 2] = 1.164383 * yuv1.y[i][j + 2] - 0.391762 * yuv1.u[i][j + 2] - 0.812968 * yuv1.v[i][j + 2];
			rgb1.g[i][j + 3] = 1.164383 * yuv1.y[i][j + 3] - 0.391762 * yuv1.u[i][j + 3] - 0.812968 * yuv1.v[i][j + 3];

			MM((rgb1.r[i] + j)) = _mm_add_pi16(MM((rgb1.r[i] + j)), const1);
			MM((rgb1.b[i] + j)) = _mm_add_pi16(MM((rgb1.b[i] + j)), const2);
			MM((rgb1.g[i] + j)) = _mm_add_pi16(MM((rgb1.g[i] + j)), const3);*/
			
			//使用乘法+右移近似
			__m64 r1 = _mm_mullo_pi16(y, MM((yuv1.y[i] + j))),
			r2 = _mm_mullo_pi16(v1, MM((yuv1.v[i] + j))),
			b2 = _mm_mullo_pi16(u2, MM((yuv1.u[i] + j))),
			g2 = _mm_mullo_pi16(u3, MM((yuv1.u[i] + j))),
			g3 = _mm_mullo_pi16(v3, MM((yuv1.v[i] + j)));
			r1 = _mm_srli_pi16(r1, 6);
			r2 = _mm_srli_pi16(r2, 6);
			b2 = _mm_srli_pi16(b2, 5);
			g2 = _mm_srli_pi16(g2, 8);
			g3 = _mm_srli_pi16(g3, 7);

			MM((rgb1.r[i] + j)) = _mm_add_pi16(_mm_add_pi16(r1, r2), const1);
			MM((rgb1.b[i] + j)) = _mm_add_pi16(_mm_add_pi16(r1, b2), const2);
			MM((rgb1.g[i] + j)) = _mm_sub_pi16(_mm_add_pi16(_mm_sub_pi16(r1, g2), const3), g3);

		}
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j++)
		{
			if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
			if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
			if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
			if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
			if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
			if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;
		}
	if (type == 1)
	{
		__m64 tmp1 = _mm_set1_pi16(16);
		__m64 tmp2 = _mm_set1_pi16(128);
		__m64 r1 = _mm_set1_pi16(66);//256
		__m64 g1 = _mm_set1_pi16(65);//2
		__m64 b1 = _mm_set1_pi16(100);//1024
		__m64 r2 = _mm_set1_pi16(76);//512
		__m64 g2 = _mm_set1_pi16(75);//256
		__m64 b2 = _mm_set1_pi16(112);//256
		__m64 g3 = _mm_set1_pi16(94);//256
		__m64 b3 = _mm_set1_pi16(74);//1024
		for (int a = 1; a < 255; a += 3)
		{
			__m64 tempa = _mm_set1_pi16(a);
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j += 4)
				{
					/*temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;*/
					//只用MMX并行最后的加法，浮点乘除加减保留
					/*int r, g, b;
					r = rgb1.r[i][j] * a / 256;
					g = rgb1.g[i][j] * a / 256;
					b = rgb1.b[i][j] * a / 256;
					temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b;

					r = rgb1.r[i][j + 1] * a / 256;
					g = rgb1.g[i][j + 1] * a / 256;
					b = rgb1.b[i][j + 1] * a / 256;
					temp.y[i][j + 1] = 0.256788 * r + 0.504129 * g + 0.097906 * b;
					temp.u[i][j + 1] = -0.148223 * r - 0.290993 * g + 0.439216 * b;
					temp.v[i][j + 1] = 0.439216 * r - 0.367788 * g - 0.071427 * b;

					r = rgb1.r[i][j + 2] * a / 256;
					g = rgb1.g[i][j + 2] * a / 256;
					b = rgb1.b[i][j + 2] * a / 256;
					temp.y[i][j + 2] = 0.256788 * r + 0.504129 * g + 0.097906 * b;
					temp.u[i][j + 2] = -0.148223 * r - 0.290993 * g + 0.439216 * b;
					temp.v[i][j + 2] = 0.439216 * r - 0.367788 * g - 0.071427 * b;

					r = rgb1.r[i][j + 3] * a / 256;
					g = rgb1.g[i][j + 3] * a / 256;
					b = rgb1.b[i][j + 3] * a / 256;
					temp.y[i][j + 3] = 0.256788 * r + 0.504129 * g + 0.097906 * b;
					temp.u[i][j + 3] = -0.148223 * r - 0.290993 * g + 0.439216 * b;
					temp.v[i][j + 3] = 0.439216 * r - 0.367788 * g - 0.071427 * b;

					MM((temp.y[i] + j)) = _mm_add_pi16(MM((temp.y[i] + j)), tmp1);
					MM((temp.u[i] + j)) = _mm_add_pi16(MM((temp.u[i] + j)), tmp2);
					MM((temp.v[i] + j)) = _mm_add_pi16(MM((temp.v[i] + j)), tmp2);*/

					//逻辑右移非常关键，算术右移可能会在a较大时产生负数，后面的计算（有符号乘法）就崩了
					__m64 r = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.r[i] + j)), tempa), 8);
					__m64 g = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.g[i] + j)), tempa), 8);
					__m64 b = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.b[i] + j)), tempa), 8);

					__m64 y1 = _mm_srli_pi16(_mm_mullo_pi16(r, r1), 8),
					y2 = _mm_srli_pi16(_mm_mullo_pi16(g, g1), 7),
					y3 = _mm_srli_pi16(_mm_mullo_pi16(b, b1), 10),
					u1 = _mm_srli_pi16(_mm_mullo_pi16(r, r2), 9),
					u2 = _mm_srli_pi16(_mm_mullo_pi16(g, g2), 8),
					u3 = _mm_srli_pi16(_mm_mullo_pi16(b, b2), 8),
					v1 = _mm_srli_pi16(_mm_mullo_pi16(r, b2), 8),
					v2 = _mm_srli_pi16(_mm_mullo_pi16(g, g3), 8),
					v3 = _mm_srli_pi16(_mm_mullo_pi16(b, b3), 10);
					MM((temp.y[i] + j)) = _mm_add_pi16(_mm_add_pi16(_mm_add_pi16(y1, y2), y3), tmp1);
					MM((temp.u[i] + j)) = _mm_add_pi16(_mm_sub_pi16(_mm_sub_pi16(u3, u2), u1), tmp2);
					MM((temp.v[i] + j)) = _mm_add_pi16(_mm_sub_pi16(_mm_sub_pi16(v1, v2), v3), tmp2);

				}
			time_e = clock();
			time += (time_e - time_s) / 1000.0;

#ifdef DEBUG
			string filename = "mmx_fade.yuv";
			store_pic(filename, temp);
#endif 

			time_s = clock();
		}
		

		
	}
	else
	{
		for (int i = 0; i < 1080; i++)
			for (int j = 0; j < 1920; j += 4)
			{

				__m64 r1 = _mm_mullo_pi16(y, MM((yuv2.y[i] + j))),
					r2 = _mm_mullo_pi16(v1, MM((yuv2.v[i] + j))),
					b2 = _mm_mullo_pi16(u2, MM((yuv2.u[i] + j))),
					g2 = _mm_mullo_pi16(u3, MM((yuv2.u[i] + j))),
					g3 = _mm_mullo_pi16(v3, MM((yuv2.v[i] + j)));
				r1 = _mm_srli_pi16(r1, 6);
				r2 = _mm_srli_pi16(r2, 6);
				b2 = _mm_srli_pi16(b2, 5);
				g2 = _mm_srli_pi16(g2, 8);
				g3 = _mm_srli_pi16(g3, 7);

				MM((rgb2.r[i] + j)) = _mm_add_pi16(_mm_add_pi16(r1, r2), const1);
				MM((rgb2.b[i] + j)) = _mm_add_pi16(_mm_add_pi16(r1, b2), const2);
				MM((rgb2.g[i] + j)) = _mm_sub_pi16(_mm_add_pi16(_mm_sub_pi16(r1, g2), const3), g3);

			}
		for (int i = 0; i < 1080; i++)
			for (int j = 0; j < 1920; j++)
			{
				if (rgb2.r[i][j] < 0)	rgb2.r[i][j] = 0;
				if (rgb2.r[i][j] > 255)	rgb2.r[i][j] = 255;
				if (rgb2.b[i][j] < 0)	rgb2.b[i][j] = 0;
				if (rgb2.b[i][j] > 255)	rgb2.b[i][j] = 255;
				if (rgb2.g[i][j] < 0)	rgb2.g[i][j] = 0;
				if (rgb2.g[i][j] > 255)	rgb2.g[i][j] = 255;
			}

		__m64 tmp1 = _mm_set1_pi16(16);
		__m64 tmp2 = _mm_set1_pi16(128);
		__m64 r1 = _mm_set1_pi16(66);//256
		__m64 g1 = _mm_set1_pi16(65);//2
		__m64 b1 = _mm_set1_pi16(100);//1024
		__m64 r2 = _mm_set1_pi16(76);//512
		__m64 g2 = _mm_set1_pi16(75);//256
		__m64 b2 = _mm_set1_pi16(112);//256
		__m64 g3 = _mm_set1_pi16(94);//256
		__m64 b3 = _mm_set1_pi16(74);//1024
		for (int a = 1; a < 255; a += 3)
		{
			__m64 tempa = _mm_set1_pi16(a);
			__m64 tempa2 = _mm_set1_pi16(256 - a);
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j += 4)
				{

					__m64 ra = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.r[i] + j)), tempa), 8);
					__m64 ga = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.g[i] + j)), tempa), 8);
					__m64 ba = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb1.b[i] + j)), tempa), 8);
					__m64 rb = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb2.r[i] + j)), tempa2), 8);
					__m64 gb = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb2.g[i] + j)), tempa2), 8);
					__m64 bb = _mm_srli_pi16(_mm_mullo_pi16(MM((rgb2.b[i] + j)), tempa2), 8);

					__m64 r = _mm_add_pi16(ra, rb);
					__m64 g = _mm_add_pi16(ga, gb);
					__m64 b = _mm_add_pi16(ba, bb);

					__m64 y1 = _mm_srli_pi16(_mm_mullo_pi16(r, r1), 8),
						y2 = _mm_srli_pi16(_mm_mullo_pi16(g, g1), 7),
						y3 = _mm_srli_pi16(_mm_mullo_pi16(b, b1), 10),
						u1 = _mm_srli_pi16(_mm_mullo_pi16(r, r2), 9),
						u2 = _mm_srli_pi16(_mm_mullo_pi16(g, g2), 8),
						u3 = _mm_srli_pi16(_mm_mullo_pi16(b, b2), 8),
						v1 = _mm_srli_pi16(_mm_mullo_pi16(r, b2), 8),
						v2 = _mm_srli_pi16(_mm_mullo_pi16(g, g3), 8),
						v3 = _mm_srli_pi16(_mm_mullo_pi16(b, b3), 10);
					MM((temp.y[i] + j)) = _mm_add_pi16(_mm_add_pi16(_mm_add_pi16(y1, y2), y3), tmp1);
					MM((temp.u[i] + j)) = _mm_add_pi16(_mm_sub_pi16(_mm_sub_pi16(u3, u2), u1), tmp2);
					MM((temp.v[i] + j)) = _mm_add_pi16(_mm_sub_pi16(_mm_sub_pi16(v1, v2), v3), tmp2);

				}
			time_e = clock();
			time += (time_e - time_s) / 1000.0;

#ifdef DEBUG
			string filename = "mmx_overlay.yuv";
			store_pic(filename, temp);
#endif // DEBUG

			time_s = clock();
		}
		

	}
	_mm_empty();
	return time;
}

double process_with_sse(int type, yuv32 &yuv1, rgb32 &rgb1, yuv32 &yuv2, rgb32 &rgb2)
{
	double time = 0;
	int time_s, time_e;
	yuv32 temp;
	string filename;
	time_s = clock();
	__m128 upBound = _mm_set_ps(255.0, 255.0, 255.0, 255.0);
	__m128 lowBound = _mm_set_ps(0, 0, 0, 0);
	for (int i = 0; i < 1080; i++)
		for (int j = 0; j < 1920; j+=4)
		{
			/*rgb1.r[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 1.596027 * (yuv1.v[i][j] - 128);
			rgb1.b[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 2.017232 * (yuv1.u[i][j] - 128);
			rgb1.g[i][j] = 1.164383 * (yuv1.y[i][j] - 16) - 0.391762 * (yuv1.u[i][j] - 128) - 0.812968 * (yuv1.v[i][j] - 128);*/
			//非对齐用loadu
			__m128i y = _mm_loadu_si128((__m128i *)(yuv1.y[i] + j));
			__m128i u = _mm_loadu_si128((__m128i *)(yuv1.u[i] + j));
			__m128i v = _mm_loadu_si128((__m128i *)(yuv1.v[i] + j));
			//整数运算
			y = _mm_sub_epi32(y, _mm_set_epi32(16, 16, 16, 16));
			u = _mm_sub_epi32(u, _mm_set_epi32(128, 128, 128, 128));
			v = _mm_sub_epi32(v, _mm_set_epi32(128, 128, 128, 128));
			//整数转浮点
			__m128 y_f = _mm_cvtepi32_ps(y);
			__m128 u_f = _mm_cvtepi32_ps(u);
			__m128 v_f = _mm_cvtepi32_ps(v);

			__m128 yr = _mm_set_ps(1.164383, 1.164383, 1.164383, 1.164383);
			__m128 vr = _mm_set_ps(1.596027, 1.596027, 1.596027, 1.596027);
			__m128 r1 = _mm_mul_ps(y_f, yr);
			__m128 r2 = _mm_mul_ps(v_f, vr);
			__m128 r_res = _mm_add_ps(r1, r2);

			__m128 ub = _mm_set_ps(2.017232, 2.017232, 2.017232, 2.017232);
			__m128 b2 = _mm_mul_ps(u_f, ub);
			__m128 b_res = _mm_add_ps(r1, b2);

			__m128 ug = _mm_set_ps(-0.391762, -0.391762, -0.391762, -0.391762);
			__m128 vg = _mm_set_ps(-0.812968, -0.812968, -0.812968, -0.812968);
			__m128 g2 = _mm_mul_ps(u_f, ug);
			__m128 g3 = _mm_mul_ps(v_f, vg);
			__m128 g_res = _mm_add_ps(_mm_add_ps(r1, g2), g3);

			/*if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
			if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
			if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
			if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
			if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
			if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;*/

			r_res = _mm_min_ps(r_res, upBound);
			g_res = _mm_min_ps(g_res, upBound);
			b_res = _mm_min_ps(b_res, upBound);

			r_res = _mm_max_ps(r_res, lowBound);
			g_res = _mm_max_ps(g_res, lowBound);
			b_res = _mm_max_ps(b_res, lowBound);
			
			__m128i r_back = _mm_cvtps_epi32(r_res);
			__m128i g_back = _mm_cvtps_epi32(g_res);
			__m128i b_back = _mm_cvtps_epi32(b_res);
			//非对齐用storeu
			_mm_storeu_si128((__m128i *)(rgb1.r[i] + j), r_back);
			_mm_storeu_si128((__m128i *)(rgb1.g[i] + j), g_back);
			_mm_storeu_si128((__m128i *)(rgb1.b[i] + j), b_back);
		}
	if (type == 1)
	{
		for (int a = 1; a < 255; a += 3)
		{
			__m128 tempa = _mm_set_ps((float)a / 256.0, (float)a / 256.0, (float)a / 256.0, (float)a / 256.0);
			__m128 temp16 = _mm_set_ps(16.0, 16.0, 16.0, 16.0);
			__m128 temp128 = _mm_set_ps(128.0, 128.0, 128.0, 128.0);
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j += 4)
				{
					/*temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;*/
					
					__m128i r = _mm_loadu_si128((__m128i *)(rgb1.r[i] + j));
					__m128i g = _mm_loadu_si128((__m128i *)(rgb1.g[i] + j));
					__m128i b = _mm_loadu_si128((__m128i *)(rgb1.b[i] + j));
					
					__m128 r_f = _mm_cvtepi32_ps(r);
					__m128 g_f = _mm_cvtepi32_ps(g);
					__m128 b_f = _mm_cvtepi32_ps(b);

					r_f = _mm_mul_ps(r_f, tempa);
					g_f = _mm_mul_ps(g_f, tempa);
					b_f = _mm_mul_ps(b_f, tempa);

					__m128 y1 = _mm_mul_ps(r_f, _mm_set_ps(0.256788, 0.256788, 0.256788, 0.256788));
					__m128 y2 = _mm_mul_ps(g_f, _mm_set_ps(0.504129, 0.504129, 0.504129, 0.504129));
					__m128 y3 = _mm_mul_ps(b_f, _mm_set_ps(0.097906, 0.097906, 0.097906, 0.097906));

					__m128 u1 = _mm_mul_ps(r_f, _mm_set_ps(-0.148223, -0.148223, -0.148223, -0.148223));
					__m128 u2 = _mm_mul_ps(g_f, _mm_set_ps(-0.290993, -0.290993, -0.290993, -0.290993));
					__m128 u3 = _mm_mul_ps(b_f, _mm_set_ps(0.439216, 0.439216, 0.439216, 0.439216));

					__m128 v1 = _mm_mul_ps(r_f, _mm_set_ps(0.439216, 0.439216, 0.439216, 0.439216));
					__m128 v2 = _mm_mul_ps(g_f, _mm_set_ps(-0.367788, -0.367788, -0.367788, -0.367788));
					__m128 v3 = _mm_mul_ps(b_f, _mm_set_ps(-0.071427, -0.071427, -0.071427, -0.071427));

					__m128 y = _mm_add_ps(_mm_add_ps(y1, y2), _mm_add_ps(y3, temp16));
					__m128 u = _mm_add_ps(_mm_add_ps(u1, u2), _mm_add_ps(u3, temp128));
					__m128 v = _mm_add_ps(_mm_add_ps(v1, v2), _mm_add_ps(v3, temp128));

					__m128i y_back = _mm_cvtps_epi32(y);
					__m128i u_back = _mm_cvtps_epi32(u);
					__m128i v_back = _mm_cvtps_epi32(v);
					
					_mm_storeu_si128((__m128i *)(temp.y[i] + j), y_back);
					_mm_storeu_si128((__m128i *)(temp.u[i] + j), u_back);
					_mm_storeu_si128((__m128i *)(temp.v[i] + j), v_back);
				}
			time_e = clock();
			time += (double)(time_e - time_s) / 1000.0;

#ifdef DEBUG
			filename = "sse_fade.yuv";
			store_pic(filename, temp);
#endif // DEBUG

			time_s = clock();
		}


	}
		
	else
	{
		for (int i = 0; i < 1080; i++)
			for (int j = 0; j < 1920; j += 4)
			{
				__m128i y = _mm_loadu_si128((__m128i *)(yuv2.y[i] + j));
				__m128i u = _mm_loadu_si128((__m128i *)(yuv2.u[i] + j));
				__m128i v = _mm_loadu_si128((__m128i *)(yuv2.v[i] + j));

				y = _mm_sub_epi32(y, _mm_set_epi32(16, 16, 16, 16));
				u = _mm_sub_epi32(u, _mm_set_epi32(128, 128, 128, 128));
				v = _mm_sub_epi32(v, _mm_set_epi32(128, 128, 128, 128));

				__m128 y_f = _mm_cvtepi32_ps(y);
				__m128 u_f = _mm_cvtepi32_ps(u);
				__m128 v_f = _mm_cvtepi32_ps(v);

				__m128 yr = _mm_set_ps(1.164383, 1.164383, 1.164383, 1.164383);
				__m128 vr = _mm_set_ps(1.596027, 1.596027, 1.596027, 1.596027);
				__m128 r1 = _mm_mul_ps(y_f, yr);
				__m128 r2 = _mm_mul_ps(v_f, vr);
				__m128 r_res = _mm_add_ps(r1, r2);

				__m128 ub = _mm_set_ps(2.017232, 2.017232, 2.017232, 2.017232);
				__m128 b2 = _mm_mul_ps(u_f, ub);
				__m128 b_res = _mm_add_ps(r1, b2);

				__m128 ug = _mm_set_ps(-0.391762, -0.391762, -0.391762, -0.391762);
				__m128 vg = _mm_set_ps(-0.812968, -0.812968, -0.812968, -0.812968);
				__m128 g2 = _mm_mul_ps(u_f, ug);
				__m128 g3 = _mm_mul_ps(v_f, vg);
				__m128 g_res = _mm_add_ps(_mm_add_ps(r1, g2), g3);

				/*if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
				if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
				if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
				if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
				if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
				if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;*/

				r_res = _mm_min_ps(r_res, upBound);
				g_res = _mm_min_ps(g_res, upBound);
				b_res = _mm_min_ps(b_res, upBound);

				r_res = _mm_max_ps(r_res, lowBound);
				g_res = _mm_max_ps(g_res, lowBound);
				b_res = _mm_max_ps(b_res, lowBound);

				__m128i r_back = _mm_cvtps_epi32(r_res);
				__m128i g_back = _mm_cvtps_epi32(g_res);
				__m128i b_back = _mm_cvtps_epi32(b_res);

				_mm_storeu_si128((__m128i *)(rgb2.r[i] + j), r_back);
				_mm_storeu_si128((__m128i *)(rgb2.g[i] + j), g_back);
				_mm_storeu_si128((__m128i *)(rgb2.b[i] + j), b_back);
			}

		for (int a = 1; a < 255; a += 3)
		{
			__m128 tempa = _mm_set_ps((float)a / 256.0, (float)a / 256.0, (float)a / 256.0, (float)a / 256.0);
			float b = 256 - a;
			__m128 tempa2 = _mm_set_ps(b / 256.0, b / 256.0, b / 256.0, b / 256.0);
			__m128 temp16 = _mm_set_ps(16.0, 16.0, 16.0, 16.0);
			__m128 temp128 = _mm_set_ps(128.0, 128.0, 128.0, 128.0);
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j += 4)
				{
					/*temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
					temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
					temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;*/

					__m128i r = _mm_loadu_si128((__m128i *)(rgb1.r[i] + j));
					__m128i g = _mm_loadu_si128((__m128i *)(rgb1.g[i] + j));
					__m128i b = _mm_loadu_si128((__m128i *)(rgb1.b[i] + j));
					__m128i r2 = _mm_loadu_si128((__m128i *)(rgb2.r[i] + j));
					__m128i g2 = _mm_loadu_si128((__m128i *)(rgb2.g[i] + j));
					__m128i b2 = _mm_loadu_si128((__m128i *)(rgb2.b[i] + j));

					__m128 r_f = _mm_cvtepi32_ps(r);
					__m128 g_f = _mm_cvtepi32_ps(g);
					__m128 b_f = _mm_cvtepi32_ps(b);
					__m128 r2_f = _mm_cvtepi32_ps(r2);
					__m128 g2_f = _mm_cvtepi32_ps(g2);
					__m128 b2_f = _mm_cvtepi32_ps(b2);

					r_f = _mm_add_ps(_mm_mul_ps(r_f, tempa), _mm_mul_ps(r2_f, tempa2));
					g_f = _mm_add_ps(_mm_mul_ps(g_f, tempa), _mm_mul_ps(g2_f, tempa2));
					b_f = _mm_add_ps(_mm_mul_ps(b_f, tempa), _mm_mul_ps(b2_f, tempa2));

					__m128 y1 = _mm_mul_ps(r_f, _mm_set_ps(0.256788, 0.256788, 0.256788, 0.256788));
					__m128 y2 = _mm_mul_ps(g_f, _mm_set_ps(0.504129, 0.504129, 0.504129, 0.504129));
					__m128 y3 = _mm_mul_ps(b_f, _mm_set_ps(0.097906, 0.097906, 0.097906, 0.097906));

					__m128 u1 = _mm_mul_ps(r_f, _mm_set_ps(-0.148223, -0.148223, -0.148223, -0.148223));
					__m128 u2 = _mm_mul_ps(g_f, _mm_set_ps(-0.290993, -0.290993, -0.290993, -0.290993));
					__m128 u3 = _mm_mul_ps(b_f, _mm_set_ps(0.439216, 0.439216, 0.439216, 0.439216));

					__m128 v1 = _mm_mul_ps(r_f, _mm_set_ps(0.439216, 0.439216, 0.439216, 0.439216));
					__m128 v2 = _mm_mul_ps(g_f, _mm_set_ps(-0.367788, -0.367788, -0.367788, -0.367788));
					__m128 v3 = _mm_mul_ps(b_f, _mm_set_ps(-0.071427, -0.071427, -0.071427, -0.071427));

					__m128 y = _mm_add_ps(_mm_add_ps(y1, y2), _mm_add_ps(y3, temp16));
					__m128 u = _mm_add_ps(_mm_add_ps(u1, u2), _mm_add_ps(u3, temp128));
					__m128 v = _mm_add_ps(_mm_add_ps(v1, v2), _mm_add_ps(v3, temp128));

					__m128i y_back = _mm_cvtps_epi32(y);
					__m128i u_back = _mm_cvtps_epi32(u);
					__m128i v_back = _mm_cvtps_epi32(v);

					_mm_storeu_si128((__m128i *)(temp.y[i] + j), y_back);
					_mm_storeu_si128((__m128i *)(temp.u[i] + j), u_back);
					_mm_storeu_si128((__m128i *)(temp.v[i] + j), v_back);
				}
			time_e = clock();
			time += (double)(time_e - time_s) / 1000.0;

#ifdef DEBUG
			filename = "sse_overlay.yuv";
			store_pic(filename, temp);
#endif // DEBUG

			time_s = clock();
		}


	}

	return time;
}
//基本与SSE相同，只修改了数据类型及相应函数
double process_with_avx(int type, yuv32 &yuv1, rgb32 &rgb1, yuv32 &yuv2, rgb32 &rgb2) 
{

	{
		double time = 0;
		int time_s, time_e;
		yuv32 temp;
		string filename;
		time_s = clock();
		__m256 upBound = _mm256_set_ps(255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0);
		__m256 lowBound = _mm256_set_ps(0, 0, 0, 0, 0, 0, 0, 0);
		for (int i = 0; i < 1080; i++)
			for (int j = 0; j < 1920; j+=8)
			{
				/*rgb1.r[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 1.596027 * (yuv1.v[i][j] - 128);
				rgb1.b[i][j] = 1.164383 * (yuv1.y[i][j] - 16) + 2.017232 * (yuv1.u[i][j] - 128);
				rgb1.g[i][j] = 1.164383 * (yuv1.y[i][j] - 16) - 0.391762 * (yuv1.u[i][j] - 128) - 0.812968 * (yuv1.v[i][j] - 128);*/
				__m256i y = _mm256_loadu_si256((__m256i *)(yuv1.y[i] + j));
				__m256i u = _mm256_loadu_si256((__m256i *)(yuv1.u[i] + j));
				__m256i v = _mm256_loadu_si256((__m256i *)(yuv1.v[i] + j));

				y = _mm256_sub_epi32(y, _mm256_set_epi32(16, 16, 16, 16, 16, 16, 16, 16));
				u = _mm256_sub_epi32(u, _mm256_set_epi32(128, 128, 128, 128, 128, 128, 128, 128));
				v = _mm256_sub_epi32(v, _mm256_set_epi32(128, 128, 128, 128, 128, 128, 128, 128));

				__m256 y_f = _mm256_cvtepi32_ps(y);
				__m256 u_f = _mm256_cvtepi32_ps(u);
				__m256 v_f = _mm256_cvtepi32_ps(v);

				__m256 yr = _mm256_set_ps(1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383);
				__m256 vr = _mm256_set_ps(1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027);
				__m256 r1 = _mm256_mul_ps(y_f, yr);
				__m256 r2 = _mm256_mul_ps(v_f, vr);
				__m256 r_res = _mm256_add_ps(r1, r2);

				__m256 ub = _mm256_set_ps(2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232);
				__m256 b2 = _mm256_mul_ps(u_f, ub);
				__m256 b_res = _mm256_add_ps(r1, b2);

				__m256 ug = _mm256_set_ps(-0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762);
				__m256 vg = _mm256_set_ps(-0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968);
				__m256 g2 = _mm256_mul_ps(u_f, ug);
				__m256 g3 = _mm256_mul_ps(v_f, vg);
				__m256 g_res = _mm256_add_ps(_mm256_add_ps(r1, g2), g3);

				/*if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
				if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
				if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
				if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
				if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
				if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;*/

				r_res = _mm256_min_ps(r_res, upBound);
				g_res = _mm256_min_ps(g_res, upBound);
				b_res = _mm256_min_ps(b_res, upBound);

				r_res = _mm256_max_ps(r_res, lowBound);
				g_res = _mm256_max_ps(g_res, lowBound);
				b_res = _mm256_max_ps(b_res, lowBound);

				__m256i r_back = _mm256_cvtps_epi32(r_res);
				__m256i g_back = _mm256_cvtps_epi32(g_res);
				__m256i b_back = _mm256_cvtps_epi32(b_res);

				_mm256_storeu_si256((__m256i *)(rgb1.r[i] + j), r_back);
				_mm256_storeu_si256((__m256i *)(rgb1.g[i] + j), g_back);
				_mm256_storeu_si256((__m256i *)(rgb1.b[i] + j), b_back);
			}
		if (type == 1)
		{
			for (int a = 1; a < 255; a += 3)
			{
				float tmp = (float)a / 256.0;
				__m256 tempa = _mm256_set_ps(tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp);
				__m256 temp16 = _mm256_set_ps(16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0);
				__m256 temp128 = _mm256_set_ps(128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0);
				for (int i = 0; i < 1080; i++)
					for (int j = 0; j < 1920; j += 8)
					{
						/*temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
						temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
						temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;*/

						__m256i r = _mm256_loadu_si256((__m256i *)(rgb1.r[i] + j));
						__m256i g = _mm256_loadu_si256((__m256i *)(rgb1.g[i] + j));
						__m256i b = _mm256_loadu_si256((__m256i *)(rgb1.b[i] + j));

						__m256 r_f = _mm256_cvtepi32_ps(r);
						__m256 g_f = _mm256_cvtepi32_ps(g);
						__m256 b_f = _mm256_cvtepi32_ps(b);

						r_f = _mm256_mul_ps(r_f, tempa);
						g_f = _mm256_mul_ps(g_f, tempa);
						b_f = _mm256_mul_ps(b_f, tempa);

						__m256 y1 = _mm256_mul_ps(r_f, _mm256_set_ps(0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788));
						__m256 y2 = _mm256_mul_ps(g_f, _mm256_set_ps(0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129));
						__m256 y3 = _mm256_mul_ps(b_f, _mm256_set_ps(0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906));

						__m256 u1 = _mm256_mul_ps(r_f, _mm256_set_ps(-0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223));
						__m256 u2 = _mm256_mul_ps(g_f, _mm256_set_ps(-0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993));
						__m256 u3 = _mm256_mul_ps(b_f, _mm256_set_ps(0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216));

						__m256 v1 = _mm256_mul_ps(r_f, _mm256_set_ps(0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216));
						__m256 v2 = _mm256_mul_ps(g_f, _mm256_set_ps(-0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788));
						__m256 v3 = _mm256_mul_ps(b_f, _mm256_set_ps(-0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427));

						__m256 y = _mm256_add_ps(_mm256_add_ps(y1, y2), _mm256_add_ps(y3, temp16));
						__m256 u = _mm256_add_ps(_mm256_add_ps(u1, u2), _mm256_add_ps(u3, temp128));
						__m256 v = _mm256_add_ps(_mm256_add_ps(v1, v2), _mm256_add_ps(v3, temp128));

						__m256i y_back = _mm256_cvtps_epi32(y);
						__m256i u_back = _mm256_cvtps_epi32(u);
						__m256i v_back = _mm256_cvtps_epi32(v);

						_mm256_storeu_si256((__m256i *)(temp.y[i] + j), y_back);
						_mm256_storeu_si256((__m256i *)(temp.u[i] + j), u_back);
						_mm256_storeu_si256((__m256i *)(temp.v[i] + j), v_back);
					}
				time_e = clock();
				time += (double)(time_e - time_s) / 1000.0;

#ifdef DEBUG
				filename = "avx_fade.yuv";
				store_pic(filename, temp);
#endif // DEBUG

				time_s = clock();
			}


		}

		else
		{
			for (int i = 0; i < 1080; i++)
				for (int j = 0; j < 1920; j += 8)
				{
					__m256i y = _mm256_loadu_si256((__m256i *)(yuv2.y[i] + j));
					__m256i u = _mm256_loadu_si256((__m256i *)(yuv2.u[i] + j));
					__m256i v = _mm256_loadu_si256((__m256i *)(yuv2.v[i] + j));

					y = _mm256_sub_epi32(y, _mm256_set_epi32(16, 16, 16, 16, 16, 16, 16, 16));
					u = _mm256_sub_epi32(u, _mm256_set_epi32(128, 128, 128, 128, 128, 128, 128, 128));
					v = _mm256_sub_epi32(v, _mm256_set_epi32(128, 128, 128, 128, 128, 128, 128, 128));

					__m256 y_f = _mm256_cvtepi32_ps(y);
					__m256 u_f = _mm256_cvtepi32_ps(u);
					__m256 v_f = _mm256_cvtepi32_ps(v);

					__m256 yr = _mm256_set_ps(1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383);
					__m256 vr = _mm256_set_ps(1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027);
					__m256 r1 = _mm256_mul_ps(y_f, yr);
					__m256 r2 = _mm256_mul_ps(v_f, vr);
					__m256 r_res = _mm256_add_ps(r1, r2);

					__m256 ub = _mm256_set_ps(2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232);
					__m256 b2 = _mm256_mul_ps(u_f, ub);
					__m256 b_res = _mm256_add_ps(r1, b2);

					__m256 ug = _mm256_set_ps(-0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762, -0.391762);
					__m256 vg = _mm256_set_ps(-0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968, -0.812968);
					__m256 g2 = _mm256_mul_ps(u_f, ug);
					__m256 g3 = _mm256_mul_ps(v_f, vg);
					__m256 g_res = _mm256_add_ps(_mm256_add_ps(r1, g2), g3);

					/*if (rgb1.r[i][j] < 0)	rgb1.r[i][j] = 0;
					if (rgb1.r[i][j] > 255)	rgb1.r[i][j] = 255;
					if (rgb1.b[i][j] < 0)	rgb1.b[i][j] = 0;
					if (rgb1.b[i][j] > 255)	rgb1.b[i][j] = 255;
					if (rgb1.g[i][j] < 0)	rgb1.g[i][j] = 0;
					if (rgb1.g[i][j] > 255)	rgb1.g[i][j] = 255;*/

					r_res = _mm256_min_ps(r_res, upBound);
					g_res = _mm256_min_ps(g_res, upBound);
					b_res = _mm256_min_ps(b_res, upBound);

					r_res = _mm256_max_ps(r_res, lowBound);
					g_res = _mm256_max_ps(g_res, lowBound);
					b_res = _mm256_max_ps(b_res, lowBound);

					__m256i r_back = _mm256_cvtps_epi32(r_res);
					__m256i g_back = _mm256_cvtps_epi32(g_res);
					__m256i b_back = _mm256_cvtps_epi32(b_res);

					_mm256_storeu_si256((__m256i *)(rgb2.r[i] + j), r_back);
					_mm256_storeu_si256((__m256i *)(rgb2.g[i] + j), g_back);
					_mm256_storeu_si256((__m256i *)(rgb2.b[i] + j), b_back);
				}

			for (int a = 1; a < 255; a += 3)
			{
				float tmp = (float)a / 256.0;
				__m256 tempa = _mm256_set_ps(tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp);
				float b = (256 - a) / 256.0;
				__m256 tempa2 = _mm256_set_ps(b, b, b, b, b, b, b, b);
				__m256 temp16 = _mm256_set_ps(16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0);
				__m256 temp128 = _mm256_set_ps(128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0);
				for (int i = 0; i < 1080; i++)
					for (int j = 0; j < 1920; j += 8)
					{
						/*temp.y[i][j] = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16;
						temp.u[i][j] = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128;
						temp.v[i][j] = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128;*/

						__m256i r = _mm256_loadu_si256((__m256i *)(rgb1.r[i] + j));
						__m256i g = _mm256_loadu_si256((__m256i *)(rgb1.g[i] + j));
						__m256i b = _mm256_loadu_si256((__m256i *)(rgb1.b[i] + j));
						__m256i r2 = _mm256_loadu_si256((__m256i *)(rgb2.r[i] + j));
						__m256i g2 = _mm256_loadu_si256((__m256i *)(rgb2.g[i] + j));
						__m256i b2 = _mm256_loadu_si256((__m256i *)(rgb2.b[i] + j));

						__m256 r_f = _mm256_cvtepi32_ps(r);
						__m256 g_f = _mm256_cvtepi32_ps(g);
						__m256 b_f = _mm256_cvtepi32_ps(b);
						__m256 r2_f = _mm256_cvtepi32_ps(r2);
						__m256 g2_f = _mm256_cvtepi32_ps(g2);
						__m256 b2_f = _mm256_cvtepi32_ps(b2);

						r_f = _mm256_add_ps(_mm256_mul_ps(r_f, tempa), _mm256_mul_ps(r2_f, tempa2));
						g_f = _mm256_add_ps(_mm256_mul_ps(g_f, tempa), _mm256_mul_ps(g2_f, tempa2));
						b_f = _mm256_add_ps(_mm256_mul_ps(b_f, tempa), _mm256_mul_ps(b2_f, tempa2));

						__m256 y1 = _mm256_mul_ps(r_f, _mm256_set_ps(0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788));
						__m256 y2 = _mm256_mul_ps(g_f, _mm256_set_ps(0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129));
						__m256 y3 = _mm256_mul_ps(b_f, _mm256_set_ps(0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906));

						__m256 u1 = _mm256_mul_ps(r_f, _mm256_set_ps(-0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223, -0.148223));
						__m256 u2 = _mm256_mul_ps(g_f, _mm256_set_ps(-0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993, -0.290993));
						__m256 u3 = _mm256_mul_ps(b_f, _mm256_set_ps(0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216));

						__m256 v1 = _mm256_mul_ps(r_f, _mm256_set_ps(0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216));
						__m256 v2 = _mm256_mul_ps(g_f, _mm256_set_ps(-0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788, -0.367788));
						__m256 v3 = _mm256_mul_ps(b_f, _mm256_set_ps(-0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427, -0.071427));

						__m256 y = _mm256_add_ps(_mm256_add_ps(y1, y2), _mm256_add_ps(y3, temp16));
						__m256 u = _mm256_add_ps(_mm256_add_ps(u1, u2), _mm256_add_ps(u3, temp128));
						__m256 v = _mm256_add_ps(_mm256_add_ps(v1, v2), _mm256_add_ps(v3, temp128));

						__m256i y_back = _mm256_cvtps_epi32(y);
						__m256i u_back = _mm256_cvtps_epi32(u);
						__m256i v_back = _mm256_cvtps_epi32(v);

						_mm256_storeu_si256((__m256i *)(temp.y[i] + j), y_back);
						_mm256_storeu_si256((__m256i *)(temp.u[i] + j), u_back);
						_mm256_storeu_si256((__m256i *)(temp.v[i] + j), v_back);
					}
				time_e = clock();
				time += (double)(time_e - time_s) / 1000.0;

#ifdef DEBUG
				filename = "avx_overlay.yuv";
				store_pic(filename, temp);
#endif // DEBUG

				time_s = clock();
			}


		}

		return time;
	}
}