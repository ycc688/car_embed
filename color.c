#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LCD_DEV_PATH "/dev/fb0"
#define MAP_LENGTH 800*480*4
#define MAP_OFFSET 0
#define OPEN_LCD_FLAG O_RDWR
#define OPEN_BMP_FLAG O_RDONLY
#define BMP_OFFSET 54
#define MALLOC_RGB 800*480*3

typedef struct dis_inf
{
	int lcd_fd;
	int bmp_fd;
	int skip;
	int *mmap_star;
	int bmp_w,bmp_h;
	char r,g,b;
	int rgb;
} DI,*P_DI;

P_DI Init();
int Free(P_DI I);
int Dis_Bmp(P_DI I, unsigned char  R, unsigned char  G, unsigned char B);

P_DI Init()
{
	//先给存放常用变量对应结构体申请空间
	P_DI I = (P_DI)malloc(sizeof(DI));
	if(I == NULL)
	{
		perror("malloc inf");
		return NULL;
	}

	I->lcd_fd = open(LCD_DEV_PATH,OPEN_LCD_FLAG); //严谨一点
	if(I->lcd_fd == -1)
	{
		perror("open lcd ");
		return NULL;
	}

	I->mmap_star = (int *)mmap(NULL,MAP_LENGTH,PROT_READ | PROT_WRITE,MAP_SHARED,I->lcd_fd,MAP_OFFSET);
	if(I->mmap_star == MAP_FAILED)
	{
		perror("mmap lcd");
		return NULL;
	}

	return I;
}

int Free(P_DI I)
{
	close(I->lcd_fd);
	munmap(I->mmap_star,MAP_LENGTH);

	return 0;
}


int Dis_Bmp(P_DI I, unsigned char  R, unsigned char  G, unsigned char B)
{



	/*根据形参中的int where_x,int where_y来确定映射指针开始赋值像素的位置*/
	int * new_p = I->mmap_star;

	I->bmp_w = 800;
	I->bmp_h = 480;

	int malloc_length = I->bmp_w*I->bmp_h*3;


	unsigned long long rgb = B | G << 8 | R<<16;;

	//循环有规律的把像素点赋值给映射指针
	int x,y,z=0;
	for(y=0; y<I->bmp_h; y++)
	{
		for(x=0; x<I->bmp_w; x++,z+=3)
		{
			//I->rgb = (I->b)<<0 | (I->g)<<8 | (I->r)<<16;
			//memcpy(new_p+800*(I->bmp_h-1-y)+x,&(I->rgb),4);
			*(new_p+800*(I->bmp_h-1-y)+x) = rgb;

		}
	}

	return 0;
}


int main(const int argc, const char *argv[])
{
	//打开lcd和bmp
	P_DI I = Init();
	if(I == NULL)
	{
		printf("LCD初始化失败！\n");
		return -1;
	}


	unsigned char r = 202;
	unsigned char g = 234;
	unsigned char b = 206;

	if(argc == 4)
	{
		r = atoi(argv[1]);
		g = atoi(argv[2]);
		b = atoi(argv[3]);
	}

	Dis_Bmp(I, r, g, b);

	Free(I);

	return 0;
}




// #include<unistd.h>
// #include<sys/stat.h>
// #include<sys/types.h>
// #include<fcntl.h>
// #include<stdio.h>
// #include <stdlib.h>
// #include<errno.h>
// #include<string.h>

// int main(const int argc, const char *argv[])
// {
// int fd = open("/dev/fb0", O_RDWR);
// if(-1 == fd)
// {
// perror("\r\n  open /dev/fb0 error!  \r\n");
// }

// char c[800*480];
// memset(c, 0xff, 800*480);


// int num = 0;
// int area = 1;


// if(argc == 2)
// {
// if(0 == strcmp(argv[1], "a"))
// {
// area = 3;
// }
// else
// {
// int temp = atoi(argv[1]);
// if(temp > 1 && temp < 4)
// {
// num = temp;
// }
// }
// }

// unsigned long long offset = 800*480*4*num;

// for(int i = 0; i < area; i++)
// {
// for(int j = 0; j < 4; j++)
// {
// lseek(fd, 800*480*j + offset, SEEK_SET);
// write(fd, c, 800*480);
// }
// }
// close(fd);
// return 0;
// }
