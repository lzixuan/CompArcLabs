使用方法 运行参数：./执行程序名 1（渐变）或2（叠加）
在源代码中#define DEBUG可生成处理后的YUV（打包附yuvplayer），生成文件体积大且极慢
PS：目前只能保证在windows+vs的环境下正常运行，如需测试程序运行请和我说一声
g++编译要加 -mmmx -msse -msse2 -mavx2编译选项，运行时有段错误