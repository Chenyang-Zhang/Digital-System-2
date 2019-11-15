#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<math.h>
#define MAX_RANDOM 100 //产生随机浮点数的范围
#define MAX_CALCULATION 1000  //验证次数

typedef union{
		struct{           //小端模式
			unsigned int mantissa : 23; //尾数位
			unsigned int exponent : 8;	//阶数位
			unsigned int sign :1;	    //符号位
		};
	float value; 
} Float;

float add_float(Float m, Float n);
float random(int a, int b);
void normalize(Float *n);
int abnormal_handle(Float m, Float n);

int main(){
	Float num1, num2;
	int i;

	for(i = 0; i < MAX_CALCULATION; i++){
		
		num1.value = random(-MAX_RANDOM, MAX_RANDOM);
		sleep(1.0);
		num2.value = random(-MAX_RANDOM, MAX_RANDOM);
		printf("The value of num1 and num2 is %f, %f respectively\n", num1.value, num2.value);
		printf("The sum of num1 and num2 calculated by fpu is %f\n", num1.value + num2.value);
		printf("The sum of num1 and num2 calculated by me is %f\n", add_float(num1, num2));
		
		if(abs((num1.value + num2.value) - add_float(num1, num2)) >= 0.001) //精度设定为0.001
			printf("Wrong calculation!\n");
		else
			printf("Correct calculation\n");
		
		printf("\n");
	}
}

//实现两个浮点数的相加
float add_float(Float m, Float n){
	Float result;
	int sign;

	sign = abnormal_handle(m, n);
	if(sign != 0){
		if(sign == 1)
			return n.value;
		else if(sign == 2)
			return m.value;
		else 
			return 0;
	}


	//对阶、尾数求和
	int diff = m.exponent - n.exponent, i = 0;
	unsigned int temp[diff] = {0};
	unsigned int mantissa_m = m.mantissa, mantissa_n = n.mantissa;
	int mantissa_r;
	
	if(diff > 0){       //第一种情况
		
		mantissa_n |= 0x800000;  //还原隐藏位的1
		while(diff > 0){               
			temp[i] = mantissa_n & 0x1;  //保留移出的尾数
			mantissa_n = mantissa_n >> 1;	//移位对阶
			diff --;
			i ++;
		}
		result.exponent = m.exponent;
		mantissa_m |= 0x800000;
		//尾数相加
		mantissa_r = pow(-1, m.sign)*mantissa_m + pow(-1, n.sign)*mantissa_n;
		if(mantissa_r >= 0)
			result.sign = 0;	
		else{
			result.sign = 1;
			mantissa_r *= -1;
		}
		
		while( (mantissa_r | 0x7fffff) != 0xffffff){
				
				mantissa_r = mantissa_r << 1;
				result.exponent --;
		}
		result.mantissa = mantissa_r & 0x7fffff;
	}
	else if(diff < 0){    //第二种情况
		
		diff *= -1;	
		mantissa_m |= 0x800000; 
		while(diff > 0){
			temp[i] = mantissa_m & 0x000001;
			mantissa_m = mantissa_m >> 1;
			diff --;
		}
		result.exponent = n.exponent;
		mantissa_n |= 0x800000;
		mantissa_r = pow(-1, m.sign)*mantissa_m + pow(-1, n.sign)*mantissa_n;
		if(mantissa_r >= 0)
			result.sign = 0;	
		else{
			result.sign = 1;
			mantissa_r *= -1;
		}
		
		while( (mantissa_r | 0x7fffff) != 0xffffff){
				
				mantissa_r = mantissa_r << 1;
				result.exponent --;
		
		}
		result.mantissa = mantissa_r & 0x7fffff;
	}
	else{
		if(m.sign == 0 && n.sign == 0){	     //第三种情况
			
			result.sign = 0;
			result.mantissa = (mantissa_m + mantissa_n) >> 1; 
			result.exponent = m.exponent + 1;
					
		}
		else if(m.sign == 1 && n.sign ==1){  //第四种情况
			
			result.sign = 1;
			result.mantissa = (mantissa_m + mantissa_n) >> 1; 
			result.exponent = m.exponent + 1;
			
		}
		else{                               //第五种情况
			
			result.exponent = m.exponent;
		    mantissa_m |= 0x800000;	
			mantissa_n |= 0x800000;
			mantissa_r = pow(-1, m.sign)*mantissa_m + pow(-1, n.sign)*mantissa_n;
			if(mantissa_r >= 0)
				result.sign = 0;	
			else{
				result.sign = 1;
				result.mantissa *= -1;
				
			}

			while((mantissa_r | 0x3fffff) != 0x7fffff){
					mantissa_r = mantissa_r << 1;
					result.exponent --;
			}
			result.mantissa = mantissa_r & 0xfffff;
	
		}	

	}

	sign = abnormal_handle(result, result);
	if(sign != 0){
		if(sign == 1)
			return n.value;
		else if(sign == 2)
			return m.value;
		else 
			return 0;
	}

	return result.value;

}

//规格化处理
void normalize(Float *n){
			
}


//随机生成a~b之间的浮点数
float random(int a,  int b){
	
	srand((int)time(0));	
	return rand()/(float)(RAND_MAX)*(b-a) + a ;	

}

//异常情况处理
int abnormal_handle(Float m, Float n){
	
	//情况1，产生+0、\0
	if((m.exponent == 0) && (m.mantissa == 0))
		return 1;
	if((n.exponent == 0) && (n.mantissa == 0))
		return 2;

	//情况2，出现非规格化数
	if(((m.exponent == 0) && (m.mantissa != 0)) || ((n.exponent == 0) && (n.mantissa != 0))){
		printf("Denormalized number!\n");
		return 3;
	}

	//情况3，出现正负无穷
	if(((m.exponent == 1) && (m.mantissa == 0)) || ((n.exponent == 1) && (n.mantissa == 0))){
		printf("Infinity!\n");
		return 4;
	}

	//情况4，出现NaN
	if(((m.exponent == 1) && (m.mantissa != 0)) || ((n.exponent == 1) && (n.mantissa != 0))){
		printf("NaN!\n");
		return 5;
	}
	return 0;
}
