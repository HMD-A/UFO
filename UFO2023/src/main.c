#include "com.h"
#include "httpServer.h"
#include "wsserver.h"
#include "md03.h"
#include "md04.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#define COM_MAIN 7
#define MIN(i, j) (((i) < (j)) ? (i) : (j))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

 RESDATA *res;
static int right, up, down, left, ynu;

static int x =0;
static int y =0;

static int pot1vol = 4000;
static int pot2vol = 4000;
static int ls1;
static int ls2;
static float gyro_deg_g=0;

static int sense_new = 0;

static float enc1_g,enc2_g;

void Line(){
    while(1){

        if (wsgetconnections() != 0)
		{

			if ((res = wsget()) != NULL)
			{
				//printf("%s\n",res->data);
                sscanf(res->data, "%d %d %d %d %d",&up, &down, &left, &right, &ynu);

			}
        
        //ここから
        //座標の時これ
        /*
        static int x =0;
        static int y =0;
        */
        
        

        //printf("state 0\n");
       
        //int x_time, y_time;   

  
       
            
        

            
	}
     
    

    }
}

void measure()
{
    //printf("Hello");
    //printf("%d\n",pot1vol);
    int degr = 0;
    char c;
    int byte = 0;
    int sum;

    int degr_diff;
    int degr_old = 0;

    int enc_diff;
    int enc1cnt = 0;
    int enc2cnt = 0;
    int enc1pre = 0;
    int enc2pre = 0;

    int pot1rec, pot2rec;

    int isFirst = 1;

    float now_deg_g=0;


    // バッファクリア
    //ここで4096個データを読めるような配列を定義
    unsigned char cBuf[4096];
    int size;

    //バッファにたまった過去のデータを読み込み、削除、sizeはバッファのデータ数
    while ((size = comsize(COM_MAIN)) > 0)
    {
        // comget(COM_MAIN);
        //最大4096個読む
        size = MIN(size, 4096);
        comgetA(COM_MAIN, cBuf, size);      //テラターム
    }

    //初期値としてnow_deg_g(0に設定)を代入、以下差分をとる形式で更新
    gyro_deg_g = now_deg_g;

    //データを最後に受け取ってからの時間を表す、初期値、通信障害などのエラー検出
    clock_t last_rec = clock();

    /* UARTは1byte(8bit)送信、MSBは先頭かどうかの区分を表す（設計の仕様上）、データは2番目から
    リミットスイッチいるならどっか空いてるバイトに押し込みたい
    ・0byteから
    ジャイロ下位7bit   ,,,ジャイロ合計14bit
    ジャイロ上位7bit
    エンコ1下位7bit    ,,,エンコーダー合計14bit
    エンコ1上位7bit
    エンコ2下位7bit
    エンコ2上位7bit
    ポテンショ1下位7bit     ,,,ポテンショメーター合計12bit
    下位5bit:ポテンショ1上位5bit 上位2bit下から:LS1,LS2
    ポテンショ2下位7bit
    ポテンショ2上位7bit
        今回ここにリミットスイッチをぶっさしてアナログ的に使う
        3つの突起が電源、入力、グラウンドなので、電源と入力の間に抵抗、
        入力とグラウンドの間にスイッチをつければ入力の電圧が
        offの時は高く,onの時は低くなる
    sum                ,,,上記のデータを全部足した（算術加算）の下位7bit
    */
    //printf("HELLO");
    while (1)
    {
        if (comsize(COM_MAIN) > 0)
        {
            //printf("Hello");
            //バッファから1byte読み出す
            c = comget(COM_MAIN);

            if ((c & 0x80) != 0)//0x80は10000000だからMSBだけ読む、つまりデータの先頭なら
            {
                byte = 1;       //何番目のデータ
                degr = c & 0x7F; //0x7Fは01111111だから下位7bitのとこだけ読む
                sum = c & 0x7F;     //通信
            }
            else if (byte == 1)
            {
                degr |= c << 7; //7bit左シフトしたものを元のdegr(ジャイロ下位7bit)に論理和
                sum += c;
                byte++;
            }
            else if (byte == 2)
            {
                enc1cnt = c;
                sum += c;
                byte++;
            }
            else if (byte == 3)
            {
                enc1cnt |= c << 7;
                sum += c;
                byte++;
            }
            else if (byte == 4)
            {
                enc2cnt = c;
                sum += c;
                byte++;
            }
            else if (byte == 5)
            {
                enc2cnt |= c << 7;
                sum += c;
                byte++;
            }
            else if (byte == 6)
            {
                pot1rec = c;
                sum += c;
                byte++;
            }
            else if (byte == 7)
            {
                pot1rec |= (c & 0x1F) << 7;
                ls1 = (c & 0x20) >> 5;
                ls2 = (c & 0x40) >> 6;
                sum += c;
                byte++;
            }
            else if (byte == 8)
            {
                pot2rec = c;
                sum += c;
                byte++;
            }
            else if (byte == 9)
            {
                pot2rec |= c << 7;
                sum += c;
                byte++;
            }
            else if (byte == 10)
            {
                if ((sum & 0x7F) == c)
                {
                    // ポテンショメーター
                    pot1vol = pot1rec;
                    pot2vol = pot2rec;

                    // ジャイロ数値処理
                    if (isFirst == 1)
                    {
                        //直前に読んだ角度と今の角度の差分
                        degr_diff = 0;
                    }
                    else
                    {
                        degr_diff = degr - degr_old;
                    }
                    degr_old = degr;

                    if (degr_diff < -180 * 16)
                        degr_diff += 360 * 16;
                    if (degr_diff > 180 * 16)
                        degr_diff -= 360 * 16;
                    gyro_deg_g += degr_diff / 16.f;

                    // エンコーダ数値処理
                    // ENC1
                    if (isFirst == 1)
                    {
                        enc_diff = 0;
                    }
                    else
                    {
                        enc_diff = enc1cnt - enc1pre;
                    }
                    enc1pre = enc1cnt;
                    if (enc_diff < -8192)
                        enc_diff += 0x3FFF; //16383
                    if (enc_diff > 8192)
                        enc_diff -= 0x3FFF;
                    enc1_g -= 6.f * M_PI * enc_diff / 400;

                    // ENC2
                    if (isFirst == 1)
                    {
                        enc_diff = 0;
                    }
                    else
                    {
                        enc_diff = enc2cnt - enc2pre;
                    }

                    enc2pre = enc2cnt;
                    if (enc_diff < -8192)
                        enc_diff += 0x3FFF;
                    if (enc_diff > 8192)
                        enc_diff -= 0x3FFF;
                    enc2_g += 6.f * M_PI * enc_diff / 400;

                    isFirst = 0;
                    sense_new = 1;
                    //printf("%d %d\n",pot1vol,pot2vol);
                    last_rec = clock();
                }

                byte = 0;
            }
            else
            {
                byte = 0;
            }
        }
        else
        {
            Sleep(1);
        }
        /*
        if ((isFirst == 0 && (double)(clock() - last_rec) / CLOCKS_PER_SEC > 0.1) || (isFirst == 1 && (double)(clock() - last_rec) / CLOCKS_PER_SEC > 1))
        {
            printf("error\n");
            exit(1);
        }
        */
    }
}

int main(){
    httpServer("../webpage",80); //http のポートが基本的に80だから
    wsinit(50001); //html のaddr参照

   
    comopen(COM_MAIN,115200);
    srand((unsigned int)time(NULL));

    CreateThread(0,0,(LPTHREAD_START_ROUTINE)Line,NULL,0,NULL);
    CreateThread(0,0,(LPTHREAD_START_ROUTINE)measure,NULL,0,NULL);
    

     static int state =0;
     while(1){


          if(state==0){
            printf("state 0\n");
            MD03SetMotor(COM_MAIN,4,0);
            MD03SetMotor(COM_MAIN,5,0);
            MD03SetMotor(COM_MAIN,6,0);  
            if(right==1){
                //clock_t x_st, x_ed;
                //x_st =clock();
                state = 1;
            }      //右ボタンが押されるまで待機  
            if(ynu==1){
                state = 10;
            }       
        }
       
    //x座標ver
        
    if(state==1){       //右ボタンが押されている時はx軸の変位
     
             MD03SetMotor(COM_MAIN,4,100);      //xが4番
             x++;
             //printf("state 1");

             if(right==0 || pot1vol<2000){      //state=1の状態で右ボタンが離されたらstateに2を代入
                MD03SetMotor(COM_MAIN,4,0);
                //x_ed = clock();
                //x_time=x_ed - x_st;
                state = 2;
            }
            if(ynu==1){
                state = 10;
            }     
             
        }
    
       
      /*

      //時間設定ver
        if(state==1){       //上ボタンが押されている間はy変位
        clock_t x_st, x_ed;
        x_st =clock();

            do{
                MD03SetMotor(COM_MAIN,4,500);   //yが5番
            } while(right == 1 && pot1vol>2000);   //rightがおされるかpot1volの値が2000以上なら継続

          //printf("state1\n");

            if(right==0){
              MD03SetMotor(COM_MAIN,4,0);        //上ボタンが離されたらモータ停止、state2へ移行
            x_ed =clock();
            x_time = x_ed - x_st;
            state = 2;   
            }
            
            
      */  
        //時間設定ver
        
            
            
        
        
        

        if(state==2){ 
             //printf("state 2\n");      //上ボタンが押されるまで待機
            if(up==1){   
                //clock_t y_st, y_ed;
                //y_st =clock();     
                state = 3; 
                
            }
        }
        //y座標ver
        if(state==3){       //上ボタンが押されている間はy変位
            MD03SetMotor(COM_MAIN,5,100);      //yが5番
            y++;
            //printf("state 3");
         // printf("y=%d\n",y);

             if(up==0 || pot2vol<2000){
                MD03SetMotor(COM_MAIN,5,0);        //上ボタンが離されたらモータ停止、state4へ移行
                //y_ed =clock();
                //y_time = y_ed - y_st;
                state = 4;
            }
        }
        
        
        /*
        //時間設定ver
        if(state==3){       //上ボタンが押されている間はy変位
        clock_t y_st, y_ed;
        y_st =clock();

            do{
                MD03SetMotor(COM_MAIN,5,500);   //yが5番
            }while(up == 1 && pot2vol>2000);    //upがおされるかpot2volの値が2000以上なら継続

          //printf("state3\n");

        if(up==0){
             MD03SetMotor(COM_MAIN,5,0);        //上ボタンが離されたらモータ停止、state4へ移行
            y_ed =clock();
            y_time = y_ed - y_st;
                state = 4;
        }
            
        }
        */
        
        
        
        if(state==4){                   //商品をつかむ～最後（自動）
        //int z_time=0;
           // printf("state 4\n");

        clock_t armopen_st,armopen_ed;
        armopen_st = clock();
            do{       //アームを開く
                MD04CCCV(COM_MAIN,7,3,250);
                armopen_ed = clock();
                printf("state 4\n");
            }while((armopen_ed - armopen_st)<2000);
        

        clock_t z_upstart,z_upend;
        z_upstart = clock();
            do{
                MD03SetMotor(COM_MAIN,6,-150);              //z軸下降
                 MD04CCCV(COM_MAIN,7,3,250);
                z_upend = clock();
                printf("state 4\n");
            }while((z_upend - z_upstart)<4500);

                MD03SetMotor(COM_MAIN,6,0);

                int crt = rand() % 3 + 2;   //乱数の設定（電流を20mA,30mA,40mA）

        clock_t armclose_st,armclose_ed;
            armclose_st = clock();
            do{       //アームを閉じる
                MD04CCCV(COM_MAIN,7,3,-250);
                armclose_ed = clock();
                printf("state 4\n");
            }while((armclose_ed - armclose_st)<2000);

        clock_t z_downstart,z_downend;
        z_downstart = clock();
            do{
                MD03SetMotor(COM_MAIN,6,150);         //z軸上昇
                MD04CCCV(COM_MAIN,7,3,-250);
                //MD04CCCV(COM_MAIN,7,crt,-25);
                z_downend = clock();
                printf("state 4\n");
            }while((z_downend - z_downstart)<4500);


                MD03SetMotor(COM_MAIN,6,0);
                state =5;
            
            //clock_t y2_st,y2_ed;

           // y2_st = clock();
            
            /*
            do{
                MD03SetMotor(COM_MAIN,5,-500);     //y座標戻す
                MD04CCCV(COM_MAIN,7,3,-25);
                //MD04CCCV(COM_MAIN,7,crt,-25);
                //y2_ed = clock();

            }while(pot2vol>2000 || y2_ed - y2_st < y_time);

             MD03SetMotor(COM_MAIN,5,0); 
            */
            
            
           
            if(state == 5){
                if(y>0 && pot2vol>2000){
                    MD03SetMotor(COM_MAIN,5,-100);     //y座標戻す
                    MD04CCCV(COM_MAIN,7,3,-250);
                    //MD04CCCV(COM_MAIN,7,crt,-25);
                    y=y-2;

                    

                    if(ynu==1){
                    state = 10;
                    }
                }

                else{
                    MD03SetMotor(COM_MAIN,5,0);
                    Sleep(500);
                    state = 6;
                }

                
            }
        
             //clock_t x2_st,x2_ed;

            //x2_st = clock();
            /*
             do{
                MD03SetMotor(COM_MAIN,4,-500);     //x座標戻す
                MD04CCCV(COM_MAIN,7,3,-25);
                //MD04CCCV(COM_MAIN,7,crt,-25);
                //x2_ed = clock();

            }while(pot1vol>2000 || x2_ed - x2_st < x_time);
            
             MD03SetMotor(COM_MAIN,4,0);
            */
           
             
        if(state==6){  
            if(pot1vol>2000 && x>0){
                MD03SetMotor(COM_MAIN,4,-100);   //x座標戻す
                //MD04CCCV(COM_MAIN,7,crt,60);
                MD04CCCV(COM_MAIN,7,3,-250);
                x--;
                //printf("state6\n");
                if(ynu==1){
                state = 10;
                }   

            }
            else{
                MD03SetMotor(COM_MAIN,4,0);
                Sleep(500);
                state = 7;
            }
        }
            
             
        if(state==7){
        clock_t armopen2_st,armopen2_ed;
            armopen2_st = clock();
            do{       //アームを開く
            printf("state 7\n");
            MD04CCCV(COM_MAIN,7,6,250);
            armopen2_ed = clock();
            state =8;
            }while((armopen2_ed - armopen2_st)<2000);
         
        }
        

        if(state==8){      //初期化
             x =0;
             y =0;
            printf("state8\n");
             MD04CCCV(COM_MAIN,7,0,0); //アームに加える電流電圧を0に
              Sleep(1000); 
             state =0;    //アームに加える電流電圧を0に
               
        }

        if(state == 10){
            for(int i=4;i<7;i++){
               MD03SetMotor(COM_MAIN,i,0); 
            }
            state = 11;
        }
        
        if(state == 11){
            if(ynu==1){
                //printf("state 11\n");
                if(right==1)
                    MD03SetMotor(COM_MAIN,4,100);
                else if(up==1)
                    MD03SetMotor(COM_MAIN,5,100);
                else if(left==1)
                    MD03SetMotor(COM_MAIN,4,-100);
                else if(down==1)
                    MD03SetMotor(COM_MAIN,5,-100);
                else{
                    for(int j=4;j<6;j++)
                     MD03SetMotor(COM_MAIN,j,0);  
                }      
            }
            if(ynu==0)
                state = 8;
            
            
        }
    
        }
        printf("state=%d  ynu=%d\n",state,ynu);
        Sleep(500);
        printf("y=%d  pot2vol=%d\n",y,pot2vol);
    }
    

    wsclose();//serverを閉じる。

    return 0;
}